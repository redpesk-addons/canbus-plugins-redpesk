#######################################################################
# Copyright (C) 2019-2025 IoT.bzh Company
# Contact: https://www.iot.bzh/licensing
# Author: : Romain Forlot <romain.forlot@iot.bzh>
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

set(TARGET_NAME gps-signals)

	# Define targets
	ADD_LIBRARY(${TARGET_NAME} MODULE )

	# Create dedicated generate targets
	find_program(ccg can-config-generator REQUIRED)
	get_target_property(dir ${TARGET_NAME} SOURCE_DIR)
	add_custom_command( COMMAND ${ccg} -m ${dir}/${TARGET_NAME}.json -o ${TARGET_NAME}.cpp
						OUTPUT ${TARGET_NAME}.cpp
					)
	add_custom_target(generate_${TARGET_NAME} DEPENDS ${TARGET_NAME}.cpp)
	target_sources(${TARGET_NAME} PRIVATE ${TARGET_NAME}.cpp)

	# Alsa Plugin properties
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
		LABELS "PLUGIN"
		PREFIX ""
		SUFFIX ".ctlso"
		OUTPUT_NAME ${TARGET_NAME}
	)

set(TARGETS_LIST "${TARGETS_LIST};${TARGET_NAME}" CACHE INTERNAL "Enumarate targets" FORCE)
