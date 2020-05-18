cmake_minimum_required( VERSION 3.10 )

set( CMAKE_CXX_STANDARD 11 )

project( NPendulum CXX )

file( GLOB_RECURSE _source_list  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	*.cpp* *.h* *.inl* *.hpp*
	${DEPENDENCIES_PATH}/glad/src/glad.c
	${DEPENDENCIES_PATH}/imgui/imconfig.h
	${DEPENDENCIES_PATH}/imgui/imgui_internal.h
	${DEPENDENCIES_PATH}/imgui/imgui.h
	${DEPENDENCIES_PATH}/imgui/imgui.cpp
	${DEPENDENCIES_PATH}/imgui/imgui_draw.cpp
	${DEPENDENCIES_PATH}/imgui/imgui_widgets.cpp
	${DEPENDENCIES_PATH}/imgui/examples/imgui_impl_glfw.h
	${DEPENDENCIES_PATH}/imgui/examples/imgui_impl_glfw.cpp
	${DEPENDENCIES_PATH}/imgui/examples/imgui_impl_opengl3.h
	${DEPENDENCIES_PATH}/imgui/examples/imgui_impl_opengl3.cpp
	)

find_package( OpenGL REQUIRED )

foreach( _source IN ITEMS ${_source_list} )
    get_filename_component( _source_path "${_source}" PATH )
    string( REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}" )
    string( REPLACE "/" "\\" _group_path "${_group_path}" )
    source_group( "${_group_path}" FILES "${_source}" )
endforeach()

include_directories( . )
include_directories( .. )
include_directories( ${DEPENDENCIES_PATH}/imgui )
include_directories( ${DEPENDENCIES_PATH}/glad/include )
include_directories( ${DEPENDENCIES_PATH}/glfw/include )
include_directories( ${DEPENDENCIES_PATH}/imgui/examples )
include_directories( ${SOLUTION_DIR}/src/ )

add_definitions( -DIMGUI_IMPL_OPENGL_LOADER_GLAD )

add_executable( NPendulum ${_source_list} )

target_link_libraries( NPendulum ${OPENGL_gl_LIBRARY} )
target_link_libraries( NPendulum ${OPENGL_glu_LIBRARY} )
target_link_libraries( NPendulum glfw ${GLFW_LIBRARIES} )

set_target_properties(
    NPendulum PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${SOLUTION_DIR}/WorkingDir" )

set( CMAKE_PDB_OUTPUT_DIRECTORY "${SOLUTION_DIR}/WorkingDir" )
