# PitEngine
 
# Building - Windows
PitEngine uses premake as its build system.
Steps to build PitEngine:
 - Execute BuildProjectWin.bat
 - Open the PitEngine.sln
 - Due to premake not marking the pch.cpp precompiled headers as [Create]
   you have to search for every pch.cpp, go to properties/C/C++/Precompiled Headers and set
    - precompiled header to Create,
    - precompiled headerfile to nothing ("")
    - Press ok
 - If running the application throws an error like "could not find OptickCore.dll" then copy OpetickCore.dll to bin/[YOUR CONFIG]/[Editor/Sandbox]
    
# Building - other platforms
Steps to build PitEngine:
 - call in terminal (from .../PitEngine) vendor/premake/premake5.exe YOUR EDITOR OF CHOICE (look into premake doc.)
 - make shure that precompiled headers are setup right in your Editor of choice
