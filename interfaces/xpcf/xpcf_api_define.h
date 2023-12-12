#ifndef XPCF_API_DEFINE_H
#define XPCF_API_DEFINE_H

#if _WIN32
#if XPCF_SHARED
#ifdef xpcf_API_DLLEXPORT
#define XPCF_EXPORT_API __declspec(dllexport)
#else
#ifdef xpcf_static_deps_API_DLLEXPORT
#define XPCF_EXPORT_API __declspec(dllexport)
#else
#define XPCF_EXPORT_API __declspec(dllimport)
#endif
#endif
#else
#define XPCF_EXPORT_API
#endif
#else
#define XPCF_EXPORT_API
#endif

#endif // XPCF_API_DEFINE_H
