cd ../
cd ../

del bin\windows-x86_64\Debug\Editor\OptickCore.dll
del bin\windows-x86_64\Debug\Sandbox\OptickCore.dll
del bin\windows-x86_64\Debug\Editor\sndfile.dll
del bin\windows-x86_64\Debug\Sandbox\sndfile.dll

type vendor\optick\lib\x64\debug\OptickCore.dll >> bin\windows-x86_64\Debug\Editor\OptickCore.dll
type vendor\optick\lib\x64\debug\OptickCore.dll >> bin\windows-x86_64\Debug\Sandbox\OptickCore.dll
type vendor\sndfile\bin\sndfile.dll >> bin\windows-x86_64\Debug\Editor\sndfile.dll
type vendor\sndfile\bin\sndfile.dll >> bin\windows-x86_64\Debug\Sandbox\sndfile.dll

PAUSE