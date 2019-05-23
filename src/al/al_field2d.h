#ifndef AL_FIELD2D_H
#define AL_FIELD2D_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "al_math.h"

template<typename T>
void al_field2d_zero(const glm::ivec2 dim, T * data) {
	// memset where possible?
	for (unsigned int i=0; i<dim.x*dim.y; i++) { data[i] = T(0); }
}

template<typename T>
void al_field2d_scale(const glm::ivec2 dim, T * data, const T v) {
	for (unsigned int i=0; i<dim.x*dim.y; i++) { data[i] *= v; }
}

// returns true if the vector v is outside of the field boundaries
inline bool al_field2d_oob(const glm::ivec2 dim, glm::vec2 v) {
	return v.x < 0 || v.x >= dim.x
		|| v.y < 0 || v.y >= dim.y;
}

inline size_t al_field2d_index(const glm::ivec2 dim, int x, int y) {
	return  wrap(x, dim.x) +
			wrap(y, dim.y) * dim.x;
}

inline size_t al_field2d_index(const glm::ivec2 dim, glm::ivec2 p) { return al_field2d_index(dim, p.x, p.y); }

inline size_t al_field2d_index_nowrap(const glm::ivec2 dim, int x, int y) {
	return  x + y*dim.x;
}

template<typename T>
inline void al_field2d_read(const glm::ivec2 dim, T * const data, int x, int y, T * val) {
	*val = data[al_field2d_index(dim, x, y)];
}

template<typename T>
inline T& al_field2d_read(const glm::ivec2 dim, T * const data, int x, int y) {
	return data[al_field2d_index(dim, x, y)];
}

template<typename T>
inline void al_field2d_read(const glm::ivec2 dim, T * const data, glm::vec2 pos, T * val) {
	al_field2d_read<T>(dim, data, pos.x, pos.y, val); 
}

template<typename T>
inline T& al_field2d_read(const glm::ivec2 dim, T * const data, glm::vec2 pos) {
	return al_field2d_read<T>(dim, data, pos.x, pos.y); 
}

template<typename T>
inline void al_field2d_write(const glm::ivec2 dim, T * data, int x, int y, const T val) {
	data[al_field2d_index(dim, x, y)] = val;
}

template<typename T>
inline T& al_field2d_write(const glm::ivec2 dim, T * data, glm::vec2 pos, const T val) {
	al_field2d_write<T>(dim, data, pos.x, pos.y, val);
}

template<typename T>
inline void al_field2d_read_interp(const glm::ivec2 dim, const T * data, double x, double y, T * val) {
	x = wrap(x, (double)dim.x);
	y = wrap(y, (double)dim.y);
	// convert 0..1 field indices to 0..(d-1) cell indices
	const unsigned xa = (const unsigned)al_floor(x);
	const unsigned ya = (const unsigned)al_floor(y);
	unsigned xb = xa+1;	if (xb == dim.x) xb = 0;
	unsigned yb = ya+1;	if (yb == dim.y) yb = 0;
	// get the normalized 0..1 interp factors, of x,y,z:
	const double xbf = al_fract(x);
	const double xaf = 1. - xbf;
	const double ybf = al_fract(y);
	const double yaf = 1. - ybf;
	// get the interpolation corner weights:
	const T faa = T(xaf * yaf);
	const T fab = T(xaf * ybf);
	const T fba = T(xbf * yaf);
	const T fbb = T(xbf * ybf);
	// get the cell addresses for each neighbor:
	const T& vaa = data[al_field2d_index(dim, xa, ya)];
	const T& vab = data[al_field2d_index(dim, xa, yb)];
	const T& vba = data[al_field2d_index(dim, xb, ya)];
	const T& vbb = data[al_field2d_index(dim, xb, yb)];
	// do the 3D interp:
	*val = ((vaa * faa) +
			(vba * fba) +
			(vab * fab) +
			(vbb * fbb) );

}

template<typename T>
inline void al_field2d_read_interp(const glm::ivec2 dim, T * data, glm::vec2 pos, T * val) {
	al_field2d_read_interp<T>(dim, data, pos.x, pos.y, val); 
}

template<typename T>
inline void al_field2d_readnorm_interp(const glm::ivec2 dim, T * data, glm::vec2 pos, T * val) {
	al_field2d_read_interp<T>(dim, data, pos.x * dim.x, pos.y * dim.y, val); 
}

template<typename T>
inline T al_field2d_readnorm_interp(const glm::ivec2 dim, T * data, glm::vec2 pos) {
	T val = T(0);
	al_field2d_read_interp<T>(dim, data, pos.x * dim.x, pos.y * dim.y, &val); 
	return val;
}

template<typename T>
inline void al_field2d_add_interp(const glm::ivec2 dim, T * data, double x, double y, const T val) {
	x = wrap(x, (double)dim.x);
	y = wrap(y, (double)dim.y);
	// convert 0..1 field indices to 0..(d-1) cell indices
	const unsigned xa = (const unsigned)al_floor(x);
	const unsigned ya = (const unsigned)al_floor(y);
	unsigned xb = xa+1;	if (xb == dim.x) xb = 0;
	unsigned yb = ya+1;	if (yb == dim.y) yb = 0;
	// get the normalized 0..1 interp factors, of x,y,z:
	const double xbf = al_fract(x);
	const double xaf = 1. - xbf;
	const double ybf = al_fract(y);
	const double yaf = 1. - ybf;
	// for each plane of the field, do the 3D interp:
	data[al_field2d_index(dim, xa, ya)] += val * T(xaf * yaf);
	data[al_field2d_index(dim, xa, yb)] += val * T(xaf * ybf);
	data[al_field2d_index(dim, xb, ya)] += val * T(xbf * yaf);
	data[al_field2d_index(dim, xb, yb)] += val * T(xbf * ybf);
}

template<typename T>
inline void al_field2d_addnorm_interp(const glm::ivec2 dim, T * data, double x, double y, const T val) {
	al_field2d_add_interp<T>(dim, data, x*dim.x, y*dim.y, val); 
}

template<typename T>
inline void al_field2d_add_interp(const glm::ivec2 dim, T * data, glm::vec2 pos, const T val) {
	al_field2d_add_interp<T>(dim, data, pos.x, pos.y, val); 
}

template<typename T>
inline void al_field2d_addnorm_interp(const glm::ivec2 dim, T * data, glm::vec2 pos, const T val) {
	al_field2d_addnorm_interp<T>(dim, data, pos.x, pos.y, val); 
}

// Gauss-Seidel relaxation scheme:
template<typename T>
inline void al_field2d_diffuse(const glm::ivec2 dim, const T * iptr, T * optr, double diffusion=0.5, unsigned passes=10) {
	double div = 1.0/((1.+4.*diffusion));
	for (unsigned n=0 ; n<passes ; n++) {
		for (size_t y=0;y<dim.y;y++) {
			for (size_t x=0;x<dim.x;x++) {
				const T& prev = iptr[al_field2d_index(dim, x, y  )];
				T&		next = optr[al_field2d_index(dim, x,  y  )];
				const T& va0 = optr[al_field2d_index(dim, x-1,y  )];
				const T& vb0 = optr[al_field2d_index(dim, x+1,y  )];
				const T& v0a = optr[al_field2d_index(dim, x,  y-1)];
				const T& v0b = optr[al_field2d_index(dim, x,  y+1)];			
				next = T(div)*(prev + T(diffusion)*(va0 + vb0 + v0a + v0b));
			}
		}
	}
}




#endif // AL_FIELD2D_H