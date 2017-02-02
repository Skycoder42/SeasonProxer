#ifndef QTMVVM_CORE_GLOBAL_H
#define QTMVVM_CORE_GLOBAL_H

#include <QtGlobal>

#if defined(QTMVVM_CORE_LIBRARY)
#  define QTMVVM_CORE_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTMVVM_CORE_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QTMVVM_CORE_GLOBAL_H
