#ifndef xpcfHelloPeopleAPI_H
#define xpcfHelloPeopleAPI_H

#if _WIN32
#ifdef xpcfHelloPeople_API_DLLEXPORT
#define xpcfHelloPeople_EXPORT_API __declspec(dllexport)
#else
#define xpcfHelloPeople_EXPORT_API __declspec(dllimport)
#endif
#else //!_WIN32
#define xpcfHelloPeople_EXPORT_API
#endif //!_WIN32

#endif // XPCFSAMPLECOMPONENTAPI_H
