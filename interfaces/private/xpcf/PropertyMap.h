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
 * @date 2017-10-27
 */

#ifndef ORG_BCOM_XPCF_PROPERTYMAP_H
#define ORG_BCOM_XPCF_PROPERTYMAP_H


#include <map>
#include "xpcf/properties/IPropertyMap.h"
#include "PropertyWrapper.h"
#include "PropertySequenceWrapper.h"
#include <xpcf/collection/Collection.h>

namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API PropertyMap : public IPropertyMap {

  public:

      PropertyMap ();

      ~PropertyMap () override;

      // IPropertyMap
      XPCFErrorCode  addProperty (SRef<IProperty> p) override;

      XPCFErrorCode  setProperty (const SRef<IProperty> p) override;

      IEnumerable<SRef<IProperty>> & getProperties() override;

      const SRef<IProperty> at (const char *name) override;

      XPCFErrorCode  wrapInteger(const char * name, int32_t & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapUnsignedInteger(const char * name, uint32_t & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapLong(const char * name, int64_t & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapUnsignedLong(const char * name, uint64_t & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapFloat(const char * name, float & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapDouble(const char * name, double & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapString(const char * name, std::string & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapUnicodeString(const char * name, std::wstring  & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapStructure(const char * name, SRef<IPropertyMap> & wrappedValue, IProperty::AccessSpecifier iotype) override;

      XPCFErrorCode  wrapIntegerVector(const char * name, std::vector<int32_t> & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapUnsignedIntegerVector(const char * name, std::vector<uint32_t> & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapLongVector(const char * name, std::vector<int64_t> & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapUnsignedLongVector(const char * name, std::vector<uint64_t> & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapFloatVector(const char * name, std::vector<float> & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapDoubleVector(const char * name, std::vector<double> & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapStringVector(const char * name, std::vector<std::string> & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapUnicodeStringVector(const char * name, std::vector<std::wstring>  & wrappedValue, IProperty::AccessSpecifier iotype) override;
      XPCFErrorCode  wrapStructureVector(const char * name, std::vector<SRef<IPropertyMap>> & wrappedValue, IProperty::AccessSpecifier iotype) override;

      static SRef<IPropertyMap> CreateInstance ();

      static PropertyMap *  CreateConcreteInstance ();


  private:
      template<class ExternType, class HoldingType>
      inline XPCFErrorCode wrap(const char * name, HoldingType & wrappedValue, IProperty::AccessSpecifier iotype);

      template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
      inline XPCFErrorCode wrap(const char * name, Sequence<HoldingType, ::std::allocator<HoldingType>> & wrappedValue, IProperty::AccessSpecifier iotype);

      std::map<std::string, SRef<IProperty>> m_propertyMap;
      Collection<SRef<IProperty>,std::vector> m_collectionPropertyMap;
};

template< class ExternType, class HoldingType >
XPCFErrorCode PropertyMap::wrap(const char * name, HoldingType & wrappedValue, IProperty::AccessSpecifier iotype)
{
    if (name == nullptr) {
        return XPCFErrorCode::_ERROR_INVALID_ARGUMENT;
    }

    SRef<xpcf::IProperty> wrappedRef = utils::make_shared<xpcf::PropertyWrapper<ExternType>>(name, wrappedValue, iotype);
    return addProperty(wrappedRef);
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType >
XPCFErrorCode PropertyMap::wrap(const char * name, Sequence<HoldingType, ::std::allocator<HoldingType>> & wrappedValue, IProperty::AccessSpecifier iotype)
{
    if (name == nullptr) {
        return XPCFErrorCode::_ERROR_INVALID_ARGUMENT;
    }

    SRef<xpcf::IProperty> wrappedRef = utils::make_shared<xpcf::PropertySequenceWrapper<Sequence, ExternType> >(name, wrappedValue, iotype);
    return addProperty(wrappedRef);
}

}}}

#endif
