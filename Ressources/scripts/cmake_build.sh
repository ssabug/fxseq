workingPath="${HOME}/dev/fxseq/"
currentPath="$(pwd)"

echo "Moving into build directory"
cd "${workingPath}"

echo "Making symbolic link to JUCE"
ln -s "${HOME}/Soft/JUCE" "${HOME}/dev/fxseq/JUCE"

mkdir build
cd build
echo "Starting build..."
cmake .. 
# to cross compile, use cmake -DCOMP_TARGET::STRING=windows ..
make 

echo "Get back to previous directory"

cd "${currentPath}"

