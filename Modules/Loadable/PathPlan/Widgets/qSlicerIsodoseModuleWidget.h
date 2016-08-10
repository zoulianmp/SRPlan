/*==============================================================================

  Copyright (c) Radiation Medicine Program, University Health Network,
  Princess Margaret Hospital, Toronto, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Kevin Wang, Princess Margaret Cancer Centre 
  and was supported by Cancer Care Ontario (CCO)'s ACRU program 
  with funds provided by the Ontario Ministry of Health and Long-Term Care
  and Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO).

==============================================================================*/

#ifndef __qSlicerIsodoseModuleWidget_h
#define __qSlicerIsodoseModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerIsodoseModuleExport.h"

class qSlicerIsodoseModuleWidgetPrivate;
class vtkMRMLNode;
class QTableWidgetItem;

/// \ingroup SlicerRt_QtModules_Isodose
class Q_SLICER_QTMODULES_ISODOSE_EXPORT qSlicerIsodoseModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerIsodoseModuleWidget(QWidget *parent=0);
  virtual ~qSlicerIsodoseModuleWidget();

  virtual void enter();

public slots:
  /// Set the current MRML scene to the widget
  virtual void setMRMLScene(vtkMRMLScene*);

  /// Process loaded scene
  void onSceneImportedEvent();

  /// Set current parameter node
  void setIsodoseNode(vtkMRMLNode *node);

  /// Update widget GUI from parameter node
  void updateWidgetFromMRML();

  /// Set number of levels
  void setNumberOfLevels(int newNumber);

protected slots:
  /// Slot handling change of dose volume node
  void doseVolumeNodeChanged(vtkMRMLNode*);

  /// Slot handling change of show dose only checkbox
  void showDoseVolumesOnlyCheckboxChanged(int);

  /// Slot for changing isoline visibility
  void setIsolineVisibility(bool);

  /// Slot for changing isosurface visibility
  void setIsosurfaceVisibility(bool);

  /// Slot for changing 3D scalar bar visibility
  void setScalarBarVisibility(bool);

  /// Slot for changing 2D scalar bar visibility
  void setScalarBar2DVisibility(bool);

  /// Slot handling clicking the Apply button
  void applyClicked();

  /// Slot called on change in logic
  void onLogicModified();

protected:
  // Generates a new isodose level name
  QString generateNewIsodoseLevel() const;

  /// Updates button states
  void updateButtonsState();

protected:
  QScopedPointer<qSlicerIsodoseModuleWidgetPrivate> d_ptr;
  
  virtual void setup();
  void onEnter();

private:
  Q_DECLARE_PRIVATE(qSlicerIsodoseModuleWidget);
  Q_DISABLE_COPY(qSlicerIsodoseModuleWidget);
};

#endif
