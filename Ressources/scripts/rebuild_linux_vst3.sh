
workingpath="${HOME}/dev/fxseq/Builds/LinuxMakefile"
buildpath="${workingpath}/build/"
vstpath="${HOME}/VST/"
pluginname="fxseq.vst3"
currentpath=$(pwd)
clear
echo "moving to dir ${workingpath}"
cd "${workingpath}"
make clean
make
echo "removing old vst ${vstpath}${pluginname}/"
rm -rf "${vstpath}${pluginname}/"
echo "copying new vst ${buildpath}/${pluginname} to ${vstpath}"
cp -r "${buildpath}/${pluginname}" "$vstpath"


echo "clean build folder for github"
make clean
echo "coming back to dir ${currentpath}"
cd "${currentpath}"

