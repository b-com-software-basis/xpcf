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

#ifndef ORG_BCOM_XPCF_PROPERTYSEQUENCEWRAPPER_H
#define ORG_BCOM_XPCF_PROPERTYSEQUENCEWRAPPER_H


#include "xpcf/properties/IProperty.h"
#include "PropertyWrapper.h"
#include <deque>
#include <vector>

namespace org { namespace bcom { namespace xpcf {
// TODO : implement nocopy accessors for PropertyMap or a propertymapwrapper

template <template<typename, typename> class Sequence, class ExternType, class HoldingType = typename deduce_from<ExternType>::InnerType>
class PropertySequenceWrapper : public IProperty {//<T> T in uint64_t double int64_t char* wchar*
public:
    static_assert(std::is_same<ExternType, float>::value || std::is_same<ExternType, double>::value || std::is_same<ExternType, int64_t>::value
                  || std::is_same<ExternType, uint64_t>::value || std::is_same<ExternType, int32_t>::value || std::is_same<ExternType, uint32_t>::value
                  || std::is_same<ExternType, const char *>::value || std::is_same<ExternType, const wchar_t *>::value || std::is_same<ExternType, SRef<IPropertyMap>>::value,
                  "type not allowed for PropertySequenceWrapper");
    static_assert(std::is_same<Sequence<ExternType, ::std::allocator<HoldingType>>, std::vector<ExternType, ::std::allocator<HoldingType>>>::value ||
                  std::is_same<Sequence<ExternType, ::std::allocator<HoldingType>>, std::deque<ExternType, ::std::allocator<HoldingType>>>::value,
                  "Sequence type not allowed for PropertySequenceWrapper");
    PropertySequenceWrapper(const char* name, Sequence<HoldingType, ::std::allocator<HoldingType>> & paramWrapped, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT); // single value wrapper
    ~PropertySequenceWrapper () override;

    const char* getName () const override;

    IProperty::PropertyType getType () override;

    uint32_t size () override;

    void setIntegerValue (int32_t val, uint32_t itemIndex = 0) override;

    void setUnsignedIntegerValue (uint32_t val, uint32_t itemIndex = 0) override;

    void setLongValue (int64_t val, uint32_t itemIndex = 0 ) override;

    void setUnsignedLongValue (uint64_t val, uint32_t itemIndex = 0 ) override;

    void setFloatingValue (float val, uint32_t itemIndex = 0 ) override;

    void setDoubleValue (double val, uint32_t itemIndex = 0 ) override;

    void setStringValue (const char * val, uint32_t itemIndex) override;

    void setUnicodeStringValue (const wchar_t * val, uint32_t itemIndex) override;

    void setStructureValue (SRef<IPropertyMap> val, uint32_t itemIndex) override;

    int32_t getIntegerValue (uint32_t itemIndex = 0) const override;

    uint32_t getUnsignedIntegerValue (uint32_t) const override;

    int64_t getLongValue (uint32_t itemIndex = 0) const override;

    uint64_t getUnsignedLongValue (uint32_t) const override;

    const char* getStringValue (uint32_t itemIndex) const override;

    const wchar_t* getUnicodeStringValue (uint32_t itemIndex) const override;

    float getFloatingValue (uint32_t itemIndex = 0) const override;

    double getDoubleValue (uint32_t itemIndex = 0) const override;

    SRef<IPropertyMap> getStructureValue (uint32_t itemIndex) const override;

    XPCFErrorCode setProperties (const char* name, uint32_t numberOfValues = 1L, IProperty::AccessSpecifier iotype = IProperty::IProperty_INOUT) override;

    XPCFErrorCode copyTo (SRef<IProperty> dest) override;

    void setName (const char *name) override;

   // SRef<ICloneable> clone () override;

    IProperty::AccessSpecifier getAccessSpecifier () const override;

    void setAccessSpecifier (IProperty::AccessSpecifier iotype) override;

  //  void unloadComponent () override final;

private:
    static constexpr IProperty::PropertyType m_type = inferPropertyType<ExternType>();

    Sequence<HoldingType, ::std::allocator<HoldingType>> & m_wrappedValue;
    std::string m_name;

    IProperty::AccessSpecifier m_accessSpecifier;

    ExternType getValue(uint32_t itemIndex) const;
    void setValue(uint32_t itemIndex, ExternType value);

};

// TODO : add method to check accessspecifier upon setter/getters

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
PropertySequenceWrapper<Sequence, ExternType, HoldingType>::PropertySequenceWrapper(const char* name, Sequence<HoldingType, ::std::allocator<HoldingType>> & paramWrapped, IProperty::AccessSpecifier iotype):m_wrappedValue(paramWrapped), m_name(name), m_accessSpecifier(iotype)
{
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
PropertySequenceWrapper<Sequence, ExternType, HoldingType>::~PropertySequenceWrapper ()
{
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
const char* PropertySequenceWrapper<Sequence, ExternType, HoldingType>::getName () const
{
    return m_name.c_str();
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
IProperty::PropertyType PropertySequenceWrapper<Sequence, ExternType, HoldingType>::getType ()
{
    return m_type;
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
uint32_t PropertySequenceWrapper<Sequence, ExternType, HoldingType>::size ()
{
    return m_wrappedValue.size();
}


template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
void PropertySequenceWrapper<Sequence, ExternType, HoldingType>::setValue(uint32_t itemIndex, ExternType value)
{
    m_wrappedValue.at(itemIndex) = value;
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
ExternType PropertySequenceWrapper<Sequence, ExternType, HoldingType>::getValue(uint32_t itemIndex) const
{
    return m_wrappedValue.at(itemIndex);
}

template<>
inline const char * PropertySequenceWrapper<std::vector, const char *>::getValue(uint32_t itemIndex) const
{
    return m_wrappedValue.at(itemIndex).c_str();
}

template<>
inline const char * PropertySequenceWrapper<std::deque, const char *>::getValue(uint32_t itemIndex) const
{
    return m_wrappedValue.at(itemIndex).c_str();
}

template<>
inline const wchar_t * PropertySequenceWrapper<std::vector, const wchar_t *>::getValue(uint32_t itemIndex) const
{
    return m_wrappedValue.at(itemIndex).c_str();
}

template<>
inline const wchar_t * PropertySequenceWrapper<std::deque, const wchar_t *>::getValue(uint32_t itemIndex) const
{
    return m_wrappedValue.at(itemIndex).c_str();
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
void PropertySequenceWrapper<Sequence, ExternType, HoldingType>::setIntegerValue (int32_t , uint32_t )
{
}

template<>
inline void PropertySequenceWrapper<std::vector, int32_t, int32_t>::setIntegerValue (int32_t value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template<>
inline void PropertySequenceWrapper<std::deque, int32_t, int32_t>::setIntegerValue (int32_t value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
void PropertySequenceWrapper<Sequence, ExternType, HoldingType>::setUnsignedIntegerValue (uint32_t , uint32_t )
{
}

template<>
inline void PropertySequenceWrapper<std::vector, uint32_t>::setUnsignedIntegerValue (uint32_t value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template<>
inline void PropertySequenceWrapper<std::deque, uint32_t>::setUnsignedIntegerValue (uint32_t value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
void PropertySequenceWrapper<Sequence, ExternType, HoldingType>::setLongValue (int64_t , uint32_t )
{
}

template<>
inline void PropertySequenceWrapper<std::vector, int64_t, int64_t>::setLongValue (int64_t value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template<>
inline void PropertySequenceWrapper<std::deque, int64_t, int64_t>::setLongValue (int64_t value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
void PropertySequenceWrapper<Sequence, ExternType, HoldingType>::setUnsignedLongValue (uint64_t , uint32_t )
{
}

template<>
inline void PropertySequenceWrapper<std::vector, uint64_t>::setUnsignedLongValue (uint64_t value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template<>
inline void PropertySequenceWrapper<std::deque, uint64_t>::setUnsignedLongValue (uint64_t value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
void PropertySequenceWrapper<Sequence, ExternType, HoldingType>::setFloatingValue (float , uint32_t )
{
}

template<>
inline void PropertySequenceWrapper<std::vector, float>::setFloatingValue (float value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template<>
inline void PropertySequenceWrapper<std::deque, float>::setFloatingValue (float value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
void PropertySequenceWrapper<Sequence, ExternType, HoldingType>::setDoubleValue (double , uint32_t )
{
}

template<>
inline void PropertySequenceWrapper<std::vector, double>::setDoubleValue (double value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template<>
inline void PropertySequenceWrapper<std::deque, double>::setDoubleValue (double value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template < template<typename, typename> class Sequence, class ExternType, class HoldingType>
void PropertySequenceWrapper<Sequence, ExternType, HoldingType>::setStringValue (const char* , uint32_t )
{
}

template<>
inline void PropertySequenceWrapper<std::vector, const char *>::setStringValue (const char* value, uint32_t itemIndex)
{
    m_wrappedValue.at(itemIndex) = value;
}

template<>
inline void PropertySequenceWrapper<std::deque, const char *>::setStringValue (const char* value, uint32_t itemIndex)
{
    m_wrappedValue.at(itemIndex) = value;
}


template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
void PropertySequenceWrapper<Sequence, ExternType, HoldingType>::setUnicodeStringValue (const wchar_t* , uint32_t )
{
}

template<>
inline void PropertySequenceWrapper<std::vector, const wchar_t *>::setUnicodeStringValue (const wchar_t* value, uint32_t itemIndex)
{
    m_wrappedValue.at(itemIndex) = value;
}

template<>
inline void PropertySequenceWrapper<std::deque, const wchar_t *>::setUnicodeStringValue (const wchar_t* value, uint32_t itemIndex)
{
    m_wrappedValue.at(itemIndex) = value;
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
void PropertySequenceWrapper<Sequence, ExternType, HoldingType>::setStructureValue (SRef<IPropertyMap> , uint32_t )
{
}

template<>
inline void PropertySequenceWrapper<std::vector, SRef<IPropertyMap>>::setStructureValue (SRef<IPropertyMap> value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template<>
inline void PropertySequenceWrapper<std::deque, SRef<IPropertyMap>>::setStructureValue (SRef<IPropertyMap> value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
int32_t PropertySequenceWrapper<Sequence, ExternType, HoldingType>::getIntegerValue (uint32_t ) const
{
    return 0;
}

template<>
inline int32_t PropertySequenceWrapper<std::vector, int32_t>::getIntegerValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template<>
inline int32_t PropertySequenceWrapper<std::deque, int32_t>::getIntegerValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}


template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
uint32_t PropertySequenceWrapper<Sequence, ExternType, HoldingType>::getUnsignedIntegerValue (uint32_t ) const
{
    return 0;
}

template<>
inline uint32_t PropertySequenceWrapper<std::vector, uint32_t>::getUnsignedIntegerValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template<>
inline uint32_t PropertySequenceWrapper<std::deque, uint32_t>::getUnsignedIntegerValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
int64_t PropertySequenceWrapper<Sequence, ExternType, HoldingType>::getLongValue (uint32_t ) const
{
    return 0;
}

template<>
inline int64_t PropertySequenceWrapper<std::vector, int64_t>::getLongValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template<>
inline int64_t PropertySequenceWrapper<std::deque, int64_t>::getLongValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}


template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
uint64_t PropertySequenceWrapper<Sequence, ExternType, HoldingType>::getUnsignedLongValue (uint32_t ) const
{
    return 0;
}

template<>
inline uint64_t PropertySequenceWrapper<std::vector, uint64_t>::getUnsignedLongValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template<>
inline uint64_t PropertySequenceWrapper<std::deque, uint64_t>::getUnsignedLongValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
const char* PropertySequenceWrapper<Sequence, ExternType, HoldingType>::getStringValue (uint32_t ) const
{
    return nullptr;
}

template<>
inline const char* PropertySequenceWrapper<std::vector, const char *>::getStringValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template<>
inline const char* PropertySequenceWrapper<std::deque, const char *>::getStringValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}


template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
const wchar_t* PropertySequenceWrapper<Sequence, ExternType, HoldingType>::getUnicodeStringValue (uint32_t ) const
{
    return nullptr;
}

template<>
inline const wchar_t* PropertySequenceWrapper<std::vector, const wchar_t *>::getUnicodeStringValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template<>
inline const wchar_t* PropertySequenceWrapper<std::deque, const wchar_t *>::getUnicodeStringValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
float PropertySequenceWrapper<Sequence, ExternType, HoldingType>::getFloatingValue (uint32_t ) const
{
    return 0;
}

template<>
inline float PropertySequenceWrapper<std::vector, float>::getFloatingValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template<>
inline float PropertySequenceWrapper<std::deque, float>::getFloatingValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
double PropertySequenceWrapper<Sequence, ExternType, HoldingType>::getDoubleValue (uint32_t ) const
{
    return 0;
}

template<>
inline double PropertySequenceWrapper<std::vector, double>::getDoubleValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template<>
inline double PropertySequenceWrapper<std::deque, double>::getDoubleValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
SRef<IPropertyMap> PropertySequenceWrapper<Sequence, ExternType, HoldingType>::getStructureValue (uint32_t ) const
{
    return nullptr;
}

template<>
inline SRef<IPropertyMap> PropertySequenceWrapper<std::vector, SRef<IPropertyMap>>::getStructureValue (uint32_t itemIndex) const
{
    return m_wrappedValue.at(itemIndex);
}

template<>
inline SRef<IPropertyMap> PropertySequenceWrapper<std::deque, SRef<IPropertyMap>>::getStructureValue (uint32_t itemIndex) const
{
    return m_wrappedValue.at(itemIndex);
}


template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
XPCFErrorCode PropertySequenceWrapper<Sequence, ExternType, HoldingType>::setProperties (const char* name, uint32_t numberOfValues, IProperty::AccessSpecifier iotype)
{
    if (name==nullptr) {
        return XPCFErrorCode::_FAIL;
    }

    if (strlen(name) == 0) {
        return XPCFErrorCode::_FAIL;
    }
    m_wrappedValue.resize(numberOfValues);
    m_name = name;
    m_accessSpecifier    = iotype;
    return XPCFErrorCode::_SUCCESS;
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
XPCFErrorCode PropertySequenceWrapper<Sequence, ExternType, HoldingType>::copyTo (SRef<IProperty> dest)
{
    if (dest==nullptr)
    {
        throw std::invalid_argument("Argument is a null pointer");
    }

    XPCFErrorCode res=XPCFErrorCode::_FAIL;
    dest->setProperties (m_name.c_str(), m_wrappedValue.size(), m_accessSpecifier);
    switch(m_type)
    {
    case IProperty_INTEGER:
    {
        uint32_t i = 0;
        for (i=0; i<m_wrappedValue.size();i++)
        {

            dest->setIntegerValue(getIntegerValue(i), i);
        }
    }break;
    case IProperty_UINTEGER:
    {
        uint32_t i = 0;
        for (i=0; i<m_wrappedValue.size() ; i++)
        {

            dest->setUnsignedIntegerValue(getUnsignedIntegerValue(i), i);
        }
    }break;
    case IProperty_FLOAT:
    {
        uint32_t i = 0;
        for (i=0; i<m_wrappedValue.size() ; i++)
        {

            dest->setFloatingValue(getFloatingValue(i), i);
        }

    }break;
    case IProperty_CHARSTR:
    {
        uint32_t i = 0;
        for (i=0; i<m_wrappedValue.size() ; i++)
        {

            dest->setStringValue(getStringValue(i), i);
        }

    }break;
    case IProperty_UNICODESTR:
    {
        uint32_t i = 0;
        for (i=0; i<m_wrappedValue.size() ; i++)
        {

            dest->setUnicodeStringValue(getUnicodeStringValue(i), i);
        }

    }break;
    case IProperty_STRUCTURE:
    {
        uint32_t i = 0;
        for (i=0; i<m_wrappedValue.size() ; i++)
        {

            dest->setStructureValue(getStructureValue(i), i);
        }
    }break;
    default:{return res;}break;
    }


    return XPCFErrorCode::_SUCCESS;
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
void PropertySequenceWrapper<Sequence, ExternType, HoldingType>::setName (const char *name)
{
    if (name==nullptr)
    {
        throw std::invalid_argument("Argument is a null pointer");
    }
    // TODO : check non empty name is set !!
    m_name = name;
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
IProperty::AccessSpecifier PropertySequenceWrapper<Sequence, ExternType, HoldingType>::getAccessSpecifier () const
{
    return m_accessSpecifier;
}

template <template<typename, typename> class Sequence, class ExternType, class HoldingType>
void PropertySequenceWrapper<Sequence, ExternType, HoldingType>::setAccessSpecifier (IProperty::AccessSpecifier iotype)
{
    m_accessSpecifier = iotype;
}

}}}

#endif
