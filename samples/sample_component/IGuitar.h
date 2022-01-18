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
 * @date 2017-03-07
 */

#ifndef IGUITAR_H
#define IGUITAR_H
#include "xpcf/api/IComponentIntrospect.h"
/**
 *  @ingroup interfaces
 */
/**
   * @class IGuitar
   * This interface declares a set of methods describing a musical instrument.
   * It inherits from org::bcom::xpcf::IComponentIntrospect to provide introspection capabilities.
   *
   * @note The InterfaceTraits<IGuitar> template struct defines the IGuitar UUID and description.
   * It allows IGuitar users to use directly the I0 type instead of its UUID to bind the component to this interface.
   */

/**
 *  @ingroup interfaces
 */

class [[xpcf::clientUUID("fe931815-479c-4659-96c3-09074747796d")]] [[xpcf::serverUUID("2ee889a4-bb14-46a0-b6e9-a391f61e4ad0")]] ITuner :
    virtual public org::bcom::xpcf::IComponentIntrospect
{
public:
    enum TuneType {
        Standard = 0,
        Nashville = 1,
        HalfTuneLower = 2,
        OneTuneLower = 3,
        TuneDADGAD = 4,
        dropD = 5
    };

    virtual ~ITuner() = default;
    virtual TuneType getTuneType () = 0;
    virtual void setTuneType(const TuneType & tuneType) = 0;
};


class [[xpcf::clientUUID("734ee845-fa91-49b3-8a9b-37338f8a40cf")]] [[xpcf::serverUUID("3e6ff511-ced5-440e-b101-880862726a30")]] [[grpc::noCompression]] IGuitar :
    virtual public org::bcom::xpcf::IComponentIntrospect
{
public:
    enum GuitarType {
        Acoustic = 0,
        Folk = 1,
        Electric = 2
    };
    virtual ~IGuitar() = default;
    virtual const char * getGuitarBrand () = 0;
    virtual uint32_t getNbStrings () = 0;
    virtual GuitarType getGuitarType () = 0;
    virtual ITuner::TuneType getTuneType () = 0;
};

class [[xpcf::clientUUID("b433029e-a2cb-4ef1-8c06-e4aff4b24bd3")]] [[xpcf::serverUUID("b2ff5795-69ba-42e2-b3bf-417ad6fd023f")]] IElectricGuitar : virtual public org::bcom::xpcf::IComponentIntrospect
{
public:
    virtual ~IElectricGuitar() = default;
    virtual void setDistortionLevel (uint32_t level) = 0;
    virtual uint32_t getDistortionLevel () = 0;
};

template <> struct org::bcom::xpcf::InterfaceTraits<IGuitar>
{
    static constexpr const char * UUID = "6279BB1D-69B5-42BD-9DA1-743C8922BCB9";
    static constexpr const char * NAME = "IGuitar";
    static constexpr const char * DESCRIPTION = "provides instrument specific methods";
};

template <> struct org::bcom::xpcf::InterfaceTraits<IElectricGuitar>
{
    static constexpr const char * UUID = "93E50702-41BA-4497-9BBE-C918C1A87814";
    static constexpr const char * NAME = "IElectricGuitar";
    static constexpr const char * DESCRIPTION = "provides electric guitar specific methods";
};

template <> struct org::bcom::xpcf::InterfaceTraits<ITuner>
{
    static constexpr const char * UUID = "1FC313B3-FAE3-4BCA-B4BE-222EFD24E607";
    static constexpr const char * NAME = "ITuner";
    static constexpr const char * DESCRIPTION = "defines guitar tune modes";
};

#endif // IGUITAR_H
