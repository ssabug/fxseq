CURRENT_DIR="$(pwd)"
PLUGIN_DIR="${HOME}/.ssabug/fxseq"

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SUB_DIR="$(dirname "$SCRIPT_DIR")"
GITHUB_FILES_PATH="$(dirname "$SUB_DIR")"

echo "/////// Creating folders in ${PLUGIN_DIR}"
mkdir -p "${PLUGIN_DIR}/config"

echo "/////// Copying files from  ${GITHUB_FILES_PATH}/Ressources/ ..."
#cp "${GITHUB_FILES_PATH}/Ressources/config_files/config.xml" "${PLUGIN_DIR}/config/"
cp -r "${GITHUB_FILES_PATH}/Ressources/skins" "${PLUGIN_DIR}/"
cp -r "${GITHUB_FILES_PATH}/Ressources/presets" "${PLUGIN_DIR}/"

echo "/////// Finished"




