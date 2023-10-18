@echo off
cls
set PLUGIN_DIR=%HomePath%\ssabug\fxseq
SET SCRIPT_DIR=%~dp0
 ::%~dp0:~0,-1%
set SOURCE_DIR=%~dp0..\..\

echo /////// Creating folders in %PLUGIN_DIR%
md %HomePath%\ssabug
md %PLUGIN_DIR%

echo /////// Copying files from  %SOURCE_DIR%\Ressources\ ...
Xcopy /E /I %SOURCE_DIR%Ressources\skins %PLUGIN_DIR%\skins
Xcopy /E /I %SOURCE_DIR%Ressources\presets %PLUGIN_DIR%\presets


echo /////// Finished