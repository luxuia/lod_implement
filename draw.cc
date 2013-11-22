#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/glfw.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

#include "common/shader.hpp"
#include "common/controls.hpp"
#include "common/texture.hpp"
#include <algorithm>
#include "layout.h"

struct Particle {
	glm::vec3 pos, speed;
	unsigned char r, g, b, a;
	float size, life;
	float depth; // -1 for dead Particle
	bool operator<(const Particle& that) const {
		return depth > that.depth;
	}
};

struct PARTICLE_CONFIG {
	GLuint vertexArrayID;
	GLuint programID;
	GLuint textureID;
	GLuint viewProjectMatrixID;
	GLuint cameraRight_worldspace_ID;
	GLuint cameraUp_worldspace_ID;
	GLuint particleVertexBuff;
	GLuint particlePositionSizeBuff;
	GLuint particleColorBuff;

	float 		lastTime;

	static const int 	MaxParticleSize = 1000000;
	Particle 	particleContainer[MaxParticleSize];
	GLfloat 	particlePositionSize[MaxParticleSize*4];
	GLubyte 	particleColor[MaxParticleSize*4];
	glm::vec3 cameraPosition;
	int 			texture;
	int 			particleNum;

	glm::mat4 	projectMatrix;
	glm::mat4 	viewMatrix;
	glm::mat4 	viewProjectMatrix;
} g;
BaseLayout *layout;


void sortParticle(Particle* particles, int particleNum) {
	std::sort( particles, particles+particleNum );
}



void initWindow(void ) {
	if ( !glfwInit() ) {
		fprintf(stderr, "cant init glfw!\n");
		exit;
	}
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE,GL_TRUE);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if ( !glfwOpenWindow(1024, 768, 0, 0, 0, 0, 32, 0, GLFW_WINDOW ) ) {
		glfwTerminate();
	}
	glewExperimental = true;
	if ( glewInit() != GLEW_OK ) {
		fprintf(stderr, "glew init failed!\n");
		exit;
	}
	glfwSetWindowTitle("Particle graph layout");
	glfwEnable( GLFW_STICKY_KEYS );
}
	static const GLfloat vertexBuffData[] = {
		-0.5, -0.5, 0,
		 0.5, -0.5, 0,
		-0.5, 0.5, 0,
		 0.5, 0.5, 0
	};
void initGL(void) {
	glClearColor( 0.0f, 0.0f, 0.4f, 0.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );
	fprintf(stderr, "start drawing\n");

	glGenVertexArrays(1, &g.vertexArrayID);
	glBindVertexArray(g.vertexArrayID);
	g.programID = LoadShaders("Particle.vertexshader", "Particle.fragmentshader");

	g.viewProjectMatrixID = glGetUniformLocation(g.programID, "VP");
	g.cameraUp_worldspace_ID = glGetUniformLocation(g.programID, "CameraUp_worldspace_ID");
	g.cameraRight_worldspace_ID = glGetUniformLocation(g.programID, "CameraRight_worldspace_ID");
	g.textureID = glGetUniformLocation(g.programID, "myTextureSampler");
	g.texture = loadDDS("particle.DDS");


	glGenBuffers(1, &g.particleVertexBuff);
	glBindBuffer(GL_ARRAY_BUFFER, g.particleVertexBuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBuffData), vertexBuffData, GL_STATIC_DRAW);

	glGenBuffers(1, &g.particlePositionSizeBuff);
	glBindBuffer(GL_ARRAY_BUFFER, g.particlePositionSizeBuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*g.MaxParticleSize, NULL, GL_STREAM_DRAW);

	glGenBuffers(1, &g.particleColorBuff);
	glBindBuffer(GL_ARRAY_BUFFER, g.particleColorBuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLubyte)*4*g.MaxParticleSize, NULL, GL_STREAM_DRAW);

	g.lastTime = glfwGetTime();
}

void initParticle() {
	g.particleNum = 0;
	FILE* fin = fopen("test.graph", "r");
	Graph* graph = new Graph(1000);
	int u, v;
	float w;
	while (fscanf(fin, "%d%d%f", &u, &v, &w) != EOF && graph->num < 100) {
		graph->add(u, v, w);
		graph->add(v, u, w);
	}
	layout = new BaseLayout(graph);
	layout->init2D();

	g.particleNum = 0;
	for (int i = 0; i < graph->num && i < 10; ++i) {
		if (graph->pre[i] != -1) {
			g.particleContainer[g.particleNum].pos = layout->pos[i];
			// fill buff
			g.particlePositionSize[g.particleNum*4] = layout->pos[i].x*10;
			g.particlePositionSize[g.particleNum*4+1] = layout->pos[i].y*10;
			g.particlePositionSize[g.particleNum*4+2] = -10.0f;//layout->pos[i].z;
			g.particlePositionSize[g.particleNum*4+3] = (rand()%1000)/2000.0f + 0.1f;

			g.particleColor[g.particleNum*4] = rand()%256;
			g.particleColor[g.particleNum*4+1] = rand()%256;
			g.particleColor[g.particleNum*4+2] = rand()%256;
			g.particleColor[g.particleNum*4+3] = (rand()%256)/3;
			fprintf(stderr, "%d-(%f,%f,%f,%f)/(%d,%d,%d,%d)\n", g.particleNum,
							g.particlePositionSize[g.particleNum*4], 
							g.particlePositionSize[g.particleNum*4+1], 
							g.particlePositionSize[g.particleNum*4+2], 
							g.particlePositionSize[g.particleNum*4+3], 
							g.particleColor[g.particleNum*4],
							g.particleColor[g.particleNum*4+1],
							g.particleColor[g.particleNum*4+2],
							g.particleColor[g.particleNum*4+3]
							);
			g.particleNum++;
		}
	}
}

void buildMatrix() {
	/*
	g.projectMatrix = glm::perspective(0.45, 4.0/3.0, 0.1, 1000.0);
	g.viewMatrix = glm::lookAt(
			glm::vec3(0, 0, 5), // position
			glm::vec3(0, 0, 0), // direction
			glm::vec3(0, 1, 0)  // up
		);
	g.cameraPosition = glm::vec3(glm::inverse(g.viewMatrix)[3]);
	*/

	computeMatricesFromInputs();
	g.projectMatrix = getProjectionMatrix();
	g.viewMatrix = getViewMatrix();
	g.viewProjectMatrix = g.projectMatrix*g.viewMatrix;
	g.cameraPosition = glm::vec3(glm::inverse(g.viewMatrix)[3]);

}

void updateParticle() {

}

void updateBuffer() {
	//fprintf(stderr, "iter\n");
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  float currentTime = glfwGetTime();
  float delta = currentTime - g.lastTime;
  g.lastTime = currentTime;
  buildMatrix();

  if (layout->iterate()) {
  	updateParticle();
  }
  //sortParticle(g.particleContainer, g.particleNum);

  glBindBuffer(GL_ARRAY_BUFFER, g.particlePositionSizeBuff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*g.MaxParticleSize, NULL, GL_STREAM_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*4*g.particleNum, g.particlePositionSize);

  glBindBuffer(GL_ARRAY_BUFFER, g.particleColorBuff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLubyte)*4*g.MaxParticleSize, NULL, GL_STREAM_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLubyte)*4*g.particleNum, g.particleColor);

  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  glUseProgram( g.programID );

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE, g.texture );
  glUniform1i(g.textureID, 0);

	glUniform3f(g.cameraRight_worldspace_ID, g.viewMatrix[0][0], g.viewMatrix[1][0], g.viewMatrix[2][0]);
	glUniform3f(g.cameraUp_worldspace_ID   , g.viewMatrix[0][1], g.viewMatrix[1][1], g.viewMatrix[2][1]);


  glUniformMatrix4fv(g.viewProjectMatrixID, 1, GL_FALSE, &g.viewProjectMatrix[0][0]);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, g.particleVertexBuff);
  glVertexAttribPointer(
  	0, 
  	3, 
  	GL_FLOAT,
  	GL_FALSE,
  	0, 
  	(void*)0
  );

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, g.particlePositionSizeBuff);
  glVertexAttribPointer(
  	1, 
  	4,
  	GL_FLOAT,
  	GL_FALSE,
  	0, 
  	(void*)0
  );

  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, g.particleColorBuff);
  glVertexAttribPointer(
  	2,
  	4, 
  	GL_UNSIGNED_BYTE,
  	GL_TRUE,
  	0,
  	(void*)0
  );

  glVertexAttribDivisor(0, 0);
  glVertexAttribDivisor(1, 1);
  glVertexAttribDivisor(2, 1);

  glDrawArraysInstanced( GL_TRIANGLE_STRIP, 0, 4, g.particleNum);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);


}

void cleanUp() {

}

int main(void) {

	initWindow();
	initGL();
	initParticle();
	do {
		updateBuffer();
		glfwSwapBuffers();
	}while ( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS );

	cleanUp();

	glfwTerminate();

	return 0;
}