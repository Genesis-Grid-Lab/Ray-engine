project(RayEngine LANGUAGES CXX)

file(GLOB sourceFile ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(${PROJECT_NAME}
  ${sourceFile}
)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/repch.h)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/vendor/Raylib/src
)


target_link_libraries(${PROJECT_NAME} PUBLIC
  raylib
)
