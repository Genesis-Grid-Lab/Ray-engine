project(App LANGUAGES CXX)

file(GLOB sourceFile  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME}
  ${sourceFile}
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/engine/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  RayEngine
)
