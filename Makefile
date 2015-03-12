all:
	g++ example.cpp GeometryCreator.cpp GLSL_helper.cpp MStackHelp.cpp mesh.cpp -DGL_GLEXT_PROTOTYPES  -lGLU -lGL -lglut