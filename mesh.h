#ifndef __MESH_H__
#define __MESH_H__

#include <cstdlib>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "common/controls.hpp"
#include "common/shader.hpp"



#include <vector>
using std::vector;

#define max(a, b) a>b?a:b
#define min(a, b) a<b?a:b

struct BO{

typedef unsigned int uint;
	GLuint bo;
	uint size;
};


struct Mesh{
	int program;
	vector<glm::vec3> vertex;
	
	vector<unsigned int> indics;

	GLuint VPid;
	GLuint baseID;
	GLuint colorID;
	int x, y;
	Mesh(int _x, int _y):x(_x+1), y(_y+1) {}

	BO vertexBuffer;
	uint ibo;
	vector<BO> lod;

	void init(void) {
		VPid = -1;
		program = LoadShaders("mesh.vert", "mesh.frag");
		VPid = glGetUniformLocation(program, "VP");
		baseID = glGetUniformLocation(program, "Base");
		colorID = glGetUniformLocation(program, "Color");
		buildMesh();
	}
	void buildMesh() {
		for (int i = 0; i < x; ++i) {
			for (int j = 0; j < y; ++j) {
				float average = 0;
				int acnt = 0;
				int base = i*y+j;
				if (base-1>=0) {
					average += vertex[base-1].y;
					acnt++;
				}
				if (base-y>=0) {
					average += vertex[base-y].y;
					acnt++;
				}
				if (acnt!=0) average /= acnt;
				vertex.push_back(glm::vec3(i, 0,//average+rand()%5381/5381.0-0.3,
																				 j));
			}
		}
		for (int i = 0; i < x-1; ++i) {
			for (int j = 0; j < y-1; ++j) {
				indics.push_back(j+y*i);
				indics.push_back(j+1+y*i);
				indics.push_back(j+(i+1)*y);

				indics.push_back(j+(i+1)*y);
				indics.push_back(j+(i+1)*y+1);
				indics.push_back(j+1+y*i);
			}
		}

		glGenBuffers(1, &vertexBuffer.bo);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.bo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertex.size(), (void*)(&vertex[0]), GL_STATIC_DRAW);
		
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indics.size(), (void*)(&indics[0]), GL_STATIC_DRAW);

		initLod();
	}
	static const int LodNum = 5;
	void initLod() {
		lod.push_back({ibo, (uint)indics.size()});
		vector<unsigned int> ib;
		for (int i = 1; i < LodNum; ++i) {
			int step = 1<<i;
			ib.clear();
			for (int j = 0; j < x-step; j+=step) {
				for (int k = 0; k < y-step; k+=step) {
					unsigned int base = j*y+k;

					ib.push_back(base);
					ib.push_back(base+step);
					ib.push_back(base+y*step);

					ib.push_back(base+y*step+step);
					ib.push_back(base+step);
					ib.push_back(base+y*step);
					
					
				}
			}
			GLuint id;
			glGenBuffers(1, &id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*ib.size(), &(ib[0]), GL_STATIC_DRAW);
			lod.push_back({id, (uint)ib.size()});
		}
	}

	glm::vec3 colorData[LodNum] = {
		glm::vec3{1.0, 1.0, 1.0},
		glm::vec3{1, 0, 1},
		glm::vec3{0, 0, 1},
		glm::vec3{1, 0, 0},
		glm::vec3{1, 1, 0}
	};


	void update(Control *control) {
		int chunkx = 16, chunky = 16;
		float totalx = chunkx*x, totaly = chunky*y;
		glUseProgram(program);
		//glBindBuffer(GL_ARRAY_BUFFER, 0); // Clear array buffer object
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glm::mat4 view = control->ViewMatrix;
		glm::mat4 project = control->ProjectionMatrix;
		glm::mat4 viewProjection = project*view;
		glUniformMatrix4fv(VPid, 1, GL_FALSE, &(viewProjection[0][0]));
		
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.bo);
		glVertexAttribPointer(
			0,
			3, 
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		);
		//glUniform3f(colorID, 1, 0, 1);//3, (&colorData[lodLevel][0]));
		static glm::vec3 lastworld, lastcamera;
		glm::vec3 camerapos = glm::vec3(glm::inverse(view)[3]);
		for (int i = 0; i < chunkx; ++i) {
			for (int j = 0; j < chunky; ++j) {
				float bx = i*(x-1), by = j*(y-1);
				glUniform2f(baseID, bx, by);
				glm::vec3 worldPos = glm::vec3(bx, 0, by);
				glm::vec3 distance = worldPos-camerapos;
				int lodLevel = max(min(glm::sqrt(glm::dot(distance, distance)/4)/80.0, LodNum-1), 0);
				/*
				if (glm::length(lastworld-worldPos)>1e-2 || glm::length(lastcamera-camerapos) > 1e-2) {
					fprintf(stderr, "%0.2f,%0.2f,%0.2f, %0.2f,%0.2f,%0.2f, %d\n", worldPos.x, worldPos.y, worldPos.z, camerapos.x, camerapos.y, camerapos.z, lodLevel);
					lastworld = worldPos;
					lastcamera =camerapos;
				} */
				
				glUniform3fv(colorID, 3, &(colorData[lodLevel][0]));
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lod[lodLevel].bo);
				glDrawElements(GL_TRIANGLES, lod[lodLevel].size, GL_UNSIGNED_INT, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glUseProgram(0);
			
	}


};




#endif // __MESH_H__
