cd ../

del bin\Debug-windows-x86_64\Editor\OptickCore.dll
del bin\Debug-windows-x86_64\Sandbox\OptickCore.dll
del bin\Debug-windows-x86_64\Editor\sndfile.dll
del bin\Debug-windows-x86_64\Sandbox\sndfile.dll

type vendor\optick\lib\x64\debug\OptickCore.dll >> bin\Debug-windows-x86_64\Editor\OptickCore.dll
type vendor\optick\lib\x64\debug\OptickCore.dll >> bin\Debug-windows-x86_64\Sandbox\OptickCore.dll
type vendor\sndfile\bin\sndfile.dll >> bin\Debug-windows-x86_64\Editor\sndfile.dll
type vendor\sndfile\bin\sndfile.dll >> bin\Debug-windows-x86_64\Sandbox\sndfile.dll

PAUSE