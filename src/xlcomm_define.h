// 定义xlcomm项目总体通用的宏
// Author: xuel

#ifndef XLCOMM_DEFINE_H_
#define XLCOMM_DEFINE_H_

#if _MSC_VER
#define XLCOMM_MSVC   
#endif  // _MSC_VER

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