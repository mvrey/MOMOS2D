# MOMOS2D
A fully functional 2D game engine written in C++ that uses GLFW for rendering. 
It also includes additional capabilities such as physics simulation (chipmunk physics), local database storing (sqlite) and advanced math calculation libraries (glm).
All external dependencies can be found at src\deps.

In order to build the project download the GENie executable from https://github.com/bkaradzic/GENie. Then run genie.bat.
This project compiles to a .lib static library, although it also includes a small demo in case you want to run it without any external code. 
To do so, change the project type from library to executable and run main.cc.
