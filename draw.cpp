#include <stdio.h>
#include <stdlib.h>
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
#include "particle.h"
#include "network.h"

Control* gControl;

// CPU representation of a particle
BaseLayout* gLayout;
ParticleConfig* gParticleConfig;
GraphNetwork* gGraphNetwork;


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
	glfwSetWindowTitle( "Particles" );

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



void update() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gControl->computeMatricesFromInputs();

	// TODO abstruct layout from Particles, it should update alone.
	gParticleConfig->update(*gControl);
	gGraphNetwork->update(gParticleConfig->layout, *gControl);

}

void initObject() {
	gControl = new Control();
	gControl->init();


//// Init graph ///////////
	FILE* fin = fopen("test.graph", "r");
	Graph* graph = new Graph(100000);
	int u, v;
	float w;
	while (fscanf(fin, "%d%d%f", &u, &v, &w) != EOF && graph->num < 100) {
		graph->add(u, v, w);
		graph->add(v, u, w);
	}
	gLayout = new BaseLayout(graph, 50);

	gParticleConfig = new ParticleConfig();
	gParticleConfig->init(gLayout);
	gGraphNetwork = new GraphNetwork();
	gGraphNetwork->init(gLayout);
	
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


	delete[] gParticleConfig->positionSizeData;

	// Cleanup VBO and shader
	glDeleteBuffers(1, &gParticleConfig->positionBuffer);
	glDeleteBuffers(1, &gParticleConfig->vertexBuffer);
	glDeleteProgram(gParticleConfig->programID);
	glDeleteTextures(1, &gParticleConfig->textureID);
	glDeleteVertexArrays(1, &gParticleConfig->vertexArrayID);
	

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

