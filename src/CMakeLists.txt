add_executable(cell main.c Playfield.c Utility.c Game.c Text.c World.c Script.c Circle.c)

# Add some warning options
if(MSVC)
	target_compile_options(cell PRIVATE /W4 /WX )
else()
	target_compile_options(cell PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(cell glfw glad lua)