#ifndef STREAM_PREREQS_H
#define STREAM_PREREQS_H

#ifdef _WIN32
    #ifdef STREAM_EXPORTS
        #define STREAM_API __declspec(dllexport)
    #else
        #define STREAM_API __declspec(dllimport)
    #endif
#else
    #define STREAM_API
#endif

#endif // STREAM_PREREQS_H
