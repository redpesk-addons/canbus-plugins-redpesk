###########################################################################
# Copyright 2015, 2016, 2017 IoT.bzh
#
# author: Romain Forlot <romain.forlot@iot.bzh>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#	http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################
set(TARGET_NAME simrad-draft-signals)

	# Define targets
	ADD_LIBRARY(${TARGET_NAME} MODULE )

	# Create dedicated generate targets
	find_program(ccg can-config-generator REQUIRED )
	get_target_property(dir ${TARGET_NAME} SOURCE_DIR)
	add_custom_command( COMMAND ${ccg} -m ${dir}/${TARGET_NAME}.json -o ${TARGET_NAME}.cpp
						OUTPUT ${TARGET_NAME}.cpp
					)
	add_custom_target(generate_${TARGET_NAME})
	target_sources(${TARGET_NAME} PRIVATE ${TARGET_NAME}.cpp)

	# Alsa Plugin properties
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
		LABELS "PLUGIN"
		PREFIX ""
		SUFFIX ".ctlso"
		OUTPUT_NAME ${TARGET_NAME}
	)

set(TARGETS_LIST "${TARGETS_LIST};${TARGET_NAME}" CACHE INTERNAL "Enumarate targets" FORCE)

set(TARGET_NAME n2k-basic-signals)

	# Define targets
	ADD_LIBRARY(${TARGET_NAME} MODULE )

	# Create dedicated generate targets
	find_program(ccg can-config-generator REQUIRED)
	get_target_property(dir ${TARGET_NAME} SOURCE_DIR)
	add_custom_command( COMMAND ${ccg} -m ${dir}/${TARGET_NAME}.json -o ${TARGET_NAME}.cpp
						OUTPUT ${TARGET_NAME}.cpp
					)
	add_custom_target(generate_${TARGET_NAME})
	target_sources(${TARGET_NAME} PRIVATE ${TARGET_NAME}.cpp)

	# Alsa Plugin properties
	SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
		LABELS "PLUGIN"
		PREFIX ""
		SUFFIX ".ctlso"
		OUTPUT_NAME ${TARGET_NAME}
	)

set(TARGETS_LIST "${TARGETS_LIST};${TARGET_NAME}" CACHE INTERNAL "Enumarate targets" FORCE)
