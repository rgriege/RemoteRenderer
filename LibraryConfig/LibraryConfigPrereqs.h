#ifndef LIBRARY_CONFIG_PREREQS_H
#define LIBRARY_CONFIG_PREREQS_H

#ifdef _WIN32
    #ifdef LibraryConfig_EXPORTS
        #define LIBRARY_CONFIG_API __declspec(dllexport)
    #else
        #define LIBRARY_CONFIG_API __declspec(dllimport)
    #endif
#else
    #define LIBRARY_CONFIG_API
#endif

#endif // LIBRARY_CONFIG_PREREQS_H
