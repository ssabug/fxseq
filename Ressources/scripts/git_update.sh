BASE_DIR="${HOME}/dev/fxseq"
CURRENT_DIR="$(pwd)"
GIT_NAME="fxseq"
GIT_URL="https://github.com/ssabug/fxseq"


if [ -n "$1" ]
then
    COMMIT_MSG="$1"

    echo "/////// MOVING TO PROJECT DIR ${BASE_DIR}"
    cd "${BASE_DIR}"

    echo "/////// removing builds and JUCE symbolic link"
    rm -rf "${BASE_DIR}/build/"
    rm "${BASE_DIR}/JUCE"

    #echo "/////// INIT PROJECT DIR ${BASE_DIR}"
    #git init
    #echo "/////// LINK TO REMOTE REPO"
    #git remote add "${GIT_NAME}" "${GIT_URL}"
    #echo "/////// PUSH TO MASTER BRANCH"
    #git push --set-upstream "${GIT_NAME}" master

    echo "/////// ADDING ALL FILES"
    git add --all

    echo "/////// COMMITTING ..."
    git commit -am "${COMMIT_MSG}"

    echo "/////// PUSHING ..."
    git push

    echo "/////// coming back to ${CURRENT_DIR}"
    cd "${CURRENT_DIR}"
else
    echo "Error : no git commit text supplied as argument"
fi



