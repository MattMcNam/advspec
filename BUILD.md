Building AdvSpec plugin
==============================

Notice
-
AdvSpec is not yet available for Linux & OS X.

Requirements
-
* **[Windows]** Visual Studio or Visual Express C++
* cmake 2.6+
* hl2sdk-ob-valve - http://hg.alliedmods.net/hl2sdks/hl2sdk-ob-valve/

Building
-  
**Windows**  
`cmake -DHL2SDK=<path-to-hl2sdk...> -DLWS_DIR=<path-to-libwebsockets> -G "Visual Studio 10" ..`  
Then open in Visual Studio/C++ Express and build.
