#ifndef REFS_H
#define REFS_H

#ifndef ORG_BCOM_XPCF_XPCF_MANAGED_CPP_ENABLED
#ifdef XPCF_USE_BOOST
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/move/unique_ptr.hpp>
#include <boost/move/make_unique.hpp>
#include <boost/function.hpp>
#include <boost/type_traits.hpp>
#include <boost/any.hpp>
namespace org {namespace bcom { namespace xpcf {
namespace utils = boost;
namespace unixpcf = boost::movelib;
}}}
#else
#include <memory>
#include <functional>
#include <utility>
#include <any>
namespace org {namespace bcom { namespace xpcf {
namespace utils = std;
namespace unixpcf = std;
}}}
#endif

#else
//#define SPtr(N) N *
#define SPtr(N) SPtr_##N
#define SRef(N) N *
#endif

template<class T>
using SRef = org::bcom::xpcf::utils::shared_ptr<T>;
template<class T>
using SPtr = org::bcom::xpcf::utils::shared_ptr<T>;

template<class T>
using UniqueRef = org::bcom::xpcf::unixpcf::unique_ptr<T>;

#endif // REFS_H
