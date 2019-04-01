set PROJDIR=%cd%
set OUTDIR=%PROJDIR%\release
set PROJNAME=invisOX
set RELEASEDIR=%PROJDIR%\out\release

mkdir %OUTDIR%
qmake -o %OUTDIR% -recursive %PROJNAME%.pro CONFIG+=release
if not %errorlevel% == 0 goto _BUILD_FAIL

cd %OUTDIR%
mingw32-make -j8
if not %errorlevel% == 0 goto _BUILD_FAIL

cd %RELEASEDIR%
del lua.exe luac.exe
del *.a
windeployqt --qmldir %PROJDIR%\ui %PROJNAME%.exe
if not %errorlevel% == 0 goto _BUILD_FAIL

echo "build success"
cd ..\


goto _BUILD_END

:_BUILD_FAIL
echo "build failed"

:_BUILD_END
