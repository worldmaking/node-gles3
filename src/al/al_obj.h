#ifndef AL_OBJ_H
#define AL_OBJ_H

#include "al_glm.h"
#include "al_math.h"
#include "al_gl.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>



struct SimpleOBJ {
	typedef unsigned int Element;

	struct Triangle {
		glm::ivec3 vertices[3]; // each vertex is position,texcoord,normal index
	};
	
	struct FaceVertex {
		std::string key;
		int position;
		int normal;
		int texcoord;
	};
	
	std::vector<Vertex> vertices;
	std::vector<Element> indices;

	// 
	// the object will be centered in a box at the origin
	// if normalized_span is nonzero, it will scaled such that it fits within it
	SimpleOBJ(std::string filename, bool center=false, float normalized_span=0) {
		FILE * file = fopen(filename.c_str(), "r");
		if (!file) {
			console.error("SimpleOBJ failed: can't open data file \"%s\".",
				filename.c_str());
			return;
		}
		char        buf[128];	
		
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texcoords;
		std::vector<Triangle> triangles;

		glm::vec3 minv = glm::vec3( FLT_MAX);
		glm::vec3 maxv = glm::vec3(-FLT_MIN);
		
		while(fscanf(file, "%s", buf) != EOF) {
			switch(buf[0]) {
			case 'v':  {             // v, vn, vt				
				glm::vec3 v;
				glm::vec2 t;
				switch(buf[1]) {
				case '\0': 
					fscanf(file, "%f %f %f", &v.x, &v.y, &v.z);
					positions.push_back(v);
					minv = glm::min(minv, v);
					maxv = glm::max(maxv, v);
					break;
				case 'n':  
					fscanf(file, "%f %f %f", &v.x, &v.y, &v.z);
					normals.push_back(glm::normalize(v));
					break;
				case 't':      
					fscanf(file, "%f %f", &t.x, &t.y);
					texcoords.push_back(t);
					break;
				}
			} break;
				/*
				case 'g':               // group
					fgets(buf, sizeof(buf), file);
	#if SINGLE_STRING_GROUP_NAMES
					sscanf(buf, "%s", buf);
	#else
					buf[strlen(buf)-1] = '\0';  // nuke newline
	#endif
					group = glmFindGroup(model, buf);
					group->material = material;
					break;
					*/
				case 'f': { 
					Triangle tri;
					glm::ivec3& tri0 = tri.vertices[0];
					glm::ivec3& tri1 = tri.vertices[1];
					glm::ivec3& tri2 = tri.vertices[2];
					fscanf(file, "%s", buf);
					/* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
					if (strstr(buf, "//")) {
						//  v//n 
						sscanf(buf , "%d//%d", &tri0.x, &tri0.z);
						fscanf(file, "%d//%d", &tri1.x, &tri1.z);
						fscanf(file, "%d//%d", &tri2.x, &tri2.z);
						triangles.push_back(tri);
						/*
						while(fscanf(file, "%d//%d", &v, &n) > 0) {
							T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
							T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
							T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
							T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
							T(numtriangles).vindices[2] = v;
							T(numtriangles).nindices[2] = n;
							group->triangles[group->numtriangles++] = numtriangles;
							numtriangles++;
						}
						*/
					} else if (sscanf(buf, "%d/%d/%d", &tri0.x, &tri0.y, &tri0.z) == 3) {
						// v/t/n 
						fscanf(file, "%d/%d/%d", &tri1.x, &tri1.y, &tri1.z);
						fscanf(file, "%d/%d/%d", &tri2.x, &tri2.y, &tri2.z);
						triangles.push_back(tri);
						/*
						while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
							T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
							T(numtriangles).tindices[0] = T(numtriangles-1).tindices[0];
							T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
							T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
							T(numtriangles).tindices[1] = T(numtriangles-1).tindices[2];
							T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
							T(numtriangles).vindices[2] = v;
							T(numtriangles).tindices[2] = t;
							T(numtriangles).nindices[2] = n;
							group->triangles[group->numtriangles++] = numtriangles;
							numtriangles++;
						}*/
					} else if (sscanf(buf, "%d/%d", &tri0.x, &tri0.y) == 2) {
						// v/t 
						fscanf(file, "%d/%d", &tri1.x, &tri1.y);
						fscanf(file, "%d/%d", &tri2.x, &tri2.y);
						triangles.push_back(tri);
						/*
						while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
							T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
							T(numtriangles).tindices[0] = T(numtriangles-1).tindices[0];
							T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
							T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
							T(numtriangles).tindices[1] = T(numtriangles-1).tindices[2];
							T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
							T(numtriangles).vindices[2] = v;
							T(numtriangles).tindices[2] = t;
							T(numtriangles).nindices[2] = n;
							group->triangles[group->numtriangles++] = numtriangles;
							numtriangles++;
						}*/
					} else {
					
						// v
						sscanf(buf, "%d", &tri0.x);
						fscanf(file, "%d", &tri1.x);
						fscanf(file, "%d", &tri2.x);
						triangles.push_back(tri);
						/*
						while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
							T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
							T(numtriangles).tindices[0] = T(numtriangles-1).tindices[0];
							T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
							T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
							T(numtriangles).tindices[1] = T(numtriangles-1).tindices[2];
							T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
							T(numtriangles).vindices[2] = v;
							T(numtriangles).tindices[2] = t;
							T(numtriangles).nindices[2] = n;
							group->triangles[group->numtriangles++] = numtriangles;
							numtriangles++;
						}*/
					}
				}
				break;
				default:
					// eat up rest of line 
					fgets(buf, sizeof(buf), file);
					break;
			}
		}
		console.log("loaded %s: %d vertices, %d texcoords, %d normals, %d triangles",
			filename.c_str(),
			positions.size(), texcoords.size(), normals.size(), triangles.size());
		vertices.clear();
		indices.clear();
		
		console.log("min %f %f %f max %f %f %f", 
			minv.x, minv.y, minv.z, 
			maxv.x, maxv.y, maxv.z);
		
		if (normalized_span) {
			glm::vec3 diff = maxv - minv;
			glm::vec3 centre = minv + diff * 0.5f;
			float normscale = normalized_span/glm::max(diff.x, glm::max(diff.y, diff.z));

			for (int i=0; i<positions.size(); i++) {
				
				if (center) { 
					positions[i] -= centre;
				}
				if (normalized_span) {
					positions[i] *= normscale;
				}
			}
		}
		
		for (int i=0; i<triangles.size(); i++) {
			// need to shift indices down by 1 because obj counts from 1
			int i0 = triangles[i].vertices[0].x-1;
			int i1 = triangles[i].vertices[1].x-1;
			int i2 = triangles[i].vertices[2].x-1;
			//console_log("%d i %d %d %d", i, indices[indices.size()-3], indices[indices.size()-2], indices[indices.size()-1]);
		}
		
		// NOTE: this assumes all face vertex indices are the same:
		for (int i=0; i<positions.size(); i++) {
			Vertex v;
			v.position = positions[i];
			v.texcoord = texcoords.size() > i ? texcoords[i] : glm::vec2(0.f);
			v.normal = normals.size() > i ? normals[i] : glm::vec3(0., 1., 0.);
			vertices.push_back(v);
		
			//Vertex& v1 = vertices[vertices.size()-1]; console_log("%d v %f %f %f n %f %f %f", i, v1.position.x, v1.position.y, v1.position.z, v1.normal.x, v1.normal.y, v1.normal.z);
		}
		for (int i=0; i<triangles.size(); i++) {
			// need to shift indices down by 1
			indices.push_back(triangles[i].vertices[0].x-1);
			indices.push_back(triangles[i].vertices[1].x-1);
			indices.push_back(triangles[i].vertices[2].x-1);
		
			//console_log("%d i %d %d %d", i, indices[indices.size()-3], indices[indices.size()-2], indices[indices.size()-1]);
		
		}
	
		console.log("loaded %s: %d vertices, %d indices",
			filename.c_str(),
			vertices.size(), indices.size());
		
	}
};

#endif
