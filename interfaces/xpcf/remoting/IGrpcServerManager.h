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

#ifndef IGRPCSERVERMANAGER_H
#define IGRPCSERVERMANAGER_H


#include <xpcf/api/IComponentIntrospect.h>
#include <xpcf/remoting/IGrpcService.h>
#include <grpc++/grpc++.h>

namespace org { namespace bcom { namespace xpcf {

class IGrpcServerManager: virtual public org::bcom::xpcf::IComponentIntrospect
{
public:
    virtual ~IGrpcServerManager() override = default;
    virtual void registerService(grpc::Service * service) = 0;
    virtual void registerService(const grpc::string & host, grpc::Service * service) = 0;
    virtual void registerService(SRef<IGrpcService> service) = 0;
    virtual void registerService(const grpc::string & host, SRef<IGrpcService> service) = 0;
    virtual void runServer() = 0;
};

}}}

template <> struct org::bcom::xpcf::InterfaceTraits<org::bcom::xpcf::IGrpcServerManager>
{
    static constexpr const char * UUID = "{8f0952bb-c064-4bcc-9030-51236b9126d5}";
    static constexpr const char * NAME = "IGrpcServerManager";
    static constexpr const char * DESCRIPTION = "IGrpcServerManager interface description";
};

#endif // IGRPCSERVERMANAGER_H
