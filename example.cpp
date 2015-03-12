#ifdef __APPLE__
#include "GLUT/glut.h"
#include <OPENGL/gl.h>
#endif
#ifdef __unix__
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include "GLSL_helper.h"
#include "mesh.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
using namespace std;
#include "MStackHelp.h"
#include "GeometryCreator.h"

int headvertexCount, torsovertexCount, footLvertexCount, footRvertexCount, handLvertexCount, handRvertexCount;
int thighRvertexCount, thighLvertexCount, armLvertexCount, armRvertexCount, vBallvertexCount;
GLuint headibo = 0, headpositions = 0, headuvs, headnormals;
GLuint torsoibo = 0, torsopositions = 0, torsouvs, torsonormals;
GLuint footLibo = 0, footLpositions = 0, footLuvs, footLnormals;
GLuint footRibo = 0, footRpositions = 0, footRuvs, footRnormals;
GLuint handLibo = 0, handLpositions = 0, handLuvs, handLnormals;
GLuint handRibo = 0, handRpositions = 0, handRuvs, handRnormals;
GLuint thighLibo = 0, thighLpositions = 0, thighLuvs, thighLnormals;
GLuint thighRibo = 0, thighRpositions = 0, thighRuvs, thighRnormals;
GLuint armLibo = 0, armLpositions = 0, armLuvs, armLnormals;
GLuint armRibo = 0, armRpositions = 0, armRuvs, armRnormals;
GLuint vBallibo = 0, vBallpositions = 0, vBalluvs, vBallnormals;
GLint h_aPosition;
GLint h_aNormal;
GLint h_uColor;
GLint h_uModelMatrix;
GLuint uNormalMatrix;
GLint h_uViewMatrix;
GLint h_uProjMatrix;
GLint h_aUV;
int ShadeProg;

GLint h_uLightPos;
GLint h_uLightColor;
GLint h_uMatAmb, h_uMatDif, h_uMatSpec, h_uMatShine;

static float g_width, g_height;
static float g_scale = 1;
int flag = 1;
int stepX, stepY;
float angle = 0, g_angle = 0;
float g_MtransX = -2, g_MtransY, g_MtransZ = 0;
float newScaleY = 1.0, newScaleX = 1.0, newScaleZ = 1.0;
float lightX = 1, lightY = 1, lightZ = 1;
float alpha = -0.215, beta = 1.65;
float startX, startY, lookX = 3.39, lookY = 4.9, lookZ = -10.07;
//float startX, startY, lookX = 3.31, lookY = -.465, lookZ = 7.46;
//float alpha = -0.14, beta = 4.60;
float ballX = -8.5, ballY = 2, ballZ = 0;
glm::vec3 eye = glm::vec3(lookX, lookY, lookZ), up = glm::vec3(0, 1, 0);
glm::vec3 lookAt = glm::vec3(eye.x + cosf(alpha) * cosf(beta), eye.y + sinf(alpha), eye.z + cosf(alpha) * cosf(atan(1) * 2 - beta));
RenderingHelper ModelTrans;
unsigned int const StepSize = 1;
float Accumulator;
int proj=0;

GLuint GNBuffObj, NormalBuffObj, GrndBuffObj, GIndxBuffObj;
int g_RiboLen, g_GiboLen;
static const float g_groundY = -0.1;      // y coordinate of the ground
static const float g_groundSize = 10.0;

// Meshes

float angleA = 0, angleS = 0;
float transX = 0, transY = 2.2;
float rArmRUp = 60, thigh =0, leg = 0;
float rArmRUp2 = 60, thigh2 = 0, leg2 = 0;
float rArmRSide = 60, rArmRSide2 = 60, fArmSide = 0, fArmSide2 = 0;
float moveY= 1.5, moveY2 = 1.5, moveZ =0, moveZ2 =0;
float armLX = 1, armLY = 0, armLZ = .2, armRX = -1, armRY = 0, armRZ = .2;
float armLX2 = 1, armLY2 = 0, armLZ2 = .2, armRX2 = -1, armRY2 =  0, armRZ2 = .2;
float legZ = .5, legZ2 = .5;
unsigned int const MeshCount = 1;
Mesh2 * Meshes[MeshCount];
unsigned int CurrentMesh;

void SetProjectionMatrix() {
	glm::mat4 Projection = glm::perspective(80.0f, (float)g_width / g_height, 0.1f, 100.f);
	safe_glUniformMatrix4fv(h_uProjMatrix, glm::value_ptr(Projection));
}

/* camera controls - do not change */
void SetView() {
	glm::mat4 view = glm::lookAt(eye, lookAt, up);
	//cout << "eye " << eye.r << " " << eye.g << " " << eye.b << endl;
	safe_glUniformMatrix4fv(h_uViewMatrix, glm::value_ptr(view));
}
/* set model transforms to the identity*/
void SetModelI() {
	glm::mat4 tmp = glm::mat4(1.0f);
	safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(tmp));
}
/* model transforms - change these to create a shape with boxes*/
void SetModel() {
	glm::mat4 Trans = glm::translate(glm::mat4(1.0f), glm::vec3(g_MtransX, g_MtransY, g_MtransZ));
	glm::mat4 RotateY = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0));
	glm::mat4 ScaleY = glm::scale(glm::mat4(1.0f), glm::vec3(newScaleX, newScaleY, newScaleZ));
	safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(Trans*ScaleY * RotateY));
}
void SetModel2()
{
	safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(ModelTrans.modelViewMatrix));
	safe_glUniformMatrix4fv(uNormalMatrix, glm::value_ptr(glm::transpose(glm::inverse(ModelTrans.modelViewMatrix))));
}

void SetMaterial(int i) {
	glUseProgram(ShadeProg);
	switch (i) {
	case 0:
		glUniform3f(h_uMatAmb, 0.2, 0.2, 0.2);
		glUniform3f(h_uMatDif, 0.0, 0.5, 0.08);
		glUniform3f(h_uMatSpec, 0.4, 0.4, 0.4);
		glUniform1f(h_uMatShine, 200.0);
		break;
	case 1:
		glUniform3f(h_uMatAmb, 0.09, 0.07, 0.08);
		glUniform3f(h_uMatDif, 0.91, 0.782, 0.82);
		glUniform3f(h_uMatSpec, 1.0, 0.913, 0.8);
		glUniform1f(h_uMatShine, 200.0);
		break;
	case 2:
		glUniform3f(h_uMatAmb, 0.02, 0.02, 0.1);
		glUniform3f(h_uMatDif, 0.0, 0.08, 0.5);
		glUniform3f(h_uMatSpec, 0.14, 0.14, 0.14);
		glUniform1f(h_uMatShine, 120.0);
		break;
	case 3:
		glUniform3f(h_uMatAmb, 0.13, 0.13, 0.14);
		glUniform3f(h_uMatDif, 0.3, 0.3, 0.4);
		glUniform3f(h_uMatSpec, 0.3, 0.3, 0.4);
		glUniform1f(h_uMatShine, 4.0);
		break;
	case 4:
		glUniform3f(h_uMatAmb, 0.13, 0.13, 0.14);
		glUniform3f(h_uMatDif, 1.0, 1.00, 1.00);
		glUniform3f(h_uMatSpec, 0.3, 0.3, 0.4);
		glUniform1f(h_uMatShine, 4.0);
		break;
	}
}

static void initGround() {

	// A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
	float GrndPos[] = {
		-g_groundSize, g_groundY, -g_groundSize,
		-g_groundSize, g_groundY, g_groundSize,
		g_groundSize, g_groundY, g_groundSize,
		g_groundSize, g_groundY, -g_groundSize
	};
	float GrndNorm[] = {
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0
	};
	unsigned short idx[] = { 0, 1, 2, 0, 2, 3 };

	g_GiboLen = 6;
	glGenBuffers(1, &GrndBuffObj);
	glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

	glGenBuffers(1, &NormalBuffObj);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBuffObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

	glGenBuffers(1, &GIndxBuffObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
}

int InstallShader(const GLchar *vShaderName, const GLchar *fShaderName) {
	GLuint VS; //handles to shader object
	GLuint FS; //handles to frag shader object
	GLint vCompiled, fCompiled, linked; //status of shader

	VS = glCreateShader(GL_VERTEX_SHADER);
	FS = glCreateShader(GL_FRAGMENT_SHADER);

	//load the source
	glShaderSource(VS, 1, &vShaderName, NULL);
	glShaderSource(FS, 1, &fShaderName, NULL);

	//compile shader and print log
	glCompileShader(VS);
	/* check shader status requires helper functions */
	printOpenGLError();
	glGetShaderiv(VS, GL_COMPILE_STATUS, &vCompiled);
	printShaderInfoLog(VS);

	//compile shader and print log
	glCompileShader(FS);
	/* check shader status requires helper functions */
	printOpenGLError();
	glGetShaderiv(FS, GL_COMPILE_STATUS, &fCompiled);
	printShaderInfoLog(FS);

	if (!vCompiled || !fCompiled) {
		printf("Error compiling either shader %s or %s", vShaderName, fShaderName);
		return 0;
	}

	//create a program object and attach the compiled shader
	ShadeProg = glCreateProgram();
	glAttachShader(ShadeProg, VS);
	glAttachShader(ShadeProg, FS);

	glLinkProgram(ShadeProg);
	/* check shader status requires helper functions */
	printOpenGLError();
	glGetProgramiv(ShadeProg, GL_LINK_STATUS, &linked);
	printProgramInfoLog(ShadeProg);

	glUseProgram(ShadeProg);

	/* get handles to attribute and uniform data in shader */
	h_aPosition = glGetAttribLocation(ShadeProg, "aPosition");
	h_aNormal = glGetAttribLocation(ShadeProg, "aNormal");
	h_uColor = glGetUniformLocation(ShadeProg, "uColor");
	h_uProjMatrix = glGetUniformLocation(ShadeProg, "uProjMatrix");
	h_uViewMatrix = glGetUniformLocation(ShadeProg, "uViewMatrix");
	h_uModelMatrix = glGetUniformLocation(ShadeProg, "uModelMatrix");
	uNormalMatrix = safe_glGetUniformLocation(ShadeProg, "uNormalMatrix");
	h_uMatAmb = safe_glGetUniformLocation(ShadeProg, "uMat.aColor");
	h_uMatDif = safe_glGetUniformLocation(ShadeProg, "uMat.dColor");
	h_uMatSpec = safe_glGetUniformLocation(ShadeProg, "uMat.sColor");
	h_uMatShine = safe_glGetUniformLocation(ShadeProg, "uMat.shine");
	h_uLightPos = safe_glGetUniformLocation(ShadeProg, "uLightPos");
	h_uLightColor = safe_glGetUniformLocation(ShadeProg, "uLColor");



	printf("sucessfully installed shader %d\n", ShadeProg);
	return 1;
}



void Initialize()
{
    // ...
	// Start Of User Initialization
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Black Background
	glClearDepth(1.0f);	// Depth Buffer Setup
	glDepthFunc(GL_LEQUAL);	// The Type Of Depth Testing
	glEnable(GL_DEPTH_TEST);// Enable Depth Testing

	ModelTrans.useModelViewMatrix();
	ModelTrans.loadIdentity();
	CurrentMesh = 0;
	Accumulator = 0.f;
	initGround();
    // open an OBJ file
	std::ifstream modelFile("head.obj");
	// parse the meshes from the file
	Model model, model2, model3, model4, model5, model6, model7, model8, model9, model10, model11;
	model.load(modelFile);
	headvertexCount = model.meshes()[0].makeVBO(&headibo, &headpositions, &headuvs, &headnormals);

	std::ifstream modelFile2("torso.obj");
	model2.load(modelFile2);
	torsovertexCount = model2.meshes()[0].makeVBO(&torsoibo, &torsopositions, &torsouvs, &torsonormals);
	
	std::ifstream modelFile3("foot_left.obj");
	model3.load(modelFile3);
	footLvertexCount = model3.meshes()[0].makeVBO(&footLibo, &footLpositions, &footLuvs, &footLnormals);

	std::ifstream modelFile4("foot_right.obj");
	model4.load(modelFile4);
	footRvertexCount = model4.meshes()[0].makeVBO(&footRibo, &footRpositions, &footRuvs, &footRnormals);

	std::ifstream modelFile5("hand_left.obj");
	model5.load(modelFile5);
	handLvertexCount = model5.meshes()[0].makeVBO(&handLibo, &handLpositions, &handLuvs, &handLnormals);

	std::ifstream modelFile6("hand_right.obj");
	model6.load(modelFile6);
	handRvertexCount = model6.meshes()[0].makeVBO(&handRibo, &handRpositions, &handRuvs, &handRnormals);

	std::ifstream modelFile7("thight_left.obj");
	model7.load(modelFile7);
	thighLvertexCount = model7.meshes()[0].makeVBO(&thighLibo, &thighLpositions, &thighLuvs, &thighLnormals);

	std::ifstream modelFile8("thigh_right.obj");
	model8.load(modelFile8);
	thighRvertexCount = model8.meshes()[0].makeVBO(&thighRibo, &thighRpositions, &thighRuvs, &thighRnormals);

	std::ifstream modelFile9("upperarm_left.obj");
	model9.load(modelFile9);
	armLvertexCount = model9.meshes()[0].makeVBO(&armLibo, &armLpositions, &armLuvs, &armLnormals);

	std::ifstream modelFile10("upperarm_right.obj");
	model10.load(modelFile10);
	armRvertexCount = model10.meshes()[0].makeVBO(&armRibo, &armRpositions, &armRuvs, &armRnormals);

	std::ifstream modelFile11("Volleyball.obj");
	model11.load(modelFile11);
	vBallvertexCount = model11.meshes()[0].makeVBO(&vBallibo, &vBallpositions, &vBalluvs, &vBallnormals);
	
   }

void transform2(GLint positions, GLint normals, GLint ibo, int vertexCount){
	SetModel2();
	safe_glEnableVertexAttribArray(h_aPosition);
	glBindBuffer(GL_ARRAY_BUFFER, positions);
	glVertexAttribPointer(h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// glEnableVertexAttribArray(h_aUV);
	//glBindBuffer(GL_ARRAY_BUFFER, uvs);
	//glVertexAttribPointer(h_aUV, 2, GL_FLOAT, GL_FALSE, 0, 0);

	safe_glEnableVertexAttribArray(h_aNormal);
	glBindBuffer(GL_ARRAY_BUFFER, normals);
	glVertexAttribPointer(h_aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// If you used the IBO (generally a good idea):
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glUniform3f(h_uColor, 0.5, 0.0, 0.5);
	/*newScaleX = .5;
	newScaleY = .5;
	newScaleZ = .5;
	g_MtransY = 5;
	g_MtransX = -10;
	//angle = 90;
	SetMaterial(3);
	SetModel();*/
	glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// else:
	//glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	safe_glDisableVertexAttribArray(h_aPosition);
	safe_glDisableVertexAttribArray(h_aNormal);

}
void transform(int pos){
	//ModelTrans.rotate(Accumulator * 35.f, vec3(0, 1, 0));
	SetModel2();

	safe_glEnableVertexAttribArray(h_aPosition);
	glBindBuffer(GL_ARRAY_BUFFER, Meshes[pos]->PositionHandle);
	safe_glVertexAttribPointer(h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	safe_glEnableVertexAttribArray(h_aNormal);
	glBindBuffer(GL_ARRAY_BUFFER, Meshes[pos]->NormalHandle);
	safe_glVertexAttribPointer(h_aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Meshes[pos]->IndexHandle);

	glUniform3f(h_uColor, 0.1f, 0.78f, 0.9f);

	glDrawElements(GL_TRIANGLES, Meshes[pos]->IndexBufferLength, GL_UNSIGNED_SHORT, 0);

	safe_glDisableVertexAttribArray(h_aPosition);
	safe_glDisableVertexAttribArray(h_aNormal);
}

void Draw()
{
    // ...
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Start our shader	
	glUseProgram(ShadeProg);
	glUniform3f(h_uLightPos, 0, 8, 4);
	glUniform3f(h_uLightColor, 1, 1, 1);
	/* set up the projection and camera - do not change */
	SetProjectionMatrix();
	SetView();
	
	//GROUND!!
	//actually draw the grond plane 
	safe_glEnableVertexAttribArray(h_aPosition);
	// bind vbo
	glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
	safe_glVertexAttribPointer(h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);


	safe_glEnableVertexAttribArray(h_aNormal);
	// bind vbo
	glBindBuffer(GL_ARRAY_BUFFER, NormalBuffObj);
	safe_glVertexAttribPointer(h_aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// bind ibo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);

	/* set the current color  - change this */
	glUniform3f(h_uColor, 1.0, 0.0, 0.0);
	SetMaterial(0);
	/* set up the current model transform - change this */
	g_MtransX = 0;
	g_MtransY = -1.2;
	g_MtransZ = 0;
	newScaleX = 2;
	newScaleZ = 1;
	angle = 0;
	SetModel();

	// draw - note the difference for using an ibo
	glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);

	// Disable the attributes used by our shader
	safe_glDisableVertexAttribArray(h_aPosition);
	
	/*/
	//SPHERE!!!!
	ModelTrans.loadIdentity();
	ModelTrans.pushMatrix();
	   ModelTrans.translate(glm::vec3(ballX, ballY, ballZ));
	   ModelTrans.scale(.5);
	   ModelTrans.rotate(Accumulator * 35.f, vec3(0, 1, 0));
	   SetMaterial(1);
	   transform(0);
	ModelTrans.popMatrix();*/
	
	
	
	//VOlleyBall
	safe_glEnableVertexAttribArray(h_aPosition);
    glBindBuffer(GL_ARRAY_BUFFER, vBallpositions);
    glVertexAttribPointer(h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

	safe_glEnableVertexAttribArray(h_aNormal);
    glBindBuffer(GL_ARRAY_BUFFER, vBallnormals);
    glVertexAttribPointer(h_aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
    // If you used the IBO (generally a good idea):
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vBallibo);
	glUniform3f(h_uColor, 0.5, 0.0, 0.5);
	newScaleX = .01;
	newScaleY = .01;
	newScaleZ = .01;
	g_MtransY = ballY;
	g_MtransX = ballX;
	g_MtransZ = ballZ;
	//angle = Accumulator * 35.f;
	SetMaterial(4);
	SetModel();
    glDrawElements(GL_TRIANGLES, vBallvertexCount, GL_UNSIGNED_INT, 0);
   // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // else:
    //glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	safe_glDisableVertexAttribArray(h_aPosition);
	safe_glDisableVertexAttribArray(h_aNormal);

	
	//RIGHT PLAYER

	//HEAD
	ModelTrans.pushMatrix();
		ModelTrans.translate(glm::vec3(-9.5, moveY, moveZ));
		ModelTrans.rotate(90, glm::vec3(0, 1, 0));
		ModelTrans.scale(.2, .2, .2);
		ModelTrans.translate(glm::vec3(0, .5, 0));
		//BODY
		ModelTrans.pushMatrix();
			ModelTrans.translate(glm::vec3(0, -2.8, 1));
			ModelTrans.rotate(angleA, glm::vec3(1, 0, 0));
			ModelTrans.translate(glm::vec3(0, -2, 0));
			//RIGHT ARM
			ModelTrans.pushMatrix();
				ModelTrans.translate(glm::vec3(-1.3, .7, .5));
				ModelTrans.rotate(rArmRSide, glm::vec3(0, 1, 0));
				ModelTrans.rotate(rArmRUp, glm::vec3(0, 0, 1));
				ModelTrans.translate(glm::vec3(.5, 0, 0));
				ModelTrans.pushMatrix();
					ModelTrans.translate(glm::vec3(armRX2, armRY2, armRZ2));
					ModelTrans.rotate(fArmSide, glm::vec3(0, 1, 0));
					ModelTrans.translate(glm::vec3(-2, 0, 0));
					SetMaterial(1);	
					transform2(handRpositions, handRnormals, handRibo, handRvertexCount);
				ModelTrans.popMatrix();
				SetMaterial(2);
				transform2(armRpositions, armRnormals, armRibo, armRvertexCount);
			ModelTrans.popMatrix();
			//LEFT ARM
			ModelTrans.pushMatrix();
				ModelTrans.translate(glm::vec3(.9, 1.8, .5));
				ModelTrans.rotate(-rArmRSide, glm::vec3(0, 1, 0));
				ModelTrans.rotate(-rArmRUp, glm::vec3(0, 0, 1));
				ModelTrans.translate(glm::vec3(.5, 0, 0));
				ModelTrans.pushMatrix();
					ModelTrans.translate(glm::vec3(armLX2, armLY2, armLZ2));
					ModelTrans.rotate(-fArmSide, glm::vec3(0, 1, 0));
					ModelTrans.translate(glm::vec3(2, 0, 0));
					SetMaterial(1);
					transform2(handLpositions, handLnormals, handLibo, handLvertexCount);
				ModelTrans.popMatrix();
				SetMaterial(2);
				transform2(armLpositions, armLnormals, armLibo, armLvertexCount);
			ModelTrans.popMatrix();
			//RIGHT LEG
			ModelTrans.pushMatrix();
				ModelTrans.translate(glm::vec3(-.75, -2.5, -.2));
				ModelTrans.rotate(thigh, glm::vec3(1, 0, 0));
				ModelTrans.translate(glm::vec3(0, -.5, 0));
				ModelTrans.pushMatrix();
					ModelTrans.translate(glm::vec3(-.95, -1.5, .7));
					ModelTrans.rotate(leg, glm::vec3(1, 0, 0));
					ModelTrans.translate(glm::vec3(0, -1.5, 0));
					SetMaterial(1);
					transform2(footRpositions, footRnormals, footRibo, footRvertexCount);
				ModelTrans.popMatrix();
				SetMaterial(3);
				transform2(thighRpositions, thighRnormals, thighRibo, thighRvertexCount);
			ModelTrans.popMatrix();
			//LEFT LEG
			ModelTrans.pushMatrix();
				ModelTrans.translate(glm::vec3(.75, -2.5, -.2));
				ModelTrans.rotate(thigh, glm::vec3(1, 0, 0));
				ModelTrans.translate(glm::vec3(0, -.5, 0));
				ModelTrans.pushMatrix();
					ModelTrans.translate(glm::vec3(1.2, -1.5, .7));
					ModelTrans.rotate(leg, glm::vec3(1, 0, 0));
					ModelTrans.translate(glm::vec3(0, -1.5, 0));
					SetMaterial(1);
					transform2(footLpositions, footLnormals, footLibo, footLvertexCount);
				ModelTrans.popMatrix();
				SetMaterial(3);
				transform2(thighLpositions, thighLnormals, thighLibo, thighLvertexCount);
			ModelTrans.popMatrix();
			SetMaterial(2);
			transform2(torsopositions, torsonormals, torsoibo, torsovertexCount);
		ModelTrans.popMatrix();
		SetMaterial(1);
		transform2(headpositions, headnormals, headibo, headvertexCount);
	ModelTrans.popMatrix();
	
	//LEFT PLAYER
	//HEAD
	ModelTrans.pushMatrix();
		ModelTrans.translate(glm::vec3(9.5, moveY2, moveZ2));
		ModelTrans.rotate(-90, glm::vec3(0, 1, 0));
		ModelTrans.scale(.2, .2, .2);
		//BODY
		ModelTrans.pushMatrix();
			ModelTrans.translate(glm::vec3(0, -2.8, 1));
			ModelTrans.rotate(angleS, glm::vec3(1, 0, 0));
			ModelTrans.translate(glm::vec3(0, -2, 0));
			//RIGHT ARM
			ModelTrans.pushMatrix();
				ModelTrans.translate(glm::vec3(-1.3, .7, .5));
				ModelTrans.rotate(rArmRSide2, glm::vec3(0, 1, 0));
				ModelTrans.rotate(rArmRUp2, glm::vec3(0, 0, 1));
				ModelTrans.translate(glm::vec3(0.5, 0, 0));
				ModelTrans.pushMatrix();
				ModelTrans.translate(glm::vec3(armRX, armRY, armRZ));
					ModelTrans.rotate(fArmSide2, glm::vec3(0, 1, 0));
					ModelTrans.translate(glm::vec3(-2, 0, 0));
					SetMaterial(1);
					transform2(handRpositions, handRnormals, handRibo, handRvertexCount);
				ModelTrans.popMatrix();
				SetMaterial(2);
				transform2(armRpositions, armRnormals, armRibo, armRvertexCount);
			ModelTrans.popMatrix();
			//LEFT ARM
			ModelTrans.pushMatrix();
			ModelTrans.translate(glm::vec3(.9, 1.8, .5));
				ModelTrans.rotate(-rArmRSide2, glm::vec3(0, 1, 0));
				ModelTrans.rotate(-rArmRUp2, glm::vec3(0, 0, 1));
				ModelTrans.translate(glm::vec3(.5, 0, 0));
				ModelTrans.pushMatrix();
				ModelTrans.translate(glm::vec3(armLX, armLY, armLZ));
					ModelTrans.rotate(-fArmSide2, glm::vec3(0, 1, 0));
					ModelTrans.translate(glm::vec3(2, 0, 0));
					SetMaterial(1);
					transform2(handLpositions, handLnormals, handLibo, handLvertexCount);
				ModelTrans.popMatrix();
				SetMaterial(2);
				transform2(armLpositions, armLnormals, armLibo, armLvertexCount);
			ModelTrans.popMatrix();
			//RIGHT LEG
			ModelTrans.pushMatrix();
				ModelTrans.translate(glm::vec3(-.75, -2.5, -.2));
				ModelTrans.rotate(thigh2, glm::vec3(1, 0, 0));
				ModelTrans.pushMatrix();
					ModelTrans.translate(glm::vec3(-.95, -1.5, .7));
					ModelTrans.rotate(leg2, glm::vec3(1, 0, 0));
					ModelTrans.translate(glm::vec3(0, -1.5, 0));
					SetMaterial(1);
					transform2(footRpositions, footRnormals, footRibo, footRvertexCount);
				ModelTrans.popMatrix();
				SetMaterial(3);
				transform2(thighRpositions, thighRnormals, thighRibo, thighRvertexCount);
			ModelTrans.popMatrix();
			//LEFT LEG
			ModelTrans.pushMatrix();
				ModelTrans.translate(glm::vec3(.75, -2.5, -.2));
				ModelTrans.rotate(thigh2, glm::vec3(1, 0, 0));
				ModelTrans.translate(glm::vec3(0, -.5, 0));
				ModelTrans.pushMatrix();
					ModelTrans.translate(glm::vec3(1.2, -1.5, .7));
					ModelTrans.rotate(leg2, glm::vec3(1, 0, 0));
					ModelTrans.translate(glm::vec3(0, -1.5, 0));
					SetMaterial(1);
					transform2(footLpositions, footLnormals, footLibo, footLvertexCount);
				ModelTrans.popMatrix();
				SetMaterial(3);
				transform2(thighLpositions, thighLnormals, thighLibo, thighLvertexCount);
			ModelTrans.popMatrix();
			SetMaterial(2);
			transform2(torsopositions, torsonormals, torsoibo, torsovertexCount);
		ModelTrans.popMatrix();
		SetMaterial(1);
		transform2(headpositions, headnormals, headibo, headvertexCount);
	ModelTrans.popMatrix();

	//Disable the shader
	glUseProgram(0);
	glutSwapBuffers();
	glutPostRedisplay();
}
void ReshapeGL(int width, int height)
{
	g_width = (float)width;
	g_height = (float)height;
	glViewport(0, 0, (GLsizei)(width), (GLsizei)(height));

}
void keyboard(unsigned char key, int x, int y){
	glm::vec3 w = glm::normalize(lookAt - eye) * -1.0f;
	glm::vec3 p = glm::cross(up, w);
	glm::vec3 u = glm::normalize(p);
	switch (key) {
	case 'w':
		eye -= .2f * w;
		lookAt -= .2f * w;
		break;
	case 's':
		eye += .2f * w;
		lookAt += .2f * w;
		break;
	case 'a':
		eye -= .2f * u;
		lookAt -= .2f * u;
		break;
	case 'd':
		eye += .2f * u;
		lookAt += .2f * u;
		break;
	case 'm':
		g_MtransX -= .1;
		break;
	case 'n':
		g_MtransX += .1;
		break;
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
	glutPostRedisplay();
}
float p2wx(int in_x) {
	//fill in with the correct return value
	float l = -1, r = 1;

	if (g_width > g_height) {
		l = -g_width / g_height;
		r = g_width / g_height;
	}

	float c = (-g_width + 1) / (l - r);
	float d = c * (-l);

	return ((float)in_x - d) / c;
}

float p2wy(int in_y) {
	//flip glut y
	in_y = g_height - in_y;

	//fill in with the correct return value
	float l = -1, r = 1, t = 1, b = -1;

	if (g_width < g_height) {
		b = -g_height / g_width;
		t = g_height = g_width;
	}

	float e = (-g_height + 1) / (b - t);
	float f = e * (-b);

	return ((float)in_y - e) / e;
}
void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN){
			startX = p2wx(x);
			startY = p2wy(y);
		}
	}
}

void Motion(int x, int y){
	beta -= (p2wx(x) - startX) * p2wx(g_width) / atan(1) * 4;
	alpha -= (p2wy(y) - startY)*  p2wy(g_height) / atan(1) * 4;
	if (alpha >= 1.4)
		alpha = 1.4;
	if (alpha <= -1.4)
		alpha = -1.4;
	cout << "beta " << beta << endl;
	cout << "alpha " << alpha << endl;
	startY = p2wy(y);
	startX = p2wx(x);
	lookAt = glm::vec3(eye.x + cosf(alpha) * cosf(beta), eye.y + sinf(alpha), eye.z + cosf(alpha) * cosf(atan(1) * 2 - beta));
	glutPostRedisplay();
}
void movePlayer(int p, int n){
	if (p){//RIGHT PLAYER
		if (n){
			if (proj == 1)
				moveZ += .08;
			else if (proj == 3)
				moveZ -= .16;
			else if (proj == 5)
				moveZ += .08;
			if (moveY <= 1.2)
				moveY = 1.2;
			else
				moveY -= .01;
			if (fArmSide >= 45)
				fArmSide = 45;
			else
				fArmSide += 1.3;
			if (thigh <= -60)
				thigh = -60;
			else
				thigh -= .7;
			if (leg >= 50)
				leg = 50;
			else
				leg += .7;
			if (angleA >= 20)
				angleA = 20;
			else
				angleA += .7;
		}
		else{
			if (moveY >= 1.5)
				moveY = 1.5;
			else
				moveY += .005;
			if (fArmSide <= 0)
				fArmSide = 0;
			else
				fArmSide -= .7;
			if (thigh >= 0)
				thigh = 0;
			else
				thigh += .9;
			if (leg <= 0)
				leg = 0;
			else
				leg -= .7;
			if (angleA <= 0)
				angleA = 0;
			else
				angleA -= .7;
		}
	}
	else{//LEFT PLAYER
		if (n){
			if (moveY2 <= 1.2)
				moveY2 = 1.2;
			else
				moveY2 -= .01;
			if (fArmSide2 >= 45)
				fArmSide2 = 45;
			else
				fArmSide2 += 1.3;
			if (thigh2 <= -70)
				thigh2 = -70;
			else
				thigh2 -= .7;
			if (leg2 >= 50)
				leg2 = 50;
			else
				leg2 += .7;
			if (angleS >= 20)
				angleS = 20;
			else
				angleS += .7;
			if (proj == 2)
				moveZ2 += .08;
			else if (proj == 4)
				moveZ2 -= .16;
			else if (proj == 6)
				moveZ2 += .08;
		}
		else{
			if (moveY2 >= 1.5)
				moveY2 = 1.5;
			else
				moveY2 += .005;
			if (angleS <= 0)
				angleS = 0;
			else
				angleS -= .7;
			if (fArmSide2 <= 0)
				fArmSide2 = 0;
			else
				fArmSide2 -= .7;
			if (thigh2 >= 0)
				thigh2 = 0;
			else
				thigh2 += .9;
			if (leg2 <= 0)
				leg2 = 0;
			else
				leg2 -= .7;
		}
	}
}

void Timer(int param)
{
	Accumulator += StepSize * 0.02f;
	if (flag){
		if (ballX <= 8.5){
			ballX += .1;
			ballY = -0.1 * powf(ballX, 2.0) + 7.5;
			stepX++;
			movePlayer(1, 0);
			if (ballX > 0)
				movePlayer(0, 1);
			float s = ceil(ballX*100)/100;
			if (ceil(ballX* 100)/100 == 8.50)
				rArmRUp2 -= 15;
		}
		if (ballX >= 8.5){
			flag = 0;
			rArmRUp2 += 15;
			proj++;
			if (proj == 7)
				proj = 1;
		}
	}
	else{
		if (ballX >= -8.6){
			ballX -= .1;
			ballY = -0.1 * powf(ballX, 2.0) + 7.5;
			stepX--;
			if (proj == 1)
				ballZ+= .04;
			if (proj == 3)
				ballZ-= .08;
			if (proj == 5)
				ballZ += .04;
			movePlayer(0, 0);
			if (ballX < 0)
				movePlayer(1, 1);
			if (ballX == -8.5)
				rArmRUp -= 15;
		}
		if (ballX <= -8.6){
			flag = 1;
			proj++;
			rArmRUp += 15;
		}
	}
	glutTimerFunc(1, Timer, 1);
}
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitWindowPosition(20, 20);
	glutInitWindowSize(1000, 600);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Volleyball");
	glutReshapeFunc(ReshapeGL);
	glutDisplayFunc(Draw);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(StepSize, Timer, 1);
	
	#ifdef _WIN32
	GLenum err = glewInit();
	if (GLEW_OK != err){
		std::cerr << "Error initializing glew! " << glewGetErrorString(err) << std::endl;
		return 1;
	}
	#endif
	Initialize();

	

	//test the openGL version
	getGLversion();
	//install the shader
	if (!InstallShader(textFileRead((char *)"Lab4_vert.glsl"), textFileRead((char *)"Lab4_frag.glsl"))) {
		printf("Error installing shader!\n");
		return 0;
	}
	//InitGeom();
	Meshes[0] = GeometryCreator::CreateSphere(glm::vec3(1.0f));
	Meshes[1] = GeometryCreator::CreateCube();
	Meshes[2] = GeometryCreator::CreateCube(vec3(0.5f, 2.f, 4.f));
	glutMainLoop();
	return 0;
}
