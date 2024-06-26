cmake_minimum_required(VERSION 3.16)

### Project ###
project(GameTemplate)

# Something about fetchcontent and timestamps
if(CMAKE_VERSION VERSION_GREATER "3.19.0") 
	cmake_policy(SET CMP0135 NEW)	
endif()
# Make set override option
cmake_policy(SET CMP0077 NEW)
# force this policy in subdirs
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

if(EMSCRIPTEN)
	set(PLATFORM Web CACHE STRING "")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_GLFW=3 --preload-file resources")
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

### 3rd party libraries ###
include(FetchContent)

## Raylib ##
FetchContent_Declare(
	raylib
	URL https://github.com/raysan5/raylib/archive/tags/5.0.zip
)

FetchContent_MakeAvailable(raylib)

# Support JPG
target_compile_definitions(
	raylib
	PUBLIC
	SUPPORT_FILEFORMAT_PNG SUPPORT_FILEFORMAT_JPG
)

## entt ##
FetchContent_Declare(
	entt
	URL https://github.com/skypjack/entt/archive/tags/v3.13.2.zip
)

FetchContent_MakeAvailable(entt)

## Box 2d
set(BOX2D_BUILD_UNIT_TESTS OFF)
set(BOX2D_BUILD_TESTBED OFF)
FetchContent_Declare(
	box2d
	URL https://github.com/erincatto/box2d/archive/tags/v2.4.1.zip
)

FetchContent_MakeAvailable(box2d)

add_subdirectory(src)
