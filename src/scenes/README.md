This directory includes files that contain hardcoded scenes. To render one of them you
have to include the file into the `renderer.cpp` file and use its global variables.

This is not the way this renderer is supposed to work, as scene files can be specified
at the command line, without recompiling.

