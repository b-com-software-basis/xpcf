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

#include "private/xpcf/Property.h"
#include "private/xpcf/PropertyMap.h"
#include "xpcf/core/Exception.h"


namespace org { namespace bcom { namespace xpcf {

SRef<IPropertyMap> getPropertyMapInstance() {
    return utils::make_shared<PropertyMap>();
}

PropertyMap::PropertyMap ()
{
}


PropertyMap::~PropertyMap ()
{
}

XPCFErrorCode PropertyMap::addProperty (SRef<IProperty> p)
{
    if (m_propertyMap.find(p->getName()) != m_propertyMap.end()) {
        return XPCFErrorCode::_FAIL;
    }
    /*
    SRef<IProperty> lp= utils::make_shared<Property>();
    p->copyTo(lp);
    m_propertyMap[lp->getName()] = lp;*/
    m_propertyMap[p->getName()] = p;
    m_collectionPropertyMap.add(p);
    return XPCFErrorCode::_SUCCESS;
}

/* WHY MAKE A COPY ??
SRef<IProperty> PropertyMap::cloneProperty (const char *name)
{

    if (!p)
    {
        //throw XPCF_NullObject();
    }

    if (m_propertyMap.find(name) == m_propertyMap.end()) {
        return XPCFErrorCode::_ERROR_UNKWOWN;
    }

    m_propertyMap.at(name)->copyTo(p);

    return XPCFErrorCode::_SUCCESS;
}*/

// <AccessSpec,ICollection reference or Collection ref>
// removed when invalidated
// collection should maintain enumerators to invalidate them ?
// collection invalidation ??
// collection invalidation means collection clearance i.e. size = 0
// enumerator invalidation means collection size != initial sequence size
// getprop must :
// - create the collection when it doesn't exist first
// - maintain the collection when it is not modified
// - remove the collection and invalidate the enumerators when a modification occured in properties


IEnumerable<SRef<IProperty>> & PropertyMap::getProperties()
{
    return m_collectionPropertyMap;
}

const SRef<IProperty> PropertyMap::at (const char *name)
{

    if (m_propertyMap.find(name) == m_propertyMap.end()) {
        return nullptr;
    }

    return m_propertyMap.at(name);
}

XPCFErrorCode PropertyMap::setProperty (const SRef<IProperty> p)
{

    if (!p) {
         throw std::invalid_argument("Argument 'p' doesn't reference an IProperty. This SRef is empty.");
    }

    std::string name(p->getName());
    if (m_propertyMap.find(name) == m_propertyMap.end()) {
        throw std::invalid_argument("No property named " + name + " exists in this IPropertyMap.");
    }

    SRef<IProperty> tmp = m_propertyMap.at(name);
    if ( tmp->getAccessSpecifier() == IProperty::IProperty_OUT) {
        throw AccessDeniedException();
    }

    if ( tmp->size() != p->size()) {
        throw std::invalid_argument("Argument property size differs from inner property size.");
    }

    if ( tmp->getType() != p->getType()) {
        throw std::invalid_argument("Argument property type differs from inner property type.");
    }

    p->copyTo(tmp);

    return XPCFErrorCode::_SUCCESS;
}

XPCFErrorCode PropertyMap::wrapInteger(const char * name, int32_t & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::remove_reference<decltype(wrappedValue)>::type>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapUnsignedInteger(const char * name, uint32_t & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::remove_reference<decltype(wrappedValue)>::type>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapLong(const char * name, int64_t & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::remove_reference<decltype(wrappedValue)>::type>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapUnsignedLong(const char * name, uint64_t & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::remove_reference<decltype(wrappedValue)>::type>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapFloat(const char * name, float & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::remove_reference<decltype(wrappedValue)>::type>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapDouble(const char * name, double & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::remove_reference<decltype(wrappedValue)>::type>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapString(const char * name, std::string & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<const char *>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapUnicodeString(const char * name, std::wstring  & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<const wchar_t *>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapStructure(const char * name, SRef<IPropertyMap> & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::remove_reference<decltype(wrappedValue)>::type>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapIntegerVector(const char * name, std::vector<int32_t> & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::vector, std::remove_reference<decltype(wrappedValue)>::type::value_type>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapUnsignedIntegerVector(const char * name, std::vector<uint32_t> & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::vector, std::remove_reference<decltype(wrappedValue)>::type::value_type>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapLongVector(const char * name, std::vector<int64_t> & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::vector, std::remove_reference<decltype(wrappedValue)>::type::value_type>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapUnsignedLongVector(const char * name, std::vector<uint64_t> & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::vector, std::remove_reference<decltype(wrappedValue)>::type::value_type>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapFloatVector(const char * name, std::vector<float> & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::vector, std::remove_reference<decltype(wrappedValue)>::type::value_type>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapDoubleVector(const char * name, std::vector<double> & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::vector, std::remove_reference<decltype(wrappedValue)>::type::value_type>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapStringVector(const char * name, std::vector<std::string> & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::vector, const char *>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapUnicodeStringVector(const char * name, std::vector<std::wstring>  & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::vector, const wchar_t *>(name, wrappedValue, iotype);
}

XPCFErrorCode PropertyMap::wrapStructureVector(const char * name, std::vector<SRef<IPropertyMap>> & wrappedValue, IProperty::AccessSpecifier iotype)
{
    return wrap<std::vector, std::remove_reference<decltype(wrappedValue)>::type::value_type>(name, wrappedValue, iotype);
}

SRef<IPropertyMap> PropertyMap::CreateInstance ()
{
    SRef<IPropertyMap> rips = utils::make_shared<PropertyMap>();
    return rips;
}

PropertyMap *  PropertyMap::CreateConcreteInstance ()
{
    PropertyMap* rips=new PropertyMap();
    return rips;
}

}}}
