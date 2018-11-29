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
 * @date 2017-08-18
 */

#ifndef ORG_BCOM_XPCF_PROPERTYMANAGER_H
#define ORG_BCOM_XPCF_PROPERTYMANAGER_H

//#define BOOST_ALL_DYN_LINK 1
#include "xpcf/component/ComponentBase.h"
#include "xpcf/core/XPCFErrorCode.h"
#include "xpcf/api/IConfigurable.h"

#include <atomic>
#include <mutex>
#include <thread>

namespace org { namespace bcom { namespace xpcf {

class IPropertyManager : public virtual IComponentIntrospect {
  public:
    virtual ~IPropertyManager() = default;
    virtual XPCFErrorCode configure(const uuids::uuid & componentUUID, SRef<IConfigurable> componentRef,const char * filepath) = 0;
    virtual XPCFErrorCode configure(const char * instanceName, const uuids::uuid & componentUUID, SRef<IConfigurable> componentRef,const char * filepath) = 0;
    virtual XPCFErrorCode serialize(const uuids::uuid & componentUUID, SRef<IConfigurable> componentRef, const char * filepath, uint32_t mode) = 0;
};


template <> struct InterfaceTraits<IPropertyManager>
{
    static constexpr const char * UUID = "C939A7FB-B0C2-47BA-9458-48F4AB228D92";
    static constexpr const char * NAME = "XPCF::IPropertyManager";
    static constexpr const char * DESCRIPTION = "Configuration and serialization functionality for component properties";
};

class XPCF_EXPORT_API PropertyManager : public ComponentBase,
        public virtual IPropertyManager {
public:
    static PropertyManager* instance();
    void unloadComponent () override final;
    XPCFErrorCode configure(const uuids::uuid & componentUUID, SRef<IConfigurable> componentRef,const char * filepath) final;
    XPCFErrorCode configure(const char * instanceName, const uuids::uuid & componentUUID, SRef<IConfigurable> componentRef,const char * filepath) final;
    XPCFErrorCode serialize(const uuids::uuid & componentUUID, SRef<IConfigurable> componentRef, const char * filepath, uint32_t mode) final;

private:
    PropertyManager();
    ~PropertyManager() override = default;
    PropertyManager(const PropertyManager&)= delete;
    PropertyManager& operator=(const PropertyManager&)= delete;
    static std::atomic<PropertyManager*> m_instance;
    static std::mutex m_mutex;

};

template <> struct ComponentTraits<PropertyManager>
{
    static constexpr const char * UUID = "FF18E038-8D17-41FF-AFEB-5927DF713DD2";
    static constexpr const char * NAME = "XPCF::PropertyManager";
    static constexpr const char * DESCRIPTION = "Property manager component";
};

/**
 * Retrieve the property manager instance.
 * @return the smart reference on the PropertyManager interface
 */
XPCF_EXPORT_API SRef<IPropertyManager> getPropertyManagerInstance();

}}} //namespace org::bcom::xpcf

#endif
