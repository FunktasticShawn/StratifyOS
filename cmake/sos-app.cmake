
set(SOS_BUILD_ARCH ${SOS_APP_ARCH})
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-build-flags.cmake)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-sdk.cmake)
sos_get_git_hash()

if(SOS_APP_VERBOSE)
	set(CMAKE_VERBOSE_MAKEFILE 1)
endif()

set(BUILD_NAME build_release_${SOS_APP_ARCH})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${BUILD_NAME})

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

if(SOS_APP_OPTIMIZATION)
	set(BUILD_OPTIMIZATION ${SOS_APP_OPTIMIZATION})
else()
	set(BUILD_OPTIMIZATION "-Os")
endif()


if(SOS_APP_ARCH STREQUAL "link")
	set(BUILD_TARGET ${SOS_APP_NAME})
	set(BUILD_LIBRARIES ${SOS_APP_LIBRARIES} api sos_link)
	set(BUILD_FLAGS ${BUILD_OPTIMIZATION})
	set(LINKER_FLAGS "-L${TOOLCHAIN_LIB_DIR}")

	add_executable(${BUILD_TARGET} ${SOS_APP_SOURCELIST} StratifySettings.json StratifyLocalSettings.json)

	target_compile_definitions(${BUILD_TARGET} PUBLIC __${SOS_APP_ARCH} ${SOS_APP_DEFINITIONS} SOS_GIT_HASH="${SOS_GIT_HASH}")

else()
	set(BUILD_TARGET ${SOS_APP_NAME}_${SOS_APP_ARCH}.elf)
	set(BUILD_LIBRARIES ${SOS_APP_LIBRARIES} api sos_crt)
	set(BUILD_FLAGS -mlong-calls ${SOS_BUILD_FLOAT_OPTIONS} ${BUILD_OPTIMIZATION})
	set(LINKER_FLAGS "-nostartfiles -nostdlib -L${TOOLCHAIN_LIB_DIR}/${SOS_BUILD_INSTALL_DIR}/${SOS_BUILD_FLOAT_DIR}/. -Wl,-Map,${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${SOS_APP_NAME}_${SOS_APP_ARCH}.map,--defsym=_app_ram_size=${SOS_APP_RAM_SIZE},--gc-sections -Tldscripts/app.ld -u crt")

	add_executable(${BUILD_TARGET} ${SOS_APP_SOURCELIST} StratifySettings.json StratifyLocalSettings.json)
	add_custom_target(bin_${SOS_APP_NAME}_${SOS_APP_ARCH} DEPENDS ${BUILD_TARGET} COMMAND ${CMAKE_OBJCOPY} -j .text -j .data -O binary ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BUILD_TARGET} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${SOS_APP_NAME})
	add_custom_target(asm_${SOS_APP_NAME}_${SOS_APP_ARCH} DEPENDS bin_${SOS_APP_NAME}_${SOS_APP_ARCH} COMMAND ${CMAKE_OBJDUMP} -S -j .text -j .priv_code -j .data -j .bss -j .sysmem -d ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BUILD_TARGET} > ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${SOS_APP_NAME}.lst)
	add_custom_target(size_${SOS_APP_ARCH} DEPENDS asm_${SOS_APP_NAME}_${SOS_APP_ARCH} COMMAND ${CMAKE_SIZE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BUILD_TARGET})
	add_custom_target(${SOS_APP_ARCH} ALL DEPENDS size_${SOS_APP_ARCH})

	target_compile_definitions(${BUILD_TARGET} PUBLIC __${SOS_APP_ARCH} __StratifyOS__ ${SOS_APP_DEFINITIONS} SOS_GIT_HASH="${SOS_GIT_HASH}")
endif()

target_link_libraries(${BUILD_TARGET} ${BUILD_LIBRARIES})
set_target_properties(${BUILD_TARGET} PROPERTIES LINK_FLAGS ${LINKER_FLAGS})
target_compile_options(${BUILD_TARGET} PUBLIC ${BUILD_FLAGS})

if(SOS_APP_INCLUDE_DIRECTORIES)
	target_include_directories(${BUILD_TARGET} PUBLIC ${SOS_APP_INCLUDE_DIRECTORIES})
endif()

target_include_directories(${BUILD_TARGET} PUBLIC ${SOS_BUILD_SYSTEM_INCLUDES})
