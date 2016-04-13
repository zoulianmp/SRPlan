/*==============================================================================

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Adam Rankin and Csaba Pinter, PerkLab, Queen's
  University and was supported through the Applied Cancer Research Unit program of Cancer
  Care Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

// Segmentations includes
#include "vtkMRMLSegmentationStorageNode.h"

#include "vtkSegmentation.h"
#include "vtkOrientedImageData.h"
#include "vtkOrientedImageDataResample.h"

// MRML includes
#include "vtkMRMLSegmentationNode.h"

// VTK includes
#include <vtkMRMLScene.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkDataObject.h>
#include <vtkPolyData.h>
#include <vtkFieldData.h>
#include <vtkStringArray.h>
#include <vtkDoubleArray.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkXMLMultiBlockDataWriter.h>
#include <vtkXMLMultiBlockDataReader.h>
#include <vtksys/SystemTools.hxx>
#include <vtkInformation.h>
#include <vtkInformationIntegerVectorKey.h>
#include <vtkInformationStringKey.h>

// ITK includes
#include <itkImageFileWriter.h>
#include <itkNrrdImageIO.h>
#include <itkExceptionObject.h>
#include <itkImageFileWriter.h>
#include <itkImageFileReader.h>
#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>

// STL & C++ includes
#include <iterator>
#include <sstream>

//----------------------------------------------------------------------------
static const std::string SERIALIZATION_SEPARATOR = "|";
static const std::string SEGMENT_ID = "ID";
static const std::string SEGMENT_NAME = "Name";
static const std::string SEGMENT_DEFAULT_COLOR = "DefaultColor";
static const std::string SEGMENT_TAGS = "Tags";
static const std::string SEGMENT_EXTENT = "Extent";
static const std::string MASTER_REPRESENTATION = "MasterRepresentation";
static const std::string CONVERSION_PARAMETERS = "ConversionParameters";
static const std::string CONTAINED_REPRESENTATION_NAMES = "ContainedRepresentationNames";

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLSegmentationStorageNode);

//----------------------------------------------------------------------------
vtkMRMLSegmentationStorageNode::vtkMRMLSegmentationStorageNode()
{
}

//----------------------------------------------------------------------------
vtkMRMLSegmentationStorageNode::~vtkMRMLSegmentationStorageNode()
{
}

//----------------------------------------------------------------------------
void vtkMRMLSegmentationStorageNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLStorageNode::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkMRMLSegmentationStorageNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL)
  {
    attName = *(atts++);
    attValue = *(atts++);
  }

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLSegmentationStorageNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
  vtkIndent indent(nIndent);
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, StorageID
void vtkMRMLSegmentationStorageNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();

  Superclass::Copy(anode);
  vtkMRMLSegmentationStorageNode *node = (vtkMRMLSegmentationStorageNode *) anode;

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLSegmentationStorageNode::InitializeSupportedReadFileTypes()
{
  this->SupportedReadFileTypes->InsertNextValue("Segmentation 4D NRRD volume (.seg.nrrd)");
  this->SupportedReadFileTypes->InsertNextValue("Segmentation Multi-block dataset (.seg.vtm)");
}

//----------------------------------------------------------------------------
void vtkMRMLSegmentationStorageNode::InitializeSupportedWriteFileTypes()
{
  Superclass::InitializeSupportedWriteFileTypes();

  vtkMRMLSegmentationNode* segmentationNode = this->GetAssociatedDataNode();
  if (segmentationNode)
  {
    const char* masterRepresentation = segmentationNode->GetSegmentation()->GetMasterRepresentationName();
    if (masterRepresentation)
    {
      if (!strcmp(masterRepresentation, vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName()))
      {
        // Binary labelmap -> 4D NRRD volume
        this->SupportedWriteFileTypes->InsertNextValue("Segmentation 4D NRRD volume (.seg.nrrd)");
      }
      else if ( !strcmp(masterRepresentation, vtkSegmentationConverter::GetSegmentationClosedSurfaceRepresentationName())
             || !strcmp(masterRepresentation, vtkSegmentationConverter::GetSegmentationPlanarContourRepresentationName()) )
      {
        // Closed surface or planar contours -> MultiBlock polydata
        this->SupportedWriteFileTypes->InsertNextValue("Segmentation Multi-block dataset (.seg.vtm)");
      }
    }
  }
}

//----------------------------------------------------------------------------
vtkMRMLSegmentationNode* vtkMRMLSegmentationStorageNode::GetAssociatedDataNode()
{
  if (!this->GetScene())
  {
    return NULL;
  }

  std::vector<vtkMRMLNode*> segmentationNodes;
  unsigned int numberOfNodes = this->GetScene()->GetNodesByClass("vtkMRMLSegmentationNode", segmentationNodes);
  for (unsigned int nodeIndex=0; nodeIndex<numberOfNodes; nodeIndex++)
  {
    vtkMRMLSegmentationNode* node = vtkMRMLSegmentationNode::SafeDownCast(segmentationNodes[nodeIndex]);
    if (node)
    {
      const char* storageNodeID = node->GetStorageNodeID();
      if (storageNodeID && !strcmp(storageNodeID, this->ID))
      {
        return vtkMRMLSegmentationNode::SafeDownCast(node);
      }
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------
const char* vtkMRMLSegmentationStorageNode::GetDefaultWriteFileExtension()
{
  vtkMRMLSegmentationNode* segmentationNode = this->GetAssociatedDataNode();
  if (segmentationNode)
  {
    const char* masterRepresentation = segmentationNode->GetSegmentation()->GetMasterRepresentationName();
    if (masterRepresentation)
    {
      if (!strcmp(masterRepresentation, vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName()))
      {
        // Binary labelmap -> 4D NRRD volume
        return "seg.nrrd";
      }
      else if ( !strcmp(masterRepresentation, vtkSegmentationConverter::GetSegmentationClosedSurfaceRepresentationName())
             || !strcmp(masterRepresentation, vtkSegmentationConverter::GetSegmentationPlanarContourRepresentationName()) )
      {
        // Closed surface or planar contours -> MultiBlock polydata
        return "seg.vtm";
      }
    }
  }

  // Master representation is not supported for writing to file
  return NULL;
}

//----------------------------------------------------------------------------
void vtkMRMLSegmentationStorageNode::ResetSupportedWriteFileTypes()
{
  this->InitializeSupportedWriteFileTypes();
}

//----------------------------------------------------------------------------
bool vtkMRMLSegmentationStorageNode::CanReadInReferenceNode(vtkMRMLNode *refNode)
{
  return refNode->IsA("vtkMRMLSegmentationNode");
}

//----------------------------------------------------------------------------
int vtkMRMLSegmentationStorageNode::ReadDataInternal(vtkMRMLNode *refNode)
{
  vtkMRMLSegmentationNode* segmentationNode = vtkMRMLSegmentationNode::SafeDownCast(refNode);
  if (!segmentationNode)
  {
    vtkErrorMacro("ReadDataInternal: Reference node is not a segmentation node");
    return 0;
  }

  std::string fullName = this->GetFullNameFromFileName();
  if (fullName == std::string(""))
  {
    vtkErrorMacro("ReadDataInternal: File name not specified");
    return 0;
  }

  // Check that the file exists
  if (vtksys::SystemTools::FileExists(fullName.c_str()) == false)
  {
    vtkErrorMacro("ReadDataInternal: segmentation file '" << fullName.c_str() << "' not found.");
    return 0;
  }

  // Try to read as labelmap first then as poly data
  if (this->ReadBinaryLabelmapRepresentation(segmentationNode->GetSegmentation(), fullName))
  {
    return 1;
  }
  else if (this->ReadPolyDataRepresentation(segmentationNode->GetSegmentation(), fullName))
  {
    return 1;
  }

  // Failed to read
  vtkErrorMacro("ReadDataInternal: File " << fullName << " could not be read neither as labelmap nor poly data");
  return 0;
}

//----------------------------------------------------------------------------
int vtkMRMLSegmentationStorageNode::ReadBinaryLabelmapRepresentation(vtkSegmentation* segmentation, std::string path)
{
  if (!vtksys::SystemTools::FileExists(path.c_str()))
  {
    vtkErrorMacro("ReadBinaryLabelmapRepresentation: Input file " << path << " does not exist!");
    return 0;
  }

  // Set up output segmentation
  if (!segmentation || segmentation->GetNumberOfSegments() > 0)
  {
    vtkErrorMacro("ReadBinaryLabelmapRepresentation: Output segmentation must exist and must be empty!");
    return 0;
  }

  // Read 4D NRRD image file
  typedef itk::ImageFileReader<BinaryLabelmap4DImageType> FileReaderType;
  FileReaderType::Pointer reader = FileReaderType::New();
  reader->SetFileName(path);
  try
  {
    reader->Update();
  }
  catch (itk::ImageFileReaderException &error)
  {
    // Do not report error as the file might contain poly data in which case ReadPolyDataRepresentation will read it alright
    vtkDebugMacro("ReadBinaryLabelmapRepresentation: Failed to load file " << path << " as segmentation. Exception:\n" << error);
    return 0;
  }
  BinaryLabelmap4DImageType::Pointer allSegmentLabelmapsImage = reader->GetOutput();

  // Read succeeded, set master representation
  segmentation->SetMasterRepresentationName(vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName());

  // Get metadata dictionary from image
  itk::MetaDataDictionary metadata = allSegmentLabelmapsImage->GetMetaDataDictionary();
  // Read common geometry extent
  std::string commonExtent;
  itk::ExposeMetaData<std::string>(metadata, SEGMENT_EXTENT.c_str(), commonExtent);
  std::stringstream ssCommonExtent;
  ssCommonExtent << commonExtent;
  int commonGeometryExtent[6] = {0,-1,0,-1,0,-1};
  ssCommonExtent >> commonGeometryExtent[0] >> commonGeometryExtent[1] >> commonGeometryExtent[2] >> commonGeometryExtent[3] >> commonGeometryExtent[4] >> commonGeometryExtent[5];
  // Read conversion parameters
  std::string conversionParameters;
  itk::ExposeMetaData<std::string>(metadata, CONVERSION_PARAMETERS.c_str(), conversionParameters);
  segmentation->DeserializeConversionParameters(conversionParameters);
  // Read contained representation names
  std::string containedRepresentationNames;
  itk::ExposeMetaData<std::string>(metadata, CONTAINED_REPRESENTATION_NAMES.c_str(), containedRepresentationNames);

  // Get image properties
  BinaryLabelmap4DImageType::RegionType itkRegion = allSegmentLabelmapsImage->GetLargestPossibleRegion();
  BinaryLabelmap4DImageType::PointType itkOrigin = allSegmentLabelmapsImage->GetOrigin();
  BinaryLabelmap4DImageType::SpacingType itkSpacing = allSegmentLabelmapsImage->GetSpacing();
  BinaryLabelmap4DImageType::DirectionType itkDirections = allSegmentLabelmapsImage->GetDirection();
  // Make image properties accessible for VTK
  double origin[3] = {itkOrigin[0], itkOrigin[1], itkOrigin[2]};
  double spacing[3] = {itkSpacing[0], itkSpacing[1], itkSpacing[2]};
  double directions[3][3] = {{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0}};
  for (unsigned int col=0; col<3; col++)
  {
    for (unsigned int row=0; row<3; row++)
    {
      directions[row][col] = itkDirections[row][col];
    }
  }

  // Read segment binary labelmaps
  for (int segmentIndex = itkRegion.GetIndex()[3]; segmentIndex < itkRegion.GetIndex()[3]+itkRegion.GetSize()[3]; ++segmentIndex)
  {
    // Create segment
    vtkSmartPointer<vtkSegment> currentSegment = vtkSmartPointer<vtkSegment>::New();
    
    // Get metadata for current segment
    std::stringstream ssIdKey;
    ssIdKey << segmentIndex << SEGMENT_ID;
    std::string idKey = ssIdKey.str();
    std::string currentSegmentID;
    itk::ExposeMetaData<std::string>(metadata, idKey.c_str(), currentSegmentID);

    std::stringstream ssNameKey;
    ssNameKey << segmentIndex << SEGMENT_NAME;
    std::string nameKey = ssNameKey.str();
    std::string currentSegmentName;
    itk::ExposeMetaData<std::string>(metadata, nameKey.c_str(), currentSegmentName);
    currentSegment->SetName(currentSegmentName.c_str());

    std::stringstream ssDefaultColorKey;
    ssDefaultColorKey << segmentIndex << SEGMENT_DEFAULT_COLOR;
    std::string defaultColorKey = ssDefaultColorKey.str();
    std::string defaultColorValue;
    itk::ExposeMetaData<std::string>(metadata, defaultColorKey.c_str(), defaultColorValue);
    std::stringstream ssDefaultColorValue;
    ssDefaultColorValue << defaultColorValue;
    double currentSegmentDefaultColor[3] = {0.0,0.0,0.0};
    ssDefaultColorValue >> currentSegmentDefaultColor[0] >> currentSegmentDefaultColor[1] >> currentSegmentDefaultColor[2];
    currentSegment->SetDefaultColor(currentSegmentDefaultColor);

    std::stringstream ssExtentKey;
    ssExtentKey << segmentIndex << SEGMENT_EXTENT;
    std::string extentKey = ssExtentKey.str();
    std::string extentValue;
    itk::ExposeMetaData<std::string>(metadata, extentKey.c_str(), extentValue);
    std::stringstream ssExtentValue;
    ssExtentValue << extentValue;
    int currentSegmentExtent[6] = {0,-1,0,-1,0,-1};
    ssExtentValue >> currentSegmentExtent[0] >> currentSegmentExtent[1] >> currentSegmentExtent[2] >> currentSegmentExtent[3] >> currentSegmentExtent[4] >> currentSegmentExtent[5];

    //TODO: Parse tags with key SEGMENT_TAGS

    // Create binary labelmap volume
    vtkSmartPointer<vtkOrientedImageData> currentBinaryLabelmap = vtkSmartPointer<vtkOrientedImageData>::New();
    currentBinaryLabelmap->SetOrigin(origin);
    currentBinaryLabelmap->SetSpacing(spacing);
    currentBinaryLabelmap->SetDirections(directions);
    currentBinaryLabelmap->SetExtent(currentSegmentExtent);
    currentBinaryLabelmap->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
    unsigned char* labelmapPtr = (unsigned char*)currentBinaryLabelmap->GetScalarPointerForExtent(currentSegmentExtent);
    
    // Define ITK region for current segment
    BinaryLabelmap4DImageType::RegionType segmentRegion;
    BinaryLabelmap4DImageType::SizeType segmentRegionSize;
    BinaryLabelmap4DImageType::IndexType segmentRegionIndex;
    segmentRegionIndex[0] = segmentRegionIndex[1] = segmentRegionIndex[2] = 0;
    segmentRegionIndex[3] = segmentIndex;
    segmentRegionSize = itkRegion.GetSize();
    segmentRegionSize[3] = 1;
    segmentRegion.SetIndex(segmentRegionIndex);
    segmentRegion.SetSize(segmentRegionSize);

    // Iterate through current segment's region and read voxel values into segment labelmap
    BinaryLabelmap4DIteratorType segmentLabelmapIterator(allSegmentLabelmapsImage, segmentRegion);
    for (segmentLabelmapIterator.GoToBegin(); !segmentLabelmapIterator.IsAtEnd(); ++segmentLabelmapIterator)
    {
      // Skip region outside extent of current segment (consider common extent boundaries)
      BinaryLabelmap4DImageType::IndexType segmentIndex = segmentLabelmapIterator.GetIndex();
      if ( segmentIndex[0] + commonGeometryExtent[0] < currentSegmentExtent[0]
        || segmentIndex[0] + commonGeometryExtent[0] > currentSegmentExtent[1]
        || segmentIndex[1] + commonGeometryExtent[2] < currentSegmentExtent[2]
        || segmentIndex[1] + commonGeometryExtent[2] > currentSegmentExtent[3]
        || segmentIndex[2] + commonGeometryExtent[4] < currentSegmentExtent[4]
        || segmentIndex[2] + commonGeometryExtent[4] > currentSegmentExtent[5] )
      {
        continue;
      }

      // Get voxel value
      unsigned char voxelValue = segmentLabelmapIterator.Get();

      // Set voxel value in current segment labelmap
      (*labelmapPtr) = voxelValue;
      ++labelmapPtr;
    }

    // Set loaded binary labelmap to segment
    currentSegment->AddRepresentation(vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName(), currentBinaryLabelmap);

    // Add segment to segmentation
    segmentation->AddSegment(currentSegment, currentSegmentID);
  }

  // Create contained representations now that all the data is loaded
  this->CreateRepresentationsBySerializedNames(segmentation, containedRepresentationNames);

  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLSegmentationStorageNode::ReadPolyDataRepresentation(vtkSegmentation* segmentation, std::string path)
{
  if (!vtksys::SystemTools::FileExists(path.c_str()))
  {
    vtkErrorMacro("ReadPolyDataRepresentation: Input file " << path << " does not exist!");
    return 0;
  }

  // Set up output segmentation
  if (!segmentation || segmentation->GetNumberOfSegments() > 0)
  {
    vtkErrorMacro("ReadPolyDataRepresentation: Output segmentation must exist and must be empty!");
    return 0;
  }

  // Add all files to storage node (multiblock dataset writes segments to individual files in a separate folder)
  this->AddPolyDataFileNames(path, segmentation);

  // Read multiblock dataset from disk
  vtkSmartPointer<vtkXMLMultiBlockDataReader> reader = vtkSmartPointer<vtkXMLMultiBlockDataReader>::New();
  reader->SetFileName(path.c_str());
  reader->Update();
  vtkMultiBlockDataSet* multiBlockDataset = vtkMultiBlockDataSet::SafeDownCast(reader->GetOutput());
  if (!multiBlockDataset)
  {
    vtkErrorMacro("ReadPolyDataRepresentation: Failed to read file " << path);
    return 0;
  }

  // Read segment poly datas
  std::string containedRepresentationNames("");
  std::string conversionParameters("");
  for (int blockIndex=0; blockIndex<multiBlockDataset->GetNumberOfBlocks(); ++blockIndex)
  {
    // Get poly data representation
    vtkPolyData* currentPolyData = vtkPolyData::SafeDownCast(multiBlockDataset->GetBlock(blockIndex));

    // Set master representation if it has not been set yet
    // (segment field data contains it, there is no global place to store it)
    if (!segmentation->GetMasterRepresentationName())
    {
      vtkStringArray* masterRepresentationArray = vtkStringArray::SafeDownCast(
        currentPolyData->GetFieldData()->GetAbstractArray(MASTER_REPRESENTATION.c_str()) );
      if (!masterRepresentationArray)
      {
        vtkErrorMacro("ReadPolyDataRepresentation: Unable to find master representation for segmentation in file " << path);
        return 0;
      }
      segmentation->SetMasterRepresentationName(masterRepresentationArray->GetValue(0).c_str());
    }
    // Read conversion parameters (stored in each segment file, but need to set only once)
    if (conversionParameters.empty())
    {
      vtkStringArray* conversionParametersArray = vtkStringArray::SafeDownCast(
        currentPolyData->GetFieldData()->GetAbstractArray(CONVERSION_PARAMETERS.c_str()) );
      conversionParameters = conversionParametersArray->GetValue(0);
      segmentation->DeserializeConversionParameters(conversionParameters);
    }
    // Read contained representation names
    if (containedRepresentationNames.empty())
    {
      containedRepresentationNames = vtkStringArray::SafeDownCast(
        currentPolyData->GetFieldData()->GetAbstractArray(CONTAINED_REPRESENTATION_NAMES.c_str()) )->GetValue(0);
    }

    // Create segment
    vtkSmartPointer<vtkSegment> currentSegment = vtkSmartPointer<vtkSegment>::New();
    currentSegment->AddRepresentation(segmentation->GetMasterRepresentationName(), currentPolyData);

    // Set segment properties
    vtkStringArray* idArray = vtkStringArray::SafeDownCast(
      currentPolyData->GetFieldData()->GetAbstractArray(SEGMENT_ID.c_str()) );
    vtkStringArray* nameArray = vtkStringArray::SafeDownCast(
      currentPolyData->GetFieldData()->GetAbstractArray(SEGMENT_NAME.c_str()) );
    vtkDoubleArray* defaultColorArray = vtkDoubleArray::SafeDownCast(
      currentPolyData->GetFieldData()->GetArray(SEGMENT_DEFAULT_COLOR.c_str()) );
    if (!idArray || !nameArray || !defaultColorArray)
    {
      vtkErrorMacro("ReadPolyDataRepresentation: Unable to find segment properties for segment number " << blockIndex << " referenced from segmentation file " << path);
      continue;
    }
    std::string currentSegmentID = idArray->GetValue(0);
    currentSegment->SetName(nameArray->GetValue(0).c_str());
    currentSegment->SetDefaultColor(defaultColorArray->GetComponent(0,0), defaultColorArray->GetComponent(0,1), defaultColorArray->GetComponent(0,2));

    //TODO: Parse tags with key SEGMENT_TAGS

    // Add segment to segmentation
    segmentation->AddSegment(currentSegment, currentSegmentID);
  }

  // Create contained representations now that all the data is loaded
  this->CreateRepresentationsBySerializedNames(segmentation, containedRepresentationNames);

  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLSegmentationStorageNode::WriteDataInternal(vtkMRMLNode *refNode)
{
  std::string fullName = this->GetFullNameFromFileName();
  if (fullName == std::string(""))
  {
    vtkErrorMacro("vtkMRMLModelNode: File name not specified");
    return 0;
  }

  vtkMRMLSegmentationNode *segmentationNode = vtkMRMLSegmentationNode::SafeDownCast(refNode);
  if (segmentationNode == NULL)
  {
    vtkErrorMacro("Segmentation node expected. Unable to write node to file.");
    return 0;
  }

  // Write only master representation
  vtkSegmentation* segmentation = segmentationNode->GetSegmentation();
  const char* masterRepresentation = segmentation->GetMasterRepresentationName();
  if (!strcmp(masterRepresentation, vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName()))
  {
    // Binary labelmap -> 4D NRRD volume
    return this->WriteBinaryLabelmapRepresentation(segmentation, fullName);
  }
  else if ( !strcmp(masterRepresentation, vtkSegmentationConverter::GetSegmentationClosedSurfaceRepresentationName())
         || !strcmp(masterRepresentation, vtkSegmentationConverter::GetSegmentationPlanarContourRepresentationName()) )
  {
    // Closed surface or planar contours -> MultiBlock polydata
    return this->WritePolyDataRepresentation(segmentation, fullName);
  }

  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLSegmentationStorageNode::WriteBinaryLabelmapRepresentation(vtkSegmentation* segmentation, std::string fullName)
{
  if (!segmentation || segmentation->GetNumberOfSegments() == 0)
  {
    vtkErrorMacro("WriteBinaryLabelmapRepresentation: Invalid segmentation to write to disk");
    return 0;
  }

  // Get and check master representation
  const char* masterRepresentation = segmentation->GetMasterRepresentationName();
  if (!masterRepresentation || strcmp(masterRepresentation, vtkSegmentationConverter::GetSegmentationBinaryLabelmapRepresentationName()))
  {
    vtkErrorMacro("WriteBinaryLabelmapRepresentation: Invalid master representation to write as image data");
    return 0;
  }

  // Determine merged labelmap dimensions and properties
  std::string commonGeometryString = segmentation->DetermineCommonLabelmapGeometry();
  vtkSmartPointer<vtkOrientedImageData> commonGeometryImage = vtkSmartPointer<vtkOrientedImageData>::New();
  vtkSegmentationConverter::DeserializeImageGeometry(commonGeometryString, commonGeometryImage);
  int* commonGeometryDimensions = commonGeometryImage->GetDimensions();
  int dimensions[4] = {commonGeometryDimensions[0],commonGeometryDimensions[1],commonGeometryDimensions[2],segmentation->GetNumberOfSegments()};
  double* commonGeometryOrigin = commonGeometryImage->GetOrigin();
  double originArray[4] = {commonGeometryOrigin[0],commonGeometryOrigin[1],commonGeometryOrigin[2],0.0};
  double* commonGeometrySpacing = commonGeometryImage->GetSpacing();
  double spacingArray[4] = {commonGeometrySpacing[0],commonGeometrySpacing[1],commonGeometrySpacing[2],1.0};
  double directionsArray[3][3] = {{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0}};
  commonGeometryImage->GetDirections(directionsArray);

  // Determine ITK image properties
  BinaryLabelmap4DImageType::SizeType regionSize;
  BinaryLabelmap4DImageType::IndexType regionIndex;
  BinaryLabelmap4DImageType::RegionType region;
  BinaryLabelmap4DImageType::PointType origin;
  BinaryLabelmap4DImageType::SpacingType spacing;
  BinaryLabelmap4DImageType::DirectionType directions;
  for (int dim = 0; dim < 4; dim++)
  {
    regionIndex[dim] = 0;
    regionSize[dim] = dimensions[dim];
    spacing[dim] = spacingArray[dim];
    origin[dim] = originArray[dim];
  }
  region.SetSize(regionSize);
  region.SetIndex(regionIndex);
  // Normalize direction vectors
  for (unsigned int col=0; col<3; col++)
  {
    double len = 0;
    unsigned int row = 0;
    for (row=0; row<3; row++)
    {
      len += directionsArray[row][col] * directionsArray[row][col];
    }
    if (len == 0.0)
    {
      len = 1.0;
    }
    len = sqrt(len);
    for (row=0; row<3; row++)
    {
      directions[row][col] = directionsArray[row][col]/len;
    }
  }
  // Add fourth dimension to directions matrix
  directions[3][3] = 1.0;
  for (unsigned int index=0; index<3; index++)
  {
    directions[3][index] = 0.0;
    directions[index][3] = 0.0;
  }

  // Create 4D labelmap image and set ITK image properties
  BinaryLabelmap4DImageType::Pointer itkLabelmapImage = BinaryLabelmap4DImageType::New();
  itkLabelmapImage->SetRegions(region);
  itkLabelmapImage->SetOrigin(origin);
  itkLabelmapImage->SetSpacing(spacing);
  itkLabelmapImage->SetDirection(directions);
  itkLabelmapImage->Allocate();

  // Create metadata dictionary
  itk::MetaDataDictionary metadata;
  // Save extent of common geometry image
  int commonGeometryExtent[6] = {0,-1,0,-1,0,-1};
  commonGeometryImage->GetExtent(commonGeometryExtent);
  std::stringstream ssCommonExtent;
  ssCommonExtent << commonGeometryExtent[0] << " " << commonGeometryExtent[1] << " " << commonGeometryExtent[2]
    << " " << commonGeometryExtent[3] << " " << commonGeometryExtent[4] << " " << commonGeometryExtent[5];
  std::string commonExtent = ssCommonExtent.str();
  itk::EncapsulateMetaData<std::string>(metadata, SEGMENT_EXTENT.c_str(), commonExtent);
  // Save master representation name
  itk::EncapsulateMetaData<std::string>(metadata, MASTER_REPRESENTATION.c_str(), masterRepresentation);
  // Save conversion parameters
  std::string conversionParameters = segmentation->SerializeAllConversionParameters();
  itk::EncapsulateMetaData<std::string>(metadata, CONVERSION_PARAMETERS.c_str(), conversionParameters);
  // Save created representation names so that they are re-created when loading
  std::string containedRepresentationNames = this->SerializeContainedRepresentationNames(segmentation);
  itk::EncapsulateMetaData<std::string>(metadata, CONTAINED_REPRESENTATION_NAMES.c_str(), containedRepresentationNames);

  // Dimensions of the output 4D NRRD file: (i, j, k, segment)
  vtkSegmentation::SegmentMap segmentMap = segmentation->GetSegments();
  unsigned int segmentIndex = 0;
  for (vtkSegmentation::SegmentMap::iterator segmentIt = segmentMap.begin(); segmentIt != segmentMap.end(); ++segmentIt, ++segmentIndex)
  {
    std::string currentSegmentID = segmentIt->first;
    vtkSegment* currentSegment = segmentIt->second.GetPointer();

    // Get master representation from segment
    vtkOrientedImageData* currentBinaryLabelmap = vtkOrientedImageData::SafeDownCast(currentSegment->GetRepresentation(masterRepresentation));
    if (!currentBinaryLabelmap)
    {
      vtkErrorMacro("WriteBinaryLabelmapRepresentation: Failed to retrieve master representation from segment " << currentSegmentID);
      continue;
    }

    // Resample current binary labelmap representation to common geometry if necessary
    if (!vtkOrientedImageDataResample::DoGeometriesMatch(commonGeometryImage, currentBinaryLabelmap))
    {
      bool success = vtkOrientedImageDataResample::ResampleOrientedImageToReferenceOrientedImage(
        currentBinaryLabelmap, commonGeometryImage, currentBinaryLabelmap );
      if (!success)
      {
        vtkWarningMacro("WriteBinaryLabelmapRepresentation: Segment " << currentSegmentID << " cannot be resampled to common geometry!");
        continue;
      }
    }

    // Set metadata for current segment
    std::stringstream ssIdKey;
    ssIdKey << segmentIndex << SEGMENT_ID;
    std::string idKey = ssIdKey.str();
    itk::EncapsulateMetaData<std::string>(metadata, idKey.c_str(), currentSegmentID);

    std::stringstream ssNameKey;
    ssNameKey << segmentIndex << SEGMENT_NAME;
    std::string nameKey = ssNameKey.str();
    itk::EncapsulateMetaData<std::string>(metadata, nameKey.c_str(), std::string(currentSegment->GetName()));

    std::stringstream ssDefaultColorKey;
    ssDefaultColorKey << segmentIndex << SEGMENT_DEFAULT_COLOR;
    std::string defaultColorKey = ssDefaultColorKey.str();
    std::stringstream ssDefaultColorValue;
    ssDefaultColorValue << currentSegment->GetDefaultColor()[0] << " " << currentSegment->GetDefaultColor()[1] << " " << currentSegment->GetDefaultColor()[2];
    std::string defaultColorValue = ssDefaultColorValue.str();
    itk::EncapsulateMetaData<std::string>(metadata, defaultColorKey.c_str(), defaultColorValue);

    std::stringstream ssExtentKey;
    ssExtentKey << segmentIndex << SEGMENT_EXTENT;
    std::string extentKey = ssExtentKey.str();
    int currentSegmentExtent[6] = {0,-1,0,-1,0,-1};
    currentBinaryLabelmap->GetExtent(currentSegmentExtent);
    std::stringstream ssExtentValue;
    ssExtentValue << currentSegmentExtent[0] << " " << currentSegmentExtent[1] << " " << currentSegmentExtent[2]
      << " " << currentSegmentExtent[3] << " " << currentSegmentExtent[4] << " " << currentSegmentExtent[5];
    std::string extentValue = ssExtentValue.str();
    itk::EncapsulateMetaData<std::string>(metadata, extentKey.c_str(), extentValue);

    //TODO: Store tags with key SEGMENT_TAGS

    // Define ITK region for the current segment
    BinaryLabelmap4DImageType::IndexType segmentRegionIndex;
    segmentRegionIndex[0] = segmentRegionIndex[1] = segmentRegionIndex[2] = 0;
    segmentRegionIndex[3] = segmentIndex;
    BinaryLabelmap4DImageType::SizeType segmentRegionSize;
    segmentRegionSize = regionSize;
    segmentRegionSize[3] = 1;
    BinaryLabelmap4DImageType::RegionType segmentRegion;
    segmentRegion.SetIndex(segmentRegionIndex);
    segmentRegion.SetSize(segmentRegionSize);

    // Get scalar pointer for binary labelmap representation. Only a few scalar types are supported
    int currentLabelScalarType = currentBinaryLabelmap->GetScalarType();
    if ( currentLabelScalarType != VTK_UNSIGNED_CHAR
      && currentLabelScalarType != VTK_UNSIGNED_SHORT
      && currentLabelScalarType != VTK_SHORT )
    {
      vtkWarningMacro("WriteBinaryLabelmapRepresentation: Segment " << currentSegmentID << " cannot be written! Binary labelmap scalar type must be unsigned char, unsighed short, or short!");
      continue;
    }
    void* voidScalarPointer = currentBinaryLabelmap->GetScalarPointer();
    unsigned char* labelmapPtrUChar = (unsigned char*)voidScalarPointer;
    unsigned short* labelmapPtrUShort = (unsigned short*)voidScalarPointer;
    short* labelmapPtrShort = (short*)voidScalarPointer;

    // Iterate through current segment labelmap and write voxel values
    BinaryLabelmap4DIteratorType segmentLabelmapIterator(itkLabelmapImage, segmentRegion);
    for (segmentLabelmapIterator.GoToBegin(); !segmentLabelmapIterator.IsAtEnd(); ++segmentLabelmapIterator)
    {
      // Skip region outside extent of current segment (consider common extent boundaries)
      BinaryLabelmap4DImageType::IndexType segmentIndex = segmentLabelmapIterator.GetIndex();
      if ( segmentIndex[0] + commonGeometryExtent[0] < currentSegmentExtent[0]
        || segmentIndex[0] + commonGeometryExtent[0] > currentSegmentExtent[1]
        || segmentIndex[1] + commonGeometryExtent[2] < currentSegmentExtent[2]
        || segmentIndex[1] + commonGeometryExtent[2] > currentSegmentExtent[3]
        || segmentIndex[2] + commonGeometryExtent[4] < currentSegmentExtent[4]
        || segmentIndex[2] + commonGeometryExtent[4] > currentSegmentExtent[5] )
      {
        segmentLabelmapIterator.Set((unsigned char)0);
        continue;
      }

      // Get labelmap value at voxel
      unsigned short label = 0;
      if (currentLabelScalarType == VTK_UNSIGNED_CHAR)
      {
        label = (*labelmapPtrUChar);
      }
      else if (currentLabelScalarType == VTK_UNSIGNED_SHORT)
      {
        label = (*labelmapPtrUShort);
      }
      else if (currentLabelScalarType == VTK_SHORT)
      {
        label = (*labelmapPtrShort);
      }

      // Write voxel value to ITK image
      segmentLabelmapIterator.Set((unsigned char)label);

      ++labelmapPtrUChar;
      ++labelmapPtrUShort;
      ++labelmapPtrShort;
    }
  } // For each segment

  // Set metadata to ITK image
  itkLabelmapImage->SetMetaDataDictionary(metadata);

  // Write image file to disk
  itk::NrrdImageIO::Pointer io = itk::NrrdImageIO::New();
  io->SetFileType(itk::ImageIOBase::Binary);

  typedef itk::ImageFileWriter<BinaryLabelmap4DImageType> WriterType;
  WriterType::Pointer nrrdWriter = WriterType::New();
  nrrdWriter->UseInputMetaDataDictionaryOn();
  nrrdWriter->SetInput(itkLabelmapImage);
  nrrdWriter->SetImageIO(io);
  nrrdWriter->SetFileName(fullName);
  nrrdWriter->SetUseCompression(this->UseCompression);
  try
  {
    nrrdWriter->Update();
  }
  catch (itk::ExceptionObject e)
  {
    vtkErrorMacro("Failed to write segmentation to file " << fullName);
    return 0;
  }

  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLSegmentationStorageNode::WritePolyDataRepresentation(vtkSegmentation* segmentation, std::string path)
{
  if (!segmentation || segmentation->GetNumberOfSegments() == 0)
  {
    vtkErrorMacro("WritePolyDataRepresentation: Invalid segmentation to write to disk");
    return 0;
  }

  // Get and check master representation
  const char* masterRepresentation = segmentation->GetMasterRepresentationName();
  if ( !masterRepresentation
    || ( strcmp(masterRepresentation, vtkSegmentationConverter::GetSegmentationClosedSurfaceRepresentationName())
      && strcmp(masterRepresentation, vtkSegmentationConverter::GetSegmentationPlanarContourRepresentationName()) ) )
  {
    vtkErrorMacro("WritePolyDataRepresentation: Invalid master representation to write as poly data");
    return 0;
  }

  // Initialize dataset to write
  vtkSmartPointer<vtkMultiBlockDataSet> multiBlockDataset = vtkSmartPointer<vtkMultiBlockDataSet>::New();
  multiBlockDataset->SetNumberOfBlocks(segmentation->GetNumberOfSegments());

  // Add segment poly datas to dataset
  vtkSegmentation::SegmentMap segmentMap = segmentation->GetSegments();
  unsigned int segmentIndex = 0;
  for (vtkSegmentation::SegmentMap::iterator segmentIt = segmentMap.begin(); segmentIt != segmentMap.end(); ++segmentIt, ++segmentIndex)
  {
    std::string currentSegmentID = segmentIt->first;
    vtkSegment* currentSegment = segmentIt->second.GetPointer();

    // Get master representation from segment
    vtkPolyData* currentPolyData = vtkPolyData::SafeDownCast(currentSegment->GetRepresentation(masterRepresentation));
    if (!currentPolyData)
    {
      vtkErrorMacro("WritePolyDataRepresentation: Failed to retrieve master representation from segment " << currentSegmentID);
      continue;
    }
    // Make temporary duplicate of the poly data so that adding the metadata does not cause invalidating the other
    // representations (which is done when the master representation is modified)
    vtkSmartPointer<vtkPolyData> currentPolyDataCopy = vtkSmartPointer<vtkPolyData>::New();
    currentPolyDataCopy->ShallowCopy(currentPolyData);

    // Set metadata for current segment
    vtkSmartPointer<vtkStringArray> masterRepresentationArray = vtkSmartPointer<vtkStringArray>::New();
    masterRepresentationArray->SetNumberOfValues(1);
    masterRepresentationArray->SetValue(0,masterRepresentation);
    masterRepresentationArray->SetName(MASTER_REPRESENTATION.c_str());
    currentPolyDataCopy->GetFieldData()->AddArray(masterRepresentationArray);

    vtkSmartPointer<vtkStringArray> idArray = vtkSmartPointer<vtkStringArray>::New();
    idArray->SetNumberOfValues(1);
    idArray->SetValue(0,currentSegmentID.c_str());
    idArray->SetName(SEGMENT_ID.c_str());
    currentPolyDataCopy->GetFieldData()->AddArray(idArray);

    vtkSmartPointer<vtkStringArray> nameArray = vtkSmartPointer<vtkStringArray>::New();
    nameArray->SetNumberOfValues(1);
    nameArray->SetValue(0,currentSegment->GetName());
    nameArray->SetName(SEGMENT_NAME.c_str());
    currentPolyDataCopy->GetFieldData()->AddArray(nameArray);

    vtkSmartPointer<vtkDoubleArray> defaultColorArray = vtkSmartPointer<vtkDoubleArray>::New();
    defaultColorArray->SetNumberOfComponents(3);
    defaultColorArray->SetNumberOfTuples(1);
    defaultColorArray->SetTuple(0, currentSegment->GetDefaultColor());
    defaultColorArray->SetName(SEGMENT_DEFAULT_COLOR.c_str());
    currentPolyDataCopy->GetFieldData()->AddArray(defaultColorArray);

    //TODO: Store tags with key SEGMENT_TAGS

    // Save conversion parameters as metadata (save in each segment file)
    std::string conversionParameters = segmentation->SerializeAllConversionParameters();
    vtkSmartPointer<vtkStringArray> conversionParametersArray = vtkSmartPointer<vtkStringArray>::New();
    conversionParametersArray->SetNumberOfValues(1);
    conversionParametersArray->SetValue(0,conversionParameters);
    conversionParametersArray->SetName(CONVERSION_PARAMETERS.c_str());
    currentPolyDataCopy->GetFieldData()->AddArray(conversionParametersArray);

    // Save contained representation names as metadata (save in each segment file)
    std::string containedRepresentationNames = this->SerializeContainedRepresentationNames(segmentation);
    vtkSmartPointer<vtkStringArray> containedRepresentationNamesArray = vtkSmartPointer<vtkStringArray>::New();
    containedRepresentationNamesArray->SetNumberOfValues(1);
    containedRepresentationNamesArray->SetValue(0,containedRepresentationNames);
    containedRepresentationNamesArray->SetName(CONTAINED_REPRESENTATION_NAMES.c_str());
    currentPolyDataCopy->GetFieldData()->AddArray(containedRepresentationNamesArray);

    // Set segment poly data to dataset
    multiBlockDataset->SetBlock(segmentIndex, currentPolyDataCopy);
  }

  // Write multiblock dataset to disk
  vtkSmartPointer<vtkXMLMultiBlockDataWriter> writer = vtkSmartPointer<vtkXMLMultiBlockDataWriter>::New();
  writer->SetInputData(multiBlockDataset);
  writer->SetFileName(path.c_str());
  if (this->UseCompression)
  {
    writer->SetDataModeToBinary();
    writer->SetCompressorTypeToZLib();
  }
  else
  {
    writer->SetDataModeToAscii();
    writer->SetCompressorTypeToNone();
  }
  writer->Write();

  // Add all files to storage node (multiblock dataset writes segments to individual files in a separate folder)
  this->AddPolyDataFileNames(path, segmentation);

  return 1;
}

//----------------------------------------------------------------------------
void vtkMRMLSegmentationStorageNode::AddPolyDataFileNames(std::string path, vtkSegmentation* segmentation)
{
  if (!segmentation)
  {
    vtkErrorMacro("AddPolyDataFileNames: Invalid segmentation!");
    return;
  }

  this->AddFileName(path.c_str());

  std::string fileNameWithoutExtension = vtksys::SystemTools::GetFilenameWithoutLastExtension(path);
  std::string parentDirectory = vtksys::SystemTools::GetParentDirectory(path);
  std::string multiBlockDirectory = parentDirectory + "/" + fileNameWithoutExtension;
  for (int segmentIndex = 0; segmentIndex < segmentation->GetNumberOfSegments(); ++segmentIndex)
  {
    std::stringstream ssSegmentFilePath;
    ssSegmentFilePath << multiBlockDirectory << "/" << fileNameWithoutExtension << "_" << segmentIndex << ".vtp";
    std::string segmentFilePath = ssSegmentFilePath.str();
    this->AddFileName(segmentFilePath.c_str());
  }
}

//----------------------------------------------------------------------------
std::string vtkMRMLSegmentationStorageNode::SerializeContainedRepresentationNames(vtkSegmentation* segmentation)
{
  if (!segmentation || segmentation->GetNumberOfSegments() == 0)
  {
    vtkErrorMacro("SerializeContainedRepresentationNames: Invalid segmentation!");
    return "";
  }

  std::stringstream ssRepresentationNames;
  std::vector<std::string> containedRepresentationNames;
  segmentation->GetContainedRepresentationNames(containedRepresentationNames);
  for (std::vector<std::string>::iterator reprIt = containedRepresentationNames.begin();
    reprIt != containedRepresentationNames.end(); ++reprIt)
  {
    ssRepresentationNames << (*reprIt) << SERIALIZATION_SEPARATOR;
  }

  return ssRepresentationNames.str();
}

//----------------------------------------------------------------------------
void vtkMRMLSegmentationStorageNode::CreateRepresentationsBySerializedNames(vtkSegmentation* segmentation, std::string representationNames)
{
  if (!segmentation || segmentation->GetNumberOfSegments() == 0 || !segmentation->GetMasterRepresentationName())
  {
    vtkErrorMacro("CreateRepresentationsBySerializedNames: Invalid segmentation!");
    return;
  }
  if (representationNames.empty())
  {
    vtkWarningMacro("CreateRepresentationsBySerializedNames: Empty representation names list, nothing to create");
    return;
  }

  std::string masterRepresentation(segmentation->GetMasterRepresentationName());
  size_t separatorPosition = representationNames.find(SERIALIZATION_SEPARATOR);
  while (separatorPosition != std::string::npos)
  {
    std::string representationName = representationNames.substr(0, separatorPosition);

    // Only create non-master representations
    if (representationName.compare(masterRepresentation))
    {
      segmentation->CreateRepresentation(representationName);
    }

    representationNames = representationNames.substr(separatorPosition+1);
    separatorPosition = representationNames.find(SERIALIZATION_SEPARATOR);
  }

}
