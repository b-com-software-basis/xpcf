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
#include "xpcf/core/Exception.h"

//#pragma warning(disable:4786)

namespace org { namespace bcom { namespace xpcf {

Property::Property ()
    : m_type(IProperty_NONE), m_attribute(IProperty_INOUT), m_name("none")
{
}

Property::Property (const char* name, uint32_t numberOfItem, IProperty::AccessSpecifier iotype)
    : Property()

{
    setProperties(name, numberOfItem, iotype);
}


Property::~Property ()
{
    clear();
}

const char* Property::getName () const
{
    return m_name.c_str();
}

IProperty::PropertyType Property::getType ()
{
    return m_type;
}

uint32_t Property::size ()
{
    return m_queue.size();
}

template <class T>
void Property::setValue(uint32_t itemIndex, T value)
{
    if (m_attribute & IProperty::AccessSpecifier::IProperty_OUT) {
        throw AccessDeniedException();
    }
    m_queue.at(itemIndex) = value;
}

template <class T>
T Property::getValue(uint32_t itemIndex, IProperty::PropertyType valueType) const
{
    if (m_type != valueType || itemIndex > m_queue.size() || m_attribute & IProperty::AccessSpecifier::IProperty_IN) {
        return static_cast<T>(0);
    }
    return utils::any_cast<T>(m_queue.at(itemIndex));
}

template <class U, class T>
const T * Property::getValue(uint32_t itemIndex, IProperty::PropertyType valueType) const
{
    if (m_type != valueType || m_attribute & IProperty::AccessSpecifier::IProperty_IN) {
        return nullptr;
    }
    return utils::any_cast<U>(m_queue.at(itemIndex)).c_str();
}

template <class T, class U>
void Property::setValue(uint32_t itemIndex, const T * value)
{
    if (value == nullptr)
    {
        throw std::invalid_argument("Argument 'value' is null");
    }

    if (m_attribute & IProperty::AccessSpecifier::IProperty_OUT) {
        throw AccessDeniedException();
    }

    checkType<T>();
    U newValue = value;
    m_queue.at(itemIndex) = newValue;
}

void Property::setIntegerValue (int32_t val, uint32_t itemIndex)
{
    setValue<int32_t>(itemIndex, val);
}

void Property::setUnsignedIntegerValue (uint32_t val, uint32_t itemIndex)
{
    setValue<uint32_t>(itemIndex, val);
}

void Property::setLongValue (int64_t val, uint32_t itemIndex)
{
    setValue<int64_t>(itemIndex, val);
}

void Property::setUnsignedLongValue (uint64_t val, uint32_t itemIndex)
{
    setValue<uint64_t>(itemIndex, val);
}

void Property::setFloatingValue (float val, uint32_t itemIndex)
{
    setValue<float>(itemIndex, val);
}

void Property::setDoubleValue (double val, uint32_t itemIndex)
{
    setValue<double>(itemIndex, val);
}


void Property::setStringValue (const char * val, uint32_t itemIndex)
{
    setValue<char, std::string>(itemIndex, val);
}

void Property::setUnicodeStringValue (const wchar_t * val, uint32_t itemIndex)
{
    setValue<wchar_t, std::wstring>(itemIndex, val);
}

void Property::setStructureValue (SRef<IPropertyMap> val, uint32_t itemIndex)
{
    if (!val)
    {
        throw std::invalid_argument("Argument 'val' is null");
    }
    setValue<SRef<IPropertyMap>>(itemIndex, val);
}

int32_t Property::getIntegerValue (uint32_t itemIndex) const
{
    return getValue<int32_t>(itemIndex, IProperty_INTEGER);
}

uint32_t Property::getUnsignedIntegerValue (uint32_t itemIndex) const
{
    return getValue<uint32_t>(itemIndex, IProperty_UINTEGER);
}

int64_t Property::getLongValue (uint32_t itemIndex) const
{
    return getValue<int64_t>(itemIndex, IProperty_INTEGER);
}

uint64_t Property::getUnsignedLongValue (uint32_t itemIndex) const
{
    return getValue<uint64_t>(itemIndex, IProperty_UINTEGER);
}


const char* Property::getStringValue (uint32_t itemIndex) const
{
    return getValue<std::string, char>(itemIndex, IProperty_CHARSTR);
}

const wchar_t* Property::getUnicodeStringValue (uint32_t itemIndex) const
{
    return getValue<std::wstring, wchar_t>(itemIndex, IProperty_UNICODESTR);
}

float Property::getFloatingValue (uint32_t itemIndex) const
{
    return getValue<float>(itemIndex, IProperty_FLOAT);
}

double Property::getDoubleValue (uint32_t itemIndex) const
{
    return getValue<double>(itemIndex, IProperty_FLOAT);
}

SRef<IPropertyMap> Property::getStructureValue (uint32_t itemIndex) const
{
    if (m_type != IProperty_STRUCTURE) {
        return nullptr;
    }
    return utils::any_cast<SRef<IPropertyMap> >(m_queue.at(itemIndex));
}

int Property::init ()
{

    int res=0;
    return res;
}

void Property::clear ()
{
    if (m_type == IProperty_NONE) {
        return ;
    }

    m_queue.clear();
    m_type = IProperty_NONE;
    return;
}

XPCFErrorCode Property::setProperties (const char* name, uint32_t numberOfValues, IProperty::AccessSpecifier iotype)
{
    if (name==nullptr)
    {
        return XPCFErrorCode::_FAIL;
    }

    if (strlen(name) == 0)
    {
        return XPCFErrorCode::_FAIL;
    }

    clear();
    m_name = name;
    m_queue.resize(numberOfValues);
    m_attribute    = iotype;

    return XPCFErrorCode::_SUCCESS;
}

XPCFErrorCode Property::copyTo (SRef<IProperty> dest)
{
    if (!dest)
    {
        throw std::invalid_argument("Argument 'dest' is null");
    }

    XPCFErrorCode res=XPCFErrorCode::_FAIL;
    dest->setProperties (m_name.c_str(), m_queue.size(), m_attribute);
    switch (m_type)
    {
    case IProperty_INTEGER:
    {
        uint32_t i = 0;
        for (i=0; i<m_queue.size();i++)
        {

            dest->setIntegerValue(getIntegerValue(i), i);
        }
    }
        break;
    case IProperty_UINTEGER:
    {
        uint32_t i = 0;
        for (i=0; i<m_queue.size();i++)
        {

            dest->setUnsignedIntegerValue(getUnsignedIntegerValue(i), i);
        }
    }
        break;
    case IProperty_FLOAT:
    {
        uint32_t i = 0;
        for (i=0; i<m_queue.size();i++)
        {

            dest->setFloatingValue(getFloatingValue(i), i);
        }

    }
        break;
    case IProperty_CHARSTR:
    {
        uint32_t i = 0;
        for (i=0; i<m_queue.size();i++)
        {

            dest->setStringValue(getStringValue(i), i);
        }

    }
        break;
    case IProperty_UNICODESTR:
    {
        uint32_t i = 0;
        for (i=0; i<m_queue.size();i++)
        {

            dest->setUnicodeStringValue(getUnicodeStringValue(i), i);
        }

    }
        break;
    case IProperty_STRUCTURE:
    {
        uint32_t i = 0;
        for (i=0; i<m_queue.size();i++)
        {

            dest->setStructureValue(getStructureValue(i), i);
        }
    }
        break;
    default:{return res;}
        break;
    }


    return XPCFErrorCode::_SUCCESS;
}

template <class T>
void Property::checkType ()
{
    IProperty::PropertyType newtype = m_type;

    if (utils::is_same<T, uint64_t>::value) {
        newtype = IProperty_UINTEGER;
    }

    if (utils::is_same<T, int64_t>::value) {
        newtype = IProperty_INTEGER;
    }

    if (utils::is_same<T, char>::value) {
        newtype = IProperty_CHARSTR;
    }

    if (utils::is_same<T, float>::value) {
        newtype = IProperty_FLOAT;
    }

    if (utils::is_same<T, wchar_t>::value) {
        newtype = IProperty_UNICODESTR;
    }

    if (utils::is_same<T, SRef<IPropertyMap>>::value) {
        newtype = IProperty_STRUCTURE;
    }

    checkType(newtype);
}

void Property::checkType (IProperty::PropertyType newtype)
{
    if (m_type != newtype )
    {
        clear();
        m_type = newtype;
        init();
    }
}

void Property::setName (const char *name)
{
    if (name==nullptr)
    {
        throw std::invalid_argument("Argument 'name' is null");
    }
    // TODO : check non empty name is set !!
    m_name = name;
}

IProperty::AccessSpecifier Property::getAccessSpecifier () const
{
    return m_attribute;
}

void Property::setAccessSpecifier (IProperty::AccessSpecifier iotype)
{
    m_attribute = iotype;
}

SRef<IProperty> Property::CreateInstance ()
{
    return utils::make_shared<Property>();
}

SRef<IProperty> Property::CreateInstance (const char* name, uint32_t numberOfValues, IProperty::AccessSpecifier iotype)
{
    SRef<IProperty> rip = utils::make_shared<Property>();
    rip->setProperties(name, numberOfValues, iotype);
    return rip;
}

Property *  Property::CreateConcreteInstance ()
{
    Property *rip=new Property();
    return rip;
}

Property * Property::CreateConcreteInstance (const char* name, uint32_t numberOfValues, IProperty::AccessSpecifier iotype)
{
    Property *rip=new Property();
    rip->setProperties(name, numberOfValues, iotype);
    return rip;
}

}}}

