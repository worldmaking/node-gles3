#ifndef AL_PLATFORM_H
#define AL_PLATFORM_H

#ifdef _MSC_VER
    #define AL_WIN
    #define WIN32_LEAN_AND_MEAN
    #define VC_EXTRALEAN
    #define NOMINMAX
    #include <Windows.h>
    #define AL_EXPORT __declspec(dllexport)
    
    #ifdef AL_IMPLEMENTATION
        #define AL_ALICE_EXPORT __declspec(dllexport)
    #else
        #define AL_ALICE_EXPORT __declspec(dllimport)
    #endif

    const char * GetLastErrorAsString() {
        static char buf[256];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                buf, sizeof(buf), NULL);
        return buf;
    }

#endif 

#ifdef __APPLE__
    #define AL_OSX
    #define AL_EXPORT
    #define AL_ALICE_EXPORT
#endif

bool isPlatformWindows() {
    #ifdef AL_WIN
    return true;
    #else
    return false;
    #endif
}

template<typename T, typename U> constexpr size_t al_offsetof(U T::*member) {
    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
}

template<typename T, typename U> constexpr size_t al_sizeof(U T::*member) {
    return sizeof((T*)nullptr->*member);
}

#endif // AL_PLATFORM_H
