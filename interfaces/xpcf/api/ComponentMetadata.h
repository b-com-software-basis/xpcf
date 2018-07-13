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
 * @date 2017-04-28
 */

#ifndef ORG_BCOM_XPCF_COMPONENTMETADATA_H
#define ORG_BCOM_XPCF_COMPONENTMETADATA_H


#include "xpcf/xpcf_api_define.h"
#include "xpcf/core/uuid.h"
#include "xpcf/api/InterfaceMetadata.h"

#include <boost/filesystem.hpp>

#include <string>
#include <vector>
namespace fs = boost::filesystem;

namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API ComponentMetadata : public InterfaceMetadata {
public:
  ComponentMetadata() = delete;
  ComponentMetadata(const char* name, const uuids::uuid& componentID,
                    const uuids::uuid& moduleUUID, const char * configFilePath);
  ComponentMetadata(const char* name, const char *componentID, const char *moduleUUID, const char * configFilePath);
  virtual ~ComponentMetadata();

  void addInterface(const uuids::uuid& interfaceUUID);
  uuids::uuid getInterface(int i) const;
  int getNbInterfaces() const;

  uuids::uuid getModuleUUID() const;
  const char *getConfigPath() const { return m_configPath.c_str(); }
  inline fs::path getFullConfigPath() const { return m_configFullPath; }

private:
  void setPath(const char* path);
  void setModuleUUID(const char * moduleUUID);
  std::basic_string<char> m_configPath;
  fs::path m_configFullPath;
  std::vector<uuids::uuid> m_interfaceUUIDs;
  uuids::uuid m_moduleUUID;
};

}}} //namespace org::bcom::xpcf

#endif
