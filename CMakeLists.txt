cmake_minimum_required(VERSION 3.12)
project(learnbgfx)

# include_directories(${CMAKE_SOURCE_DIR}/deps/glm-0.9.9.9)
# include_directories(${CMAKE_SOURCE_DIR}/deps/assimp/include)

# add_library(toy 
# 	common/shader.cc common/camera.cc src/glad.c common/lo_common.cc common/shape.cc
# 	common/material.cc common/model.cc common/mesh.cc common/scene.cc common/cubemap.cc
# 	common/instance_renderer.cc common/directional_light.cc)
# target_include_directories(toy PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)

set(dep_libs_win opengl32 legacy_stdio_definitions)
set(dep_libs_linux GL dl m assimp)
set(dep_libs_common toy glfw)

set(bx_dir ${CMAKE_SOURCE_DIR}/../bx)
set(bx_include_dir ${bx_dir}/include)

set(bimg_dir ${CMAKE_SOURCE_DIR}/../bimg)
set(bimg_include_dir ${bimg_dir}/include)

set(bgfx_dir ${CMAKE_SOURCE_DIR}/../bgfx)
set(bgfx_include_dir ${bgfx_dir}/include)
set(bgfx_lib_dir ${bgfx_dir}/bgfx/.build/win64_vs2019/bin)
set(bgfx_example_common_dir ${bgfx_dir}/examples/common)

include_directories(${bx_include_dir})
include_directories(${bx_include_dir}/compat/msvc)
include_directories(${bimg_include_dir})
include_directories(${bgfx_include_dir})
include_directories(${bgfx_example_common_dir})
include_directories(${bgfx_dir}/3rdparty)

ADD_LIBRARY(bgfx-example-common STATIC IMPORTED)
SET_TARGET_PROPERTIES(bgfx-example-common PROPERTIES IMPORTED_LOCATION ${bgfx_dir}/.build/win64_vs2019/bin/example-commonDebug.lib)

ADD_LIBRARY(bgfx-example-glue STATIC IMPORTED)
SET_TARGET_PROPERTIES(bgfx-example-glue PROPERTIES IMPORTED_LOCATION ${bgfx_dir}/.build/win64_vs2019/bin/example-glueDebug.lib)

ADD_LIBRARY(bgfx STATIC IMPORTED)
SET_TARGET_PROPERTIES(bgfx PROPERTIES IMPORTED_LOCATION ${bgfx_dir}/.build/win64_vs2019/bin/bgfxDebug.lib)

ADD_LIBRARY(bimg STATIC IMPORTED)
SET_TARGET_PROPERTIES(bimg PROPERTIES IMPORTED_LOCATION ${bgfx_dir}/.build/win64_vs2019/bin/bimgDebug.lib)

ADD_LIBRARY(bimg_decode STATIC IMPORTED)
SET_TARGET_PROPERTIES(bimg_decode PROPERTIES IMPORTED_LOCATION ${bgfx_dir}/.build/win64_vs2019/bin/bimg_decodeDebug.lib)

ADD_LIBRARY(bx STATIC IMPORTED)
SET_TARGET_PROPERTIES(bx PROPERTIES IMPORTED_LOCATION ${bgfx_dir}/.build/win64_vs2019/bin/bxDebug.lib)

set(dep_libs bgfx-example-common bgfx-example-glue bgfx bimg_decode bimg bx)

add_compile_definitions(ENTRY_CONFIG_IMPLEMENT_MAIN=1)

# ENTRY_CONFIG_IMPLEMENT_MAIN=1

if (WIN32)
	# ADD_LIBRARY(glfw STATIC IMPORTED)
	# SET_TARGET_PROPERTIES(glfw PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/deps/glfw-3.3.2.bin.WIN64/lib-vc2019/glfw3.lib)
	# include_directories(${CMAKE_SOURCE_DIR}/deps/glfw-3.3.2.bin.WIN64/include)

	# ADD_LIBRARY(assimp STATIC IMPORTED)
	# SET_TARGET_PROPERTIES(assimp PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/deps/assimp/lib/# assimp-vc142-mtd.lib)

	# set(dep_libs ${dep_libs_common} ${dep_libs_win} assimp)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /std:c++17")
else ()
  set(dep_libs ${dep_libs_common} ${dep_libs_linux})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif (WIN32)

macro(add_bgfx_target)
	add_executable(${ARGV0} ${ARGV1}/${ARGV0}.cpp)
	target_include_directories(${ARGV0} PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/common)
	target_link_libraries(${ARGV0} LINK_PUBLIC ${dep_libs})
	set_target_properties(${ARGV0} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
	set_target_properties(${ARGV0} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/runtime")
endmacro(add_bgfx_target)

add_bgfx_target(helloworld 01-helloworld)
add_bgfx_target(cube 02-cube)
add_bgfx_target(mesh 03-mesh)
add_bgfx_target(instancing 04-instancing)
