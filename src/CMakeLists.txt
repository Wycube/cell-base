add_executable(cell main.c Playfield.c Utility.c)

# Add some warning options
if(MSVC)
	target_compile_options(cell PRIVATE /W4 /WX )
else()
	target_compile_options(cell PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

target_link_libraries(cell glfw glad)