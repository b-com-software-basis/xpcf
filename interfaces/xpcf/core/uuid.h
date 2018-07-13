#ifndef UUID_H
#define UUID_H

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>

namespace org {namespace bcom { namespace xpcf {

namespace uuids = boost::uuids;

inline uuids::uuid toUUID(const char * uuidString)
{
    uuids::string_generator gen;
    return gen(uuidString);
}

inline uuids::uuid toUUID(const std::string & uuidString)
{
    return toUUID(uuidString.c_str());
}

}}}

#endif
