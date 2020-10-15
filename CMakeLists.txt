###########################################################################
# Copyright 2015 - 2018 IoT.bzh
#
# author: Romain Forlot <romain.forlot@iot.bzh>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.3)

# Project Info
# ------------------
set(PROJECT_NAME redpesk-can-low-level-plugins)
set(PROJECT_AUTHOR "Romain Forlot")
set(PROJECT_AUTHOR_MAIL "romain.forlot@iot.bzh")
set(PROJECT_LANGUAGES "C;CXX")

project(${PROJECT_NAME} DESCRIPTION "Expose CAN Low Level APIs through AGL Framework")

include(FindPkgConfig)
include(GNUInstallDirs)
include(CTest)
enable_testing()

set(CAN_LOW_LEVEL_BINDING_NAME "rp-can-low-level" CACHE STRING "Name of the CAN low level binding. Needed to install the config and plugin at the correct place")

add_compile_definitions("AFB_BINDING_VERSION=3")

# Compilation Mode (DEBUG, RELEASE)
# ----------------------------------
set(CMAKE_BUILD_TYPE "RELEASE" CACHE STRING "Default Build variant chosen. (Overwritten by cli if given)")

# Prefix path where will be installed the files
# Default: /usr/local (need root permission to write in)
# ------------------------------------------------------
set(INSTALL_PREFIX $ENV{HOME}/opt)

# Customize link option
# -----------------------------
list (APPEND link_libraries -pthread)

add_subdirectory("etc")

pkg_check_modules(lowcan REQUIRED rp-can-low-level)

add_link_options(-L${lowcan_LIBDIR})
link_libraries(low-can ${lowcan_LIBRARIES})

file(GLOB plugin_dirs LIST_DIRECTORIES true src/*)

set(TARGETS_LIST "" CACHE INTERNAL "Enumarate targets")

foreach(dir ${plugin_dirs})
	add_subdirectory(${dir} EXCLUDE_FROM_ALL)
endforeach()

foreach(target ${TARGETS_LIST})
get_target_property(pre ${target} PREFIX)
get_target_property(out ${target} OUTPUT_NAME)
get_target_property(suf ${target} SUFFIX)
get_target_property(bdir ${target} BINARY_DIR)
message("=== Including plugin: '${target}'")
# Create the dedicated install targets
add_custom_target(install_${target}
COMMAND install -m644 ${bdir}/${pre}${out}${suf} ${CMAKE_INSTALL_PREFIX}/${CAN_LOW_LEVEL_BINDING_NAME}/lib/plugins
COMMAND
)
add_dependencies(install_${target} ${target})
endforeach()
