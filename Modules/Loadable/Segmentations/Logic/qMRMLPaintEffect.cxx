/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:09 $
Version:   $Revision: 1.11 $

=========================================================================auto=*/

// MRML includes
#include "qMRMLPaintEffect.h"
#include "vtkMRMLScene.h"

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

#include "vtkInteractorObserver.h"
#include "vtkRendererCollection.h"
#include <vtkActorCollection.h>
#include <vtkActor.h>
#include "vtkIntArray.h"
#include "vtkStdString.h"

#include "vtkMRMLSliceNode.h"
#include "vtkGeneralTransform.h"

#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"



#include "vtkMatrix4x4.h"



// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// STD includes
#include <iostream>
#include <sstream>
#include <algorithm> // for std::sort


vtkStandardNewMacro(qMRMLPaintEffect);

qMRMLPaintEffect::qMRMLPaintEffect()
{
	this->brushSize = 10 ; //defalt size is 10 mm
	this->shape = qMRMLPaintEffect::Circle;
	this->pixelMode = true;
	
	this->paintCoordinates = vtkPoints2D::New();
	
}

//Call after Base Class Setup Steps
void qMRMLPaintEffect::SetupBrush()
{
	//initialize the brush data
	this->brush = vtkPolyData::New();
	this->CreateGlyph(this->brush);


	vtkPolyDataMapper2D * mapper = vtkPolyDataMapper2D::New();
	this->brushActor = vtkActor2D::New();
	mapper->SetInputData(this->brush);
	this->brushActor->SetMapper(mapper);
	this->brushActor->VisibilityOff();

	this->renderer->AddActor2D(this->brushActor);
	this->actors->AddItem(this->brushActor);

	//python code call default parameters
	//this->ProcessEvent();

}




qMRMLPaintEffect::~qMRMLPaintEffect()
{
	this->actors->Delete();

} 




//clean up actors and observers
void qMRMLPaintEffect::CleanUp()
{
	int number = this->feedbackActors->GetNumberOfItems();
	this->feedbackActors->InitTraversal();
	for (int i = 0; i < number; i++)
	{
		this->renderer->RemoveActor2D(this->feedbackActors->GetNextActor2D());
	}
	this->sliceView->scheduleRender();
	
	Superclass::CleanUp();
}


/*
void qMRMLPaintEffect::PaintEffectEventCallback(vtkObject *caller,
	unsigned long eid,
	void *clientData,
	void *callData)
{
	qMRMLPaintEffect* self =
		reinterpret_cast<qMRMLPaintEffect *>(clientData);

	if (self) 	self->ProcessEvent(caller, eid, callData);
}


void qMRMLPaintEffect::SetupEventsObservation()
{


	this->interactorObserverTags = vtkUnsignedLongArray::New();

	this->events = vtkIntArray::New();

	this->events->InsertNextValue(vtkCommand::LeftButtonPressEvent);
	this->events->InsertNextValue(vtkCommand::LeftButtonReleaseEvent);
	this->events->InsertNextValue(vtkCommand::MiddleButtonPressEvent);
	this->events->InsertNextValue(vtkCommand::MiddleButtonReleaseEvent);
	this->events->InsertNextValue(vtkCommand::RightButtonPressEvent);
	this->events->InsertNextValue(vtkCommand::RightButtonReleaseEvent);
	this->events->InsertNextValue(vtkCommand::MouseMoveEvent);
	this->events->InsertNextValue(vtkCommand::KeyPressEvent);
	this->events->InsertNextValue(vtkCommand::EnterEvent);
	this->events->InsertNextValue(vtkCommand::LeaveEvent);


	//vtkSmartPointer<vtkCallbackCommand> callback =
	//	vtkSmartPointer<vtkCallbackCommand>::New();

	vtkCallbackCommand* callback = vtkCallbackCommand::New();

	callback->SetCallback(qMRMLPaintEffect::PaintEffectEventCallback);
	callback->SetClientData(this);


	for (int i = 0; i < events->GetSize(); i++)
	{
		int e = this->events->GetValue(i);

		unsigned long tag = this->interactor->AddObserver(e, callback, 1.0);
		this->interactorObserverTags->InsertNextValue(tag);
	}



	this->sliceNodeTags = vtkUnsignedLongArray::New();

	vtkMRMLSliceNode * sliceNode = this->sliceLogic->GetSliceNode();

	unsigned long tag = sliceNode->AddObserver(vtkCommand::ModifiedEvent, callback, 1.0);
	this->sliceNodeTags->InsertNextValue(tag);


}
*/

void qMRMLPaintEffect::ProcessEvent(vtkObject *caller, unsigned long event, void *callData)
{
	//events from the interactor

	if (event == vtkCommand::LeftButtonPressEvent)
	{
		this->actionState = "painting";
		int *xy = this->interactor->GetEventPosition();
		this->PaintAddPoint(xy[0], xy[1]);
		this->AbortEvent(event);
	}
	else if (event == vtkCommand::LeftButtonReleaseEvent)
	{
		this->PaintApply();
		this->actionState = "";
		this->CursorOn();
	}
	else if (event == vtkCommand::RightButtonPressEvent)
	{
		
	}
	else if (event == vtkCommand::RightButtonReleaseEvent)
	{
		

	}
	else if (event == vtkCommand::MouseMoveEvent)
	{
		this->brushActor->VisibilityOn();
		if (this->actionState == "painting")
		{
			int *xy = this->interactor->GetEventPosition();
			this->PaintAddPoint(xy[0], xy[1]);
			this->AbortEvent(event);
		}
	
	}
	else if (event == vtkCommand::EnterEvent)
	{
		this->brushActor->VisibilityOn();
	}
	else if (event == vtkCommand::LeaveEvent)
	{
		this->brushActor->VisibilityOff();
	}
	else if (event == vtkCommand::KeyPressEvent)
	{
		
		char * key = this->interactor->GetKeySym();
		if (!strcmp(key, "plus") || !strcmp(key, "equal"))
		{
			this->ScaleBrushSize(1.2);
		}
		if (!strcmp(key, "minus") || !strcmp(key, "underscore"))
		{
			this->ScaleBrushSize(0.8);
		}
	}
	else
	{
		Superclass::ProcessEvent(caller, event, callData);
	}

	//# events from the slice node
	if (caller && caller->IsA("vtkMRMLSliceNode"))
	{
		
		if (this->brush)
		{
			this->CreateGlyph(this->brush);	
		}
	}

	this->PositionActors();

}



//create a brush circle of the right radius in XY space
//- assume uniform scaling between XY and RAS which
//is enforced by the view interactors
void qMRMLPaintEffect::CreateGlyph(vtkPolyData * brush)
{
	vtkMRMLSliceNode * sliceNode = this->sliceLogic->GetSliceNode();

	vtkMatrix4x4* inner_rasToXY = vtkMatrix4x4::New();
	inner_rasToXY->DeepCopy(sliceNode->GetXYToRAS());
	inner_rasToXY->Invert();
	double maximum = 0;
	int maxindex = 0;

	for (int i = 0; i < 3; i++)
	{
		if (abs(inner_rasToXY->GetElement(0, i))>maximum)
		{
			maximum = abs(inner_rasToXY->GetElement(0, i));
			maxindex = i;
		}
	}

	float point[4] = { 0,0,0,0 };
	point[maxindex] = this->brushSize;

	float* xyBrushSize = inner_rasToXY->MultiplyPoint(point);

	float Radius3d = sqrt(xyBrushSize[0] * xyBrushSize[0] + xyBrushSize[1] * xyBrushSize[1] + xyBrushSize[2] * xyBrushSize[2]);


	//# make a circle paint brush
	vtkPoints* points = vtkPoints::New();
	vtkCellArray* lines = vtkCellArray::New();
	brush->SetPoints(points);
	brush->SetLines(lines);
	double PI = 3.1415926;
	double	TWOPI = PI * 2;
	double	PIoverSIXTEEN = PI / 16;
	vtkIdType	prevPoint = -1;
	vtkIdType	firstPoint = -1;

	vtkIdType p;

	double	angle = 0;
	while (angle <=TWOPI)
	{
		double x = Radius3d *  cos(angle);
		double y = Radius3d *  sin(angle);
	    p = points->InsertNextPoint(x, y, 0);
		if (prevPoint != -1)
		{
			vtkIdList * idList = vtkIdList::New();
			idList->InsertNextId(prevPoint);
			idList->InsertNextId(p);
			brush->InsertNextCell(VTK_LINE, idList);
		}
		prevPoint = p;
		if (firstPoint == -1)
			firstPoint = p;
		angle = angle + PIoverSIXTEEN;	
	}

	//# make the last line in the circle
	vtkIdList* idList = vtkIdList::New();
	idList->InsertNextId(p);
	idList->InsertNextId(firstPoint);
	brush->InsertNextCell(VTK_LINE, idList);
}

//update paint feedback glyph to follow mouse
void qMRMLPaintEffect::PositionActors()
{
	if (this->brushActor)
	{
		int* xy = this->interactor->GetEventPosition();
		double d_xy[2] = { double(xy[0]),double(xy[1])};
		this->brushActor->SetPosition(d_xy);
		this->sliceView->scheduleRender();
	}
			
}


void qMRMLPaintEffect::ScaleBrushSize(double scaleFactor)
{
	this->brushSize = this->brushSize * scaleFactor;
}

// depending on the delayedPaint mode, either paint the given point or queue it up with a marker
//   for later painting
void qMRMLPaintEffect::PaintAddPoint(int x, int y)
{

	this->paintCoordinates->InsertNextPoint(double(x), double(y));
	if (this->delayedPaint && !this->pixelMode)
	{
		this->PaintFeedback();
	}
	else
	{
		this->PaintApply();
	}
	

}

//add a feedback actor(copy of the paint radius Actor) for any points that don't have one yet.
//	If the list is empty, clear out the old actors
void qMRMLPaintEffect::PaintFeedback()
{
	int nCoordinates = this->paintCoordinates->GetNumberOfPoints();
	int nActors = this->feedbackActors->GetNumberOfItems();

	if (this->paintCoordinates->GetNumberOfPoints() == 0)
	{
		int number = this->feedbackActors->GetNumberOfItems();
		this->feedbackActors->InitTraversal();
		for (int i = 0; i < number; i++)
		{
			this->renderer->RemoveActor2D(this->feedbackActors->GetNextActor2D());
		}

		return;
	}
	else if (nCoordinates > nActors)
	{
		for (int i = nActors;i < nCoordinates; i++)
		{
			vtkActor2D *a = vtkActor2D::New();
			this->feedbackActors->AddItem(a);
			vtkPolyDataMapper2D * mapper = vtkPolyDataMapper2D::New();
			a->SetMapper(mapper);
			double * xy = this->paintCoordinates->GetPoint(i);
			a->SetPosition(xy[0], xy[1]);
			vtkProperty2D * property = a->GetProperty();
			property->SetColor(.7, .7, 0);
			property->SetOpacity(.5);
			this->renderer->AddActor2D(a);
		}
	}
}

void qMRMLPaintEffect::PaintApply()
{
	int nCoordinates = this->paintCoordinates->GetNumberOfPoints();
	for (int i = 0;i < nCoordinates; i++)
	{
		double * xy = this->paintCoordinates->GetPoint(i);
		this->PaintBrush(xy[0], xy[1]);
	}
	this->paintCoordinates->Reset();
	this->PaintFeedback();

	//# TODO: workaround for new pipeline in slicer4
    //# - editing image data of the calling modified on the node
	//#   does not pull the pipeline chain
    //# - so we trick it by changing the image data first
		
	vtkMRMLSliceLayerLogic *labelLogic = this->sliceLogic->GetLabelLayer();
	vtkMRMLVolumeNode *	labelNode = labelLogic->GetVolumeNode();
	this->editorLogic->markVolumeNodeAsModified(labelNode);

}

//paint with a brush that is circular(or optionally spherical) in XY space
//(could be streched or rotate when transformed to IJK)
//- make sure to hit every pixel in IJK space
//- apply the threshold if selected
void qMRMLPaintEffect::PaintBrush(double x, double y)
{
	vtkMRMLSliceNode * sliceNode = this->sliceLogic->GetSliceNode();

	vtkMRMLSliceLayerLogic *labelLogic = this->sliceLogic->GetLabelLayer();
	vtkMRMLVolumeNode *	labelNode = labelLogic->GetVolumeNode();
	vtkImageData * labelImage = labelNode->GetImageData();

	vtkMRMLSliceLayerLogic *backgroundLogic = this->sliceLogic->GetBackgroundLayer();
	vtkMRMLVolumeNode *	backgroundNode = backgroundLogic->GetVolumeNode();
	vtkImageData * backgroundImage = backgroundNode->GetImageData();

	if (!labelNode) return;


	//# get the brush bounding box in ijk coordinates
    //# - get the xy bounds
    //# - transform to ijk
    //# - clamp the bounds to the dimensions of the label image

	double * bounds = this->brush->GetPoints()->GetBounds();
	double	left = x + bounds[0];
	double	right = x + bounds[1];
	double	bottom = y + bounds[2];
	double	top = y + bounds[3];

	vtkGeneralTransform * xyToIJK = labelLogic->GetXYToIJKTransform();

	double tlpoint[3] = { left, top, 0 };
	double trpoint[3] = { right, top, 0 };
	double blpoint[3] = { left, bottom, 0 };
	double brpoint[3] = { right, bottom, 0 };

	double * tlIJK = xyToIJK->TransformDoublePoint(tlpoint);
	double * trIJK = xyToIJK->TransformDoublePoint(trpoint);
	double * blIJK = xyToIJK->TransformDoublePoint(blpoint);
	double * brIJK = xyToIJK->TransformDoublePoint(brpoint);

	int * dims = labelImage->GetDimensions();

	//# clamp the top, bottom, left, right to the
	//# valid dimensions of the label image
	int	tl[3] = { 0, 0, 0 };
	int	tr[3] = { 0, 0, 0 };
	int	bl[3] = { 0, 0, 0 };
	int	br[3] = { 0, 0, 0 };

	for (int i = 0;i < 3;i++)
	{
		tl[i] = int(round(tlIJK[i]));
		if (tl[i] < 0)  tl[i] = 0;
		if (tl[i] >= dims[i])  tl[i] = dims[i] - 1;

		tr[i] = int(round(trIJK[i]));
		if (tr[i] < 0) tr[i] = 0;
		if (tr[i] > dims[i]) tr[i] = dims[i] - 1;
      
		bl[i] = int(round(blIJK[i]));
		if (bl[i] < 0) bl[i] = 0;
		if (bl[i] > dims[i]) bl[i] = dims[i] - 1;


		br[i] = int(round(brIJK[i]));
		if (br[i] < 0) br[i] = 0;
		if (br[i] > dims[i]) br[i] = dims[i] - 1;
	}

	//# If the region is smaller than a pixel then paint it using paintPixel mode,
	//# to make sure at least one pixel is filled on each click
	int maxRowDelta = 0;
	int maxColumnDelta = 0;

	for (int i = 0;i<3;i++)
	{
		int d = abs(tr[i] - tl[i]);
		if (d > maxColumnDelta) maxColumnDelta = d;

		d = abs(br[i] - bl[i]);
		if (d > maxColumnDelta) maxColumnDelta = d;

		d = abs(bl[i] - tl[i]);
		if (d > maxRowDelta) maxRowDelta = d;

		d = abs(br[i] - tr[i]);			
		if (d > maxRowDelta) maxRowDelta = d;

	}

	if (maxRowDelta <= 1 || maxColumnDelta <= 1) this->PaintPixel(x, y);
		


	//# get the layers and nodes
	//# and ijk to ras matrices including transforms

	vtkMatrix4x4 * backgroundIJKToRAS = vtkMatrix4x4::New();

	vtkMatrix4x4 * labelIJKToRAS = vtkMatrix4x4::New();


	this->GetIJKToRASMatrix(backgroundNode, backgroundIJKToRAS);
	this->GetIJKToRASMatrix(labelNode, labelIJKToRAS);


	vtkMatrix4x4 * xyToRAS = sliceNode->GetXYToRAS();

	float xypoint[4] = { x, y, 0, 1 };


	float * raspoint = xyToRAS->MultiplyPoint(xypoint);

	float brushCenter[3] = { raspoint[0],raspoint[1],raspoint[2] };

	int paintLabel = this->GetPaintLabel();
	bool paintOver = this->GetPaintOver();

	if (!this->painter)
	{
		this->painter = vtkImageSlicePaint::New();
	}


	this->painter->SetBackgroundImage(backgroundImage);
	this->painter->SetBackgroundIJKToWorld(backgroundIJKToRAS);
	this->painter->SetWorkingImage(labelImage);
	this->painter->SetWorkingIJKToWorld(labelIJKToRAS);
	this->painter->SetTopLeft(tl[0], tl[1], tl[2]);
	this->painter->SetTopRight(tr[0], tr[1], tr[2]);
	this->painter->SetBottomLeft(bl[0], bl[1], bl[2]);
	this->painter->SetBottomRight(br[0], br[1], br[2]);
	this->painter->SetBrushCenter(brushCenter[0], brushCenter[1], brushCenter[2]);
	this->painter->SetBrushRadius(this->brushSize/2.0);
	this->painter->SetPaintLabel(paintLabel);
	this->painter->SetPaintOver(paintOver);
	//this->painter->SetThresholdPaint(paintThreshold);
	//this->painter->SetThresholdPaintRange(paintThresholdMin, paintThresholdMax);

	this->painter->Paint();
}


void qMRMLPaintEffect::PaintPixel(double x, double y)
{

}

void qMRMLPaintEffect::SetBrushShape(BrushType shape)
{
	this->shape = shape;
}

void qMRMLPaintEffect::SetBrushSize(int size)
{
	this->brushSize = size;
}