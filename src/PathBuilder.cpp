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

#include "PathBuilder.h"
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>

#ifdef WIN32
#include <stdlib.h>
#else
#include <pwd.h>
#include <sys/types.h>
#endif

#include <regex>
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

fs::path PathBuilder::getUTF8PathObserver(std::string sourcePath)
{
    return getUTF8PathObserver(sourcePath.c_str());
}

fs::path PathBuilder::replaceRootEnvVars(std::string sourcePath)
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

fs::path PathBuilder::buildModuleFilePath(std::string moduleName, std::string filePathStr)
{
    fs::detail::utf8_codecvt_facet utf8;
    fs::path filePath(replaceRootEnvVars(filePathStr));

#ifdef XPCFSUBDIRSEARCH
    fs::path modeSubDir = XPCFSUBDIRSEARCH;

    if ( boost::filesystem::exists( filePath / modeSubDir ) ) {
        filePath /= modeSubDir;
    }
#endif

    fs::path moduleFileName(moduleName, utf8);
    filePath /= moduleFileName;
    return filePath;
}

fs::path PathBuilder::getHomePath()
{
    fs::path homePath;
#ifdef WIN32
    homePath = getenv("USERPROFILE");
    if (homePath == nullptr) {
        homePath = getenv("HOMEDRIVE");
        homePath /= getenv("HOMEPATH");
    }
#else
    struct passwd* pwd = getpwuid(getuid());
    if (pwd) {
        homePath = pwd->pw_dir;
    }
    else {
        // try the $HOME environment variable
        homePath = getenv("HOME");
    }
    fs::detail::utf8_codecvt_facet utf8;
#endif
    return homePath;
}

const char * PathBuilder::getXPCFHomePath()
{
    fs::path xpcfHomePath = getHomePath();
    xpcfHomePath /= ".xpcf";
    fs::detail::utf8_codecvt_facet utf8;
    return xpcfHomePath.generic_string(utf8).c_str();
}

}}}
