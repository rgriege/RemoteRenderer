#ifdef STREAM_EXPORTS
    #define STREAM_API __declspec(dllexport)
#else
    #define STREAM_API __declspec(dllimport)
#endif