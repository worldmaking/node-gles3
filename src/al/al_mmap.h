#ifndef AL_MMAP_H
#define AL_MMAP_H

#include "al_platform.h"

// TODO: how many of these are really needed?
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef AL_OSX
#include <unistd.h>
#include <sys/mman.h>
#endif


template<typename T>
struct Mmap {

	T * shared = 0;
	int readWrite = 0;

	// the memory map share:
	#ifdef AL_WIN
		HANDLE mmap_handle = 0;
	#endif
	
	#ifdef AL_OSX
		int fd = -1;
	#endif

	// Windows code is currently wrong
	// See https://stackoverflow.com/questions/19498925/createfilemapping-returns-null

	T * create(std::string path, bool readWrite=0) {
		// TODO assert shared == 0
		this->readWrite = readWrite;
		printf("mmap %s of size %d\n", path.c_str(), sizeof(T));
		
		#ifdef AL_WIN
			HANDLE file = CreateFileA(path.c_str(), 
				readWrite ? GENERIC_READ | GENERIC_WRITE : GENERIC_READ, // what I want to do with it
				FILE_SHARE_READ | FILE_SHARE_WRITE, // what I want to allow others to do with it
				NULL, // change this to allow child processes to inherit the handle
				OPEN_ALWAYS, // how to proceed if file exists or does not exist: open if exists, create if it doesn't
				FILE_ATTRIBUTE_NORMAL, // any special attributes
				NULL
			);
			if (file == INVALID_HANDLE_VALUE) {
				fprintf(stderr, "Error opening/creating file %s: %s", path.c_str(), GetLastErrorAsString()); 
				return 0;
			}

			
			mmap_handle = CreateFileMappingA(file, 
				NULL, // change this to allow child processes to inherit the handle
				readWrite ? PAGE_READWRITE : PAGE_READONLY, // what I want to do with it
				0, sizeof(T), // size to map
				NULL // name
			); 
			//mmap_handle = OpenFileMappingA(FILE_MAP_READ, FALSE, path);
			if (!mmap_handle) {
				fprintf(stderr, "Error mapping file %s: %s", path.c_str(), GetLastErrorAsString()); 
				CloseHandle(file);
				return 0;
			}
			
			shared = (T *)MapViewOfFile(mmap_handle, readWrite ? FILE_MAP_WRITE : FILE_MAP_READ, 0, 0, sizeof(T));
			if (!shared) {
				CloseHandle(file);
				CloseHandle(mmap_handle);
				mmap_handle = NULL;
				fprintf(stderr, "Error mapping view of file %s: %s", path.c_str(), GetLastErrorAsString()); 
				return 0;
			}	
		#endif

		#ifdef AL_OSX
			// create:
			fd = open(path.c_str(), readWrite ? O_CREAT | O_RDWR : O_RDONLY, 0666); // 0666 or 0644 or 0600?
			if (fd == -1) {
				fprintf(stderr, "Error opening file for writing");
				return 0;
			}   
			// validate size
			struct stat fileInfo = {0};
			if (fstat(fd, &fileInfo) == -1) {
				fprintf(stderr, "Error getting the file size");
				return 0;
			}
			fprintf(stdout, "file %s is size %ji", path.c_str(), (intmax_t)fileInfo.st_size);
			// stretch / verify size
			if (readWrite) {
				if (fileInfo.st_size < sizeof(T)) {
					if (lseek(fd, sizeof(T)-1, SEEK_SET) == -1 || write(fd, "", 1) == -1) {
						close(fd);
						fprintf(stderr, "Error stretching the file");
						return 0;
					}
					// update size
					if (fstat(fd, &fileInfo) == -1) {
						fprintf(stderr, "Error getting the file size");
						return 0;
					}
				}
			}
			fprintf(stdout, "file %s is size %ji", path.c_str(), (intmax_t)fileInfo.st_size);
			
			if (fileInfo.st_size == 0) {
				fprintf(stderr, "file %s is empty", path.c_str());
				return 0;
			} else if (fileInfo.st_size < sizeof(T)) {
				fprintf(stderr, "file %s is too small", path.c_str());
				return 0;
			} 
			
			auto flag = PROT_READ;
			if (readWrite) flag |= PROT_WRITE;			
			shared = (T *)mmap(0, sizeof(T), flag, MAP_SHARED, fd, 0);
			if (shared == MAP_FAILED) {
				close(fd);
				fprintf(stderr, "mmapping the file");
				return 0;
			}	
			
		#endif
		return shared;
	}
	
	~Mmap() {
		destroy();
	}
	
	// sync writes changes to disk
	bool sync() {
		if (!shared) return true;

		#ifdef AL_WIN 
			if (readWrite && !FlushViewOfFile(shared, 0)) {
				fprintf(stderr, "Could not sync the file to disk: %s", GetLastErrorAsString()); 
				return false;
			}
		#endif
		#ifdef AL_OSX
			if (readWrite && msync(shared, sizeof(T), MS_SYNC) == -1) {
				fprintf(stderr, "Could not sync the file to disk");
				return false;
			}
		#endif 
		return true;
	}
	
	void destroy(bool doSync=false) {
		if (!shared) return;
		
		if (doSync) sync();

		#ifdef AL_WIN
			if (mmap_handle) {
				UnmapViewOfFile(shared);
				shared = 0;
			}
			if (mmap_handle) {
				CloseHandle(mmap_handle);
				mmap_handle = 0;
			}
		#endif
		
		#ifdef AL_OSX
			// Don't forget to free the mmapped memory
			if (munmap(shared, sizeof(T)) == -1) {
				fprintf(stderr, "Error un-mmapping the file");
			}
			if (fd > 0) {
				close(fd);
				fd = -1;
			}
		#endif
		
		shared = 0;
	}
};

#endif //AL_MMAP_H
