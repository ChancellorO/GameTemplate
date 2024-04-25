file(GLOB GAME_SOURCES CONFIGURE_DEPENDS *.cpp *.hpp)

add_executable(Game ${GAME_SOURCES})

target_link_libraries(Game raylib EnTT box2d)

# Copy resources dir to binary dir
add_custom_command(
	TARGET Game
	POST_BUILD
	COMMAND
		${CMAKE_COMMAND} -E copy_directory
			${CMAKE_SOURCE_DIR}/resources
			$<TARGET_FILE_DIR:Game>/resources
)