@echo off
setlocal

rem set ROOT=C:\dev\bluebar\cpp\projects
set COMPILER="%ProgramFiles(x86)%\Microsoft Visual Studio 9.0\Common7\IDE\devenv.com"
set PROJ_NAME=Links2
set BASE_FILE=%PROJ_NAME%.exe
set SIGCHECK="tools\sigcheck.exe"
set LOG_FILE=build.log

rem Init log file
echo. > %LOG_FILE%

rem Test if running as admin (result is unconclusive but it's a good place to start)
set ME=%~nx0
set TEMP_FILE=%RANDOM%%RANDOM%.tmp
xcopy /y %ME% "%ProgramFiles(x86)%\%TEMP_FILE%"* >> %LOG_FILE% 2>&1
if %errorlevel% == 0 (
	del /f/q "%ProgramFiles(x86)%\%TEMP_FILE%"

) else (
	rem Warning
	echo -----------------------------------
	echo Make sure you are running as admin!
	echo -----------------------------------
	exit /b
)

rem ---------------------------------------------------------------------------
rem Clean output directories
del /f/q debug\*.exe >> %LOG_FILE% 2>&1
del /f/q release\*.exe >> %LOG_FILE% 2>&1

rem ---------------------------------------------------------------------------
echo Building Links2 - Newsgeek (Debug) . . .
set CL=/DBUILD_NEWSGEEK
%COMPILER% /rebuild debug Links2.sln >> %LOG_FILE%
for /f "tokens=1-3" %%i in ('"%SIGCHECK% debug\%BASE_FILE%"') do (if "%%i %%j"=="File version:" set FILE_VERSION=%%k)
set BRANDED_FILE=%PROJ_NAME%_newsgeek_v%FILE_VERSION%.exe
mv -f debug\%BASE_FILE% debug\%BRANDED_FILE%

echo Building Links2 - Newsgeek (Release) . . .
set CL=/DBUILD_NEWSGEEK
%COMPILER% /rebuild release Links2.sln >> %LOG_FILE%
for /f "tokens=1-3" %%i in ('"%SIGCHECK% debug\%BASE_FILE%"') do (if "%%i %%j"=="File version:" set FILE_VERSION=%%k)
set BRANDED_FILE=%PROJ_NAME%_newsgeek_v%FILE_VERSION%.exe
mv -f release\%BASE_FILE% release\%BRANDED_FILE%

rem ---------------------------------------------------------------------------
echo Building Links2 - Indiantimes (Debug) . . .
set CL=/DBUILD_INDIANTIMES
%COMPILER% /rebuild debug Links2.sln >> %LOG_FILE%
for /f "tokens=1-3" %%i in ('"%SIGCHECK% debug\%BASE_FILE%"') do (if "%%i %%j"=="File version:" set FILE_VERSION=%%k)
set BRANDED_FILE=%PROJ_NAME%_indiantimes_v%FILE_VERSION%.exe
mv -f debug\%BASE_FILE% debug\%BRANDED_FILE%

echo Building Links2 - Indiantimes (Release) . . .
set CL=/DBUILD_INDIANTIMES
%COMPILER% /rebuild release Links2.sln >> %LOG_FILE%
for /f "tokens=1-3" %%i in ('"%SIGCHECK% debug\%BASE_FILE%"') do (if "%%i %%j"=="File version:" set FILE_VERSION=%%k)
set BRANDED_FILE=%PROJ_NAME%_indiantimes_v%FILE_VERSION%.exe
mv -f release\%BASE_FILE% release\%BRANDED_FILE%

rem ---------------------------------------------------------------------------
echo Building Links2 - BBnews (Debug) . . .
set CL=/DBUILD_BBNEWS
%COMPILER% /rebuild debug Links2.sln >> %LOG_FILE%
for /f "tokens=1-3" %%i in ('"%SIGCHECK% debug\%BASE_FILE%"') do (if "%%i %%j"=="File version:" set FILE_VERSION=%%k)
set BRANDED_FILE=%PROJ_NAME%_bbnews_v%FILE_VERSION%.exe
mv -f debug\%BASE_FILE% debug\%BRANDED_FILE%

echo Building Links2 - BBnews (Release) . . .
set CL=/DBUILD_BBNEWS
%COMPILER% /rebuild release Links2.sln >> %LOG_FILE%
for /f "tokens=1-3" %%i in ('"%SIGCHECK% debug\%BASE_FILE%"') do (if "%%i %%j"=="File version:" set FILE_VERSION=%%k)
set BRANDED_FILE=%PROJ_NAME%_bbnews_v%FILE_VERSION%.exe
mv -f release\%BASE_FILE% release\%BRANDED_FILE%

rem ---------------------------------------------------------------------------
echo Building Links2 - Timesindia (Debug) . . .
set CL=/DBUILD_TIMESINDIA
%COMPILER% /rebuild debug Links2.sln >> %LOG_FILE%
for /f "tokens=1-3" %%i in ('"%SIGCHECK% debug\%BASE_FILE%"') do (if "%%i %%j"=="File version:" set FILE_VERSION=%%k)
set BRANDED_FILE=%PROJ_NAME%_timesindia_v%FILE_VERSION%.exe
mv -f debug\%BASE_FILE% debug\%BRANDED_FILE%

echo Building Links2 - Timesindia (Release) . . .
set CL=/DBUILD_TIMESINDIA
%COMPILER% /rebuild release Links2.sln >> %LOG_FILE%
for /f "tokens=1-3" %%i in ('"%SIGCHECK% debug\%BASE_FILE%"') do (if "%%i %%j"=="File version:" set FILE_VERSION=%%k)
set BRANDED_FILE=%PROJ_NAME%_timesindia_v%FILE_VERSION%.exe
mv -f release\%BASE_FILE% release\%BRANDED_FILE%

rem ---------------------------------------------------------------------------
echo Building Links2 - Olnews (Debug) . . .
set CL=/DBUILD_OLNEWS
%COMPILER% /rebuild debug Links2.sln >> %LOG_FILE%
for /f "tokens=1-3" %%i in ('"%SIGCHECK% debug\%BASE_FILE%"') do (if "%%i %%j"=="File version:" set FILE_VERSION=%%k)
set BRANDED_FILE=%PROJ_NAME%_olnews_v%FILE_VERSION%.exe
mv -f debug\%BASE_FILE% debug\%BRANDED_FILE%

echo Building Links2 - Olnews (Release) . . .
set CL=/DBUILD_OLNEWS
%COMPILER% /rebuild release Links2.sln >> %LOG_FILE%
for /f "tokens=1-3" %%i in ('"%SIGCHECK% debug\%BASE_FILE%"') do (if "%%i %%j"=="File version:" set FILE_VERSION=%%k)
set BRANDED_FILE=%PROJ_NAME%_olnews_v%FILE_VERSION%.exe
mv -f release\%BASE_FILE% release\%BRANDED_FILE%


endlocal
@echo on
