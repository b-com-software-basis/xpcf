/**
 * @copyright Copyright (c) 2019 B-com http://www.b-com.com/
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
 * @date 2019-06-17
 */

#ifndef ORG_BCOM_XPCF_IINJECTABLEMETADATA_H
#define ORG_BCOM_XPCF_IINJECTABLEMETADATA_H

#include "IComponentIntrospect.h"
#include <xpcf/core/uuid.h>
#include <string>

namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API InjectableMetadata {
public:
    InjectableMetadata(const uuids::uuid & serviceUUID, bool optional = false);
    InjectableMetadata(const uuids::uuid & serviceUUID, const char * name, bool optional = false);
    virtual ~InjectableMetadata();
    bool optional() const;
    bool isNamed();
    const char * getName() const;
    const uuids::uuid & getUuid() const ;

private:
    class InjectableMetadataImpl;
    UniqueRef<InjectableMetadataImpl> m_pimpl;
};

class XPCF_EXPORT_API Injector : public InjectableMetadata {
public:
    Injector( const std::function<void(SRef<IComponentIntrospect>)> & injector, uuids::uuid serviceUUID, bool optional = false);
    Injector( const std::function<void(SRef<IComponentIntrospect>)> & injector, uuids::uuid serviceUUID, const char * name, bool optional = false);
    virtual ~Injector() override;
    void inject(SRef<IComponentIntrospect> instance);

private:
    class InjectorImpl;
    UniqueRef<InjectorImpl> m_pimpl;
};

}}}

#endif // ORG_BCOM_XPCF_IINJECTABLEMETADATA_H
