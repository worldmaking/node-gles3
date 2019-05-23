#ifndef AL_FS_H
#define AL_FS_H

#include <string>

#include "al_platform.h"

#ifndef AL_FILE_DELIMITER_STR
	#ifdef AL_WIN
		#define AL_FILE_DELIMITER_STR	"\\"
	#else
		#define AL_FILE_DELIMITER_STR	"/"
	#endif
#endif
#define AL_FILE_DELIMITER (AL_FILE_DELIMITER_STR[0])

// remove trailing slash from a path string
std::string al_fs_strip_post_slash(const std::string& path){
	if (path.back() == '\\' || path.back() == '/'){
		return path.substr(0, path.size()-1);
	}
	return path;
}

std::string al_fs_strip_pre_slash(const std::string& path){
	if (path.front() == '\\' || path.front() == '/'){
		return path.substr(1, path.size());
	}
	return path;
}

std::string al_fs_dirname(const std::string path) {
    size_t pos = path.find_last_of(AL_FILE_DELIMITER);
    if(std::string::npos != pos){
        return path.substr(0, pos+1);
    }
    return "." AL_FILE_DELIMITER_STR;
}

std::string al_fs_extension(const std::string& path){
	size_t pos = path.find_last_of('.');
	if(path.npos != pos){
		return path.substr(pos);
	}
	return "";
}

#ifdef AL_WIN

#include <direct.h> // _getcwd
#include <Shlwapi.h> // PathFileExists, PathIsDirectory

namespace al {
    namespace fs {
        struct ReadOnlyFileHandle{
            HANDLE mHandle = INVALID_HANDLE_VALUE;
            ReadOnlyFileHandle(const std::string& path){
                mHandle = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
            }
            ~ReadOnlyFileHandle(){
                if(opened()) CloseHandle(mHandle);
            }
            bool opened() const { return mHandle != INVALID_HANDLE_VALUE; }
        };
    }
}

double al_fs_filetime_seconds(const FILETIME& fileTime){
	// File times in 100-nanosecond intervals that have elapsed since
	// 12:00 A.M. January 1, 1601 Coordinated Universal Time (UTC).
	// Need to return seconds since Jan 1, 1970. The difference is 134774 days.
	// https://msdn.microsoft.com/en-us/library/ms724290(v=vs.85).aspx
    ULARGE_INTEGER t;
	t.LowPart = fileTime.dwLowDateTime;
	t.HighPart = fileTime.dwHighDateTime;
	return t.QuadPart * 1e-7 - 11644473600.;
}

// returns file modification date
double al_fs_modified(const std::string path) {
	al::fs::ReadOnlyFileHandle fh(path);
    if (fh.opened()){
		FILETIME ftCreate, ftAccess, ftWrite;
		if(GetFileTime(fh.mHandle, &ftCreate, &ftAccess, &ftWrite)){
			return al_fs_filetime_seconds(ftWrite);
		}
	}
	return 0.;
}

double al_fs_accessed(const std::string path) {
	al::fs::ReadOnlyFileHandle fh(path);
    if (fh.opened()){
		FILETIME ftCreate, ftAccess, ftWrite;
		if(GetFileTime(fh.mHandle, &ftCreate, &ftAccess, &ftWrite)){
			return al_fs_filetime_seconds(ftAccess);
		}
	}
	return 0.;
}

size_t al_fs_size(const std::string path) {
    al::fs::ReadOnlyFileHandle fh(path);
    if(fh.opened()){
		return GetFileSize(fh.mHandle, NULL);
	}
	return 0;
}

std::string al_fs_absolute(const std::string& path) {
	TCHAR dirPart[4096];
	TCHAR ** filePart={NULL};
	GetFullPathName((LPCTSTR)path.c_str(), sizeof(dirPart), dirPart, filePart);
	std::string result = (char *)dirPart;
	if(filePart != NULL && *filePart != 0){
		result += (char *)*filePart;
	}
	return result;
}

bool al_fs_exists(const std::string& path){
	return PathFileExists(path.c_str());
}

bool al_fs_isDirectory(const std::string& path){
	return PathIsDirectory(path.c_str());
}

#else

#include <dirent.h>
#include <unistd.h> // rmdir, getcwd
#include <stdlib.h> // realpath
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

// returns file modification date
double al_fs_modified(const std::string path) {
	struct stat s;
	if(::stat(path.c_str(), &s) == 0){
		return s.st_mtime;
	}
	return 0.;
}

double al_fs_accessed(const std::string path) {
	struct stat s;
	if(::stat(path.c_str(), &s) == 0){
		return s.st_atime;
	}
	return 0.;
}

size_t al_fs_size(const std::string path) {
	struct stat s;
	if(::stat(path.c_str(), &s) == 0){
		return s.st_size;
	}
	return 0;
}

std::string al_fs_absolute(const std::string& path) {
	char temp[PATH_MAX];
	char * result = realpath(path.c_str(), temp);
	return result ? result : "";
}

bool al_fs_exists(const std::string& path){
	struct stat s;
	return ::stat(al_fs_strip_post_slash(path).c_str(), &s) == 0;
}

bool al_fs_isDirectory(const std::string& path){
	struct stat s;
	if(0 == ::stat(al_fs_strip_post_slash(path).c_str(), &s)){	// exists?
		if(s.st_mode & S_IFDIR){		// is dir?
			return true;
		}
	}
	// if(s.st_mode & S_IFREG) // is file?
	return false;
}

#endif // not AL_WIN

#endif // AL_FS_H