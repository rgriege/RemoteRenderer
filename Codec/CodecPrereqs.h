#ifndef CODEC_PREREQS_H
#define CODEC_PREREQS_H

#ifdef _WIN32
    #ifdef Codec_EXPORTS
        #define CODEC_API __declspec(dllexport)
    #else
        #define CODEC_API __declspec(dllimport)
    #endif
#else
    #define CODEC_API
#endif

#endif // CODEC_PREREQS_H
