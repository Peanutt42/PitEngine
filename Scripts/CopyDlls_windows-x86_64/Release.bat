cd ../
cd ../

del bin\windows-x86_64\Release\Editor\OptickCore.dll
del bin\windows-x86_64\Release\Sandbox\OptickCore.dll
del bin\windows-x86_64\Release\Editor\sndfile.dll
del bin\windows-x86_64\Release\Sandbox\sndfile.dll

type vendor\optick\lib\x64\release\OptickCore.dll >> bin\windows-x86_64\Release\Editor\OptickCore.dll
type vendor\optick\lib\x64\release\OptickCore.dll >> bin\windows-x86_64\Release\Sandbox\OptickCore.dll
type vendor\sndfile\bin\sndfile.dll >> bin\windows-x86_64\Release\Editor\sndfile.dll
type vendor\sndfile\bin\sndfile.dll >> bin\windows-x86_64\Release\Sandbox\sndfile.dll

PAUSE