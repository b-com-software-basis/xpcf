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

#ifndef ORG_BCOM_XPCF_PATHBUILDER_H
#define ORG_BCOM_XPCF_PATHBUILDER_H

#include <boost/filesystem.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>

namespace fs = boost::filesystem;

namespace org { namespace bcom { namespace xpcf {

class PathBuilder
{
public:
    PathBuilder();
    fs::path findRegistries();
    static fs::path getUTF8PathObserver(const std::string & sourcePath);
    static fs::path getUTF8PathObserver(const char * sourcePath);
    static fs::path replaceRootEnvVars(const std::string & sourcePath);
    static fs::path buildModuleFilePath(const std::string & moduleName,const std::string & filePath);
    static fs::path buildModuleFolderPath(const std::string & filePath);
    static fs::path getHomePath();
    static fs::path getXPCFHomePath();
    static fs::path appendModuleDecorations(const fs::path & sl);
    static fs::path appendModuleDecorations(const char * sl);
    static inline bool is_shared_library(const std::string& s) {
        fs::detail::utf8_codecvt_facet utf8;
        fs::path libPath(s,utf8);
        auto linkStatus = fs::symlink_status(libPath);
        if (linkStatus.type() == fs::symlink_file) {
            return false;
        }
        return (s.find(".dll") != std::string::npos || s.find(".so") != std::string::npos || s.find(".dylib") != std::string::npos);
        // side effects on path typically toto.app/contents/...
              /*  && s.find(".lib") == std::string::npos
                && s.find(".exp") == std::string::npos
                && s.find(".pdb") == std::string::npos
                && s.find(".manifest") == std::string::npos
                && s.find(".rsp") == std::string::npos
                && s.find(".obj") == std::string::npos
                && s.find(".a") == std::string::npos;*/
    }

    static inline bool is_shared_library(const char* p) {
        return is_shared_library(std::string(p));
    }

    static inline bool is_shared_library(const fs::path& p) {
        return is_shared_library(p.string());
    }

};

}}}

#endif // PATHBUILDER_H
