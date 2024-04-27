cmake --build ./build || exit

pushd build/src/

./Game

popd