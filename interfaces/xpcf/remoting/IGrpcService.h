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

#ifndef IGRPCSERVICE_H
#define IGRPCSERVICE_H

#include <xpcf/api/IComponentIntrospect.h>
#include <grpc++/grpc++.h>

namespace org { namespace bcom { namespace xpcf {

class IGrpcService: virtual public org::bcom::xpcf::IComponentIntrospect
{
public:
    virtual ~IGrpcService() override = default;
    virtual ::grpc::Service * getService() = 0;
};

}}}

template <> struct org::bcom::xpcf::InterfaceTraits<org::bcom::xpcf::IGrpcService>
{
    static constexpr const char * UUID = "{baab5e42-2c24-48de-953b-07237af419e4}";
    static constexpr const char * NAME = "IGrpcService";
    static constexpr const char * DESCRIPTION = "IGrpcService interface description";
};
#endif // IGRPCSERVICE_H
