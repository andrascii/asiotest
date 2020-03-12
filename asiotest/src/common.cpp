#include "common.h"
#include <string>

#if defined(WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <pthread.h>
#else
#include <sys/prctl.h>
#endif

#if defined (WIN32)
#define USE_WINDOWS_SET_THREAD_NAME_HACK
#endif

#if defined (USE_WINDOWS_SET_THREAD_NAME_HACK)

namespace {

#pragma pack(push,8)
  typedef struct tagTHREADNAME_INFO {
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
  } THREADNAME_INFO;
#pragma pack(pop)

void win_set_thread_name(DWORD threadID, const std::string& thread_name) {
  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = thread_name.data();
  info.dwThreadID = threadID;
  info.dwFlags = 0;

  __try {
    const DWORD MS_VC_EXCEPTION = 0x406D1388;
    RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
  }
}

}

#endif

namespace Common {

void set_thread_name(const std::string& name) {
#ifdef WIN32
  win_set_thread_name(static_cast<DWORD>(-1), name);
#elif defined(__APPLE__)
  //const pthread_t pid = pthread_self();
  pthread_setname_np(name.data());
#else
  prctl(PR_SET_NAME, name.data(), 0, 0, 0);
#endif
}

}
