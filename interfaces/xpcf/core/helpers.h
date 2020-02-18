#ifndef HELPERS_H
#define HELPERS_H

#include <map>

#ifndef SWIG
namespace org { namespace bcom { namespace xpcf {

template < typename Key, typename T> bool mapContains(const std::map<Key,T> & mapContainer, Key k)
{
    if (mapContainer.find(k) != mapContainer.end()) {
        return true;
    }
    return false;
}


class ScopeExit {
public:
    ScopeExit(const std::function<void(void)> & f):m_func(f) {}
    ~ScopeExit () {
        m_func();
    }
private:
    const std::function<void(void)> & m_func;
};

}}}
#endif // SWIG

#endif // HELPERS_H
