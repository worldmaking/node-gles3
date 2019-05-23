#ifndef AL_DISTANCE_H
#define AL_DISTANCE_H

#include "al_math.h"
#include "al_field3d.h"
#include <vector>

/*
	Signed distance functions

	Distance-bound fields should be Lipschitz continuous, i.e. the gradient of the field is everywhere <= 1.

	Useful resources:
	- http://mercury.sexy/hg_sdf
	- http://graphics.cs.illinois.edu/sites/default/files/zeno.pdf
	- http://lgdv.cs.fau.de/get/2234
	- http://www.pouet.net/topic.php?which=7931&page=1
*/

float sdf_union(float a, float b) {
	return glm::min(a, b);
}

// r is radius
float sdf_sphere(glm::vec3 p, float r) {
	return glm::length(p) - r;
}

// cylinder in +Z axis
float sdf_cylinder(glm::vec3 p, float radius, float height) {
	float d = glm::length(glm::vec2(p.x, p.y)) - radius; // i.e. infinite cylinder in Z
	d = glm::max(d, glm::abs(p.z) - height); // truncated by height in Z
	return d;
}

// i.e. a cylinder of +/- height in Z, with round caps
float sdf_capsule(glm::vec3 p, float radius, float height) {
	float d1 = glm::length(glm::vec2(p.x, p.y)) - radius; // i.e. infinite cylinder in Z
	float d2 = glm::length(glm::vec3(p.x, p.y, glm::abs(p.z) - height)) - radius; // i.e. sphere at +/- height
	float az = glm::abs(p.z);
	return glm::mix(d1, d2, glm::step(height, az)); // blend by height (more accurate than boolean union?)
}

// n is plane normal, d is distance from origin
float sdf_plane(glm::vec3 p, glm::vec3 n, float d) {
	return glm::dot(p, n) + d;
}

// b is bounds (radius) in each axis
float sdf_box(glm::vec3 p, glm::vec3 b) {
	glm::vec3 d = glm::abs(p) - b;
	return glm::length(glm::max(d, glm::vec3(0))) + al_max(glm::min(d, glm::vec3(0)));
}

// box is infinite length in whichever axis is not included in p
// b is bounds (radius) in each axis
float sdf_box_cylinder(glm::vec2 p, glm::vec2 b) {
	glm::vec2 d = glm::abs(p) - b;
	return glm::length(glm::max(d, glm::vec2(0))) + al_max(glm::min(d, glm::vec2(0)));
}

// this is the Chamfer Distance method; it isn't the most accurate, but is relatively simple
void sdf_from_binary(const glm::ivec3 dim, const float * binary, float * dst, float inside_value = 0.5f) {
	// the Euclidean distance weights for the three neighbor types in a 3x3x3 kernel
	float d1 = 1.f;
	float d2 = sqrt(2.f); 
	float d3 = sqrt(3.f);

	// init dst:
	for (int i=0, z=0; z<dim.z; z++) {
		for (int y=0; y<dim.y; y++) {
			for (int x=0; x<dim.x; x++, i++) {
				// neighborhood:
				const float b = binary[i];
				const float abb = binary[al_field3d_index(dim, x-1, y, z)]; 
				const float cbb = binary[al_field3d_index(dim, x+1, y, z)]; 
				const float bab = binary[al_field3d_index(dim, x, y-1, z)]; 
				const float bcb = binary[al_field3d_index(dim, x, y+1, z)]; 
				const float bba = binary[al_field3d_index(dim, x, y, z-1)]; 
				const float bbc = binary[al_field3d_index(dim, x, y, z+1)];	

				// average:
				//const float nhoodavg = abb+cbb+bab+bcb+bba+bbc / 6.f;

				if (b != abb || b != cbb || b != bab || b != bcb || b != bba || b != bbc) {
					// this is a border cell, because neighbor != center
					dst[i] = 0.5f; //glm::abs(b - nhoodavg);
				} else {
					// this is an unknown cell
					dst[i] = FLT_MAX;
				}
			}
		}
	}

	// forward pass:
	for (int i=0, z=0; z<dim.z; z++) {
		for (int y=0; y<dim.y; y++) {
			for (int x=0; x<dim.x; x++, i++) {
				float d = dst[i];
				const float bba = dst[al_field3d_index(dim, x  , y  , z-1)]+d1; 
				const float abb = dst[al_field3d_index(dim, x-1, y  , z  )]+d1;  
				const float bab = dst[al_field3d_index(dim, x  , y-1, z  )]+d1; 

				const float aab = dst[al_field3d_index(dim, x-1, y-1, z  )]+d2;
				const float cab = dst[al_field3d_index(dim, x+1, y-1, z  )]+d2; 
				const float baa = dst[al_field3d_index(dim, x  , y-1, z-1)]+d2; 
				const float aba = dst[al_field3d_index(dim, x-1, y  , z-1)]+d2; 
				const float bca = dst[al_field3d_index(dim, x  , y+1, z-1)]+d2; 
				const float cba = dst[al_field3d_index(dim, x+1, y  , z-1)]+d2;

				const float aaa = dst[al_field3d_index(dim, x-1, y-1, z-1)]+d3; 
				const float caa = dst[al_field3d_index(dim, x+1, y-1, z-1)]+d3; 
				const float aca = dst[al_field3d_index(dim, x-1, y+1, z-1)]+d3; 
				const float cca = dst[al_field3d_index(dim, x+1, y+1, z-1)]+d3; 

				if (bba < d) d = bba;
				if (abb < d) d = abb;
				if (bab < d) d = bab;

				if (aab < d) d = aab;
				if (cab < d) d = cab;
				if (baa < d) d = baa;
				if (aba < d) d = aba;
				if (bca < d) d = bca;
				if (cba < d) d = cba;

				if (aaa < d) d = aaa;
				if (caa < d) d = caa;
				if (aca < d) d = aca;
				if (cca < d) d = cca;
				dst[i] = d;
			}
		}
	}
	// backward pass:
	for (int i=dim.x*dim.y*dim.z-1, z=dim.z-1; z>=0; z--) {
		for (int y=dim.y-1; y>=0; y--) {
			for (int x=dim.x-1; x>=0; x--, i--) {
				float d = dst[i];
				const float bbc = dst[al_field3d_index(dim, x  , y  , z+1)]+d1;

				const float cbb = dst[al_field3d_index(dim, x+1, y  , z  )]+d1; 
				const float bcb = dst[al_field3d_index(dim, x  , y+1, z  )]+d1; 
				const float ccb = dst[al_field3d_index(dim, x+1, y+1, z  )]+d2;
				const float acb = dst[al_field3d_index(dim, x-1, y+1, z  )]+d2; 

				const float bcc = dst[al_field3d_index(dim, x  , y+1, z+1)]+d2; 
				const float cbc = dst[al_field3d_index(dim, x+1, y  , z+1)]+d2; 
				const float bac = dst[al_field3d_index(dim, x  , y-1, z+1)]+d2; 
				const float abc = dst[al_field3d_index(dim, x-1, y  , z+1)]+d2; 

				const float ccc = dst[al_field3d_index(dim, x+1, y+1, z+1)]+d3; 
				const float aac = dst[al_field3d_index(dim, x-1, y-1, z+1)]+d3; 
				const float cac = dst[al_field3d_index(dim, x+1, y-1, z+1)]+d3; 
				const float acc = dst[al_field3d_index(dim, x-1, y+1, z+1)]+d3; 
				if (bbc < d) d = bbc;

				if (cbb < d) d = cbb;
				if (bcb < d) d = bcb;
				if (ccb < d) d = ccb;
				if (acb < d) d = acb;

				if (bcc < d) d = bcc;
				if (cbc < d) d = cbc;
				if (bac < d) d = bac;
				if (abc < d) d = abc;

				if (ccc < d) d = ccc;
				if (aac < d) d = aac;
				if (cac < d) d = cac;
				if (acc < d) d = acc;
				dst[i] = d;
			}
		}
	}

	// mark inner vs. outer by comparing with binary input:
	for (int i=0; i<dim.x*dim.y*dim.z; i++) {
		const float d = dst[i];
		dst[i] = (binary[i] <= inside_value) ? -d : d;
	}
}

// this is the Dead Reckoning method, which in theory is more accurate for a slightly higher cost
// http://perso.ensta-paristech.fr/~manzaner/Download/IAD/Grevera_04.pdf
void sdf_from_binary_deadreckoning(const glm::ivec3 dim, const float * binary, float * dst, float inside_value = 0.5f) {

	const int elements = dim.x * dim.y * dim.z;
	
	// for each pixel, the corresponding nearest border pixel 
	std::vector<glm::vec3> p; // TODO: vec3 or ivec3?
	p.resize(elements);

	// the Euclidean distance weights for the three neighbor types in a 3x3x3 kernel
	float d1 = 1.f;
	float d2 = sqrt(2.f); 
	float d3 = sqrt(3.f);

	// init:
	for (int i=0, z=0; z<dim.z; z++) {
		for (int y=0; y<dim.y; y++) {
			for (int x=0; x<dim.x; x++, i++) {
				// neighborhood:
				const float b = binary[i];
				const float abb = binary[al_field3d_index(dim, x-1, y, z)]; 
				const float cbb = binary[al_field3d_index(dim, x+1, y, z)]; 
				const float bab = binary[al_field3d_index(dim, x, y-1, z)]; 
				const float bcb = binary[al_field3d_index(dim, x, y+1, z)]; 
				const float bba = binary[al_field3d_index(dim, x, y, z-1)]; 
				const float bbc = binary[al_field3d_index(dim, x, y, z+1)];	
				
				if (b != abb || b != cbb || b != bab || b != bcb || b != bba || b != bbc) {
					// this is a border cell, because neighbor != center

					// TODO:
					// to get an acurate distance for this cell, need to estimate where the point lies within it
					// which we can guess by comparing the neighbours
				//	float s = sqrtf((glm::abs(b-abb) + glm::abs(b-cbb) + glm::abs(b-bab) + glm::abs(b-bcb) + glm::abs(b-bba) + glm::abs(b-bbc)) * 2.f);

					dst[i] = 0.5;
					p[i] = glm::vec3(x, y, z);
				} else {
					// this is an unknown cell
					dst[i] = FLT_MAX;
					p[i] = glm::vec3(-1);
				}
			}
		}
	}

	// forward pass:
	for (int i=0, z=0; z<dim.z; z++) {
		for (int y=0; y<dim.y; y++) {
			for (int x=0; x<dim.x; x++, i++) {
				const glm::vec3 loc = glm::vec3(x, y, z);
				glm::vec3& pp = p[i];
				float& d = dst[i];

				// d1:
				const size_t bba = al_field3d_index(dim, x  , y  , z-1); 
				const size_t abb = al_field3d_index(dim, x-1, y  , z  );  
				const size_t bab = al_field3d_index(dim, x  , y-1, z  ); 
				// d2:
				const size_t aab = al_field3d_index(dim, x-1, y-1, z  );
				const size_t cab = al_field3d_index(dim, x+1, y-1, z  ); 
				const size_t baa = al_field3d_index(dim, x  , y-1, z-1); 
				const size_t aba = al_field3d_index(dim, x-1, y  , z-1); 
				const size_t bca = al_field3d_index(dim, x  , y+1, z-1); 
				const size_t cba = al_field3d_index(dim, x+1, y  , z-1);
				// d3:
				const size_t aaa = al_field3d_index(dim, x-1, y-1, z-1); 
				const size_t caa = al_field3d_index(dim, x+1, y-1, z-1); 
				const size_t aca = al_field3d_index(dim, x-1, y+1, z-1); 
				const size_t cca = al_field3d_index(dim, x+1, y+1, z-1); 
				
				if (dst[bba]+d1 < d) { pp = p[bba]; d = glm::length(loc - pp); }
				if (dst[abb]+d1 < d) { pp = p[abb]; d = glm::length(loc - pp); }
				if (dst[bab]+d1 < d) { pp = p[bab]; d = glm::length(loc - pp); }

				if (dst[aab]+d2 < d) { pp = p[aab]; d = glm::length(loc - pp); }
				if (dst[cab]+d2 < d) { pp = p[cab]; d = glm::length(loc - pp); }
				if (dst[baa]+d2 < d) { pp = p[baa]; d = glm::length(loc - pp); }
				if (dst[aba]+d2 < d) { pp = p[aba]; d = glm::length(loc - pp); }
				if (dst[bca]+d2 < d) { pp = p[bca]; d = glm::length(loc - pp); }
				if (dst[cba]+d2 < d) { pp = p[cba]; d = glm::length(loc - pp); }

				if (dst[aaa]+d3 < d) { pp = p[aaa]; d = glm::length(loc - pp); }
				if (dst[caa]+d3 < d) { pp = p[caa]; d = glm::length(loc - pp); }
				if (dst[aca]+d3 < d) { pp = p[aca]; d = glm::length(loc - pp); }
				if (dst[cca]+d3 < d) { pp = p[cca]; d = glm::length(loc - pp); }				
			}
		}
	}
	// backward pass:
	for (int i=dim.x*dim.y*dim.z-1, z=dim.z-1; z>=0; z--) {
		for (int y=dim.y-1; y>=0; y--) {
			for (int x=dim.x-1; x>=0; x--, i--) {
				const glm::vec3 loc = glm::vec3(x, y, z);
				glm::vec3& pp = p[i];
				float& d = dst[i];

				// d1:
				const size_t bbc = al_field3d_index(dim, x  , y  , z-1); 
				const size_t cbb = al_field3d_index(dim, x+1, y  , z  ); 
				const size_t bcb = al_field3d_index(dim, x  , y+1, z  ); 
				// d2:
				const size_t ccb = al_field3d_index(dim, x+1, y+1, z  ); 
				const size_t acb = al_field3d_index(dim, x-1, y+1, z  ); 
				const size_t bcc = al_field3d_index(dim, x  , y+1, z+1); 
				const size_t cbc = al_field3d_index(dim, x+1, y  , z+1); 
				const size_t bac = al_field3d_index(dim, x  , y-1, z+1); 
				const size_t abc = al_field3d_index(dim, x-1, y  , z+1); 
				// d3:
				const size_t ccc = al_field3d_index(dim, x+1, y+1, z+1); 
				const size_t aac = al_field3d_index(dim, x-1, y-1, z+1); 
				const size_t cac = al_field3d_index(dim, x+1, y-1, z+1); 
				const size_t acc = al_field3d_index(dim, x-1, y+1, z+1); 
				
				if (dst[bbc]+d1 < d) { pp = p[bbc]; d = glm::length(loc - pp); }
				if (dst[cbb]+d1 < d) { pp = p[cbb]; d = glm::length(loc - pp); }
				if (dst[bcb]+d1 < d) { pp = p[bcb]; d = glm::length(loc - pp); }

				if (dst[ccb]+d2 < d) { pp = p[ccb]; d = glm::length(loc - pp); }
				if (dst[acb]+d2 < d) { pp = p[acb]; d = glm::length(loc - pp); }
				if (dst[bcc]+d2 < d) { pp = p[bcc]; d = glm::length(loc - pp); }
				if (dst[cbc]+d2 < d) { pp = p[cbc]; d = glm::length(loc - pp); }
				if (dst[bac]+d2 < d) { pp = p[bac]; d = glm::length(loc - pp); }
				if (dst[abc]+d2 < d) { pp = p[abc]; d = glm::length(loc - pp); }

				if (dst[ccc]+d3 < d) { pp = p[ccc]; d = glm::length(loc - pp); }
				if (dst[aac]+d3 < d) { pp = p[aac]; d = glm::length(loc - pp); }
				if (dst[cac]+d3 < d) { pp = p[cac]; d = glm::length(loc - pp); }
				if (dst[acc]+d3 < d) { pp = p[acc]; d = glm::length(loc - pp); }
			}
		}
	}

	// mark inner vs. outer by comparing with binary input:
	for (int i=0; i<dim.x*dim.y*dim.z; i++) {
		const float d = dst[i];
		dst[i] = (binary[i] <= inside_value) ? -d : d;
	}
}

// compute normal of an SDF field by using the gradient over 4 tetrahedral points around a location `p`
// (cheaper than the usual technique of sampling 6 cardinal points)
// 'p' is assumed to be a normalized 0..1 position over the dim 'dim' that covers 'field'
// `eps` is the distance to compare points around the location `p` 
// a smaller eps gives sharper edges, but it should be large enough to overcome sampling error
// in theory, the gradient magnitude of an SDF should everywhere = 1, 
// but in practice this isn’t always held, so need to normalize() the result
glm::vec3 sdf_field_normal4(const glm::ivec3 dim, float * const field, glm::vec3 p, const float eps) {
	glm::vec2 e = glm::vec2(-eps, eps);
	// tetrahedral points
	const glm::vec3 e1 = glm::vec3( eps, -eps, -eps);
	const glm::vec3 e2 = glm::vec3(-eps, -eps,  eps);
	const glm::vec3 e3 = glm::vec3(-eps,  eps, -eps);
	const glm::vec3 e4 = glm::vec3( eps,  eps,  eps);
	const float t1 = al_field3d_readnorm_interp(dim, field, p + e1);
	const float t2 = al_field3d_readnorm_interp(dim, field, p + e2);
	const float t3 = al_field3d_readnorm_interp(dim, field, p + e3);
	const float t4 = al_field3d_readnorm_interp(dim, field, p + e4);
	const glm::vec3 n = (e1*t1 + e2*t2 + e3*t3 + e4*t4);
	// normalize for a consistent SDF:
	//return n / (4.f*eps*eps);
	// otherwise:
	return glm::normalize(n);
}

#endif // AL_DISTANCE_H