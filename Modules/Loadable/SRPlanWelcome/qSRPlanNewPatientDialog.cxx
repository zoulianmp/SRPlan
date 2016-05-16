#include "qSRPlanNewPatientDialog.h"


#include "ui_qSRPlanNewPatientDialog.h"


#include "vtkMRMLPatientInfoNode.h"




#include "qSlicerApplication.h"




//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_SlicerWelcome
class  qSRPlanNewPatientDialogPrivate : public Ui_qSRPlanNewPatientDialog
{
	Q_DECLARE_PUBLIC(qSRPlanNewPatientDialog);
protected:
	qSRPlanNewPatientDialog* const q_ptr;
public:
	qSRPlanNewPatientDialogPrivate(qSRPlanNewPatientDialog& object);
	void setupUi(QDialog* dialog);

	 
};

//-----------------------------------------------------------------------------
// qSlicerWelcomeModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSRPlanNewPatientDialogPrivate::qSRPlanNewPatientDialogPrivate(qSRPlanNewPatientDialog& object)
	: q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
void qSRPlanNewPatientDialogPrivate::setupUi(QDialog* dialog)
{
	this->Ui_qSRPlanNewPatientDialog::setupUi(dialog);

	/*
	// Create the button group ensuring that only one collabsibleWidgetButton will be open at a time
	ctkButtonGroup * group = new ctkButtonGroup(widget);

	// Add all collabsibleWidgetButton to a button group
	QList<ctkCollapsibleButton*> collapsibles = widget->findChildren<ctkCollapsibleButton*>();
	foreach(ctkCollapsibleButton* collapsible, collapsibles)
	{
	group->addButton(collapsible);
	}

	*/
}


//-----------------------------------------------------------------------------
// qSlicerWelcomeModuleWidget methods

//-----------------------------------------------------------------------------
qSRPlanNewPatientDialog::qSRPlanNewPatientDialog(QWidget* _parent)
	: Superclass(_parent)
	, d_ptr(new qSRPlanNewPatientDialogPrivate(*this))
{
	this->setup();
}

//-----------------------------------------------------------------------------
qSRPlanNewPatientDialog::~qSRPlanNewPatientDialog()
{
}

//-----------------------------------------------------------------------------
void qSRPlanNewPatientDialog::setup()
{
	Q_D(qSRPlanNewPatientDialog);
	d->setupUi(this);


	connect(d->pushButton_loadimage, SIGNAL(clicked()),
		this, SLOT(onConfirmInfoAndLoadImageClicked()));

	connect(d->pushButton_cancel, SIGNAL(clicked()),
		this, SLOT(reject()));


	/*

	connect(d->OpenPatientButton, SIGNAL(clicked()),
		this, SLOT(loadNonDicomData()));

	connect(d->NewPatientButton, SIGNAL(clicked()),
		this, SLOT(loadNonDicomData()));


	connect(d->LoadDicomDataButton, SIGNAL(clicked()),
	this, SLOT(loadDicomData()));

	connect(d->LoadSampleDataButton, SIGNAL(clicked()),
	this, SLOT (loadRemoteSampleData()));
	connect(d->EditApplicationSettingsButton, SIGNAL(clicked()),
	this, SLOT (editApplicationSettings()));

	#ifndef Slicer_BUILD_DICOM_SUPPORT
	d->LoadDicomDataButton->setDisabled(true);
	#endif

	*/

	//this->Superclass::setup();
}

void qSRPlanNewPatientDialog::onConfirmInfoAndLoadImageClicked()
{
	this->CreateBaseSubjectHierarchy();
	this->accept();

}



bool qSRPlanNewPatientDialog::CreateBaseSubjectHierarchy()
{
	Q_D(qSRPlanNewPatientDialog);

	qSlicerApplication * app = qSlicerApplication::application();
	vtkMRMLScene * scene = app->mrmlScene();

	vtkMRMLPatientInfoNode * infornode = vtkSmartPointer<vtkMRMLPatientInfoNode>::New();


	vtkStdString sf = (d->lineEdit_id->text()).toStdString();
	infornode->SetPatientID(sf);

	infornode->SetPatientID((d->lineEdit_id->text()).toStdString());
	infornode->SetPatientName((d->lineEdit_name->text()).toStdString());
	infornode->SetPatientAge((d->lineEdit_age->text()).toInt());

	vtkStdString genderstr = (d->comboBox_gender->currentText()).toStdString();




	if (!strcmp(genderstr, "Male"))
	{
		infornode->SetPatientGender(vtkMRMLPatientInfoNode::Male);
	}
	else if (!strcmp(genderstr, "Female"))
	{
		infornode->SetPatientGender(vtkMRMLPatientInfoNode::Female);
	}
	else if (!strcmp(genderstr, "Other"))
	{
		infornode->SetPatientGender(vtkMRMLPatientInfoNode::Other);
	}

    //scene->AddNode(infornode);

	infornode->SetName("PatientInfoNode");
	infornode->SetScene(scene);

	

	return true;

}


