

#include <string>

#ifdef _WIN32

#include <Windows.h>
#include <comdef.h>
#include <pathcch.h>

#endif

std::string getExecDirectory() {

#ifdef WIN32

  WCHAR path[MAX_PATH];
  GetModuleFileNameW(NULL, path, MAX_PATH);
  PathCchRemoveFileSpec(path, MAX_PATH);

  return std::string(_bstr_t(path));

#endif
}

char separator() {
#ifdef _WIN32
  return '\\';
#else
  return '/';
#endif
}