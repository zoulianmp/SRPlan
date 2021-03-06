/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qSlicerWelcomeModule_h
#define __qSlicerWelcomeModule_h

// CTK includes
#include <ctkPimpl.h>

// SlicerQt includes
#include "qSlicerLoadableModule.h"
#include "qSRPlanWelcomeModuleExport.h"

class qSlicerAbstractModuleWidget;
class qSlicerWelcomeModulePrivate;



/// \ingroup Slicer_QtModules_SlicerWelcome
class Q_SRPlan_QTMODULES_WELCOME_EXPORT qSlicerWelcomeModule :
  public qSlicerLoadableModule
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "qSlicerWelcomeModule" FILE "qSlicerWelcomeModule.JSON")
    Q_INTERFACES(qSlicerLoadableModule)

public:

  typedef qSlicerLoadableModule Superclass;
  qSlicerWelcomeModule(QObject *parent=0);
  virtual ~qSlicerWelcomeModule();

  qSlicerGetTitleMacro(QTMODULE_TITLE);



  virtual QStringList categories()const;
  virtual QIcon icon()const;


  /// Help to use the module
  virtual QString helpText()const;
  virtual QString acknowledgementText()const;
  virtual QStringList contributors()const;

protected:

  /// Create and return the widget representation associated to this module
  virtual qSlicerAbstractModuleRepresentation * createWidgetRepresentation();

  /// Create and return the logic associated to this module
  virtual vtkMRMLAbstractLogic* createLogic();

protected:
  QScopedPointer<qSlicerWelcomeModulePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerWelcomeModule);
  Q_DISABLE_COPY(qSlicerWelcomeModule);
};

#endif
