workingpath="${1}/Builds/LinuxMakefile"
buildpath="${workingpath}/build/"
pluginname="fxseq.vst3"
currentpath=$(pwd)

if [ -n "$1" ]
then
    clear
    echo "Enter the full path of your JUCE/modules path"
    read JUCEPATH
    echo "moving to dir ${workingath}"
    cd "${workingath}"
    echo "Modifying variable JUCE_MODULES_INSTALLATION_DIR in ${workingpath}/Makefile"
    sed -i "/JUCE_MODULES_INSTALLATION_DIR :=/ c\JUCE_MODULES_INSTALLATION_DIR := ${JUCEPATH}" "${workingpath}/Makefile"
    make clean
    make
    echo "Compilation ended."
    echo "Check if errors above, else you can copy ${buildpath}${pluginname} folder to your VST directory"
else
    echo "Error : no github files folder provided as argument"
fi

