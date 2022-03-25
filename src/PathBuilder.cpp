/**
 * @copyright Copyright (c) 2017 B-com http://www.b-com.com/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author Loïc Touraine
 *
 * @file
 * @brief description of file
 * @date 2017-11-23
 */

#include "private/xpcf/PathBuilder.h"
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
#include <boost/predef/os.h>

#ifdef WIN32
#include <stdlib.h>
#else
#include <pwd.h>
#include <sys/types.h>
#endif

#include <regex>

#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
namespace fs = boost::filesystem;

namespace org { namespace bcom { namespace xpcf {

PathBuilder::PathBuilder()
{

}

fs::path PathBuilder::findRegistries()
{
    return "";
}

fs::path PathBuilder::getUTF8PathObserver(const char * sourcePath)
{
    fs::detail::utf8_codecvt_facet utf8;
    fs::path utf8ObservedPath(sourcePath, utf8);
    return utf8ObservedPath;
}

fs::path PathBuilder::getUTF8PathObserver(const std::string & sourcePath)
{
    return getUTF8PathObserver(sourcePath.c_str());
}

fs::path PathBuilder::replaceRootEnvVars(const std::string & sourcePath)
{
    // find any $ENVVAR and substitut
    fs::path completePath = getUTF8PathObserver(sourcePath);
    // howto manage i18n on env vars ? : env vars don't support accented characters
    std::string regexStr="^\\$([a-zA-Z0-9_]*)/";
    std::regex envVarRegex(regexStr, std::regex_constants::extended);
    std::smatch sm;
    if (std::regex_search(sourcePath, sm, envVarRegex)) {
        std::string matchStr = sm.str(1);
        char * envVar = getenv(matchStr.c_str());
        if (envVar != nullptr) {
            fs::path envStr (envVar);
            std::string replacedStr = std::regex_replace(sourcePath, envVarRegex, "");
            fs::path subdir (replacedStr);
            completePath = envStr / subdir;
        }
    }
    return completePath;
}


fs::path PathBuilder::buildModuleFolderPath(const std::string & filePathStr)
{
    fs::path filePath(replaceRootEnvVars(filePathStr));

#ifdef XPCFSUBDIRSEARCH
    fs::path modeSubDir = XPCFSUBDIRSEARCH;

    if ( fs::exists( filePath / modeSubDir ) ) {
        filePath /= modeSubDir;
    }
#endif
    return filePath;
}

fs::path PathBuilder::buildModuleFilePath(const std::string & moduleName, const std::string & filePathStr)
{
    fs::path filePath = buildModuleFolderPath(filePathStr);

    fs::path moduleFileName = getUTF8PathObserver(moduleName.c_str());
    filePath /= moduleFileName;
    return filePath;
}

fs::path PathBuilder::getHomePath()
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

fs::path PathBuilder::getXPCFHomePath()
{
    fs::path xpcfHomePath = getHomePath();
    xpcfHomePath /= ".xpcf";
    return xpcfHomePath;
}

static fs::path suffix() {
    // https://sourceforge.net/p/predef/wiki/OperatingSystems/
#if BOOST_OS_MACOS || BOOST_OS_IOS
    return ".dylib";
#elif BOOST_OS_WINDOWS
    return L".dll";
#else
    return ".so";
#endif
}

fs::path PathBuilder::appendModuleDecorations(const fs::path & sl)
{
    fs::path actual_path = sl;
    if ( actual_path.stem() == actual_path.filename() ) { // there is no suffix
        actual_path += suffix().native();
    }
#if BOOST_OS_WINDOWS
    if (!fs::exists(actual_path)) {
        // MinGW loves 'lib' prefix and puts it even on Windows platform
        actual_path = (actual_path.has_parent_path() ? actual_path.parent_path() / L"lib" : L"lib").native() + actual_path.filename().native();
    }
#else //posix
    actual_path = (std::strncmp(actual_path.filename().string().c_str(), "lib", 3)
                ? (actual_path.has_parent_path() ? actual_path.parent_path() / L"lib" : L"lib").native() + actual_path.filename().native()
                : actual_path);
#endif
    return actual_path;
}

fs::path PathBuilder::appendModuleDecorations(const char * sl)
{
    fs::path currentPath = getUTF8PathObserver(sl);
    return appendModuleDecorations(currentPath);
}

}}}
