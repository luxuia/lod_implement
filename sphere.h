#ifndef __SPHERE__
#define __SPHERE__

#include <glm/glm.hpp>


struct Sphere {
	static const float PI = 3.1415926;

	float* vertex;
	float* normal;
	int numVert;
	int numNorm;

	GLuint vertexID;

	glm::vec3* getPoint(float a, float b) {
		return new glm::vec3(sin(a)*cos(b), cos(a)*cos(b), sin(b));
	}

	void addTriangle(glm::vec3* a, glm::vec3* b, glm::vec3* c, glm::vec3* n) {
		
		vertex[numVert++] = a->x;
		vertex[numVert++] = a->y;
		vertex[numVert++] = a->z;

		vertex[numVert++] = b->x;
		vertex[numVert++] = b->y;
		vertex[numVert++] = b->z;
		
		vertex[numVert++] = c->x;
		vertex[numVert++] = c->y;
		vertex[numVert++] = c->z;

		glm::vec3 norm = glm::normalize(*a);
		normal[numNorm++] = norm.x;
		normal[numNorm++] = norm.y;
		normal[numNorm++] = norm.z;

		norm = glm::normalize(*b);
		normal[numNorm++] = norm.x;
		normal[numNorm++] = norm.y;
		normal[numNorm++] = norm.z;

		norm = glm::normalize(*c);
		normal[numNorm++] = norm.x;
		normal[numNorm++] = norm.y;
		normal[numNorm++] = norm.z;

	}

	void setupMesh(int n) {
		numVert = 0;
		numNorm = 0;
		vertex = new float[n*n*3*2*3];
		normal = new float[n*n*3*2*3];
		for (int ny = 0; ny < n; ny++) {
			float beta = (ny)/(float)n*2*PI;
			float beta1 = (ny+1)/(float)n*2*PI;
			for (int nx = 0; nx < n; nx++) {
				float alpha = nx/(float)n*2*PI;
				float theta = (nx+1)/(float)n*2*PI;
				glm::vec3* p1 = getPoint(alpha, beta);
				glm::vec3* p2 = getPoint(theta, beta);
				glm::vec3* p3 = getPoint(theta, beta1);
				glm::vec3* p4 = getPoint(alpha, beta1);
				glm::vec3 n1 = glm::cross(*p3-*p1, *p2-*p1);
				glm::vec3 n2 = glm::cross(*p3-*p1, *p4-*p1);
				addTriangle(p1, p2, p3, &n1);
				addTriangle(p1, p3, p4, &n2);
			}
		}
	}

	Sphere() {

	}

	Sphere(int n) {
		setupMesh(n);
	}

	GLuint vertexBuffer;
	GLuint positionSizeBuffer;
	GLuint normalBuffer;


/////
//  dim: sphere unit part
//  n: number of sphere allocate in GPU memory.
/////
	void init(int dim, int n) {
		setupMesh(dim);
		glGenVertexArrays(1, &vertexID);
		glBindVertexArray(vertexID);

		/// Vertex Buffer
		
		
	}

	float* getVertext() {
		return vertex;
	}

};




#endif // __SPHERE__