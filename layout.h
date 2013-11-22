#ifndef __LAYOUT__
#define __LAYOUT__

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

struct Edge {
	int u;
	float w;
	int next;
};

struct Graph {
	int total;
	int maxBound;
	Edge* edge;
	float* inDegree;
	int* hash;
	int* pre;
	int num;
	Graph(int n) {
	
		total = 0;
		num = 0;
		maxBound = n*4;
		edge = new Edge[n*5];
		pre = new int[n*5];
		hash = new int[n*5];
		memset(pre, -1, sizeof(int)*n*5);
		memset(hash, -1, sizeof(int)*n*5);
		inDegree = NULL;
	};

	void calculateDegree() {
		if (inDegree == NULL) {
			inDegree = new float[num];
		}
		//float total = 0;
		for (int u = 0; u < num; ++u) {
			if (pre[u] != -1) {
				for (int v, e = pre[u]; e != -1; e = edge[e].next) {
					v = edge[e].u;
					if (v == u)
						continue;
					inDegree[u]+=edge[e].w;
					inDegree[v]+=edge[e].w;
					//total+=edge[e].w;
				}
			}
		}
		for (int u = 0; u < num; ++u) {
			if (pre[u] != -1) {
				inDegree[u] = log(inDegree[u]+1)+1.0;
				fprintf(stderr, "degree:%f\n", inDegree[u]);
			}
		}
	}
	int add(int u, int v, float w = 1.0) {
		if (total > maxBound) {
			fprintf(stderr, "edge memory out, need to add memory control modul");
			return -1;
		}
		if (hash[v] == -1) {
			hash[v] = num++;
		}
		if (hash[u] == -1) {
			hash[u] = num++;
		}
//		u = hash[u];
	//	v = hash[v];
		edge[total].u = v;
		edge[total].w = w;
		edge[total].next = pre[u];
		num = std::max(num, std::max(u, v));
		pre[u] = total++;
		return 0;
	}
};


struct BaseLayout {
	Graph* g;
	glm::vec3* pos;
	int iterations;
	float force_constant;
	BaseLayout(Graph* _g, int iter = 50, float cforce = 0.01):
					g(_g), pos(NULL), iterations(iter), force_constant(cforce) {

						// TODO parameter
			
			init3D();

	};
	int init2D(int n = -1) {
		if (n == -1) {
			n = g->total;
		}
		pos = new glm::vec3[g->num+1];
		for (int i = 0; i <= g->num; ++i) {
			if (g->pre[i] != -1) {
				float angle = 2*M_PI*i/(g->num+1);
				pos[i] = glm::vec3(cos(angle), sin(angle), 0);
			}
		}
		return 0;
	}

	int init3D() {
		pos = new glm::vec3[g->num+1];
		int split = g->num/4;
		for (int i = 0; i <= g->num; ++i) {
			if (g->pre[i] != -1) {
				float angle = 2*M_PI*i/split;
				float theta = i*M_PI/2/split;
				pos[i] = glm::vec3(cos(angle)*sin(theta), sin(angle)*sin(theta), cos(theta)); 
			}
		}
	}

	int iterate(int iter = 0) {
		if (iter > iterations)
			return 0;
		float separation_constant = 2;
		double k = 1;
		k/=g->total;
		k = separation_constant*std::pow(k, 1.0/2);
		//double k = separation_constant * std::pow(accumulate(space.begin(), space.end(), T(1), std::multiplies<T>()) / T(g.n), T(1) / T(Dim));
		float temperature = 1-iter/iterations;
		
		for (int u = 0; u <= g->num; ++u) {
			glm::vec3 force;
			if (g->pre[u] != -1) {
				for (int v = 0; v <= g->num; ++v) {
					if (g->pre[v] != -1 && v != u) {
						glm::vec3 dis = pos[v] - pos[u];
						force -=  glm::normalize(dis)*float(k*k)/glm::length(dis)*force_constant;
					}
				}

				for (int v, e = g->pre[u]; e != -1; e = g->edge[e].next) {
					v = g->edge[e].u;
					if (v == u)
						continue;
					glm::vec3 dis = pos[v] - pos[u];
					force += glm::normalize(dis)*glm::dot(dis, dis)*(float)(force_constant/k);
				}
				//fprintf(stderr, "%f, %f, %f\n", force.x, force.y, force.z);
				pos[u] += glm::normalize(force)*std::min(temperature, glm::length(force));
			}
		}
		fprintf(stderr, "iterate %d finished\n", iter);
		return 1;                                               
	}

	void normalToSpace() {
		float maxx = -1000, maxy=-10000, minx=10000, miny=10000;
		for (int i = 0; i <= g->num; ++i) {
			if (g->pre[i] != -1) {
				maxx = std::max(pos[i].x, maxx);
				maxy = std::max(pos[i].y, maxy);
				minx = std::min(pos[i].x, minx);
				miny = std::min(pos[i].y, miny);
			}
		}
		//fprintf(stderr, "max: %f min:%f\n", maxx, minx);
		for (int i = 0; i <= g->num; ++i) {
			if (g->pre[i] != -1) {
				pos[i].x = (pos[i].x-minx)/(maxx-minx)*4;
				pos[i].y = (pos[i].y-miny)/(maxy-miny)*4;
			}
		}
	}

	~BaseLayout() {
		delete pos;
	}

};





#endif // __LAYOUT__