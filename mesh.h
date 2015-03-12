// mesh.h
// By Ross Light, 2012.
// Released as public domain. Don't be evil. :)

#ifndef _MESH_H_
#define _MESH_H_

#include <string>
#include <vector>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#endif
#ifdef __unix__
#include <GL/gl.h>
#endif

#ifdef _WIN32
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdexcept>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr

class Mesh;
struct Face;

typedef std::vector<Mesh> MeshVector;

class Model
{
    friend class Mesh;
public:
    void load(std::istream& is);
    const MeshVector& meshes() const { return mMeshes; }
private:
    MeshVector mMeshes;
    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec2> mUVs;
    std::vector<glm::vec3> mNormals;
};

class Mesh
{
    friend class Model;
public:
    const std::string& name() const { return mName; }
    int makeVBO(GLuint* indexHandle, GLuint* posHandle, GLuint* uvHandle, GLuint* normalHandle) const;
private:
    Mesh(const Model& model, const std::string& name);

    const Model* mModel;
    std::string mName;
    std::vector<Face> mFaces;
};

struct Face
{
    struct Vertex
    {
        int mVertex;
        int mUV;
        int mNormal;
    };

    Vertex V[3];
};

std::istream& operator>>(std::istream& is, Face::Vertex& vert);
bool operator<(const Face::Vertex& a, const Face::Vertex& b);
bool operator==(const Face::Vertex& a, const Face::Vertex& b);
bool operator!=(const Face::Vertex& a, const Face::Vertex& b);

#endif
