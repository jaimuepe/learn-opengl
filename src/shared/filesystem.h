#ifndef MY_FILESYSTEM_H
#define MY_FILESYSTEM_H

#include <filesystem>
#include <optional>
#include <string>
#include <limits.h>

#ifdef _WIN32

// to avoid redefinition warning
#undef APIENTRY

#include <Windows.h>
#include <comdef.h>
#include <pathcch.h>

#elif linux
#include <unistd.h>

#endif

namespace
{

#ifdef WIN32
  std::string getExecDirectory()
  {

    WCHAR path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    PathCchRemoveFileSpec(path, MAX_PATH);

    return std::string(_bstr_t(path));
  }
#elif linux
  std::string getExecDirectory()
  {

    char buff[PATH_MAX];

    ssize_t len = readlink("/proc/self/exe", buff, sizeof(buff) - 1);

    for (ssize_t i = len - 1; i >= 0; --i)
    {
      if (buff[i] == '/')
      {
        buff[i] = '\0';
        break;
      }
    }
    return std::string(buff);
  }
#endif

  std::filesystem::path execPath = std::filesystem::path(getExecDirectory());

  std::filesystem::path binPath = execPath.parent_path().parent_path();

  std::filesystem::path resPath =
      std::filesystem::path(binPath).append("resources");

} // namespace

#ifdef _WIN32
constexpr char separator = '\\';
#else
constexpr char separator = '/';
#endif

std::filesystem::path getExecPath()
{
  return execPath;
}

std::filesystem::path getBinPath() { return binPath; }

std::filesystem::path getResPath() { return resPath; }

std::string getExecDirectory() { return execPath.string(); }

std::string getBinDirectory() { return binPath.string(); }

std::string getResDirectory() { return resPath.string(); }

#endif // MY_FILESYSTEM_H