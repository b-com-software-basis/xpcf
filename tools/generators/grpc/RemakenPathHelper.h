/**
 * @copyright Copyright (c) 2020 B-com http://www.b-com.com/
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
 * @date 2020-09-25
 */

#ifndef REMAKENPATHHELPER_H
#define REMAKENPATHHELPER_H

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class RemakenPathHelper
{
public:
    RemakenPathHelper();
    static fs::path computeRemakenRootPackageDir();
    static fs::path getUTF8PathObserver(const std::string & sourcePath);
    static fs::path getUTF8PathObserver(const char * sourcePath);
    static fs::path getHomePath();
    static fs::path getRemakenRoot();
    static const std::string_view & sharedLibraryPathEnvName();
};

#endif // REMAKENPATHHELPER_H
