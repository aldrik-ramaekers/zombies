call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
@set OUT_DIR=build\\
@set OUT_EXE=zombies
@set SOURCES=main.c
@set LIBS=opengl32.lib Advapi32.lib Shell32.lib Ole32.lib User32.lib Pathcch.lib SDL2_mixer.lib
@set FLAGS=/DMODE_DEBUG /DEBUG:FULL /Ob0 /MT /Oy- /Zi


cl  /std:c11 /analyze:stacksize 40000 /nologo %FLAGS% /w /MD /EHsc /Isrc /utf-8 %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fd%OUT_DIR%/vc140.pdb /Fo%OUT_DIR%/ /link  /LIBPATH:libs/msvc %LIBS%
if "%1"=="-r" call "%OUT_DIR%/%OUT_EXE%.exe"
if "%1"=="-d" call devenv "%OUT_DIR%/%OUT_EXE%.exe"
