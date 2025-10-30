@echo off

rem output directory

set target="Targets\%1"

rem generate cmake build files

cmake -S . -B %target% -DCMAKE_BUILD_TYPE=%1 -DUSE_FETCH_GLFW="true" -DWINDOW_BACKEND_GLFW=true

rem compile cmake build files

cmake --build %target% --config %1