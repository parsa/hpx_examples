::rd /s /q cmake-build-debug
cmake -C cmake_cache.cmake -H. -Bcmake-build-debug -G"Visual Studio 15 2017 Win64"
::pause

cmake --build cmake-build-debug --config Debug --target app1_exe

pause