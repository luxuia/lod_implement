#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <GL/glew.h>
// Include GLFW
#include <GL/glfw.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


struct Control {
	// Initial position : on +Z
	glm::vec3 position; 
	// Initial horizontal angle : toward -Z
	float horizontalAngle;
	// Initial vertical angle : none
	float verticalAngle;
	// Initial Field of View
	float initialFoV;

	float speed; // 3 units / second
	float mouseSpeed;

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	void init() {
		position = glm::vec3( 0, 5, 5 ); 
	// Initial horizontal angle : toward -Z
		horizontalAngle = 3.14f;
		// Initial vertical angle : none
	  verticalAngle = 3.14/8;
		// Initial Field of View
		initialFoV = 45.0f;

		speed = 9.0f; // 3 units / second
		mouseSpeed = 0.005f;
	}

	void computeMatricesFromInputs();
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

};
#endif