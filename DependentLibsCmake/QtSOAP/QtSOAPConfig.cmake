###########################################################################
#
#  Library:   CTK
#
#  Copyright (c) Kitware Inc.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.commontk.org/LICENSE
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

#
# QtSOAPConfig.cmake - QtSOAP CMake configuration file for external projects.
#

SET(QtSOAP_LIBRARIES QtSOAP)

# The QtSOAP include file directories.
SET(QtSOAP_INCLUDE_DIRS "C:/BaseLibsDebug/QtSOAP/include")
 
# The QtSOAP library directories. Note that if
# QtSOAP_CONFIGURATION_TYPES is set (see below) then these directories
# will be the parent directories under which there will be a directory
# of runtime binaries for each configuration type.
SET(QtSOAP_LIBRARY_DIRS "C:/BaseLibsDebug/QtSOAP/lib")
  
# The QtSOAP runtime library directories. Note that if
# QtSOAP_CONFIGURATION_TYPES is set (see below) then these directories
# will be the parent directories under which there will be a directory
# of runtime libraries for each configuration type.
SET(QtSOAP_RUNTIME_LIBRARY_DIRS "C:/BaseLibsDebug/QtSOAP/lib")
 
# The location of the UseQtSOAP.cmake file.
SET(QtSOAP_USE_FILE "C:/BaseLibsDebug/QtSOAP/UseQtSOAP.cmake")
  
 
# A QtSOAP install tree always provides one build configuration. 
# A QtSOAP build tree may provide either one or multiple build 
# configurations depending on the CMake generator used. 
# Since QtSOAP can be used either from a build tree or an install 
# tree it is useful for outside projects to know the configurations available. 
# If this QtSOAPConfig.cmake is in a QtSOAP install 
# tree QtSOAP_CONFIGURATION_TYPES will be empty and 
# QtSOAP_BUILD_TYPE will be set to the value of
# CMAKE_BUILD_TYPE used to build QtSOAP. If QtSOAPConfig.cmake 
# is in a QtSOAP build tree then QtSOAP_CONFIGURATION_TYPES 
# and QtSOAP_BUILD_TYPE will have values matching CMAKE_CONFIGURATION_TYPES 
# and CMAKE_BUILD_TYPE for that build tree (only one will ever be set).
SET(QtSOAP_CONFIGURATION_TYPES Debug;Release;MinSizeRel;RelWithDebInfo)
SET(QtSOAP_BUILD_TYPE )
