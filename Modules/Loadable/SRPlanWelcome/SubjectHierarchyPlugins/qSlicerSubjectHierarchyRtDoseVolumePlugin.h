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

#ifndef __qSlicerSubjectHierarchyRtDoseVolumePlugin_h
#define __qSlicerSubjectHierarchyRtDoseVolumePlugin_h

// SlicerRt includes
#include "qSlicerSubjectHierarchyAbstractPlugin.h"

//#include "qSlicerDicomRtImportExportSubjectHierarchyPluginsExport.h"

#include "qSRPlanWelcomeSubjectHierarchyPluginsExport.h"

class qSlicerSubjectHierarchyRtDoseVolumePluginPrivate;
class vtkMRMLNode;
class vtkMRMLSubjectHierarchyNode;

/// \ingroup SlicerRt_QtModules_DicomRtImport
class Q_SRPlan_WELCOME_SUBJECT_HIERARCHY_PLUGINS_EXPORT qSlicerSubjectHierarchyRtDoseVolumePlugin : public qSlicerSubjectHierarchyAbstractPlugin
{
public:
  Q_OBJECT

public:
  typedef qSlicerSubjectHierarchyAbstractPlugin Superclass;
  qSlicerSubjectHierarchyRtDoseVolumePlugin(QObject* parent = NULL);
  virtual ~qSlicerSubjectHierarchyRtDoseVolumePlugin();

public:
  /// Determines if the actual plugin can handle a subject hierarchy node. The plugin with
  /// the highest confidence number will "own" the node in the subject hierarchy (set icon, tooltip,
  /// set context menu etc.)
  /// \param node Note to handle in the subject hierarchy tree
  /// \return Floating point confidence number between 0 and 1, where 0 means that the plugin cannot handle the
  ///   node, and 1 means that the plugin is the only one that can handle the node (by node type or identifier attribute)
  virtual double canOwnSubjectHierarchyNode(vtkMRMLSubjectHierarchyNode* node)const;

  /// Get role that the plugin assigns to the subject hierarchy node.
  ///   Each plugin should provide only one role.
  Q_INVOKABLE virtual const QString roleForPlugin()const;

  /// Get icon of an owned subject hierarchy node
  /// \return Icon to set, NULL if nothing to set
  virtual QIcon icon(vtkMRMLSubjectHierarchyNode* node);

  /// Get visibility icon for a visibility state
  virtual QIcon visibilityIcon(int visible);

  /// Open module belonging to node and set inputs in opened module
  virtual void editProperties(vtkMRMLSubjectHierarchyNode* node);

  /// Generate tooltip for a owned subject hierarchy node
  virtual QString tooltip(vtkMRMLSubjectHierarchyNode* node)const;

  /// Set display visibility of a owned subject hierarchy node
  virtual void setDisplayVisibility(vtkMRMLSubjectHierarchyNode* node, int visible);

  /// Get display visibility of a owned subject hierarchy node
  /// \return Display visibility (0: hidden, 1: shown, 2: partially shown)
  virtual int getDisplayVisibility(vtkMRMLSubjectHierarchyNode* node)const;

  /// Get node context menu item actions to add to tree view
  Q_INVOKABLE virtual QList<QAction*> nodeContextMenuActions()const;

  /// Show context menu actions valid for  given subject hierarchy node.
  /// \param node Subject Hierarchy node to show the context menu items for. If NULL, then shows menu items for the scene
  virtual void showContextMenuActionsForNode(vtkMRMLSubjectHierarchyNode* node);

protected slots:
  /// Convert currently selected volume node to RT dose volume. Set dose unit name and value in a pop-up dialog
  void convertCurrentNodeToRtDoseVolume();

protected:
  QScopedPointer<qSlicerSubjectHierarchyRtDoseVolumePluginPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerSubjectHierarchyRtDoseVolumePlugin);
  Q_DISABLE_COPY(qSlicerSubjectHierarchyRtDoseVolumePlugin);
};

#endif
