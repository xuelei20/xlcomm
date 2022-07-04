// 定义xlcomm项目整体通用的宏
// Author: xuel

#ifndef XLCOMM_XLCOMM_DEFINE_H_
#define XLCOMM_XLCOMM_DEFINE_H_

#ifdef _MSC_VER
#ifdef XLCOMM_EXPORT
#define XLCOMM_API    __declspec(dllexport)
#else
#define XLCOMM_API    __declspec(dllimport)
#endif  // XLCOMM_EXPORT
#else
#define XLCOMM_API 
#endif  // _MSC_VER

#endif  // XLCOMM_XLCOMM_DEFINE_H_