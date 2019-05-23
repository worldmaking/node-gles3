#ifndef AL_FIELD3D_H
#define AL_FIELD3D_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "al_math.h"

template<typename T>
void al_field3d_zero(const glm::ivec3 dim, T * data) {
	// memset where possible?
	for (unsigned int i=0; i<dim.x*dim.y*dim.z; i++) { data[i] = T(0); }
}

template<typename T>
void al_field3d_scale(const glm::ivec3 dim, T * data, const T v) {
	for (unsigned int i=0; i<dim.x*dim.y*dim.z; i++) { data[i] *= v; }
}

// returns true if the vector v is outside of the field boundaries
inline bool al_field3d_oob(const glm::ivec3 dim, glm::vec3 v) {
	return v.x < 0 || v.x >= dim.x
		|| v.y < 0 || v.y >= dim.y
		|| v.z < 0 || v.z >= dim.z;
}

inline size_t al_field3d_index(const glm::ivec3 dim, int x, int y, int z) {
	return  wrap(x, dim.x) +
			wrap(y, dim.y) * dim.x +
			wrap(z, dim.z) * dim.x*dim.y;
}

inline size_t al_field3d_index(const glm::ivec3 dim, glm::ivec3 p) { return al_field3d_index(dim, p.x, p.y, p.z); }

inline size_t al_field3d_index_nowrap(const glm::ivec3 dim, int x, int y, int z) {
	return  x + y*dim.x + z*dim.x*dim.y;
}

//template<typename T> T * cell(const T * data, int x, int y, int z) { return data + index(x, y, z); }

template<typename T>
inline void al_field3d_read(const glm::ivec3 dim, const T * data, int x, int y, int z, T * val) {
	val = data + al_field3d_index(dim, x, y, z);
}

template<typename T>
inline T al_field3d_read(const glm::ivec3 dim, const T * data, int x, int y, int z) {
	return data[al_field3d_index(dim, x, y, z)];
}

template<typename T>
inline void al_field3d_read(const glm::ivec3 dim, T * data, glm::vec3 pos, T * val) {
	al_field3d_read<T>(dim, data, pos.x, pos.y, pos.z, val); 
}

template<typename T>
inline void al_field3d_read_interp(const glm::ivec3 dim, const T * data, double x, double y, double z, T * val) {
	x = wrap(x, (double)dim.x);
	y = wrap(y, (double)dim.y);
	z = wrap(z, (double)dim.z);
	// convert 0..1 field indices to 0..(d-1) cell indices
	const unsigned xa = (const unsigned)al_floor(x);
	const unsigned ya = (const unsigned)al_floor(y);
	const unsigned za = (const unsigned)al_floor(z);
	unsigned xb = xa+1;	if (xb == dim.x) xb = 0;
	unsigned yb = ya+1;	if (yb == dim.y) yb = 0;
	unsigned zb = za+1;	if (zb == dim.z) zb = 0;
	// get the normalized 0..1 interp factors, of x,y,z:
	const float xbf = al_fract(x);
	const float xaf = 1. - xbf;
	const float ybf = al_fract(y);
	const float yaf = 1. - ybf;
	const float zbf = al_fract(z);
	const float zaf = 1. - zbf;
	// get the interpolation corner weights:
	const float faaa = xaf * yaf * zaf;
	const float faab = xaf * yaf * zbf;
	const float faba = xaf * ybf * zaf;
	const float fabb = xaf * ybf * zbf;
	const float fbaa = xbf * yaf * zaf;
	const float fbab = xbf * yaf * zbf;
	const float fbba = xbf * ybf * zaf;
	const float fbbb = xbf * ybf * zbf;
	// get the cell addresses for each neighbor:
	const T& vaaa = data[al_field3d_index(dim, xa, ya, za)];
	const T& vaab = data[al_field3d_index(dim, xa, ya, zb)];
	const T& vaba = data[al_field3d_index(dim, xa, yb, za)];
	const T& vabb = data[al_field3d_index(dim, xa, yb, zb)];
	const T& vbaa = data[al_field3d_index(dim, xb, ya, za)];
	const T& vbab = data[al_field3d_index(dim, xb, ya, zb)];
	const T& vbba = data[al_field3d_index(dim, xb, yb, za)];
	const T& vbbb = data[al_field3d_index(dim, xb, yb, zb)];
	// do the 3D interp:
	*val = ((vaaa * faaa) +
			(vbaa * fbaa) +
			(vaba * faba) +
			(vaab * faab) +
			(vbab * fbab) +
			(vabb * fabb) +
			(vbba * fbba) +
			(vbbb * fbbb) );

}

template<typename T>
inline void al_field3d_read_interp(const glm::ivec3 dim, T * data, glm::vec3 pos, T * val) {
	al_field3d_read_interp<T>(dim, data, pos.x, pos.y, pos.z, val); 
}

template<typename T>
inline T al_field3d_read_interp(const glm::ivec3 dim, T * data, glm::vec3 pos) {
	T val;
	al_field3d_read_interp<T>(dim, data, pos.x, pos.y, pos.z, &val); 
	return val;
}

template<typename T>
inline void al_field3d_readnorm_interp(const glm::ivec3 dim, T * data, glm::vec3 pos, T * val) {
	al_field3d_read_interp<T>(dim, data, pos.x * dim.x, pos.y * dim.y, pos.z * dim.z, val); 
}

template<typename T>
inline T al_field3d_readnorm_interp(const glm::ivec3 dim, T * data, glm::vec3 pos) {
	T val = T(0);
	al_field3d_read_interp<T>(dim, data, pos.x * dim.x, pos.y * dim.y, pos.z * dim.z, &val); 
	return val;
}

template<typename T>
inline void al_field3D_add(const glm::ivec3 dim, T * data, int x, int y, int z, T grad) {
	data[al_field3d_index(dim, x, y, z)] += grad;
}

template<typename T>
inline void al_field3d_add_interp(const glm::ivec3 dim, T * data, double x, double y, double z, const T val) {
	x = wrap(x, (double)dim.x);
	y = wrap(y, (double)dim.y);
	z = wrap(z, (double)dim.z);
	// convert 0..1 field indices to 0..(d-1) cell indices
	const unsigned xa = (const unsigned)al_floor(x);
	const unsigned ya = (const unsigned)al_floor(y);
	const unsigned za = (const unsigned)al_floor(z);
	unsigned xb = xa+1;	if (xb == dim.x) xb = 0;
	unsigned yb = ya+1;	if (yb == dim.y) yb = 0;
	unsigned zb = za+1;	if (zb == dim.z) zb = 0;
	// get the normalized 0..1 interp factors, of x,y,z:
	const double xbf = al_fract(x);
	const double xaf = 1. - xbf;
	const double ybf = al_fract(y);
	const double yaf = 1. - ybf;
	const double zbf = al_fract(z);
	const double zaf = 1. - zbf;
	// get the interpolation corner weights:
	const double faaa = xaf * yaf * zaf;
	const double faab = xaf * yaf * zbf;
	const double faba = xaf * ybf * zaf;
	const double fabb = xaf * ybf * zbf;
	const double fbaa = xbf * yaf * zaf;
	const double fbab = xbf * yaf * zbf;
	const double fbba = xbf * ybf * zaf;
	const double fbbb = xbf * ybf * zbf;
	// for each plane of the field, do the 3D interp:
	data[al_field3d_index(dim, xa, ya, za)] += val * T(faaa);
	data[al_field3d_index(dim, xa, ya, zb)] += val * T(faab);
	data[al_field3d_index(dim, xa, yb, za)] += val * T(faba);
	data[al_field3d_index(dim, xa, yb, zb)] += val * T(fabb);
	data[al_field3d_index(dim, xb, ya, za)] += val * T(fbaa);
	data[al_field3d_index(dim, xb, ya, zb)] += val * T(fbab);
	data[al_field3d_index(dim, xb, yb, za)] += val * T(fbba);
	data[al_field3d_index(dim, xb, yb, zb)] += val * T(fbbb);
}

template<typename T>
inline void al_field3d_addnorm_interp(const glm::ivec3 dim, T * data, double x, double y, double z, const T val) {
	al_field3d_add_interp<T>(dim, data, x*dim.x, y*dim.y, z*dim.z, val); 
}

template<typename T>
inline void al_field3d_add_interp(const glm::ivec3 dim, T * data, glm::vec3 pos, const T val) {
	al_field3d_add_interp<T>(dim, data, pos.x, pos.y, pos.z, val); 
}

template<typename T>
inline void al_field3d_addnorm_interp(const glm::ivec3 dim, T * data, glm::vec3 pos, const T val) {
	al_field3d_addnorm_interp<T>(dim, data, pos.x, pos.y, pos.z, val); 
}

// Gauss-Seidel relaxation scheme:
template<typename T>
inline void al_field3d_diffuse(const glm::ivec3 dim, const T * iptr, T * optr, double diffusion=0.5, unsigned passes=10) {
	T div = T(1.0/((1.+6.*diffusion)));
	T diffuse = T(diffusion);
	for (unsigned n=0 ; n<passes ; n++) {
		for (size_t z=0;z<dim.z;z++) {
			for (size_t y=0;y<dim.y;y++) {
				for (size_t x=0;x<dim.x;x++) {
					size_t here = al_field3d_index(dim, x, y, z);

					const T prev = iptr[here];
					const T va00 = optr[al_field3d_index(dim, x-1,y,  z  )];
					const T vb00 = optr[al_field3d_index(dim, x+1,y,  z  )];
					const T v0a0 = optr[al_field3d_index(dim, x,  y-1,z  )];
					const T v0b0 = optr[al_field3d_index(dim, x,  y+1,z  )];
					const T v00a = optr[al_field3d_index(dim, x,  y,  z-1)];
					const T v00b = optr[al_field3d_index(dim, x,  y,  z+1)];

					optr[here]   = div*(prev + diffuse*(va00 + vb00 + v0a0 + v0b0 + v00a + v00b));
				}

			}
		}
	}
}


// velptr represents a field of velocities, used to advect content
// inptr is the content to be advected
// outptr is the resulting content after advection
// all fields must have the same dim
// vptr and iptr could be the same field (self-advection of velocity field)
template<typename T, typename TV>
inline void al_field3d_advect(const glm::ivec3 dim, const TV * velptr, T * inptr, T * outptr, float rate=0.5) {

	for (size_t z=0;z<dim.z;z++) {
		for (size_t y=0;y<dim.y;y++) {
			for (size_t x=0;x<dim.x;x++) {
				size_t i = al_field3d_index(dim, x, y, z);

				// back trace: (current cell offset by vector at cell)
				const TV vel = velptr[i];
				const TV pos = TV(x, y, z) - vel*TV(rate);
				const T backvel = al_field3d_read_interp(dim, inptr, pos);

				// read interpolated input field value from back-traced location:
				// TODO: this should be outptr[i] = ...
				outptr[i] = backvel;
					//al_field3d_read_interp(dim, inptr, )

			}
		}
	}
}

inline void al_field3d_derive_gradient(const glm::ivec3 dim, const glm::vec3 * iptr, float * gptr) {
	// calculate gradient.
	// previous instantaneous magnitude of velocity gradient
	//		= average of velocity gradients per axis:
	const float hx = -0.5f/dim.x; 
	const float hy = -0.5f/dim.y; 
	const float hz = -0.5f/dim.z; 
	for (size_t z=0;z<dim.z;z++) {
		for (size_t y=0;y<dim.y;y++) {
			for (size_t x=0;x<dim.x;x++) {
				// gradients per axis:
				// TODO: is doing it plane-by-plane correct? 
				// or should I get the magnitude of the actual gradient vec3?
				const float dx = al_field3d_read(dim, iptr, x+1,y,z).x 
							   - al_field3d_read(dim, iptr, x-1,y,z).x;
				const float dy = al_field3d_read(dim, iptr, x,y+1,z).y 
							   - al_field3d_read(dim, iptr, x,y-1,z).y;
				const float dz = al_field3d_read(dim, iptr, x,y,z+1).z 
							   - al_field3d_read(dim, iptr, x,y,z-1).z;

				// gradient at current cell:
				const float grad = hx*dx + hy*dy + hz*dz;
				// add to output:
				al_field3D_add(dim, gptr, x, y, z, grad);
			}
		}
	}
}

inline void al_field3d_subtract_gradient(const glm::ivec3 dim, const float * gptr, glm::vec3 * optr) {

	const float hx = -0.5f*dim.x; 
	const float hy = -0.5f*dim.y; 
	const float hz = -0.5f*dim.z; 
	for (size_t z=0;z<dim.z;z++) {
		for (size_t y=0;y<dim.y;y++) {
			for (size_t x=0;x<dim.x;x++) {
				// gradients per axis:
				const float dx = al_field3d_read(dim, gptr, x+1,y,z) 
							   - al_field3d_read(dim, gptr, x-1,y,z);
				const float dy = al_field3d_read(dim, gptr, x,y+1,z) 
							   - al_field3d_read(dim, gptr, x,y-1,z);
				const float dz = al_field3d_read(dim, gptr, x,y,z+1) 
							   - al_field3d_read(dim, gptr, x,y,z-1);
				// gradient at current cell:
				const glm::vec3 grad = glm::vec3(hx*dx, hy*dy, hz*dz);
				// add to output:
				al_field3D_add(dim, optr, x, y, z, grad);
			}
		}
	}
}

template<typename T=float>
class Array {
public:
	
	// dims should be powers of 2.
	void initialize(size_t dimx, size_t dimy, size_t dimz, size_t components = 1) {
		mDimX = dimx;
		mDimY = dimy;
		mDimZ = dimz;
		mDimWrapX = mDimX-1;
		mDimWrapY = mDimY-1;
		mDimWrapZ = mDimZ-1;
		mDim3 = mDimX*mDimY*mDimZ;
		mComponents = components;
		mLength = mDim3 * mComponents;
		mSize = sizeof(T) * mLength;
		mStride[0] = mComponents;
		mStride[1] = mStride[0] * mDimX;
		mStride[2] = mStride[1] * mDimY;
		
		//printf("created Field3D %d %d %d [%d] len %d size %d stride %d %d %d\n", mDimX, mDimY, mDimZ, mComponents, mLength, mSize,  mStride[0], mStride[1], mStride[2]);
		
		// allocate:
		data = (T *)malloc(mSize);
		memset(data, 0, mSize);
	}
	void initialize(glm::dvec3 dim, int components = 1) {
		initialize(dim.x, dim.y, dim.z, components);
	}
	
	size_t length() const { return mLength; }
	size_t size() const { return mSize; }
	size_t components() const { return mComponents; }
	size_t dimx() const { return mDimX; }
	size_t dimy() const { return mDimY; }
	size_t dimz() const { return mDimZ; }
	size_t stride(int dim) const { return mStride[dim]; }
	
	void print() {
		printf("Array %dx%dx%d[%d]\n", mDimX, mDimY, mDimZ, mComponents);
	}

	// interpolated read, with normalized position
	// i.e. positions 0..1 are mapped to 0..dim
	template<typename T1>
	void readnorm(const glm::vec3 pos, T1 * elems) {
		read_interp(pos.x * mDimX, pos.y * mDimY, pos.z * mDimZ, elems);
	}
	template<typename T1>
	void readnorm(const glm::vec4 pos, T1 * elems) {
		read_interp(pos.x * mDimX, pos.y * mDimY, pos.z * mDimZ, elems);
	}
	
	template<typename T1>
	void read_interp(const glm::dvec3 pos, T1 * data) const;

	template<typename T1>
	void read_interp(const glm::vec3 pos, T1 * data) const;
	
	template<typename T1>
	void read_interp(double x, double y, double z, T1 * val) const;

	
	template<typename T1>
	inline void read(int x, int y, int z, T1 * val) const;
	
	void add(Array<T>& src) {
		//this->print(); src.print();
		// assumes dim/comonents etc. match
		for (int i=0; i<mLength; i++) {
			data[i] += src.data[i];
		}
	}

	// interpolated add, with normalized position
	// i.e. positions 0..1 are mapped to 0..dim
	inline void addnorm(const glm::vec3 pos, const T * elems) {
		add(pos.x * mDimX, pos.y * mDimY, pos.z * mDimZ, elems);
	}
	// interpolated add, with normalized position
	// i.e. positions 0..1 are mapped to 0..dim
	inline void addnorm(const glm::vec4 pos, const T * elems) {
		add(pos.x * mDimX, pos.y * mDimY, pos.z * mDimZ, elems);
	}

	// interpolated add:
	inline void add(const glm::dvec3 pos, const T * elems) {
		add(pos.x, pos.y, pos.z, elems);
	}

	// interpolated add:
	inline void add(const glm::vec3 pos, const T * elems) {
		add(pos.x, pos.y, pos.z, elems);
	}
	
	void add(double x, double y, double z, const T * elems);
	
	void zero() {
		for (unsigned int i=0; i<mLength; i++) { data[i] = 0; }
	}
	
	void scale(T v) {
		for (int i=0; i<mLength; i++) { data[i] *= v; }
	}
	
	// get the ith cell:
	T * operator[](int i) { return data + i*mStride[0]; }
	
	T * ptr() { return data; }
	// pointer index of a particular cell:
	size_t index(int x, int y, int z) const;
	// access a particular element:
	T * cell(int x, int y, int z, int k=0) const;
	
	// returns true if the vector v is outside of the field boundaries
	inline bool oob(glm::dvec3 v);
	
	T * data;
	size_t mDimX, mDimY, mDimZ, mDim3, mDimWrapX, mDimWrapY, mDimWrapZ, mComponents, mLength, mSize;
	size_t mStride[3];
};

/*!
	Field processing often requires double-buffering
 */

template<typename T=float>
class Field3D {
public:
	
	// dims should be powers of 2.
	void initialize(size_t dimx, size_t dimy, size_t dimz, size_t components = 1) {
		mDimX = dimx;
		mDimY = dimy;
		mDimZ = dimz;
		mDim3 = mDimX*mDimY*mDimZ;
		mDimWrapX = mDimX-1;
		mDimWrapY = mDimY-1;
		mDimWrapZ = mDimZ-1;
		mFront = 1;
		
		mArray0.initialize(mDimX, mDimY, mDimZ, components);
		mArray1.initialize(mDimX, mDimY, mDimZ, components);
	}
	void initialize(glm::dvec3 dim, int components = 1) {
		initialize(dim.x, dim.y, dim.z, components);
	}
	
	size_t length() const { return mArray0.length(); }
	size_t size() const { return mArray0.mSize; }
	size_t components() const { return mArray0.mComponents; }
	glm::ivec3 dim() const { return glm::ivec3(mArray0.mDimX, mArray0.mDimY, mArray0.mDimZ); }
	size_t dimx() const { return mArray0.mDimX; }
	size_t dimy() const { return mArray0.mDimY; }
	size_t dimz() const { return mArray0.mDimZ; }
	size_t stride(int dim) const { return (unsigned)mArray0.mStride[dim]; }
	
	// front is what is currently interacted with
	// back is used for intermediate processing
	Array<T>& front() { return mFront ? mArray0 : mArray1; }
	Array<T>& back() { return mFront ? mArray1 : mArray0; }
	const Array<T>& front() const { return mFront ? mArray0 : mArray1; }
	const Array<T>& back() const { return mFront ? mArray1 : mArray0; }
	
	// swap buffers:
	void swap() { mFront = !mFront; }
	
	void diffuse(double diffusion, unsigned passes=14);
	/*
	 /// read the intensity at a particular location:
	 template<typename T1>
	 void read(const Vec<3,T1> pos, T * elems) const;
	 
	 
	 /// multiply the front array:
	 void scale(T v);
	 /// src must have matching layout
	 void scale(const Array& src);
	 /// src must have matching layout, except for components (which may be 1)
	 void scale1(const Array& src);
	 /// multiply by 1./(src+1.), as a 'damping' factor:
	 void damp(const Array& src);	// src must have matching layout
	 /// add to front array. src must have matching layout
	 void add(Array& src);
	 /// add uniform vector to front array. vec must have as many components as the field
	 void add(T * vec);
	 /// add intensity at a particular location:
	 template<typename T1>
	 void add(const Vec<3,T1> pos, const T * elems);
	 void add(int x, int y, int z, const T * elems);
	 /// single component case:
	 template<typename T1>
	 void add(const Vec<3,T1> pos, T elem);
	 // fill with noise:
	 void adduniform(rnd::Random<>& rng, T scalar = T(1));
	 void adduniformS(rnd::Random<>& rng, T scalar = T(1));
	 // fill with sines:
	 void setHarmonic(T px=T(1), T py=T(1), T pz=T(1));
	 // 3-component fields only: scale velocities at boundaries
	 void boundary();
	 
	 // advect a field.
	 // velocity field should have 3 components
	 void advect(const Array& velocities, T rate = T(1.));
	 static void advect(Array& dst, const Array& src, const Array& velocities, T rate = T(1.));
	 
	 //
	 //	Clever part of Jos Stam's work.
	 //		A velocity field can become divergent (have regions that are purely emanating or aggregating)
	 //			violating the definition of an incompressible fluid
	 //		But, since a velocity field can be seen as an incompressible velocity field + a gradient field,
	 //			we can subtract a gradient field from our bad velocity field to get an incompressible one
	 //		To calculate this gradient field and then subtract it, we use this function:
	 //
	 // grabs the previous instantaneous magnitude of velocity gradient
	 void calculateGradientMagnitude(Array& gradient);
	 void subtractGradientMagnitude(const Array& gradient);
	 
	 void relax(double a, int iterations);
	 */
	
	void advect(const Array<T>& velocities, T rate) {
		swap();
		advect(front(), back(), velocities, rate);
	}
	void advect(Array<T>& dst, const Array<T>& src, const Array<T>& velocities, T rate);
	
	void calculateGradientMagnitude(Array<T>& gradient);
	void subtractGradientMagnitude(const Array<T>& gradient);

	void calculateGradient(Array<glm::vec3>& gradient);
	
	size_t mDimX, mDimY, mDimZ, mDim3, mDimWrapX, mDimWrapY, mDimWrapZ;
	volatile int mFront;	// which one is the front buffer?
	Array<T> mArray0, mArray1; //mArrays[2];	// double-buffering
};

template<typename T=float>
class Fluid3D {
public:
	enum BoundaryMode {
		NONE = 0,
		CLAMP = 1,
		FIELD = 2
	};
	
	void initialize(int dimx, int dimy, int dimz) {
		velocities.initialize(dimx, dimy, dimz, 3);
		gradient.initialize(dimx, dimy, dimz, 1);
		//printf("initialized\n");
		//boundaries.initialize(dimx, dimy, dimz, 1);
	}
	void initialize(glm::dvec3 dim) {
		initialize(dim.x, dim.y, dim.z);
	}
	
	void readVelocity(const glm::dvec3 pos, glm::dvec3& vel) const {
		velocities.front().read_interp(pos, &vel.x);
	}
	
	/*
	 Fluid3D(int dimx=32, int dimy=32, int dimz=32)
	 :	velocities(3, dimx, dimy, dimz),
		gradient(1, dimx, dimy, dimz),
		boundaries(1, Array::type<T>(), dimx, dimy, dimz),
		passes(14),
		viscocity(0.00001),
		selfadvection(0.9),
		selfdecay(0.99),
		selfbackgroundnoise(0.001),
		mBoundaryMode(CLAMP)
	 {
		// set all values to T(1):
		T one = 1;
		boundaries.set3d(&one);
	 }
	 ~Fluid3D() {}
	 
	 template<typename T1>
	 void addVelocity(const Vec<3,T1> pos, const Vec<3,T> vel) {
		velocities.add(pos, vel.elems());
	 }
	 void addVelocity(int x, int y, int z, const Vec<3,T> vel) {
		velocities.add(x, y, z, vel.elems());
	 }
	 
	 template<typename T1>
	 void addGradient(const Vec<3,T1> pos, float& g) {
		gradient.add(pos, &g);
	 }
	 
	 template<typename T1>
	 void readGradient(const Vec<3,T1> pos, float& g) const {
		gradient.read(pos, &g);
	 }
	 
	 /// fluid simulation step
	 // TODO: add dt param
	 void update() {
		// VELOCITIES:
		// add a bit of random noise:
		velocities.adduniformS(rng, selfbackgroundnoise);
		// assume new data is in front();
		// smoothen the new data:
		velocities.diffuse(viscocity, passes);
		// zero velocities at boundaries:
		boundary();
		// (diffused data now in velocities.front())
		// stabilize:
		project();
		// (projected data now in velocities.front())
		// advect velocities:
		velocities.advect(velocities.back(), selfadvection);
		// zero velocities at boundaries:
		boundary();
		// (advected data now in velocities.front())
		// stabilize again:
		project();
		// (projected data now in velocities.front())
		velocities.scale(selfdecay);
		// zero velocities at boundaries:
		boundary();
		
		gradient.front().zero();
		gradient.back().zero();
	 }
	 
	 void boundary() {
		switch (mBoundaryMode) {
	 case CLAMP:
	 velocities.boundary();
	 break;
	 case FIELD:
	 velocities.scale1(boundaries);
	 break;
	 default:
	 break;
		}
	 }
	 
	 
	 
	 void boundary(BoundaryMode b) { mBoundaryMode = b; }
	 */
	
	void project(int passes) {
		gradient.back().zero();
		// prepare new gradient data:
		velocities.calculateGradientMagnitude(gradient.front());
		// diffuse it:
		gradient.diffuse(0.5, passes);
		// subtract from current velocities:
		velocities.subtractGradientMagnitude(gradient.front());
	}
	
	Field3D<T> velocities, gradient;
	//Array<T> boundaries;
	//unsigned passes;
	//T viscocity, selfadvection, selfdecay, selfbackgroundnoise;
	//BoundaryMode mBoundaryMode;
};

// returns true if the vector v is outside of the field boundaries
template <typename T>
inline bool Array<T>::oob(glm::dvec3 v) {
	return (v.x < 0 || v.x >= dimx()
		 || v.y < 0 || v.y >= dimy()
		 || v.z < 0 || v.z >= dimz());
}

template<typename T>
inline size_t Array<T>::index(int x, int y, int z) const {
	return	((x&mDimWrapX) * stride(0)) +
	((y&mDimWrapY) * stride(1)) +
	((z&mDimWrapZ) * stride(2));
}

template<typename T>
inline T * Array<T>::cell(int x, int y, int z, int k) const {
	return data + (index(x, y, z) + k);
}

template<typename T>
template<typename T1>
inline void Array<T>::read_interp(const glm::dvec3 pos, T1 * val) const {
	read_interp(pos.x, pos.y, pos.z, val);
}

template<typename T>
template<typename T1>
inline void Array<T>::read_interp(const glm::vec3 pos, T1 * val) const {
	read_interp(pos.x, pos.y, pos.z, val);
}

template<typename T>
template<typename T1>
inline void Array<T>::read(int x, int y, int z, T1 * val) const {
	x = wrap(x, (int)mDimX);
	y = wrap(y, (int)mDimY);
	z = wrap(z, (int)mDimZ);
	T * paaa = cell(x, y, z);
	for (size_t p=0; p<mComponents; p++) {
		val[p] = (T1)(paaa[p]);
	}
}


template<typename T>
template<typename T1>
inline void Array<T>::read_interp(double x, double y, double z, T1 * val) const {
	x = wrap(x, (double)mDimX);
	y = wrap(y, (double)mDimY);
	z = wrap(z, (double)mDimZ);
	// convert 0..1 field indices to 0..(d-1) cell indices
	const unsigned xa = (const unsigned)al_floor(x);
	const unsigned ya = (const unsigned)al_floor(y);
	const unsigned za = (const unsigned)al_floor(z);
	unsigned xb = xa+1;	if (xb == mDimX) xb = 0;
	unsigned yb = ya+1;	if (yb == mDimY) yb = 0;
	unsigned zb = za+1;	if (zb == mDimZ) zb = 0;
	// get the normalized 0..1 interp factors, of x,y,z:
	double xbf = al_fract(x);
	double xaf = 1.f - xbf;
	double ybf = al_fract(y);
	double yaf = 1.f - ybf;
	double zbf = al_fract(z);
	double zaf = 1.f - zbf;
	// get the interpolation corner weights:
	double faaa = xaf * yaf * zaf;
	double faab = xaf * yaf * zbf;
	double faba = xaf * ybf * zaf;
	double fabb = xaf * ybf * zbf;
	double fbaa = xbf * yaf * zaf;
	double fbab = xbf * yaf * zbf;
	double fbba = xbf * ybf * zaf;
	double fbbb = xbf * ybf * zbf;
	// get the cell addresses for each neighbor:
	T * paaa = cell(xa, ya, za);
	T * paab = cell(xa, ya, zb);
	T * paba = cell(xa, yb, za);
	T * pabb = cell(xa, yb, zb);
	T * pbaa = cell(xb, ya, za);
	T * pbab = cell(xb, ya, zb);
	T * pbba = cell(xb, yb, za);
	T * pbbb = cell(xb, yb, zb);
	// for each plane of the field, do the 3D interp:
	for (size_t p=0; p<mComponents; p++) {
		val[p]=(T1)((paaa[p] * faaa) +
					(pbaa[p] * fbaa) +
					(paba[p] * faba) +
					(paab[p] * faab) +
					(pbab[p] * fbab) +
					(pabb[p] * fabb) +
					(pbba[p] * fbba) +
					(pbbb[p] * fbbb) );
	}
}

template<typename T>
inline void Array<T>::add(double x, double y, double z, const T * val) {
	x = wrap<double>(x, (double)mDimX, 0.);
	y = wrap<double>(y, (double)mDimY, 0.);
	z = wrap<double>(z, (double)mDimZ, 0.);
	const unsigned xa = (const unsigned)al_floor(x);
	const unsigned ya = (const unsigned)al_floor(y);
	const unsigned za = (const unsigned)al_floor(z);
	unsigned xb = xa+1;	if (xb == mDimX) xb = 0;
	unsigned yb = ya+1;	if (yb == mDimY) yb = 0;
	unsigned zb = za+1;	if (zb == mDimZ) zb = 0;
	// get the normalized 0..1 interp factors, of x,y,z:
	double xbf = al_fract(x);
	double xaf = 1.f - xbf;
	double ybf = al_fract(y);
	double yaf = 1.f - ybf;
	double zbf = al_fract(z);
	double zaf = 1.f - zbf;
	// get the interpolation corner weights:
	double faaa = xaf * yaf * zaf;
	double faab = xaf * yaf * zbf;
	double faba = xaf * ybf * zaf;
	double fabb = xaf * ybf * zbf;
	double fbaa = xbf * yaf * zaf;
	double fbab = xbf * yaf * zbf;
	double fbba = xbf * ybf * zaf;
	double fbbb = xbf * ybf * zbf;
	T * paaa = cell(xa, ya, za);
	T * paab = cell(xa, ya, zb);
	T * paba = cell(xa, yb, za);
	T * pabb = cell(xa, yb, zb);
	T * pbaa = cell(xb, ya, za);
	T * pbab = cell(xb, ya, zb);
	T * pbba = cell(xb, yb, za);
	T * pbbb = cell(xb, yb, zb);
	// for each plane of the field, do the 3D interp:
	for (uint8_t p=0; p<mComponents; p++) {
		T tmp = val[p];
		paaa[p] += T(tmp * faaa);
		paab[p] += T(tmp * faab);
		paba[p] += T(tmp * faba);
		pabb[p] += T(tmp * fabb);
		pbaa[p] += T(tmp * fbaa);
		pbab[p] += T(tmp * fbab);
		pbba[p] += T(tmp * fbba);
		pbbb[p] += T(tmp * fbbb);
	}
}

template<typename T>
inline void Field3D<T> :: advect(Array<T>& dst, const Array<T>& src, const Array<T>& velocities, T rate) {
	const size_t stride0 = src.stride(0);
	const size_t stride1 = src.stride(1);
	const size_t stride2 = src.stride(2);
	const size_t dim0 = src.mDimX;
	const size_t dim1 = src.mDimY;
	const size_t dim2 = src.mDimZ;
	const size_t dimwrap0 = dim0-1;
	const size_t dimwrap1 = dim1-1;
	const size_t dimwrap2 = dim2-1;
	
	float * outptr = dst.data;
	float * velptr = velocities.data;
	
	const size_t vstride0 = velocities.stride(0);
	const size_t vstride1 = velocities.stride(1);
	const size_t vstride2 = velocities.stride(2);
	
#define CELL(p, x, y, z, k) (((T *)((p) + (((x)&dimwrap0)*stride0) +  (((y)&dimwrap1)*stride1) +  (((z)&dimwrap2)*stride2)))[(k)])
#define VCELL(p, x, y, z, k) (((T *)((p) + (((x)&dimwrap0)*vstride0) +  (((y)&dimwrap1)*vstride1) +  (((z)&dimwrap2)*vstride2)))[(k)])
	
	for (size_t z=0;z<dim2;z++) {
		for (size_t y=0;y<dim1;y++) {
			for (size_t x=0;x<dim0;x++) {
				// back trace: (current cell offset by vector at cell)
				T * bp  = &(CELL(outptr, x, y, z, 0));
				T * vp	= &(VCELL(velptr, x, y, z, 0));
				glm::dvec3 pos;
				pos.x = x - rate * vp[0];
				pos.y = y - rate * vp[1];
				pos.z = z - rate * vp[2];
				
				// read interpolated input field value into back-traced location:
				src.read_interp(pos, bp);
			}
		}
	}
#undef CELL
#undef VCELL
}

// Gauss-Seidel relaxation scheme:
template<typename T>
inline void Field3D<T> :: diffuse(double diffusion, unsigned passes) {
	swap();
	Array<T>& out = front();
	const Array<T>& in = back();
	const size_t stride0 = out.mStride[0];
	const size_t stride1 = out.mStride[1];
	const size_t stride2 = out.mStride[2];
	const size_t components = out.mComponents;
	const T * iptr = in.data;
	T * optr = out.data;
	double div = 1.0/((1.+6.*diffusion));
#define INDEX(p, x, y, z) (p + ((((x)&mDimWrapX)*stride0) + (((y)&mDimWrapY)*stride1) + (((z)&mDimWrapZ)*stride2)))
	
	for (unsigned n=0 ; n<passes ; n++) {
		for (size_t z=0;z<mDimZ;z++) {
			for (size_t y=0;y<mDimY;y++) {
				for (size_t x=0;x<mDimX;x++) {
					const T * prev =	INDEX(iptr, x,	y,	z);
					T *		  next =	INDEX(optr, x,	y,	z);
					const T * va00 =	INDEX(optr, x-1,y,	z);
					const T * vb00 =	INDEX(optr, x+1,y,	z);
					const T * v0a0 =	INDEX(optr, x,	y-1,z);
					const T * v0b0 =	INDEX(optr, x,	y+1,z);
					const T * v00a =	INDEX(optr, x,	y,	z-1);
					const T * v00b =	INDEX(optr, x,	y,	z+1);
					for (size_t k=0;k<components;k++) {
						next[k] = T(div*(
										 prev[k] +
										 diffusion * (
													  va00[k] + vb00[k] +
													  v0a0[k] + v0b0[k] +
													  v00a[k] + v00b[k]
													  )
										 ));
					}
				}
			}
		}
	}
#undef INDEX
}


template<typename T>
inline void Field3D<T> :: calculateGradientMagnitude(Array<T>& gradient) {
	const size_t stride0 = stride(0);
	const size_t stride1 = stride(1);
	const size_t stride2 = stride(2);
	const size_t gstride0 = gradient.stride(0);
	const size_t gstride1 = gradient.stride(1);
	const size_t gstride2 = gradient.stride(2);
	
#define CELL(p, x, y, z, k) (((T *)((p) + (((x)&mDimWrapX)*stride0) +  (((y)&mDimWrapY)*stride1) +  (((z)&mDimWrapZ)*stride2)))[(k)])
#define CELLG(p, x, y, z) (((T *)((p) + (((x)&mDimWrapX)*gstride0) +  (((y)&mDimWrapY)*gstride1) +  (((z)&mDimWrapZ)*gstride2)))[0])
	
	// calculate gradient.
	// previous instantaneous magnitude of velocity gradient
	//		= average of velocity gradients per axis:
	const double hx = -0.5/mDimX; //1./3.; //0.5/mDim;
	const double hy = -0.5/mDimY; //1./3.; //0.5/mDim;
	const double hz = -0.5/mDimZ; //1./3.; //0.5/mDim;
	float * iptr = front().data;
	float * gptr = gradient.data;
	
	for (size_t z=0;z<mDimZ;z++) {
		for (size_t y=0;y<mDimY;y++) {
			for (size_t x=0;x<mDimX;x++) {
				// gradients per axis:
				const T xgrad = CELL(iptr, x+1,y,	z,	0) - CELL(iptr, x-1,y,	z,	0);
				const T ygrad = CELL(iptr, x,	y+1,z,	1) - CELL(iptr, x,	y-1,z,	1);
				const T zgrad = CELL(iptr, x,	y,	z+1,2) - CELL(iptr, x,	y,	z-1,2);
				// gradient at current cell:
				const T grad = T(hx*xgrad + hy*ygrad + hz*zgrad);
				// add to 1-plane field
				CELLG(gptr, x, y, z) += grad;
			}
		}
	}
#undef CELL
#undef CELLG
}

template<typename T>
inline void Field3D<T> :: calculateGradient(Array<glm::vec3>& gradient) {
	const Array<T>& arr = front();
	
#define CELL(p, x, y, z, k) (((T *)(((char *)p) + (((x)&mDimWrapX)*stride0) +  (((y)&mDimWrapY)*stride1) +  (((z)&mDimWrapZ)*stride2)))[(k)])
	
	float * iptr = front().data;
	glm::vec3 * gptr = gradient.data;
	
	int i=0;
	for (size_t z=0;z<mDimZ;z++) {
		for (size_t y=0;y<mDimY;y++) {
			for (size_t x=0;x<mDimX;x++) {
				const T x0 = iptr[arr.index(x  , y, z)];	// or just x?
				const T x2 = iptr[arr.index(x+1, y, z)];
				const T y0 = iptr[arr.index(x, y  , z)];
				const T y2 = iptr[arr.index(x, y+1, z)];
				const T z0 = iptr[arr.index(x, y, z  )];
				const T z2 = iptr[arr.index(x, y, z+1)];
				// gradient is simply this:
				gptr[i] = glm::vec3(x2, y2, z2) - glm::vec3(x0, y0, z0);
				i++;
			}
		}
	}
#undef CELL
}

template<typename T>
inline void Field3D<T> :: subtractGradientMagnitude(const Array<T>& gradient) {
	const size_t stride0 = stride(0);
	const size_t stride1 = stride(1);
	const size_t stride2 = stride(2);
	const size_t gstride0 = gradient.stride(0);
	const size_t gstride1 = gradient.stride(1);
	const size_t gstride2 = gradient.stride(2);
	
#define INDEX(p, x, y, z) ((T *)(p + ((((x)&mDimWrapX)*stride0) + (((y)&mDimWrapY)*stride1) + (((z)&mDimWrapZ)*stride2))))
#define CELLG(p, x, y, z) (((T *)((p) + (((x)&mDimWrapX)*gstride0) +  (((y)&mDimWrapY)*gstride1) +  (((z)&mDimWrapZ)*gstride2)))[0])
	
	// now subtract gradient from current field:
	float * gptr = gradient.data;
	float * optr = front().data;
	//const double h = 1.; ///3.;
	const float hx = mDimX * 0.5f;
	const float hy = mDimY * 0.5f;
	const float hz = mDimZ * 0.5f;
	for (size_t z=0;z<mDimZ;z++) {
		for (size_t y=0;y<mDimY;y++) {
			for (size_t x=0;x<mDimX;x++) {
				// cell to update:
				T * vel = INDEX(optr, x, y, z);
				// gradients per axis:
				vel[0] -= hx * ( CELLG(gptr, x+1,y,	z  ) - CELLG(gptr, x-1,y,	z  ) );
				vel[1] -= hy * ( CELLG(gptr, x,	y+1,z  ) - CELLG(gptr, x,	y-1,z  ) );
				vel[2] -= hz * ( CELLG(gptr, x,	y,	z+1) - CELLG(gptr, x,	y,	z-1) );
			}
		}
	}
	
#undef INDEX
#undef CELLG
}

#endif
