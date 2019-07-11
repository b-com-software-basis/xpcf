#ifndef HELPERS_H
#define HELPERS_H

#include <map>

namespace org {namespace bcom { namespace xpcf {
template < typename Key, typename T> bool mapContains(const std::map<Key,T> & mapContainer, Key k)
{
    if (mapContainer.find(k) != mapContainer.end()) {
        return true;
    }
    return false;
}

}}}

#endif // HELPERS_H
