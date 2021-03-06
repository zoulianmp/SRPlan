project(vtkSlicerRtCommon)

# --------------------------------------------------------------------------
# Configure headers
# --------------------------------------------------------------------------
set(configure_header_file vtkSlicerRtCommonConfigure.h)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/${configure_header_file}.in
  ${CMAKE_CURRENT_BINARY_DIR}/${configure_header_file}
  )

# --------------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------------

SET (SlicerRtCommon_SRCS 
  SlicerRtCommon.cxx
  SlicerRtCommon.h
  SlicerRtCommon.txx
  vtkLabelmapToModelFilter.cxx
  vtkLabelmapToModelFilter.h
  vtkPolyDataToLabelmapFilter.cxx
  vtkPolyDataToLabelmapFilter.h
  vtkSlicerAutoWindowLevelLogic.cxx
  vtkSlicerAutoWindowLevelLogic.h
  )

SET (SlicerRtCommon_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${Slicer_Libs_INCLUDE_DIRS}  CACHE INTERNAL "" FORCE)

# --------------------------------------------------------------------------
# Build the library
# --------------------------------------------------------------------------

set(lib_name ${PROJECT_NAME})
SET (SlicerRtCommon_LIBS ${SlicerRtCommon_LIBS}
  ${VTK_LIBRARIES}
  MRMLCore

  )

  





  
set(include_dirs
  ${SlicerRtCommon_INCLUDE_DIRS}
  ${vtkSegmentationCore_INCLUDE_DIRS}
  ${MRMLCore_INCLUDE_DIRS}
  )

include_directories(${include_dirs})


#INCLUDE_DIRECTORIES( ${SlicerRtCommon_INCLUDE_DIRS} )
ADD_LIBRARY(${lib_name} ${SlicerRtCommon_SRCS})
TARGET_LINK_LIBRARIES( ${lib_name} ${SlicerRtCommon_LIBS} )




if(SRPlan_OUT_UNIFY)
   add_custom_command(
        TARGET ${lib_name}
        POST_BUILD
        COMMAND  ${CMAKE_COMMAND} -E copy  $<TARGET_FILE:${lib_name}> ${SRPlan_OUT_Library})

endif()



if(NOT DEFINED ${PROJECT_NAME}_INSTALL_BIN_DIR)
  set(${PROJECT_NAME}_INSTALL_BIN_DIR bin)
endif()
if(NOT DEFINED ${PROJECT_NAME}_INSTALL_LIB_DIR)
  set(${PROJECT_NAME}_INSTALL_LIB_DIR lib/${PROJECT_NAME})
endif()


# Set loadable modules output
set_target_properties(${lib_name} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_NAME}_INSTALL_BIN_DIR"
  LIBRARY_OUTPUT_DIRECTORY "${PROJECT_NAME}_INSTALL_BIN_DIR"
  ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_NAME}_INSTALL_BIN_DIR"
  )

IF (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND NOT WIN32)
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF()

# --------------------------------------------------------------------------
# Install library
# --------------------------------------------------------------------------
install(TARGETS ${lib_name}
  RUNTIME DESTINATION ${PROJECT_NAME}_INSTALL_BIN_DIR COMPONENT RuntimeLibraries
  LIBRARY DESTINATION ${PROJECT_NAME}_INSTALL_BIN_DIR COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${PROJECT_NAME}_INSTALL_BIN_DIR COMPONENT Development
  )

# --------------------------------------------------------------------------
# Python wrapping
# --------------------------------------------------------------------------
if(VTK_WRAP_PYTHON AND BUILD_SHARED_LIBS)

  set(Slicer_Wrapped_LIBRARIES
    )

  foreach(library ${lib_name})
    if(TARGET ${library}PythonD)
      list(APPEND Slicer_Wrapped_LIBRARIES ${library}PythonD)
    endif()
  endforeach()

  SlicerMacroPythonWrapModuleVTKLibrary(
    NAME ${PROJECT_NAME}
    SRCS ${SlicerRtCommon_SRCS}
    WRAPPED_TARGET_LIBRARIES ${Slicer_Wrapped_LIBRARIES}
    RELATIVE_PYTHON_DIR "."
    )

  # Set python module logic output
  set_target_properties(${PROJECT_NAME}Python ${PROJECT_NAME}PythonD PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_QTLOADABLEMODULES_BIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_QTLOADABLEMODULES_LIB_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_QTLOADABLEMODULES_LIB_DIR}"
    )

  # Export target
  set_property(GLOBAL APPEND PROPERTY Slicer_TARGETS ${PROJECT_NAME}Python ${PROJECT_NAME}PythonD)
endif()
