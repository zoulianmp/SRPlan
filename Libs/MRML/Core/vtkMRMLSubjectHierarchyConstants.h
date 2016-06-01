/*==============================================================================

  Program: 3D Slicer

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

#ifndef __vtkMRMLSubjectHierarchyConstants_h
#define __vtkMRMLSubjectHierarchyConstants_h

// MRML includes
#include "vtkMRML.h"

// STD includes
#include <cstdlib>
#include <string>
#include <vector>

class VTK_MRML_EXPORT vtkMRMLSubjectHierarchyConstants
{
public:
  //----------------------------------------------------------------------------
  // Constant strings (std::string types for easy concatenation)
  //----------------------------------------------------------------------------

  // Subject hierarchy constants
  static const std::string GetSubjectHierarchyNodeNamePostfix()
    { return "_SubjectHierarchy"; };
  static const std::string GetSubjectHierarchyAttributePrefix()
    { return "SubjectHierarchy."; };
  static const std::string GetSubjectHierarchyExcludeFromTreeAttributeName()
    { return vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyAttributePrefix() + "ExcludeFromPotentialNodesList"; };
  static const std::string GetHighlightedSubjectHierarchyNodeAttributeName()
    { return vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyAttributePrefix() + "Highlighted"; };
  static const std::string GetVirtualBranchSubjectHierarchyNodeAttributeName()
    { return vtkMRMLSubjectHierarchyConstants::GetSubjectHierarchyAttributePrefix() + "VirtualBranch"; };
  static const std::string GetSubjectHierarchyNewNodeNamePrefix()
    { return "New"; };

  //****************************************************************
  //************* Levels
  //****************************************************************
  // Non-DICOM levels
  static const char* GetSubjectHierarchyLevelFolder()
    { return "Folder"; };

  static const char* GetSubjectHierarchyLevelSRPatient()
  {
	  return "SRPatient";
  };

  static const char* GetSubjectHierarchyLevelSRCourse()
  {
	  return "SRCourse";
  };

  static const char* GetSubjectHierarchyLevelSRPlan()
  {
	  return "SRPlan";
  };

  static const char* GetSubjectHierarchyLevelSRSubplan()
  {
	  return "SRSubplan";
  };

  //****************************************************************
  //*************UID Names
  //****************************************************************
  //SRPlan Components uidNames

  static const char* GetSRPlanPatientUIDName ()
  {
	  return "SRPlan-PatientUID";
  };

 
  static const char* GetSRPlanCourseUIDName()
  {
	  return "SRPlan-CourseUID";
  };

  static const char* GetSRPlanPlanUIDName()
  {
	  return "SRPlan-PlanUID";
  };

  static const char* GetSRPlanImageVolumeUIDName()
  {
	  return "SRPlan-ImageVolumeUID";
  };

  static const char * GetSRPlanAssignedVolumeofStructureSetUIDName()
  {
	  return "SRPlan-AssignedVolumeUID"; //Used to record the Assigned Image Volume UID of SH Node 
  }

  static const char* GetSRPlanDoseVolumeUIDName()
  {
	  return "SRPlan-DoseVolumeUID";
  };

  static const char* GetSRPlanUserOriginUIDName()
  {
	  return "SRPlan-UserOriginUID";
  };


  static const char* GetSRPlanPOIsFolderUIDName()
  {
	  return "SRPlan-POIsFolderUID";
  };



  static const char* GetSRPlanUserDefinedPointUIDName()
  {
	  return "SRPlan-UserDefinedPointUID";
  };


  static const char* GetSRPlanStructureSetUIDName()
  {
	  return "SRPlan-StructureSetUID";
  };

  static const char* GetSRPlanStructureUIDName()
  {
	  return "SRPlan-StructureUID";
  };

  static const char* GetSRPlanTreatPathUIDName()
  {
	  return "SRPlan-TreatPathUID";
  };

  static const char* GetSRPlanControlPointUIDName()
  {
	  return "SRPlan-ControlPointUID";
  };

  //****************************************************************
  //*************BaseName
  //****************************************************************
  //SRPlan SubjectHierarchy Node BaseName

  static const char* GetSRPlanPatientNodeBaseName()
  {
	  return "SRPlan-PatientNode";
  };


  static const char* GetSRPlanCourseNodeBaseName()
  {
	  return "SRPlan-CourseNode";
  };

  static const char* GetSRPlanPlanNodeBaseName()
  {
	  return "SRPlan-PlanNode";
  };

  static const char* GetSRPlanImageVolumeNodeBaseName()
  {
	  return "SRPlan-ImageVolumeNode";
  };

  static const char* GetSRPlanDoseVolumeNodeBaseName()
  {
	  return "SRPlan-DoseVolumeNode";
  };

  static const char* GetSRPlanUserOriginNodeBaseName()
  {
	  return "SRPlan-UserOriginNode";
  };


  static const char* GetSRPlanUserDefinedPointNodeBaseName()
  {
	  return "SRPlan-UserDefinedPointNode";
  };


  static const char* GetSRPlanStructureSetNodeBaseName()
  {
	  return "SRPlan-StructureSetNode";
  };

  static const char* GetSRPlanStructureNodeBaseName()
  {
	  return "SRPlan-StructureNode";
  };

  static const char* GetSRPlanTreatPathNodeBaseName()
  {
	  return "SRPlan-TreatPathNode";
  };

  static const char* GetSRPlanControlPointNodeBaseName()
  {
	  return "SRPlan-ControlPointNode";
  };

  //****************************************************************
  //*************UIDS
  //****************************************************************
  // Primary Image Volume UID
  static const char* GetSRPlanPrimaryImageVolumeUID()
  {
	  return "SRPlan-PrimaryImageVolume";
  };


  static const char* GetSRPlanPOIsFolderUID()
  {
	  return "SRPlan-POIs";
  };


  // DICOM levels
  static const char* GetDICOMLevelPatient()
    { return "Patient"; };
  static const char* GetDICOMLevelStudy()
    { return "Study"; };
  static const char* GetDICOMLevelSeries()
    { return "Series"; };
  static const char* GetDICOMLevelSubseries()
    { return "Subseries"; };

  // DICOM attributes
  static const char* GetDICOMUIDName()
    { return "DICOM"; };
  static const char* GetDICOMInstanceUIDName()
    { return "DICOMInstanceUID"; };
  static const std::string GetDICOMAttributePrefix()
    { return "DICOM."; };
  static std::string GetDICOMReferencedInstanceUIDsAttributeName()
    { return vtkMRMLSubjectHierarchyConstants::GetDICOMAttributePrefix() + "ReferencedInstanceUIDs"; };

  // Patient tags
  static const std::string GetDICOMPatientNameTagName()
    { return "PatientName"; };
  static const std::string GetDICOMPatientNameAttributeName()
    { return vtkMRMLSubjectHierarchyConstants::GetDICOMAttributePrefix() + vtkMRMLSubjectHierarchyConstants::GetDICOMPatientNameTagName(); };
  static const std::string GetDICOMPatientIDTagName()
    { return "PatientID"; };
  static const std::string GetDICOMPatientIDAttributeName()
    { return vtkMRMLSubjectHierarchyConstants::GetDICOMAttributePrefix() + vtkMRMLSubjectHierarchyConstants::GetDICOMPatientIDTagName(); };
  static const std::string GetDICOMPatientSexTagName()
    { return "PatientSex"; };
  static const std::string GetDICOMPatientSexAttributeName()
    { return vtkMRMLSubjectHierarchyConstants::GetDICOMAttributePrefix() + vtkMRMLSubjectHierarchyConstants::GetDICOMPatientSexTagName(); };
  static const std::string GetDICOMPatientBirthDateTagName()
    { return "PatientBirthDate"; };
  static const std::string GetDICOMPatientBirthDateAttributeName()
    { return vtkMRMLSubjectHierarchyConstants::GetDICOMAttributePrefix() + vtkMRMLSubjectHierarchyConstants::GetDICOMPatientBirthDateTagName(); };
  static const std::string GetDICOMPatientCommentsTagName()
    { return "PatientComments"; };
  static const std::string GetDICOMPatientCommentsAttributeName()
    { return vtkMRMLSubjectHierarchyConstants::GetDICOMAttributePrefix() + vtkMRMLSubjectHierarchyConstants::GetDICOMPatientCommentsTagName(); };

  /// Get patient tag names (attribute names are these values prefixed!)
  static const std::vector<std::string> GetDICOMPatientTagNames()
  {
    std::vector<std::string> patientTagNames;
    patientTagNames.push_back(vtkMRMLSubjectHierarchyConstants::GetDICOMPatientNameTagName());
    patientTagNames.push_back(vtkMRMLSubjectHierarchyConstants::GetDICOMPatientIDTagName());
    patientTagNames.push_back(vtkMRMLSubjectHierarchyConstants::GetDICOMPatientSexTagName());
    patientTagNames.push_back(vtkMRMLSubjectHierarchyConstants::GetDICOMPatientBirthDateTagName());
    patientTagNames.push_back(vtkMRMLSubjectHierarchyConstants::GetDICOMPatientCommentsTagName());
    return patientTagNames;
  }

  // Study tags
  static const std::string  GetDICOMStudyInstanceUIDTagName()
    { return "StudyInstanceUID"; };
  static const std::string  GetDICOMStudyIDTagName()
    { return "StudyID"; };
  static const std::string  GetDICOMStudyDescriptionTagName()
    { return "StudyDescription"; };
  static const std::string  GetDICOMStudyDescriptionAttributeName()
    { return vtkMRMLSubjectHierarchyConstants::GetDICOMAttributePrefix() + vtkMRMLSubjectHierarchyConstants::GetDICOMStudyDescriptionTagName(); };
  static const std::string GetDICOMStudyDateTagName()
    { return "StudyDate"; };
  static const std::string GetDICOMStudyDateAttributeName()
    { return vtkMRMLSubjectHierarchyConstants::GetDICOMAttributePrefix() + vtkMRMLSubjectHierarchyConstants::GetDICOMStudyDateTagName(); };
  static const std::string GetDICOMStudyTimeTagName()
    { return "StudyTime"; };
  static const std::string GetDICOMStudyTimeAttributeName()
    { return vtkMRMLSubjectHierarchyConstants::GetDICOMAttributePrefix() + vtkMRMLSubjectHierarchyConstants::GetDICOMStudyTimeTagName(); };

  /// Get study tag names (attribute names are these values prefixed!)
  static const std::vector<std::string> GetDICOMStudyTagNames()
  {
    std::vector<std::string> studyTagNames;
    studyTagNames.push_back(vtkMRMLSubjectHierarchyConstants::GetDICOMStudyDescriptionTagName());
    studyTagNames.push_back(vtkMRMLSubjectHierarchyConstants::GetDICOMStudyDateTagName());
    studyTagNames.push_back(vtkMRMLSubjectHierarchyConstants::GetDICOMStudyTimeTagName());
    return studyTagNames;
  }

  // Series tags
  static const std::string GetDICOMSeriesModalityAttributeName()
    { return vtkMRMLSubjectHierarchyConstants::GetDICOMAttributePrefix() + "Modality"; };
  static const std::string GetDICOMSeriesNumberAttributeName()
    { return vtkMRMLSubjectHierarchyConstants::GetDICOMAttributePrefix() + "SeriesNumber"; };

};

#endif
