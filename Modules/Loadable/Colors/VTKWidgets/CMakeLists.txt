project(vtkSRPlan${MODULE_NAME}ModuleVTKWidgets)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "VTK_SRPlan_${MODULE_NAME_UPPER}_VTKWIDGETS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  )

set(${KIT}_SRCS
  vtkSlicerScalarBarActor.cxx
  vtkSlicerScalarBarActor.h
  )

set(${KIT}_TARGET_LIBRARIES
  )

#-----------------------------------------------------------------------------
SRPlanMacroBuildModuleLogic(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )


    
if(SRPlan_OUT_UNIFY)
   add_custom_command(
        TARGET ${KIT}
        POST_BUILD
        COMMAND  ${CMAKE_COMMAND} -E copy  $<TARGET_FILE:${KIT}> ${SRPlan_OUT_QtLoadable})

endif()
