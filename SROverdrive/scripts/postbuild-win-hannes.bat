@echo off

REM - CALL "$(SolutionDir)scripts\postbuild-win.bat" "$(TargetExt)" "$(BINARY_NAME)" "$(Configuration)" "$(Platform)" "$(COPY_VST2)" "$(TargetPath)" "$(VST2_32_PATH)" "$(VST2_64_PATH)" "$(VST3_32_PATH)" "$(VST3_64_PATH)" "$(AAX_32_PATH)" "$(AAX_64_PATH)" "$(VST2_32_PATH_DEBUG)" "$(VST2_64_PATH_DEBUG)" "$(VST3_32_PATH_DEBUG)" "$(VST3_64_PATH_DEBUG)" "$(AAX_32_PATH_DEBUG)" "$(AAX_64_PATH_DEBUG)" "$(BUILD_DIR)" "$(VST_ICON)" "$(AAX_ICON)" "
REM $(CREATE_BUNDLE_SCRIPT)"

set FORMAT=%1
set NAME=%2
set CONFIGURATION=%3
set PLATFORM=%4
set COPY_VST2=%5
set BUILT_BINARY=%6
set VST2_32_PATH=%7
set VST2_64_PATH=%8 
set VST3_32_PATH=%9
shift
shift 
shift
shift
shift 
shift
shift
shift
shift
set VST3_64_PATH=%1
set AAX_32_PATH=%2
set AAX_64_PATH=%3
set VST2_32_PATH_DEBUG=%4
set VST2_64_PATH_DEBUG=%5 
set VST3_32_PATH_DEBUG=%6
set VST3_64_PATH_DEBUG=%7
set AAX_32_PATH_DEBUG=%8
set AAX_64_PATH_DEBUG=%9
shift
shift
shift
shift
set BUILD_DIR=%6
set VST_ICON=%7
set AAX_ICON=%8
set CREATE_BUNDLE_SCRIPT=%9

echo POSTBUILD SCRIPT VARIABLES -----------------------------------------------------
echo FORMAT %FORMAT% 
echo NAME %NAME% 
echo PLATFORM %PLATFORM% 
echo CONFIGURATION %CONFIGURATION%
echo COPY_VST2 %COPY_VST2% 
echo BUILT_BINARY %BUILT_BINARY% 
echo VST2_32_PATH %VST2_32_PATH% 
echo VST2_64_PATH %VST2_64_PATH% 
echo VST3_32_PATH %VST3_32_PATH% 
echo VST3_64_PATH %VST3_64_PATH%
echo VST2_32_PATH_DEBUG %VST2_32_PATH_DEBUG% 
echo VST2_64_PATH_DEBUG %VST2_64_PATH_DEBUG% 
echo VST3_32_PATH_DEBUG %VST3_32_PATH_DEBUG% 
echo VST3_64_PATH_DEBUG %VST3_64_PATH_DEBUG%
echo BUILD_DIR %BUILD_DIR%
echo VST_ICON %VST_ICON% 
echo AAX_ICON %AAX_ICON% 

if %FORMAT% == ".exe" (
  set FORMATSTRING="Standalone"
)
if %FORMAT% == ".dll" (
  set FORMATSTRING="VST2"
)
if %FORMAT% == ".vst3" (
  set FORMATSTRING="VST3"
)
if %FORMAT% == ".aaxplugin" (
  set FORMATSTRING="AAX"
)

echo FORMATSTRING %FORMATSTRING%
echo CREATE_BUNDLE_SCRIPT %CREATE_BUNDLE_SCRIPT%
echo END POSTBUILD SCRIPT VARIABLES -----------------------------------------------------



if %CONFIGURATION% == "Release" (
if %PLATFORM% == "Win32" (
  if %FORMAT% == ".exe" (
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%_%PLATFORM%.exe
  )

  if %FORMAT% == ".dll" (
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%_%PLATFORM%.dll
  )
  
  if %FORMAT% == ".dll" (
    if %COPY_VST2% == "1" (
      echo copying %PLATFORM% binary to %PLATFORM% %FORMATSTRING% Plugins %CONFIGURATION% folder ... 
      copy /y %BUILT_BINARY% %VST2_32_PATH%
    ) else (
      echo not copying %PLATFORM% %FORMATSTRING% %CONFIGURATION% binary
    )
  )
  
  if %FORMAT% == ".vst3" (
    echo copying %PLATFORM% binary to %FORMATSTRING% %CONFIGURATION% BUNDLE ..
    call %CREATE_BUNDLE_SCRIPT% %BUILD_DIR%\%NAME%.vst3 %VST_ICON% %FORMAT%
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%.vst3\Contents\x86-win
    if exist %VST3_32_PATH% ( 
      echo copying VST3 bundle to %PLATFORM% %FORMATSTRING% Plugins %CONFIGURATION% folder ...
      call %CREATE_BUNDLE_SCRIPT% %VST3_32_PATH%\%NAME%.vst3 %VST_ICON% %FORMAT%
      xcopy /E /H /Y %BUILD_DIR%\%NAME%.vst3\Contents\*  %VST3_32_PATH%\%NAME%.vst3\Contents\
    ) else (
      echo No %PLATFORM% %FORMATSTRING% %CONFIGURATION% binary folder specified
    )
  )
  
  if %FORMAT% == ".aaxplugin" (
    echo copying %PLATFORM% binary to %FORMATSTRING% BUNDLE ..
    call %CREATE_BUNDLE_SCRIPT% %BUILD_DIR%\%NAME%.aaxplugin %AAX_ICON% %FORMAT%
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%.aaxplugin\Contents\Win32
    echo copying %PLATFORM% bundle to %PLATFORM% %FORMATSTRING% Plugins %CONFIGURATION% folder ... 
    call %CREATE_BUNDLE_SCRIPT% %BUILD_DIR%\%NAME%.aaxplugin %AAX_ICON% %FORMAT%
    xcopy /E /H /Y %BUILD_DIR%\%NAME%.aaxplugin\Contents\* %AAX_32_PATH%\%NAME%.aaxplugin\Contents\
  )
)

if %PLATFORM% == "x64" (
  if not exist "%ProgramFiles(x86)%" (
    echo "This batch script fails on 32 bit windows... edit accordingly"
  )

  if %FORMAT% == ".exe" (
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%_%PLATFORM%.exe
  )

  if %FORMAT% == ".dll" (
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%_%PLATFORM%.dll
  )
  
  if %FORMAT% == ".dll" (
    if %COPY_VST2% == "1" (
      echo copying %PLATFORM% binary to %PLATFORM% %FORMATSTRING% Plugins %CONFIGURATION% folder ... 
      copy /y %BUILT_BINARY% %VST2_64_PATH%
    ) else (
      echo not copying %PLATFORM% %FORMATSTRING% %CONFIGURATION% binary
    )
  )
  
  if %FORMAT% == ".vst3" (
    echo copying %PLATFORM% binary to %FORMATSTRING% %CONFIGURATION% BUNDLE ...
    call %CREATE_BUNDLE_SCRIPT% %BUILD_DIR%\%NAME%.vst3 %VST_ICON% %FORMAT%
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%.vst3\Contents\x86_64-win
    if exist %VST3_64_PATH% (
      echo copying %FORMATSTRING% bundle to %PLATFORM% %FORMATSTRING% Plugins %CONFIGURATION% folder ...
      call %CREATE_BUNDLE_SCRIPT% %VST3_64_PATH%\%NAME%.vst3 %VST_ICON% %FORMAT%
      xcopy /E /H /Y %BUILD_DIR%\%NAME%.vst3\Contents\*  %VST3_64_PATH%\%NAME%.vst3\Contents\
    ) else (
      echo No %PLATFORM% %FORMATSTRING% %CONFIGURATION% binary folder specified
    )
  )
  
  if %FORMAT% == ".aaxplugin" (
    echo copying %PLATFORM% binary to %FORMATSTRING% %CONFIGURATION% BUNDLE ...
    call %CREATE_BUNDLE_SCRIPT% %BUILD_DIR%\%NAME%.aaxplugin %AAX_ICON% %FORMAT%
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%.aaxplugin\Contents\x64
    echo copying %PLATFORM% bundle to %PLATFORM% %FORMATSTRING% Plugins %CONFIGURATION% folder ... 
    call %CREATE_BUNDLE_SCRIPT% %BUILD_DIR%\%NAME%.aaxplugin %AAX_ICON% %FORMAT%
    xcopy /E /H /Y %BUILD_DIR%\%NAME%.aaxplugin\Contents\* %AAX_64_PATH%\%NAME%.aaxplugin\Contents\
  )
)
)
if %CONFIGURATION% == "Debug" (
if %PLATFORM% == "Win32" (
  if %FORMAT% == ".exe" (
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%_%PLATFORM%.exe
  )

  if %FORMAT% == ".dll" (
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%_%PLATFORM%.dll
  )
  
  if %FORMAT% == ".dll" (
    if %COPY_VST2% == "1" (
      echo copying %PLATFORM% binary to %PLATFORM% %FORMATSTRING% Plugins %CONFIGURATION% folder ... 
      copy /y %BUILT_BINARY% %VST2_32_PATH_DEBUG%
    ) else (
      echo not copying %PLATFORM% %FORMATSTRING% %CONFIGURATION% binary
    )
  )
  
  if %FORMAT% == ".vst3" (
    echo copying %PLATFORM% binary to %FORMATSTRING% BUNDLE ..
    call %CREATE_BUNDLE_SCRIPT% %BUILD_DIR%\%NAME%.vst3 %VST_ICON% %FORMAT%
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%.vst3\Contents\x86-win
    if exist %VST3_32_PATH_DEBUG% ( 
      echo copying %FORMATSTRING% bundle to %PLATFORM% %FORMATSTRING% Plugins %CONFIGURATION% folder ...
      call %CREATE_BUNDLE_SCRIPT% %VST3_32_PATH_DEBUG%\%NAME%.vst3 %VST_ICON% %FORMAT%
      xcopy /E /H /Y %BUILD_DIR%\%NAME%.vst3\Contents\*  %VST3_32_PATH_DEBUG%\%NAME%.vst3\Contents\
    ) else (
      echo No %PLATFORM% %FORMATSTRING% %CONFIGURATION% binary folder specified
    )
  )
  
  if %FORMAT% == ".aaxplugin" (
    echo copying %PLATFORM% binary to %FORMATSTRING% %CONFIGURATION% BUNDLE ..
    call %CREATE_BUNDLE_SCRIPT% %BUILD_DIR%\%NAME%.aaxplugin %AAX_ICON% %FORMAT%
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%.aaxplugin\Contents\Win32
    echo copying %PLATFORM% bundle to %PLATFORM% %FORMATSTRING% Plugins %CONFIGURATION% folder ... 
    call %CREATE_BUNDLE_SCRIPT% %BUILD_DIR%\%NAME%.aaxplugin %AAX_ICON% %FORMAT%
    xcopy /E /H /Y %BUILD_DIR%\%NAME%.aaxplugin\Contents\* %AAX_32_PATH_DEBUG%\%NAME%.aaxplugin\Contents\
  )
)

if %PLATFORM% == "x64" (
  if not exist "%ProgramFiles(x86)%" (
    echo "This batch script fails on 32 bit windows... edit accordingly"
  )

  if %FORMAT% == ".exe" (
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%_%PLATFORM%.exe
  )

  if %FORMAT% == ".dll" (
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%_%PLATFORM%.dll
  )
  
  if %FORMAT% == ".dll" (
    if %COPY_VST2% == "1" (
      echo copying %PLATFORM% binary to %PLATFORM% %FORMATSTRING% Plugins %CONFIGURATION% folder ... 
      copy /y %BUILT_BINARY% %VST2_64_PATH_DEBUG%
    ) else (
      echo not copying %PLATFORM% %FORMATSTRING% %CONFIGURATION% binary
    )
  )
  
  if %FORMAT% == ".vst3" (
    echo copying %PLATFORM% binary to %FORMATSTRING% BUNDLE ...
    call %CREATE_BUNDLE_SCRIPT% %BUILD_DIR%\%NAME%.vst3 %VST_ICON% %FORMAT%
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%.vst3\Contents\x86_64-win
    if exist %VST3_64_PATH_DEBUG% (
      echo copying %FORMATSTRING% bundle to %PLATFORM% %FORMATSTRING% Plugins %CONFIGURATION% folder ...
      call %CREATE_BUNDLE_SCRIPT% %VST3_64_PATH%\%NAME%.vst3 %VST_ICON% %FORMAT%
      xcopy /E /H /Y %BUILD_DIR%\%NAME%.vst3\Contents\*  %VST3_64_PATH_DEBUG%\%NAME%.vst3\Contents\
    ) else (
      echo No %PLATFORM% %FORMATSTRING% %CONFIGURATION% binary folder specified
    )
  )
  
  if %FORMAT% == ".aaxplugin" (
    echo copying %PLATFORM% binary to %FORMATSTRING% BUNDLE ...
    call %CREATE_BUNDLE_SCRIPT% %BUILD_DIR%\%NAME%.aaxplugin %AAX_ICON% %FORMAT%
    copy /y %BUILT_BINARY% %BUILD_DIR%\%NAME%.aaxplugin\Contents\x64
    echo copying %PLATFORM% bundle to %PLATFORM% %FORMATSTRING% Plugins %CONFIGURATION% folder ... 
    call %CREATE_BUNDLE_SCRIPT% %BUILD_DIR%\%NAME%.aaxplugin %AAX_ICON% %FORMAT%
    xcopy /E /H /Y %BUILD_DIR%\%NAME%.aaxplugin\Contents\* %AAX_64_PATH_DEBUG%\%NAME%.aaxplugin\Contents\
  )
)
)