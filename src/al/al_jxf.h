#ifndef AL_JXF_H
#define AL_JXF_H

#include "al_math.h"

/*
	Read/write .jxf (Jitter matrix) files
	See https://cycling74.com/sdk/MaxSDK-6.0.4/html/chapter_jit_jxf.html

	Note that .jxf stores in BigEndian (unlike all modern PCs)
	so it needs to be converted as it is read. 
*/

#define SWAP16(x) ((int16_t)(((((uint16_t)(x))>>8)&0x00ff)+((((uint16_t)(x))<<8)&0xff00)))
#define SWAP32(x) ((int32_t)(((((uint32_t)(x))>>24L)&0x000000ff)+((((uint32_t)(x))>>8L)&0x0000ff00)+ \
((((uint32_t)(x))<<24L)&0xff000000)+((((uint32_t)(x))<<8L)&0x00ff0000)))

// This is the basic container definition


struct JXFHeader {
	uint32_t container_id;      //'FORM'
	int32_t filesize;     	 	// filesize
	uint32_t form_id;   		//'JIT!'
	uint32_t version_id;    	//'FVER'
	int32_t version_size;      	//12
	uint32_t version;    		//timestamp
	uint32_t matrix_id;       	//'MTRX'
	int32_t size;     	 		//varies(should be equal to 
		//24+(4*dimcount)+(typesize*planecount*totalpoints))
	int32_t offset;      		//data offset(should be equal to 24+(4*dimcount))
	uint32_t type;     			//'CHAR','LONG','FL32','FL64'
	int32_t planecount;
	int32_t dimcount;
	int32_t dim[3];
};



#endif