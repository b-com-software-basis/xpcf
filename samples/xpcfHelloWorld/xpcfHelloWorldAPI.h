#ifndef xpcfHelloWorldAPI_H
#define xpcfHelloWorldAPI_H

#if _WIN32
#ifdef xpcfHelloWorld_API_DLLEXPORT
#define xpcfHelloWorld_EXPORT_API __declspec(dllexport)
#else
#define xpcfHelloWorld_EXPORT_API __declspec(dllimport)
#endif
#else //!_WIN32
#define xpcfHelloWorld_EXPORT_API
#endif //!_WIN32

#endif // XPCFSAMPLECOMPONENTAPI_H
