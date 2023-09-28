CURRENT_DIR="$(pwd)"
PLUGIN_DIR="${HOME}/.ssabug/fxseq"

if [ -n "$1" ]
then
    GITHUB_FILES_PATH="$1"

    echo "/////// Creating folders in ${PLUGIN_DIR}"
    mkdir -p "${PLUGIN_DIR}/config"

    echo "/////// Copying files from  ${GITHUB_FILES_PATH}/Ressources/ ..."
    cp "${GITHUB_FILES_PATH}/Ressources/config_files/config.xml" "${PLUGIN_DIR}/config/"
    cp -r "${GITHUB_FILES_PATH}/Ressources/skins" "${PLUGIN_DIR}/"
    cp -r "${GITHUB_FILES_PATH}/Ressources/presets" "${PLUGIN_DIR}/"

    echo "/////// Finished"

else
    echo "Error : no git hub files folder provided as argument"
fi



