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

namespace fs = boost::filesystem;
namespace org { namespace bcom { namespace xpcf {

class PathBuilder
{
public:
    PathBuilder();
    fs::path findRegistries();
    static fs::path getUTF8PathObserver(std::string sourcePath);
    static fs::path getUTF8PathObserver(const char * sourcePath);
    static fs::path replaceRootEnvVars(std::string sourcePath);
    static fs::path buildModuleFilePath(std::string moduleName, std::string filePath);
    static fs::path getHomePath();
    static fs::path getXPCFHomePath();
};

}}}

#endif // PATHBUILDER_H
