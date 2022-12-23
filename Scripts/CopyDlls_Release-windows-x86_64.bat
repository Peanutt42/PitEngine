cd ../

del bin\Release-windows-x86_64\Editor\OptickCore.dll
del bin\Release-windows-x86_64\Sandbox\OptickCore.dll
del bin\Release-windows-x86_64\Editor\sndfile.dll
del bin\Release-windows-x86_64\Sandbox\sndfile.dll

type vendor\optick\lib\x64\release\OptickCore.dll >> bin\Release-windows-x86_64\Editor\OptickCore.dll
type vendor\optick\lib\x64\release\OptickCore.dll >> bin\Release-windows-x86_64\Sandbox\OptickCore.dll
type vendor\sndfile\bin\sndfile.dll >> bin\Release-windows-x86_64\Editor\sndfile.dll
type vendor\sndfile\bin\sndfile.dll >> bin\Release-windows-x86_64\Sandbox\sndfile.dll

PAUSE