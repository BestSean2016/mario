#ifndef PLEDITOR_GLOBAL_H
#define PLEDITOR_GLOBAL_H

#define QT_VISIBILITY_AVAILABLE


#  ifdef _WINDOWS_
#    define DLL_EXPORT     __declspec(dllexport)
#    define DLL_IMPORT     __declspec(dllimport)
#  elif defined(QT_VISIBILITY_AVAILABLE)
#    define DLL_EXPORT     __attribute__((visibility("default")))
#    define DLL_IMPORT     __attribute__((visibility("default")))
#    define DLL_HIDDEN     __attribute__((visibility("hidden")))
#  endif


#if defined(PLEDITOR_LIBRARY)
#  define PLED_API DLL_EXPORT
#else
#  define PLED_API DLL_IMPORT
#endif

#ifndef UNUSE
#define UNUSE(e) (void)(e)
#endif //UNUSE




#endif // PLEDITOR_GLOBAL_H
