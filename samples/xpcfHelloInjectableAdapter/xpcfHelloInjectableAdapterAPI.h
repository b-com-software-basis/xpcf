#ifndef xpcfHelloInjectableAdapterAPI_H
#define xpcfHelloInjectableAdapterAPI_H

#if _WIN32
#ifdef xpcfHelloInjectableAdapter_API_DLLEXPORT
#define xpcfHelloInjectableAdapter_EXPORT_API __declspec(dllexport)
#else
#define xpcfHelloInjectableAdapter_EXPORT_API __declspec(dllimport)
#endif
#else //!_WIN32
#define xpcfHelloInjectableAdapter_EXPORT_API
#endif //!_WIN32

#endif // XPCFSAMPLECOMPONENTAPI_H
