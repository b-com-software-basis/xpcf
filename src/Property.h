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

#ifndef ORG_BCOM_XPCF_PROPERTY_H
#define ORG_BCOM_XPCF_PROPERTY_H

#include "xpcf/properties/IProperty.h"
#include "xpcf/xpcf_api_define.h"

#include <deque>
#include <string.h>

namespace org { namespace bcom { namespace xpcf {
// idea : it is possible to create a matrix structure param with uint cols uint rows Type vector ?
// note : accessors' methods throw std::out_of_range when itemIndex exceeds the underlying container boundary
class XPCF_EXPORT_API Property : public IProperty
{

public:
    Property ();

    Property (const char* name, uint32_t numberOfItem, IProperty::AccessSpecifier iotype = IProperty_INOUT);

    ~Property () override;

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

    int32_t getIntegerValue (uint32_t itemIndex = 0) const override;

    uint32_t getUnsignedIntegerValue (uint32_t) const override;

    int64_t getLongValue (uint32_t itemIndex = 0) const override;

    uint64_t getUnsignedLongValue (uint32_t) const override;

    const char* getStringValue (uint32_t itemIndex = 0) const override;

    const wchar_t* getUnicodeStringValue (uint32_t itemIndex = 0) const override;

    float getFloatingValue (uint32_t itemIndex = 0) const override;

    double getDoubleValue (uint32_t itemIndex = 0) const override;

    SRef<IPropertyMap> getStructureValue (uint32_t itemIndex = 0) const override;

    XPCFErrorCode setProperties (const char* name, uint32_t numberOfValues = 1L, IProperty::AccessSpecifier iotype = IProperty_INOUT) override;

    XPCFErrorCode copyTo (SRef<IProperty> dest) override;

    void setName (const char *name) override;

    IProperty::AccessSpecifier getAccessSpecifier () const override;

    void setAccessSpecifier (IProperty::AccessSpecifier iotype) override;

    static SRef<IProperty> CreateInstance ();

    static SRef<IProperty> CreateInstance (const char* name, uint32_t numberOfValues = 1L, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT);

    static Property *  CreateConcreteInstance ();

    static Property * CreateConcreteInstance (const char* name, uint32_t numberOfValues = 1L, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT);


private:
    int init ();
    void clear ();
    void checkType (IProperty::PropertyType newtype);
    template <class T> void checkType ();
    template <class T> T getValue(uint32_t, IProperty::PropertyType valueType)  const;
    template <class U, class T> const T * getValue(uint32_t, IProperty::PropertyType valueType)  const;
    template <class T, class U> void setValue(uint32_t, const T * value);
    template <class T> void setValue(uint32_t, T value);

    std::string m_name;
    IProperty::PropertyType m_type;
    std::deque<utils::any> m_queue;
    IProperty::AccessSpecifier m_attribute;
};

}}}

#endif
