project(RayEngine LANGUAGES CXX)

file(GLOB coreFile ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/*.cpp)

add_library(${PROJECT_NAME}
  ${coreFile}
)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/repch.h)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/vendor/Raylib/src
  ${CMAKE_SOURCE_DIR}/vendor/glm
)


target_link_libraries(${PROJECT_NAME} PUBLIC
  raylib
  glm
)
