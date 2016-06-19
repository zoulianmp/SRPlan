/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.18 $

=========================================================================auto=*/

#ifndef __qMRMLThresholdEffect_h
#define __qMRMLThresholdEffect_h

// MRML includes
#include "vtkMRML.h"
#include "vtkObserverManager.h"
#include "vtkIdTypeArray.h"
#include "vtkIntArray.h"
#include "vtkImageSlicePaint.h"

//Editor 
#include "qMRMLSegmentsEditorLogic.h"
#include "vtkMRMLSliceLayerLogic.h"
#include "qMRMLEffect.h"

#include "qMRMLSliceWidget.h"
#include "vtkMRMLSliceLogic.h"
#include "qMRMLSliceView.h"

class vtkMRMLScene;


// VTK includes

#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

#include <vtkActorCollection.h>
#include <vtkActor.h>
#include <vtkUnsignedLongArray.h>


#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>
class vtkCallbackCommand;

// Slicer VTK add-on includes
#include <vtkLoggingMacros.h>

// STD includes
#include <map>
#include <string>
#include <vector>

#include "qSRPlanSegmentationsModuleWidgetsExport.h"

class Q_SRPlan_MODULE_SEGMENTATIONS_WIDGETS_EXPORT qMRMLThresholdEffect : public qMRMLEffect
{
  

public:
  vtkTypeMacro(qMRMLThresholdEffect,qMRMLEffect);
  //void PrintSelf(ostream& os, vtkIndent indent);



  virtual void ProcessEvent(vtkObject *caller, unsigned long event,void *callData);
 

  //clean up actors and observers
  virtual void CleanUp();

  


public:





protected:


  
  qMRMLThresholdEffect();
  qMRMLThresholdEffect(qMRMLSliceWidget* sliceWidget);
  /// critical to have a virtual destructor!
  ~qMRMLThresholdEffect();
  qMRMLThresholdEffect(const qMRMLThresholdEffect&);
  void operator=(const qMRMLThresholdEffect&);



 };

#endif
