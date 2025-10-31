project(RayEngine LANGUAGES CXX)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode.")
    # Add debug-specific flags or definitions here
    add_compile_definitions(-DRE_DEBUG)
else()
    message(STATUS "Building in Release or other mode.")
endif()

file(GLOB coreFile ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/*.cpp)
file(GLOB sceneFile ${CMAKE_CURRENT_SOURCE_DIR}/src/Scene/*.cpp)

add_library(${PROJECT_NAME}
  ${coreFile}
  ${sceneFile}
)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/repch.h)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/vendor/Raylib/src
  ${CMAKE_SOURCE_DIR}/vendor/glm
   ${CMAKE_SOURCE_DIR}/vendor/imgui
  ${CMAKE_SOURCE_DIR}/vendor/imgui/backends
  ${CMAKE_SOURCE_DIR}/vendor/entt/src/entt
  ${CMAKE_SOURCE_DIR}/Vendor/rImGui
)


target_link_libraries(${PROJECT_NAME} PUBLIC
  raylib
  glm
  EnTT
  imgui
  rImGui
)


# copy data
if(EXISTS ${CMAKE_SOURCE_DIR}/Data)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND 
        ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_SOURCE_DIR}/Data
        ${EXECUTABLE_OUTPUT_PATH}/Data
    )
else()
    message(WARNING "[WARNING] no data directory!")
  endif()

# copy resources
if(EXISTS ${CMAKE_SOURCE_DIR}/Resources)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND 
        ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_SOURCE_DIR}/Resources
        # ${EXECUTABLE_OUTPUT_PATH}/Resources
	$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources
    )
else()
    message(WARNING "[WARNING] no resources directory!")
endif()