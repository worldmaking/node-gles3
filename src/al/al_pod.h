#ifndef AL_POD_H
#define AL_POD_H

#include "include/al/al_math.h"

/*
	A collection of data structures for algorithms
	that work with Plain Old Data (POD) types only
	That means, **no pointers and no virtuals**

	POD types are thus fully serializable and resumable;
	amenable to network streaming, mmap, memcpy, etc. 

	Identities must be representecd by array indices of where they are primarily stored
	Linked lists need to be arrays of such indices
	etc. 
*/


/*
	a super-simple stack to store up to N IDs. 
	Assuming T is POD-friendly, so is this:
*/
template<int N=128, typename T=int32_t>
struct Lifo {
	T list[N];
	int64_t count;
	
	void init(bool prefill=false) {
		count = 0;
		if (prefill) {
			for (count=0; count<N; ) push(T(count));
		}
	}
	
	bool push(T name) {
		if (count < N) {
			list[count] = name;
			count++;
			return true;
		}
		return false;
	}
	

	T pop() {
		if (count > 0) {
			count--;
			return list[count];
		}
		return T(-1);
	}
	
	int available() { return count; }
	int empty() { return count <= 0; }
};

/*
	A simple base-class for objects that are typed and located within a list
*/
struct TypedIdentity {
	int32_t type = 0;
	int32_t idx = 0;
};

/*
	A very simplistic 2D neighbourhood storage
	Each cell can only record zero or one occupants at a time
	No pointers or virtuals, thus POD-friendly
*/
template<int SPACE_DIM>
struct Space {

	TypedIdentity cells[SPACE_DIM * SPACE_DIM];

	// notice that it isn't bounds-checking
	static const int32_t raw_index(glm::vec2 pos) {
		return int32_t(pos.x * SPACE_DIM) + int32_t(pos.y * SPACE_DIM)*SPACE_DIM;
	}
	
	void unset(glm::vec2 pos) {
		TypedIdentity& o = cells[raw_index(pos)];
		o.type = 0;
		o.idx = -1;
	}
	
	void set(TypedIdentity& a, glm::vec2 pos) {
		TypedIdentity& o = cells[raw_index(pos)];
		o.type = a.type;
		o.idx = a.idx;
	}
	
	void set(TypedIdentity * a, glm::vec2 pos) {
		TypedIdentity& o = cells[raw_index(pos)];
		o.type = a->type;
		o.idx = a->idx;
	}
	
	TypedIdentity * get(glm::vec2 pos) {
		return &cells[raw_index(pos)];
	}
	
	void reset() {
		for (int i=0; i<SPACE_DIM * SPACE_DIM; i++) {
			cells[i].type = 0;
			cells[i].idx = -1;
		}
	}
};


#endif // AL_POD_H