// 定义xlcomm总体通用的宏
// Author: xuel

#ifndef XLCOMM_DEFINE_H_
#define XLCOMM_DEFINE_H_

#ifndef XLCOMM_MSVC
#define XLCOMM_MSVC   _MSC_VER
#endif  // XLCOMM_MSVC

#ifdef XLCOMM_MSVC
#ifdef XLCOMM_EXPORT
#define XLCOMM_API    __declspec(dllexport)
#else
#define XLCOMM_API    __declspec(dllimport)
#endif  // XLCOMM_EXPORT
#else
#define XLCOMM_API    
#endif  // XLCOMM_MSVC

#endif  // XLCOMM_DEFINE_H_