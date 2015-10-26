#ifndef FVIZ_H
#define FVIZ_H

#if defined(WIN32)
#ifdef FVIZ_BUILDING_DLL
#define FVIZ_PUBLIC __declspec(dllexport)
#else
#define FVIZ_PUBLIC __declspec(dllimport)
#endif
#else
#define FVIZ_PUBLIC
#endif


#include <string>

FVIZ_PUBLIC
void foo(std::string msg);

#endif  // FVIZ_H
