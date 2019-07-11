#ifndef xpcfHelloConfigurableAPI_H
#define xpcfHelloConfigurableAPI_H

#if _WIN32
#ifdef xpcfHelloConfigurable_API_DLLEXPORT
#define xpcfHelloConfigurable_EXPORT_API __declspec(dllexport)
#else
#define xpcfHelloConfigurable_EXPORT_API __declspec(dllimport)
#endif
#else //!_WIN32
#define xpcfHelloConfigurable_EXPORT_API
#endif //!_WIN32

#endif // XPCFSAMPLECOMPONENTAPI_H
