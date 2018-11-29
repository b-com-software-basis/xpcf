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

#ifndef ORG_BCOM_XPCF_ConfigurableBase_H
#define ORG_BCOM_XPCF_ConfigurableBase_H

#include "xpcf/api/IConfigurable.h"
#include "xpcf/component/ComponentBase.h"

namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API ConfigurableBase : public ComponentBase,
        public virtual IConfigurable
{
public:
    ConfigurableBase() = delete;
    ConfigurableBase(const uuids::uuid & uuid, const SRef<IPropertyMap> configuration = nullptr);
    ConfigurableBase(std::map<std::string,std::string> componentTrait, const SRef<IPropertyMap> configuration = nullptr);
    virtual ~ConfigurableBase() override;

    // It is the component that decides if it must or must not unload when all
    // refs are out
    virtual void unloadComponent() override = 0;

    // IConfigurable
    XPCFErrorCode configure(const char * filepath) final;
    XPCFErrorCode configure(const char * filepath, const char * xpath) final;
    inline virtual XPCFErrorCode onConfigured() override { return XPCFErrorCode::_SUCCESS; }
    XPCFErrorCode serialize(const char * filepath, uint32_t mode) final;
    SRef<IPropertyMap> getPropertyRootNode() const final;
    SRef<IProperty> getProperty(const char * name) const final;
    IEnumerable<SRef<IProperty>> & getProperties() const final;
    SRef<IEnumerator<SRef<IProperty>>> getPropertiesEnumerator() const final;
    bool hasProperties() const final;

private:
    class InternalImpl;
    UniqueRef<InternalImpl> m_internalImpl;
};

}}} //namespace org::bcom::xpcf

#endif
