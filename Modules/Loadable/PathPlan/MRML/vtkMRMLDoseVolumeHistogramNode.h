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

#ifndef __vtkMRMLDoseVolumeHistogramNode_h
#define __vtkMRMLDoseVolumeHistogramNode_h

// MRML includes
#include <vtkMRML.h>
#include <vtkMRMLNode.h>

// STD includes
#include <vector>
#include <map>

#include "vtkSRPlanPathPlanModuleMRMLExport.h"

class vtkMRMLScalarVolumeNode;
class vtkMRMLChartNode;
class vtkMRMLDoubleArrayNode;
class vtkMRMLSegmentationNode;

/// \ingroup SlicerRt_QtModules_DoseVolumeHistogram
class VTK_SRPlan_PATHPLAN_MODULE_MRML_EXPORT vtkMRMLDoseVolumeHistogramNode : public vtkMRMLNode
{
public:
  static vtkMRMLDoseVolumeHistogramNode *New();
  vtkTypeMacro(vtkMRMLDoseVolumeHistogramNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  /// Create instance of a GAD node. 
  virtual vtkMRMLNode* CreateNodeInstance();

  /// Set node attributes from name/value pairs 
  virtual void ReadXMLAttributes( const char** atts);

  /// Write this node's information to a MRML file in XML format. 
  virtual void WriteXML(ostream& of, int indent);

  /// Copy the node's attributes to this object 
  virtual void Copy(vtkMRMLNode *node);

  /// Get unique node XML tag name (like Volume, Model) 
  virtual const char* GetNodeTagName() {return "DoseVolumeHistogram";};

public:
  /// Get dose volume node
  vtkMRMLScalarVolumeNode* GetDoseVolumeNode();
  /// Set and observe dose volume node
  void SetAndObserveDoseVolumeNode(vtkMRMLScalarVolumeNode* node);

  /// Get segmentation node
  vtkMRMLSegmentationNode* GetSegmentationNode();
  /// Set and observe segmentation node
  void SetAndObserveSegmentationNode(vtkMRMLSegmentationNode* node);

  /// Get chart node
  vtkMRMLChartNode* GetChartNode();
  /// Set and observe chart node
  void SetAndObserveChartNode(vtkMRMLChartNode* node);

  /// Get list of all the DVH double array node IDs in the scene
  void GetDvhDoubleArrayNodes(std::vector<vtkMRMLNode*> &nodes);
  /// Add DVH double array node reference
  void AddDvhDoubleArrayNode(vtkMRMLDoubleArrayNode* node);
  /// Remove all DVH double array node references
  void RemoveAllDvhDoubleArrayNodes();

  /// Get selected segment IDs
  void GetSelectedSegmentIDs(std::vector<std::string> &selectedSegmentIDs)
  {
    selectedSegmentIDs = this->SelectedSegmentIDs;
  }
  /// Set selected segment IDs
  void SetSelectedSegmentIDs(std::vector<std::string> selectedSegmentIDs)
  {
    this->SelectedSegmentIDs = selectedSegmentIDs;
  }

  /// Get show in chart check states
  void GetShowInChartCheckStates(std::vector<bool> &checkboxStates)
  {
    checkboxStates = this->ShowInChartCheckStates;
  }
  /// Set show in chart check states
  void SetShowInChartCheckStates(std::vector<bool> checkboxStates)
  {
    this->ShowInChartCheckStates = checkboxStates;
  }

  /// Clear automatic oversampling factors map
  void ClearAutomaticOversamplingFactors()
  {
    this->AutomaticOversamplingFactors.clear();
  }
  /// Add automatic oversampling factor to map
  void AddAutomaticOversamplingFactor(std::string segmentID, double factor)
  {
    this->AutomaticOversamplingFactors[segmentID] = factor;
  }
  void GetAutomaticOversamplingFactors(std::map<std::string, double> &factors)
  {
    factors = this->AutomaticOversamplingFactors;
  }

  /// Get/Set Show/Hide all checkbox state
  vtkGetMacro(ShowHideAll, int);
  vtkSetMacro(ShowHideAll, int);

  /// Get/Set input dose values for V metrics
  vtkGetStringMacro(VDoseValues);
  vtkSetStringMacro(VDoseValues);

  /// Get/Set show Cc for V metrics checkbox state
  vtkGetMacro(ShowVMetricsCc, bool);
  vtkSetMacro(ShowVMetricsCc, bool);
  vtkBooleanMacro(ShowVMetricsCc, bool);

  /// Get/Set show % for V metrics checkbox state
  vtkGetMacro(ShowVMetricsPercent, bool);
  vtkSetMacro(ShowVMetricsPercent, bool);
  vtkBooleanMacro(ShowVMetricsPercent, bool);

  /// Get/Set input volume cc values for D metrics
  vtkGetStringMacro(DVolumeValuesCc);
  vtkSetStringMacro(DVolumeValuesCc);

  /// Get/Set input volume % values for D metrics
  vtkGetStringMacro(DVolumeValuesPercent);
  vtkSetStringMacro(DVolumeValuesPercent);

  /// Get/Set show Gy for D metrics checkbox state
  vtkGetMacro(ShowDMetrics, bool);
  vtkSetMacro(ShowDMetrics, bool);
  vtkBooleanMacro(ShowDMetrics, bool);

  /// Get/Set show dose volumes only checkbox state
  vtkGetMacro(ShowDoseVolumesOnly, bool);
  vtkSetMacro(ShowDoseVolumesOnly, bool);
  vtkBooleanMacro(ShowDoseVolumesOnly, bool);

  /// Get/Set automatic oversampling flag
  vtkGetMacro(AutomaticOversampling, bool);
  vtkSetMacro(AutomaticOversampling, bool);
  vtkBooleanMacro(AutomaticOversampling, bool);

protected:
  vtkMRMLDoseVolumeHistogramNode();
  ~vtkMRMLDoseVolumeHistogramNode();
  vtkMRMLDoseVolumeHistogramNode(const vtkMRMLDoseVolumeHistogramNode&);
  void operator=(const vtkMRMLDoseVolumeHistogramNode&);

protected:
  /// List of segment IDs selected in the chosen segmentation. If empty, then all segments are considered selected
  std::vector<std::string> SelectedSegmentIDs;

  /// State of Show/Hide all checkbox
  int ShowHideAll;

  /// Vector of checkbox states for the case the user makes the show/hide all checkbox state
  /// partially checked. Then the last configuration is restored. The flags correspond to the
  /// referenced DVH double array node with the same index.
  std::vector<bool> ShowInChartCheckStates;

  /// Input dose values for V metrics
  char* VDoseValues;

  /// State of Show Cc for V metrics checkbox
  bool ShowVMetricsCc;

  /// State of Show % for V metrics checkbox
  bool ShowVMetricsPercent;

  /// Input volume cc values for D metrics
  char* DVolumeValuesCc;

  /// Input volume % values for D metrics
  char* DVolumeValuesPercent;

  /// State of Show Gy for D metrics checkbox
  bool ShowDMetrics;

  /// State of Show dose volumes only checkbox
  bool ShowDoseVolumesOnly;

  /// Flag determining whether automatic oversampling is used
  /// If on, then oversampling is automatically determined based on the segments and dose volume, and used
  /// for both dose and segmentation when computing DVH.
  bool AutomaticOversampling;

  /// Automatic oversampling factors stored for each selected segment.
  /// If oversampling is automatic then they need to be stored for reporting purposes.
  /// This property is not saved to the scene, as these are temporary values.
  std::map<std::string, double> AutomaticOversamplingFactors;
};

#endif
