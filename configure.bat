
:: NOTE: This script is only meant to be used as part of the ignition developers' CI system
:: Users and developers should build and install this library using cmake and Visual Studio


:: Install dependencies
call %win_lib% :download_unzip_install curl-7.57.0-vc15-x64-dll-MD.zip
call %win_lib% :download_unzip_install jsoncpp-1.8.4-vc15-x64-dll-MD.zip
call %win_lib% :download_unzip_install libyaml-0.1.7-vc15-x64-md.zip
call %win_lib% :download_unzip_install libzip-1.4.0_zlip-1.2.11_vc15-x64-dll-MD.zip
call %win_lib% :install_ign_project ign-cmake ign-cmake2
call %win_lib% :install_ign_project ign-tools ign-tools1
call %win_lib% :install_ign_project ign-common ign-common3
call %win_lib% :install_ign_project ign-msgs master

:: Set configuration variables
@set build_type=Release
@if not "%1"=="" set build_type=%1
@echo Configuring for build type %build_type%

:: Go to the directory that this configure.bat file exists in
cd /d %~dp0

:: Create a build directory and configure
md build
cd build
cmake .. -G "NMake Makefiles" -DCMAKE_INSTALL_PREFIX="%WORKSPACE_INSTALL_DIR%" -DCMAKE_BUILD_TYPE="%build_type%" -DBUILD_TESTING:BOOL=False
:: Note: We disable testing by default. If the intention is for the CI to build and test
:: this project, then the CI script will turn it back on.

:: If the caller wants to build and/or install, they should do so after calling this script
