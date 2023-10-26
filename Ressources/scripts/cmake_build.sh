workingPath="${HOME}/dev/fxseq/"
currentPath="$(pwd)"

echo "Moving into build directory"
cd "${workingPath}"

mkdir build
cd build
echo "Starting build..."
cmake ..

echo "Get back to previous directory"

cd "${currentPath}"

