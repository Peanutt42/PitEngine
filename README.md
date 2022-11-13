# PitEngine
 
> **Warning**
> Very Experimental!
> PitEngine is in super rought development right now so many systems
  aren't ready for any project as mostly everything is in early development
 
# Building - Windows
PitEngine uses premake as its build system.
Steps to build PitEngine:
 - Install vcpkg (https://vcpkg.io/en/getting-started.html) then run:
   - vcpkg install physx:x64-windows and vcpkg install openal-soft:x64-windows
 - Execute Scripts/BuildProjectWin.bat
 - Open the PitEngine.sln
 - Due to premake not marking the pch.cpp precompiled headers as [Create]
   you have to search for every pch.cpp, go to properties/C/C++/Precompiled Headers and set
    - precompiled header to Create,
    - precompiled headerfile to nothing ("")
    - Press ok
