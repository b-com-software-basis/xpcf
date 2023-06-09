#include "RemakenPathHelper.h"
#include <boost/predef.h>
#include <iostream>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>

#ifdef WIN32
#include <stdlib.h>
#else
#include <pwd.h>
#include <map>
#include <sys/types.h>
#endif

using namespace std;
RemakenPathHelper::RemakenPathHelper()
{

}

#ifndef ROOTFOLDERENV
#define ROOTFOLDERENV "REMAKEN_PKG_ROOT"
#endif

class Constants {
public:
    static constexpr const char * REMAKENPKGROOT = ROOTFOLDERENV;
    static constexpr const char * XPCFMODULEROOT = "XPCF_MODULE_ROOT";
    static constexpr const char * REMAKENPKGFILENAME = ".packagespath";
    static constexpr const char * REMAKEN_FOLDER = ".remaken";
    static constexpr const char * REMAKEN_PROFILES_FOLDER = "profiles";
    static constexpr const char * REMAKEN_CACHE_FILE = ".remaken-cache";
    static constexpr const char * ARTIFACTORY_API_KEY = "artifactoryApiKey";
    static constexpr const char * QMAKE_RULES_DEFAULT_TAG = "4.8.0";
    static constexpr const char * PKGINFO_FOLDER = ".pkginfo";
    static constexpr const char * VCPKG_REPOURL = "https://github.com/microsoft/vcpkg";
};

std::string getOS()
{
#ifdef BOOST_OS_ANDROID_AVAILABLE
    return "android";
#endif
#ifdef BOOST_OS_IOS_AVAILABLE
    return "ios";
#endif
#ifdef BOOST_OS_LINUX_AVAILABLE
    return "linux";
#endif
#ifdef BOOST_OS_SOLARIS_AVAILABLE
    return "solaris";
#endif
#ifdef BOOST_OS_MACOS_AVAILABLE
    return "mac";
#endif
#ifdef BOOST_OS_WINDOWS_AVAILABLE
    return "win";
#endif
}

std::string getToolChain()
{
#ifdef BOOST_OS_ANDROID_AVAILABLE
    return "clang";
#endif
#ifdef BOOST_OS_IOS_AVAILABLE
    return "clang";
#endif
#ifdef BOOST_OS_LINUX_AVAILABLE
    return "gcc";
#endif
#ifdef BOOST_OS_SOLARIS_AVAILABLE
    return "gcc";
#endif
#ifdef BOOST_OS_MACOS_AVAILABLE
    return "clang";
#endif
#ifdef BOOST_OS_WINDOWS_AVAILABLE
    return "cl";
#endif
}


fs::path RemakenPathHelper::getUTF8PathObserver(const char * sourcePath)
{
    fs::detail::utf8_codecvt_facet utf8;
    fs::path utf8ObservedPath(sourcePath, utf8);
    return utf8ObservedPath;
}

fs::path RemakenPathHelper::getUTF8PathObserver(const std::string & sourcePath)
{
    return getUTF8PathObserver(sourcePath.c_str());
}

fs::path RemakenPathHelper::getHomePath()
{
    char * homePathStr;
    fs::path homePath;
#ifdef WIN32
    homePathStr = getenv("USERPROFILE");
    if (homePathStr == nullptr) {
        homePathStr = getenv("HOMEDRIVE");
        if (homePathStr) {
            homePath = getUTF8PathObserver(homePathStr);
            homePath /= getenv("HOMEPATH");
        }
    }
    else {
        homePath = getUTF8PathObserver(homePathStr);
    }
#else
    struct passwd* pwd = getpwuid(getuid());
    if (pwd) {
        homePathStr = pwd->pw_dir;
    }
    else {
        // try the $HOME environment variable
        homePathStr = getenv("HOME");
    }
    homePath = getUTF8PathObserver(homePathStr);
#endif
    return homePath;
}

fs::path RemakenPathHelper::getRemakenRoot() {
fs::detail::utf8_codecvt_facet utf8;
fs::path remakenRootPath = getHomePath() / Constants::REMAKEN_FOLDER;
remakenRootPath /= "packages";
char * rootDirectoryVar = getenv(Constants::REMAKENPKGROOT);
if (rootDirectoryVar != nullptr) {
    std::cerr<<Constants::REMAKENPKGROOT<<" environment variable exists"<<std::endl;
    remakenRootPath = rootDirectoryVar;
}
else if (fs::exists(remakenRootPath / Constants::REMAKENPKGFILENAME)) {
    fs::path pkgFile = remakenRootPath / Constants::REMAKENPKGFILENAME;
    ifstream fis(pkgFile.string(utf8),ios::in);
    fs::path pkgPath;
    while (!fis.eof()) {
        std::string curLine;
        getline(fis,curLine);
        if (!curLine.empty()) {
            pkgPath = curLine;
        }
    }
    fis.close();
    remakenRootPath = pkgPath;
}
return remakenRootPath;
}

fs::path RemakenPathHelper::computeRemakenRootPackageDir()
{
    fs::detail::utf8_codecvt_facet utf8;
    fs::path osSubFolder(getOS() + "-" + getToolChain() , utf8);
    fs::path remakenRootDir = getRemakenRoot()/osSubFolder;
    return remakenRootDir;
}

static const std::map<const std::string_view,const  std::string_view> os2sharedPathEnv = {
    {"mac","DYLD_LIBRARY_PATH"},
    {"win","PATH"},
    {"unix","LD_LIBRARY_PATH"},
    {"android","LD_LIBRARY_PATH"},
    {"ios","DYLD_LIBRARY_PATH"},
    {"linux","LD_LIBRARY_PATH"}
};


const std::string_view & RemakenPathHelper::sharedLibraryPathEnvName()
{
    if (os2sharedPathEnv.find(getOS()) == os2sharedPathEnv.end()) {
        return os2sharedPathEnv.at("unix");
    }
    return os2sharedPathEnv.at(getOS());
}
