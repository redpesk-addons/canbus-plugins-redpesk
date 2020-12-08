#######################################################################
# Copyright (C) 2019-2020 IoT.bzh Company
# Contact: https://www.iot.bzh/licensing
# Author : Romain Forlot <romain.forlot@iot.bzh>
#
# This file is part of the canbus-plugins-redpesk of the RedPesk project.
#
# $RP_BEGIN_LICENSE$
# Commercial License Usage
# Licensees holding valid commercial IoT.bzh licenses may use this file in
# accordance with the commercial license agreement provided with the
# Software or, alternatively, in accordance with the terms contained in
# a written agreement between you and The IoT.bzh Company. For licensing terms
# and conditions see https://www.iot.bzh/terms-conditions. For further
# information use the contact form at https://www.iot.bzh/contact.
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU General
# Public license version 3. This license is as published by the Free Software
# Foundation and appearing in the file LICENSE.GPLv3 included in the packaging
# of this file. Please review the following information to ensure the GNU
# General Public License requirements will be met
# https://www.gnu.org/licenses/gpl-3.0.html.
# $RP_END_LICENSE$
#
# canbus-plugins-redpesk: plugins for the canbus-binding
########################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.3)

# Project Info
# ------------------
set(PROJECT_NAME canbus-plugins-redpesk)
set(PROJECT_AUTHOR "Romain Forlot")
set(PROJECT_AUTHOR_MAIL "romain.forlot@iot.bzh")
set(PROJECT_LANGUAGES "C;CXX")

project(${PROJECT_NAME} DESCRIPTION "Expose CAN Low Level APIs through AGL Framework")

include(FindPkgConfig)
include(GNUInstallDirs)
include(CTest)
enable_testing()

set(CAN_LOW_LEVEL_BINDING_NAME "canbus-binding" CACHE STRING "Name of the CAN low level binding. Needed to install the config and plugin at the correct place")

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

pkg_check_modules(lowcan REQUIRED canbus-binding)
pkg_check_modules(helpers REQUIRED afb-libhelpers)
pkg_check_modules(controller REQUIRED afb-libcontroller)

include_directories("${lowcan_INCLUDEDIR}")
include_directories("${helpers_INCLUDEDIR}")
include_directories("${controller_INCLUDEDIR}")

link_libraries(canbus ${lowcan_LIBRARIES} -L${lowcan_LIBDIR})

link_libraries(${helpers_LIBRARIES} -L${helpers_LIBDIR})

link_libraries(${controller_LIBRARIES} -L${controller_LIBDIR})

file(GLOB plugin_dirs LIST_DIRECTORIES true src/*)

set(TARGETS_LIST "" CACHE INTERNAL "Enumarate targets")

foreach(dir ${plugin_dirs})
	add_subdirectory(${dir} EXCLUDE_FROM_ALL)
endforeach()

foreach(target ${TARGETS_LIST})
	target_compile_definitions(${target} PRIVATE AFB_BINDING_VERSION=3)
	get_target_property(pre ${target} PREFIX)
	get_target_property(out ${target} OUTPUT_NAME)
	get_target_property(suf ${target} SUFFIX)
	get_target_property(bdir ${target} BINARY_DIR)
	message("=== Including plugin: '${target}'")
	# Create the dedicated install targets
	add_custom_target(install_${target}
		COMMAND install -d \${DESTDIR}${CMAKE_INSTALL_PREFIX}/${CAN_LOW_LEVEL_BINDING_NAME}/lib/plugins
		COMMAND install ${bdir}/${pre}${out}${suf} \${DESTDIR}${CMAKE_INSTALL_PREFIX}/${CAN_LOW_LEVEL_BINDING_NAME}/lib/plugins
		COMMAND
	)
	add_dependencies(install_${target} ${target})
endforeach()
