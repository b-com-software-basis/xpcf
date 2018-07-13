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

#ifndef ORG_BCOM_XPCF_PROPERTYWRAPPER_H
#define ORG_BCOM_XPCF_PROPERTYWRAPPER_H


#include "xpcf/properties/IProperty.h"
// NOTE : use template specialization

namespace org { namespace bcom { namespace xpcf {

// TODO : implement nocopy accessors for PropertyMap or a propertymapwrapper
template <class T> inline static constexpr IProperty::PropertyType inferPropertyType();

template <> inline constexpr IProperty::PropertyType inferPropertyType<float>()
{
    return IProperty::IProperty_FLOAT;
}

template <> inline constexpr IProperty::PropertyType inferPropertyType<double>()
{
    return IProperty::IProperty_DOUBLE;
}

template <> inline constexpr IProperty::PropertyType inferPropertyType<int32_t>()
{
    return IProperty::IProperty_INTEGER;
}

template <> inline constexpr IProperty::PropertyType inferPropertyType<uint32_t>()
{
    return IProperty::IProperty_UINTEGER;
}

template <> inline constexpr IProperty::PropertyType inferPropertyType<int64_t>()
{
    return IProperty::IProperty_LONG;
}

template <> inline constexpr IProperty::PropertyType inferPropertyType<uint64_t>()
{
    return IProperty::IProperty_ULONG;
}

template <> inline constexpr IProperty::PropertyType inferPropertyType<const wchar_t *>()
{
    return IProperty::IProperty_UNICODESTR;
}

template <> inline constexpr IProperty::PropertyType inferPropertyType<const char *>()
{
    return IProperty::IProperty_CHARSTR;
}

template <> inline constexpr IProperty::PropertyType inferPropertyType<SRef<IPropertyMap>>()
{
    return IProperty::IProperty_STRUCTURE;
}

// Primary template
template<typename T>
struct deduce_from
{
};

// Specialization for double: U -> double
template<>
struct deduce_from<double>
{
    typedef double InnerType;
};

template<>
struct deduce_from<float>
{
    typedef float InnerType;
};

template<>
struct deduce_from<int32_t>
{
    typedef int32_t InnerType;
};

template<>
struct deduce_from<uint32_t>
{
    typedef uint32_t InnerType;
};

template<>
struct deduce_from<int64_t>
{
    typedef int64_t InnerType;
};

template<>
struct deduce_from<uint64_t>
{
    typedef uint64_t InnerType;
};

template<>
struct deduce_from<SRef<IPropertyMap>>
{
    typedef SRef<IPropertyMap> InnerType;
};

template<>
struct deduce_from<const char *>
{
    typedef std::string InnerType;
};

template<>
struct deduce_from<const wchar_t *>
{
    typedef std::wstring InnerType;
};


template <class ExternType, class HoldingType = typename deduce_from<ExternType>::InnerType >
class PropertyWrapper : public IProperty {//<T> T in uint64_t double int64_t char* wchar*
public:
    static_assert(std::is_same<ExternType, float>::value || std::is_same<ExternType, double>::value || std::is_same<ExternType, int64_t>::value
                  || std::is_same<ExternType, uint64_t>::value || std::is_same<ExternType, int32_t>::value || std::is_same<ExternType, uint32_t>::value
                  || std::is_same<ExternType, const char *>::value || std::is_same<ExternType, const wchar_t *>::value || std::is_same<ExternType, SRef<IPropertyMap>>::value,
                  "type not allowed for PropertyWrapper");
    PropertyWrapper(const char* name, HoldingType & paramWrapped, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT); // single value wrapper
    ~PropertyWrapper () override;

    const char* getName () const override;

    IProperty::PropertyType getType () override;

    uint32_t size () override;

    void setIntegerValue (int32_t val, uint32_t itemIndex = 0) override;

    void setUnsignedIntegerValue (uint32_t val, uint32_t itemIndex = 0) override;

    void setLongValue (int64_t val, uint32_t itemIndex = 0 ) override;

    void setUnsignedLongValue (uint64_t val, uint32_t itemIndex = 0 ) override;

    void setFloatingValue (float val, uint32_t itemIndex = 0 ) override;

    void setDoubleValue (double val, uint32_t itemIndex = 0 ) override;

    void setStringValue (const char * val, uint32_t itemIndex = 0) override;

    void setUnicodeStringValue (const wchar_t * val, uint32_t itemIndex = 0) override;

    void setStructureValue (SRef<IPropertyMap> val, uint32_t itemIndex = 0) override;

    int32_t getIntegerValue (uint32_t itemIndex = 0)  const override;

    uint32_t getUnsignedIntegerValue (uint32_t) const override;

    int64_t getLongValue (uint32_t itemIndex = 0) const override;

    uint64_t getUnsignedLongValue (uint32_t) const override;

    const char* getStringValue (uint32_t itemIndex = 0) const override;

    const wchar_t* getUnicodeStringValue (uint32_t itemIndex = 0) const override;

    float getFloatingValue (uint32_t itemIndex = 0) const override;

    double getDoubleValue (uint32_t itemIndex = 0) const override;

    SRef<IPropertyMap> getStructureValue (uint32_t itemIndex = 0) const override;

    XPCFErrorCode setProperties (const char* name, uint32_t numberOfValues = 1L, IProperty::AccessSpecifier iotype = IProperty::IProperty_INOUT) override;

    XPCFErrorCode copyTo (SRef<IProperty> dest) override;

    void setName (const char *name) override;

    //SRef<ICloneable> clone () ;//override;

    IProperty::AccessSpecifier getAccessSpecifier () const override;

    void setAccessSpecifier (IProperty::AccessSpecifier iotype) override;

    //void unloadComponent () override final;

private:
    static constexpr IProperty::PropertyType m_type = inferPropertyType<ExternType>();

    HoldingType & m_wrappedValue;
    std::string m_name;

    IProperty::AccessSpecifier m_accessSpecifier;
    const  uint32_t m_size = 1;

    ExternType getValue(uint32_t itemIndex)  const;
    void setValue(uint32_t itemIndex, ExternType value);

};

// TODO : add method to check accessspecifier upon setter/getters

template <class ExternType, class HoldingType>
PropertyWrapper<ExternType, HoldingType>::PropertyWrapper(const char* name, HoldingType & paramWrapped,
                                                        IProperty::AccessSpecifier iotype):m_name(name), m_wrappedValue(paramWrapped), m_accessSpecifier(iotype)
{
}

template <class ExternType, class HoldingType>
PropertyWrapper<ExternType, HoldingType>::~PropertyWrapper ()
{
}

template<class ExternType, class HoldingType>
inline const char* PropertyWrapper<ExternType, HoldingType>::getName () const
{
    return m_name.c_str();
}

template<class ExternType, class HoldingType>
IProperty::PropertyType PropertyWrapper<ExternType, HoldingType>::getType ()
{
    return m_type;
}

template<class ExternType, class HoldingType>
inline uint32_t PropertyWrapper<ExternType, HoldingType>::size ()
{
    return m_size;
}


template<class ExternType, class HoldingType>
void PropertyWrapper<ExternType, HoldingType>::setValue(uint32_t itemIndex, ExternType value)
{
    if (itemIndex >= m_size) {
        throw std::out_of_range("itemIndex is greater than container size");
    }
    m_wrappedValue = value;
}

template<class ExternType, class HoldingType>
ExternType PropertyWrapper<ExternType, HoldingType>::getValue(uint32_t itemIndex)  const
{
    if (itemIndex >= m_size)
    {
        throw std::out_of_range("itemIndex is greater than container size");
    }
    return m_wrappedValue;
}

template<>
inline const char * PropertyWrapper<const char *>::getValue(uint32_t itemIndex) const
{
    if (itemIndex >= m_size)
    {
        throw std::out_of_range("itemIndex is greater than container size");
    }
    return m_wrappedValue.c_str();
}

template<>
inline const wchar_t * PropertyWrapper<const wchar_t *>::getValue(uint32_t itemIndex) const
{
    if (itemIndex >= m_size)
    {
        throw std::out_of_range("itemIndex is greater than container size");
    }
    return m_wrappedValue.c_str();
}


template<class ExternType, class HoldingType>
void PropertyWrapper<ExternType, HoldingType>::setIntegerValue (int32_t val, uint32_t itemIndex)
{

}

template<>
inline void PropertyWrapper<int32_t, int32_t>::setIntegerValue (int32_t value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}


template<class ExternType, class HoldingType>
void PropertyWrapper<ExternType, HoldingType>::setUnsignedIntegerValue (uint32_t val, uint32_t itemIndex)
{

}

template<>
inline void PropertyWrapper<uint32_t>::setUnsignedIntegerValue (uint32_t value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template<class ExternType, class HoldingType>
void PropertyWrapper<ExternType, HoldingType>::setLongValue (int64_t val, uint32_t itemIndex)
{

}

template<>
inline void PropertyWrapper<int64_t, int64_t>::setLongValue (int64_t value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template<class ExternType, class HoldingType>
void PropertyWrapper<ExternType, HoldingType>::setUnsignedLongValue (uint64_t val, uint32_t itemIndex)
{

}

template<>
inline void PropertyWrapper<uint64_t>::setUnsignedLongValue (uint64_t value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template<class ExternType, class HoldingType>
void PropertyWrapper<ExternType, HoldingType>::setFloatingValue (float value, uint32_t itemIndex)
{

}

template<>
inline void PropertyWrapper<float>::setFloatingValue (float value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}

template<class ExternType, class HoldingType>
void PropertyWrapper<ExternType, HoldingType>::setDoubleValue (double value, uint32_t itemIndex)
{

}

template<>
inline void PropertyWrapper<double>::setDoubleValue (double value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}


template < class ExternType, class HoldingType>
void PropertyWrapper<ExternType, HoldingType>::setStringValue (const char* val, uint32_t itemIndex)
{

}

template<>
inline void PropertyWrapper<const char *>::setStringValue (const char* value, uint32_t itemIndex)
{
    m_wrappedValue = value;
}


template<class ExternType, class HoldingType>
void PropertyWrapper<ExternType, HoldingType>::setUnicodeStringValue (const wchar_t* val, uint32_t itemIndex)
{

}

template<>
inline void PropertyWrapper<const wchar_t *>::setUnicodeStringValue (const wchar_t* value, uint32_t itemIndex)
{
    m_wrappedValue = value;
}



template<class ExternType, class HoldingType>
void PropertyWrapper<ExternType, HoldingType>::setStructureValue (SRef<IPropertyMap> val, uint32_t itemIndex)
{

}

template<>
inline void PropertyWrapper<SRef<IPropertyMap>>::setStructureValue (SRef<IPropertyMap> value, uint32_t itemIndex)
{
    setValue(itemIndex, value);
}


template<class ExternType, class HoldingType>
int32_t PropertyWrapper<ExternType, HoldingType>::getIntegerValue (uint32_t itemIndex) const
{
    return 0;
}

template<>
inline int32_t PropertyWrapper<int32_t>::getIntegerValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}


template<class ExternType, class HoldingType>
uint32_t PropertyWrapper<ExternType, HoldingType>::getUnsignedIntegerValue (uint32_t itemIndex) const
{
    return 0;
}

template<>
inline uint32_t PropertyWrapper<uint32_t>::getUnsignedIntegerValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template<class ExternType, class HoldingType>
int64_t PropertyWrapper<ExternType, HoldingType>::getLongValue (uint32_t itemIndex) const
{
    return 0;
}

template<>
inline int64_t PropertyWrapper<int64_t>::getLongValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}


template<class ExternType, class HoldingType>
uint64_t PropertyWrapper<ExternType, HoldingType>::getUnsignedLongValue (uint32_t itemIndex) const
{
    return 0;
}

template<>
inline uint64_t PropertyWrapper<uint64_t>::getUnsignedLongValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template<class ExternType, class HoldingType>
const char* PropertyWrapper<ExternType, HoldingType>::getStringValue (uint32_t itemIndex) const
{
    return nullptr;
}

template<>
inline const char* PropertyWrapper<const char *>::getStringValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}


template<class ExternType, class HoldingType>
const wchar_t* PropertyWrapper<ExternType, HoldingType>::getUnicodeStringValue (uint32_t itemIndex) const
{
    return nullptr;
}

template<>
inline const wchar_t* PropertyWrapper<const wchar_t *>::getUnicodeStringValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template<class ExternType, class HoldingType>
float PropertyWrapper<ExternType, HoldingType>::getFloatingValue (uint32_t itemIndex) const
{
    return 0;
}

template<>
inline float PropertyWrapper<float>::getFloatingValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template<class ExternType, class HoldingType>
double PropertyWrapper<ExternType, HoldingType>::getDoubleValue (uint32_t itemIndex) const
{
    return 0;
}

template<>
inline double PropertyWrapper<double>::getDoubleValue (uint32_t itemIndex) const
{
    return getValue(itemIndex);
}

template<class ExternType, class HoldingType>
SRef<IPropertyMap> PropertyWrapper<ExternType, HoldingType>::getStructureValue (uint32_t itemIndex) const
{
    return nullptr;
}

template<>
inline SRef<IPropertyMap> PropertyWrapper<SRef<IPropertyMap>>::getStructureValue (uint32_t itemIndex) const
{
    return m_wrappedValue;
}

template<class ExternType, class HoldingType>
XPCFErrorCode PropertyWrapper<ExternType, HoldingType>::setProperties (const char* name, uint32_t numberOfValues, IProperty::AccessSpecifier iotype)
{
    if (name==nullptr) {
        return XPCFErrorCode::_FAIL;
    }

    if (strlen(name) == 0) {
        return XPCFErrorCode::_FAIL;
    }
    if (numberOfValues > m_size) {
        return XPCFErrorCode::_FAIL;
    }
    m_name = name;
    m_accessSpecifier    = iotype;
    return XPCFErrorCode::_SUCCESS;
}

template<class ExternType, class HoldingType>
XPCFErrorCode PropertyWrapper<ExternType, HoldingType>::copyTo (SRef<IProperty> dest)
{
    if (dest==nullptr)
    {
        throw std::invalid_argument("Argument is a null pointer");
    }

    XPCFErrorCode res=XPCFErrorCode::_FAIL;
    dest->setProperties (m_name.c_str(), m_size, m_accessSpecifier);
    switch (m_type)
    {
    case IProperty_INTEGER:
    {
        uint32_t i = 0;
        for (i=0; i<m_size;i++)
        {

            dest->setIntegerValue(getIntegerValue(i), i);
        }
    }break;
    case IProperty_UINTEGER:
    {
        uint32_t i = 0;
        for (i=0; i<m_size;i++)
        {

            dest->setUnsignedIntegerValue(getUnsignedIntegerValue(i), i);
        }
    }break;
    case IProperty_FLOAT:
    {
        uint32_t i = 0;
        for (i=0; i<m_size;i++)
        {

            dest->setFloatingValue(getFloatingValue(i), i);
        }

    }break;
    case IProperty_CHARSTR:
    {
        uint32_t i = 0;
        for (i=0; i<m_size;i++)
        {

            dest->setStringValue(getStringValue(i), i);
        }

    }break;
    case IProperty_UNICODESTR:
    {
        uint32_t i = 0;
        for (i=0; i<m_size;i++)
        {

            dest->setUnicodeStringValue(getUnicodeStringValue(i), i);
        }

    }break;
    case IProperty_STRUCTURE:
    {
        uint32_t i = 0;
        for (i=0; i<m_size;i++)
        {

            dest->setStructureValue(getStructureValue(i), i);
        }
    }break;
    default:{return res;}break;
    }


    return XPCFErrorCode::_SUCCESS;
}

template<class ExternType, class HoldingType>
void PropertyWrapper<ExternType, HoldingType>::setName (const char *name)
{
    if (name==nullptr)
    {
        throw std::invalid_argument("Argument is a null pointer");
    }
    // TODO : check non empty name is set !!
    m_name = name;
}

template<class ExternType, class HoldingType>
IProperty::AccessSpecifier PropertyWrapper<ExternType, HoldingType>::getAccessSpecifier () const
{
    return m_accessSpecifier;
}

template<class ExternType, class HoldingType>
void PropertyWrapper<ExternType, HoldingType>::setAccessSpecifier (IProperty::AccessSpecifier iotype)
{
    m_accessSpecifier = iotype;
}

}}}

#endif
