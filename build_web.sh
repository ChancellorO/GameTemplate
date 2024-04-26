./extern/emsdk/emsdk activate latest \
	&& source ./extern/emsdk/emsdk_env.sh \
	&& emcmake cmake -B build . \
	&& cmake --build build \
	&& python3 -m http.server --directory ./build/src/
