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

  This file was originally developed by Csaba Pinter, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

// vtkMRMLDoseVolumeHistogram includes
#include "vtkMRMLDoseVolumeHistogramNode.h"

// SlicerRT includes
//#include "vtkSlicerDoseVolumeHistogramModuleLogicExport.h"

// Segmentations includes
#include "vtkMRMLSegmentationNode.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLChartNode.h>
#include <vtkMRMLDoubleArrayNode.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// STD includes
#include <sstream>

//------------------------------------------------------------------------------
static const char* DOSE_VOLUME_REFERENCE_ROLE = "doseVolumeRef";
static const char* SEGMENTATION_REFERENCE_ROLE = "segmentationRef";
static const char* CHART_REFERENCE_ROLE = "chartRef";
static const char* DVH_DOUBLE_ARRAY_REFERENCE_ROLE = "dvhDoubleArrayRef";

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLDoseVolumeHistogramNode);

//----------------------------------------------------------------------------
vtkMRMLDoseVolumeHistogramNode::vtkMRMLDoseVolumeHistogramNode()
{
  this->SelectedSegmentIDs.clear();
  this->ShowHideAll = 0;
  this->ShowInChartCheckStates.clear();
  this->VDoseValues = NULL;
  this->ShowVMetricsCc = false;
  this->ShowVMetricsPercent = false;
  this->DVolumeValuesCc = NULL;
  this->DVolumeValuesPercent = NULL;
  this->ShowDMetrics = false;
  this->ShowDoseVolumesOnly = true;
  this->AutomaticOversampling = false;
  this->AutomaticOversamplingFactors.clear();

  this->HideFromEditors = false;
}

//----------------------------------------------------------------------------
vtkMRMLDoseVolumeHistogramNode::~vtkMRMLDoseVolumeHistogramNode()
{
  this->SelectedSegmentIDs.clear();
  this->ShowInChartCheckStates.clear();
  this->SetVDoseValues(NULL);
  this->SetDVolumeValuesCc(NULL);
  this->SetDVolumeValuesPercent(NULL);
  this->AutomaticOversamplingFactors.clear();
}

//----------------------------------------------------------------------------
void vtkMRMLDoseVolumeHistogramNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  // Write all MRML node attributes into output stream
  vtkIndent indent(nIndent);

  of << indent << " SelectedSegmentIDs=\"";
  for (std::vector<std::string>::iterator it = this->SelectedSegmentIDs.begin(); it != this->SelectedSegmentIDs.end(); ++it)
    {
    of << (*it) << "|";
    }
  of << "\"";

  {
    std::stringstream ss;
    ss << this->ShowHideAll;
    of << indent << " ShowHideAll=\"" << ss.str() << "\"";
  }

  of << indent << " ShowInChartCheckStates=\"";
  for (std::vector<bool>::iterator it = this->ShowInChartCheckStates.begin(); it != this->ShowInChartCheckStates.end(); ++it)
    {
    of << ((*it) ? "true" : "false") << "|";
    }
  of << "\"";

  {
    std::stringstream ss;
    if ( this->VDoseValues )
      {
      ss << this->VDoseValues;
      of << indent << " VDoseValues=\"" << ss.str() << "\"";
      }
  }

  of << indent << " ShowVMetricsCc=\"" << (this->ShowVMetricsCc ? "true" : "false") << "\"";

  of << indent << " ShowVMetricsPercent=\"" << (this->ShowVMetricsPercent ? "true" : "false") << "\"";

  {
    std::stringstream ss;
    if ( this->DVolumeValuesCc )
      {
      ss << this->DVolumeValuesCc;
      of << indent << " DVolumeValuesCc=\"" << ss.str() << "\"";
      }
  }

  {
    std::stringstream ss;
    if ( this->DVolumeValuesPercent )
      {
      ss << this->DVolumeValuesPercent;
      of << indent << " DVolumeValuesPercent=\"" << ss.str() << "\"";
      }
  }

  of << indent << " ShowDMetrics=\"" << (this->ShowDMetrics ? "true" : "false") << "\"";

  of << indent << " ShowDoseVolumesOnly=\"" << (this->ShowDoseVolumesOnly ? "true" : "false") << "\"";

  of << indent << " AutomaticOversampling=\"" << (this->AutomaticOversampling ? "true" : "false") << "\"";
}

//----------------------------------------------------------------------------
void vtkMRMLDoseVolumeHistogramNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;

  while (*atts != NULL) 
    {
    attName = *(atts++);
    attValue = *(atts++);

    if (!strcmp(attName, "SelectedSegmentIDs")) 
      {
      std::stringstream ss;
      ss << attValue;
      std::string valueStr = ss.str();
      std::string separatorCharacter("|");

      this->SelectedSegmentIDs.clear();
      size_t separatorPosition = valueStr.find( separatorCharacter );
      while (separatorPosition != std::string::npos)
        {
        this->SelectedSegmentIDs.push_back(valueStr.substr(0, separatorPosition));
        valueStr = valueStr.substr( separatorPosition+1 );
        separatorPosition = valueStr.find( separatorCharacter );
        }
      if (!valueStr.empty())
        {
        this->SelectedSegmentIDs.push_back(valueStr);
        }
      }
    else if (!strcmp(attName, "ShowHideAll")) 
      {
      std::stringstream ss;
      ss << attValue;
      int intAttValue;
      ss >> intAttValue;
      this->ShowHideAll = intAttValue;
      }
    else if (!strcmp(attName, "ShowInChartCheckStates")) 
      {
      std::stringstream ss;
      ss << attValue;
      std::string valueStr = ss.str();
      std::string separatorCharacter("|");

      this->ShowInChartCheckStates.clear();
      size_t separatorPosition = valueStr.find( separatorCharacter );
      while (separatorPosition != std::string::npos)
        {
        this->ShowInChartCheckStates.push_back(
          (valueStr.substr(0, separatorPosition).compare("true") ? false : true));
        valueStr = valueStr.substr( separatorPosition+1 );
        separatorPosition = valueStr.find( separatorCharacter );
        }
      if (!valueStr.empty())
        {
        this->ShowInChartCheckStates.push_back(
          (valueStr.compare("true") ? false : true));
        }
      }
    else if (!strcmp(attName, "VDoseValues")) 
      {
      std::stringstream ss;
      ss << attValue;
      this->SetVDoseValues(ss.str().c_str());
      }
    else if (!strcmp(attName, "ShowVMetricsCc")) 
      {
      this->ShowVMetricsCc = 
        (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "ShowVMetricsPercent")) 
      {
      this->ShowVMetricsPercent = 
        (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "DVolumeValuesCc")) 
      {
      std::stringstream ss;
      ss << attValue;
      this->SetDVolumeValuesCc(ss.str().c_str());
      }
    else if (!strcmp(attName, "DVolumeValuesPercent")) 
      {
      std::stringstream ss;
      ss << attValue;
      this->SetDVolumeValuesPercent(ss.str().c_str());
      }
    else if (!strcmp(attName, "ShowDMetrics")) 
      {
      this->ShowDMetrics = (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "ShowDoseVolumesOnly")) 
      {
      this->ShowDoseVolumesOnly = (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "AutomaticOversampling")) 
      {
      this->AutomaticOversampling = (strcmp(attValue,"true") ? false : true);
      }
    }
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLDoseVolumeHistogramNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  this->DisableModifiedEventOn();

  vtkMRMLDoseVolumeHistogramNode *node = (vtkMRMLDoseVolumeHistogramNode *) anode;

  this->SelectedSegmentIDs = node->SelectedSegmentIDs;
  this->ShowHideAll = node->ShowHideAll;
  this->ShowInChartCheckStates = node->ShowInChartCheckStates;

  this->SetVDoseValues(node->VDoseValues);
  this->ShowVMetricsCc = node->ShowVMetricsCc;
  this->ShowVMetricsPercent = node->ShowVMetricsPercent;

  this->SetDVolumeValuesCc(node->DVolumeValuesCc);
  this->SetDVolumeValuesPercent(node->DVolumeValuesPercent);
  this->ShowDMetrics = node->ShowDMetrics;
  this->ShowDoseVolumesOnly = node->ShowDoseVolumesOnly;
  this->AutomaticOversampling = node->AutomaticOversampling;

  this->DisableModifiedEventOff();
  this->InvokePendingModifiedEvent();
}

//----------------------------------------------------------------------------
void vtkMRMLDoseVolumeHistogramNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  os << indent << " SelectedSegmentIDs:   ";
  for (std::vector<std::string>::iterator it = this->SelectedSegmentIDs.begin(); it != this->SelectedSegmentIDs.end(); ++it)
    {
    os << indent << (*it) << "|";
    }
  os << "\n";

  os << indent << "ShowHideAll:   " << this->ShowHideAll << "\n";

  {
    os << indent << "ShowInChartCheckStates:   ";
    for (std::vector<bool>::iterator it = this->ShowInChartCheckStates.begin(); it != this->ShowInChartCheckStates.end(); ++it)
      {
      os << indent << ((*it) ? "true" : "false") << "|";
      }
    os << "\n";
  }

  os << indent << "VDoseValues:   " << this->VDoseValues << "\n";
  os << indent << "ShowVMetricsCc:   " << (this->ShowVMetricsCc ? "true" : "false") << "\n";
  os << indent << "ShowVMetricsPercent:   " << (this->ShowVMetricsPercent ? "true" : "false") << "\n";

  os << indent << "DVolumeValuesCc:   " << this->DVolumeValuesCc << "\n";
  os << indent << "DVolumeValuesPercent:   " << this->DVolumeValuesPercent << "\n";
  os << indent << "ShowDMetrics:   " << (this->ShowDMetrics ? "true" : "false") << "\n";
  os << indent << "ShowDoseVolumesOnly:   " << (this->ShowDoseVolumesOnly ? "true" : "false") << "\n";
  os << indent << "AutomaticOversampling:   " << (this->AutomaticOversampling ? "true" : "false") << "\n";
}

//----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkMRMLDoseVolumeHistogramNode::GetDoseVolumeNode()
{
  return vtkMRMLScalarVolumeNode::SafeDownCast( this->GetNodeReference(DOSE_VOLUME_REFERENCE_ROLE) );
}

//----------------------------------------------------------------------------
void vtkMRMLDoseVolumeHistogramNode::SetAndObserveDoseVolumeNode(vtkMRMLScalarVolumeNode* node)
{
  this->SetNodeReferenceID(DOSE_VOLUME_REFERENCE_ROLE, (node ? node->GetID() : NULL));
}

//----------------------------------------------------------------------------
vtkMRMLSegmentationNode* vtkMRMLDoseVolumeHistogramNode::GetSegmentationNode()
{
  return vtkMRMLSegmentationNode::SafeDownCast( this->GetNodeReference(SEGMENTATION_REFERENCE_ROLE) );
}

//----------------------------------------------------------------------------
void vtkMRMLDoseVolumeHistogramNode::SetAndObserveSegmentationNode(vtkMRMLSegmentationNode* node)
{
  this->SetNodeReferenceID(SEGMENTATION_REFERENCE_ROLE, (node ? node->GetID() : NULL));
}

//----------------------------------------------------------------------------
vtkMRMLChartNode* vtkMRMLDoseVolumeHistogramNode::GetChartNode()
{
  return vtkMRMLChartNode::SafeDownCast( this->GetNodeReference(CHART_REFERENCE_ROLE) );
}

//----------------------------------------------------------------------------
void vtkMRMLDoseVolumeHistogramNode::SetAndObserveChartNode(vtkMRMLChartNode* node)
{
  this->SetNodeReferenceID(CHART_REFERENCE_ROLE, (node ? node->GetID() : NULL));
}

//----------------------------------------------------------------------------
void vtkMRMLDoseVolumeHistogramNode::GetDvhDoubleArrayNodes(std::vector<vtkMRMLNode*> &nodes)
{
  // Disable modify event for this operation as it triggers unwanted calls of
  // qSlicerDoseVolumeHistogramModuleWidget::refreshDvhTable thus causing crash
  this->DisableModifiedEventOn();
  this->GetNodeReferences(DVH_DOUBLE_ARRAY_REFERENCE_ROLE, nodes);
  this->DisableModifiedEventOff();
}

//----------------------------------------------------------------------------
void vtkMRMLDoseVolumeHistogramNode::AddDvhDoubleArrayNode(vtkMRMLDoubleArrayNode* node)
{
  if (!node)
  {
    return;
  }
  this->AddNodeReferenceID(DVH_DOUBLE_ARRAY_REFERENCE_ROLE, node->GetID());
}

//----------------------------------------------------------------------------
void vtkMRMLDoseVolumeHistogramNode::RemoveAllDvhDoubleArrayNodes()
{
  this->RemoveNodeReferenceIDs(DVH_DOUBLE_ARRAY_REFERENCE_ROLE);
}
