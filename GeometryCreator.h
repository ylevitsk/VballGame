/*
 * OpenGL 3.0 Core-friendly replacement for glu primitives
 *
 *  Created on: 9/26/2012
 *      Author: Ian Dunn
 */

#ifdef __APPLE__
#include "GLUT/glut.h"
#include <OPENGL/gl.h>
#endif

#ifdef __unix__
#include <GL/glut.h>
#endif

#ifdef _WIN32
#include <GL/glew.h>
#include <GL/glut.h>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "freeglut.lib")
#endif

#include "glm/glm.hpp"
#include <vector>


struct Mesh2
{
    GLuint PositionHandle;
    GLuint NormalHandle;
    GLuint IndexHandle;

    unsigned int IndexBufferLength;

    Mesh2();
    Mesh2(std::vector<float> const & Positions, std::vector<float> const & Normals, std::vector<unsigned short> const & Indices);
};

class GeometryCreator
{

public:

    static Mesh2 * CreateCube(glm::vec3 const & Size = glm::vec3(1.f));

    static Mesh2 * CreateCylinder(
        float const baseRadius, 
        float const topRadius, 
        float const height, 
        unsigned int const slices, 
        unsigned int const stacks);

    static Mesh2 * CreateDisc(
        float const innerRadius, 
        float const outerRadius, 
        float const height, 
        unsigned int const slices, 
        unsigned int const stacks);

    static Mesh2 * CreateSphere(
        glm::vec3 const & Radii = glm::vec3(1.f), 
        unsigned int const Slices = 32, 
        unsigned int const Stacks = 16);

    static Mesh2 * CreateTorus(
        float const innerRadius, 
        float const outerRadius, 
        unsigned int const sides, 
        unsigned int const rings);

};
