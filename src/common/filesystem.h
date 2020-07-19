
#include <filesystem>
#include <optional>
#include <string>

#ifdef _WIN32

#include <Windows.h>
#include <comdef.h>
#include <pathcch.h>

#endif

namespace {

std::string getExecDirectory() {

#ifdef WIN32

  WCHAR path[MAX_PATH];
  GetModuleFileNameW(NULL, path, MAX_PATH);
  PathCchRemoveFileSpec(path, MAX_PATH);
  return std::string(_bstr_t(path));

#endif
}

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

std::filesystem::path getExecPath() { return execPath; }

std::filesystem::path getBinPath() { return binPath; }

std::filesystem::path getResPath() { return resPath; }

std::string getExecDirectory() { return execPath.string(); }

std::string getBinDirectory() { return binPath.string(); }

std::string getResDirectory() { return resPath.string(); }