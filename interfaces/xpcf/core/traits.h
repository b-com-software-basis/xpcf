#ifndef TRAITS_H
#define TRAITS_H

#include "xpcf/core/uuid.h"
#include "xpcf/api/InterfaceTraits.h"
#include "xpcf/component/ComponentTraits.h"
#include <map>

namespace org {namespace bcom { namespace xpcf {

namespace uuids = boost::uuids;

template < typename T >
inline uuids::uuid toUUID()
{
    static_assert(is_interface<T>::value || is_component<T>::value,
                  "Type passed to toUUID is neither a component nor an interface "
                  "or [Interface/Component]Traits not defined for T !!");
    std::string uuidString;
    if (is_interface<T>::value) {
        uuidString = InterfaceTraits<T>::UUID;
    }
    if (is_component<T>::value) {
        uuidString = ComponentTraits<T>::UUID;
    }
    return toUUID(uuidString);
}

template <typename T> std::map<std::string,std::string> toMap() {
    static_assert(is_interface<T>::value || is_component<T>::value,
                  "Type passed to toMap is neither a component nor an interface "
                  "or [Interface/Component]Traits not defined for T !!");
    std::map<std::string,std::string> traitMap;
    if (is_interface<T>::value) {
        traitMap["UUID"] = InterfaceTraits<T>::UUID;
        traitMap["NAME"] = ComponentTraits<T>::NAME;
        traitMap["DESCRIPTION"] = InterfaceTraits<T>::DESCRIPTION;
    }
    if (is_component<T>::value) {
        traitMap["UUID"] = ComponentTraits<T>::UUID;
        traitMap["NAME"] = ComponentTraits<T>::NAME;
        traitMap["DESCRIPTION"] = ComponentTraits<T>::DESCRIPTION;
    }
    return traitMap;
}

}}}


#define XPCF_DEFINE_INTERFACE_TRAITS(fullComponentType,uuidString,nameString,descriptionString) \
template <> struct org::bcom::xpcf::InterfaceTraits<fullComponentType> \
{ \
    static constexpr const char * UUID = uuidString; \
    static constexpr const char * NAME = nameString; \
    static constexpr const char * DESCRIPTION = descriptionString; \
};

#define XPCF_DEFINE_COMPONENT_TRAITS(fullComponentType,uuidString,nameString,descriptionString) \
template <> struct org::bcom::xpcf::ComponentTraits<fullComponentType> \
{ \
    static constexpr const char * UUID = uuidString; \
    static constexpr const char * NAME = nameString; \
    static constexpr const char * DESCRIPTION = descriptionString; \
};


#endif // TRAITS_H
