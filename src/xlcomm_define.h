// 定义xlcomm项目总体通用的宏
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

#define XLCOMM_NOT_COPY_OR_MOVE(class_name)   \
  class_name##(const class_name##&) = delete; \
  class_name##& operator=(const class_name##&) = delete;

#define XLCOMM_MOVE_ONLY(class_name)   \
  class_name##(class_name##&&) = default; \
  class_name##& operator=(class_name##&&) = default;

#endif  // XLCOMM_DEFINE_H_