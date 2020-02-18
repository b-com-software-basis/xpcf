#ifndef UUID_H
#define UUID_H

#ifdef XPCF_USE_BOOST
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#else
#include <uuid.h>
#endif
#include <string>

namespace org {namespace bcom { namespace xpcf {

#ifdef XPCF_USE_BOOST
namespace uuids = boost::uuids;
#else
namespace uuids = ::uuids;
#endif

inline uuids::uuid toUUID(const char * uuidString)
{
#ifdef XPCF_USE_BOOST
    uuids::string_generator gen;
    return gen(uuidString);
#else
    return uuids::uuid::from_string(uuidString).value();
#endif
}

inline uuids::uuid toUUID(const std::string & uuidString)
{
    return toUUID(uuidString.c_str());
}

}}}

#endif
