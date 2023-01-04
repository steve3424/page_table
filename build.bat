@echo off

rem -FAs assembly output
set CompilerFlags=-nologo -MT -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4127 -wd4201 -wd4281 -wd4100 -wd4189 -wd4505 -wd4706 -DDEBUG=1 -FC -Z7
set LinkerFlags=-incremental:no 

mkdir build
pushd build

REM 64-bit build
del *.pdb > NUL 2> NUL
cl %CompilerFlags% ..\src\main.c -Fmmain.map /link -opt:ref user32.lib winmm.lib %LinkerFlags%
popd 
