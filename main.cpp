#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <algorithm>

#include <GL/glew.h>

#include <GL/glfw.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"

#include "mesh.h"
#include <vector>
using std::vector;

Mesh* gMesh;
Control* gControl;

int initWindow() {
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE,GL_TRUE);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	if( !glfwOpenWindow( 1024, 768, 0,0,0,0, 32,0, GLFW_WINDOW ) )
	{
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	glfwSetMousePos(1024/2, 768/2);
	glfwSwapInterval(1);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
	glfwSetWindowTitle( "Grid" );

	// Ensure we can capture the escape key being pressed below
	glfwEnable( GLFW_STICKY_KEYS );
	return 0;
}



void initGL() {
		// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
}


GLuint program;
GLuint abo;
GLuint ibo;

int x = 10, z = 10;
vector<glm::vec3> vertexData;
GLuint vpID;

vector<unsigned int> indices;
void update() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gControl->computeMatricesFromInputs();
	/*
	glUseProgram(program);
	glm::mat4 vp = gControl->ProjectionMatrix*gControl->ViewMatrix;
	glUniformMatrix4fv(vpID, 1, GL_FALSE, &(vp[0][0]));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, abo);
	glVertexAttribPointer(
		0, 
		3,
		GL_FLOAT,
		GL_FALSE,
		0, 
		0
	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
*/
	gMesh->update(gControl);
	// TODO abstruct layout from Particles, it should update alone.

}

void initObject() {
	gControl = new Control();
	gControl->init();

/*
	for (int i = 0; i < x; ++i) {
		for (int j = 0; j < z; ++j) {
			vertexData.push_back(glm::vec3(i, -1, j));
		}
	}

	for (int i = 0; i < x-1; ++i) {
		for (int j = 0; j < z-1; ++j) {
			indices.push_back(i+j*z);
			indices.push_back(i+1+j*z);
			indices.push_back(i+(j+1)*z);

			indices.push_back(i+(j+1)*z);
			indices.push_back(i+(j+1)*z+1);
			indices.push_back(i+j*z+1);
		}
	}

	program = LoadShaders("mesh.vert", "mesh.frag");
	vpID = glGetUniformLocation(program, "VP");
	glGenBuffers(1, &abo);
	glBindBuffer(GL_ARRAY_BUFFER, abo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertexData.size(), &vertexData[0], GL_STATIC_DRAW);


	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW);
*/
	gMesh = new Mesh(64, 64);
	gMesh->init();
	
	fprintf(stderr, "Object init over\n");
}

int main( void )
{
	// Initialise GLFW
	initWindow();
	initGL();
	initObject();
	
	
	// The VBO containing the 4 vertices of the particles.
	// Thanks to instancing, they will be shared by all particles.
	
	do
	{
		update();

		// Swap buffers
		glfwSwapBuffers();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
		   glfwGetWindowParam( GLFW_OPENED ) );


	// Cleanup VBO and shader

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

