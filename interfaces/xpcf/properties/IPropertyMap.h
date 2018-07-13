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

#ifndef ORG_BCOM_XPCF_IPROPERTYMAP_H
#define ORG_BCOM_XPCF_IPROPERTYMAP_H

#include "xpcf/properties/IProperty.h"
#include "xpcf/collection/IEnumerable.h"
#include <string>

namespace org { namespace bcom { namespace xpcf {

/**
   * @interface IPropertyMap
   * The concept is very simple. The properties are described as a
   * set of {names, type, value(s), attribute}.
   * @par
   * The @b type describes the kind of data that the property uses. It
   * can be a string, an integer, a float...
   * @par
   * The application that uses the component can set a particular
   * @b value but it is up to the component to check its validity.
   * A property is always an array. However, it can contain a single value.
   * @par
   * The @b access @b specifier depicts the modification right that a user
   * application has on the component properties. That access specifier can
   * be @b in, @b out or @b inout.
   * <ul>
   * <li>An @b in property will only be modified by the user application. </li>
   * <li>An @b out property is only modified by the component that
   * implements the IPropertyMap interface. For the user
   * application, an @b out property is a read-only property.</li>
   * <li>An @b inout can be modified by the user application and the component.</li>
   * </ul>
   */


class IPropertyMap
{
public:
    typedef enum  {
        IPropertyMap_IN =0x01,
        IPropertyMap_OUT = 0x02,
        IPropertyMap_INOUT= 0x04,
        IPropertyMap_ALL= 0x07 //IN|OUT|INOUT

    } AccessSpecifier;

    virtual ~IPropertyMap() = default;

    virtual XPCFErrorCode  addProperty (SRef<IProperty> p) = 0;

    /**
   * @fn virtual XPCFErrorCode setProperty (const SRef<IProperty> p) = 0
   * To set a property.
   * The property must be a property recognised by the @b IPropertyMap implementation.
   * The property will be added to the IPropertyMap implementation.
   *
   * @param p is a class that implements the interface @b IProperty.
   * The component must verify the validity of the value.
   *
   * @return
   * <ul>
   * <li> @b IPropertyMap_UNKNOWNPROPERTY : must be returned when the property is not supported by the component.</li>
   * <li> @b IPropertyMap_INVALIDTYPE: must be returned when the property is not of the good type.</li>
   * <li> @b IPropertyMap_INVALIDSIZE: must be returned when the property has not the good number of values.</li>
   * <li> @b IPropertyMap_ACCESSDENIED: must be returned when the property attribute is IProperty_OUT
   * when it is obtained with the methods getProperty or getPropertys.
   * The value of attribute checked is not the value of the argument property.</li>
   * <li> @b XPCFErrorCode::_SUCCESS is returned otherwise.</li>
   * </ul>
   * @throws XPCF_NullObject if "p" is null
   */
    virtual XPCFErrorCode  setProperty (const SRef<IProperty> p) = 0;

    /**
    * To get the properties held by the object that implements this interface.
    *
    * @return an IEnumerable collection upon IProperty's references
    */
    virtual IEnumerable<SRef<IProperty>> & getProperties() = 0;

    virtual const SRef<IProperty> at (const char *name) = 0;

    virtual XPCFErrorCode  wrapInteger(const char * name, int32_t & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapUnsignedInteger(const char * name, uint32_t & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapLong(const char * name, int64_t & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapUnsignedLong(const char * name, uint64_t & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapFloat(const char * name, float & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapDouble(const char * name, double & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapString(const char * name, std::string & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapUnicodeString(const char * name, std::wstring  & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapStructure(const char * name, SRef<IPropertyMap> & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;

    virtual XPCFErrorCode  wrapIntegerVector(const char * name, std::vector<int32_t> & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapUnsignedIntegerVector(const char * name, std::vector<uint32_t> & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapLongVector(const char * name, std::vector<int64_t> & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapUnsignedLongVector(const char * name, std::vector<uint64_t> & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapFloatVector(const char * name, std::vector<float> & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapDoubleVector(const char * name, std::vector<double> & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapStringVector(const char * name, std::vector<std::string> & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapUnicodeStringVector(const char * name, std::vector<std::wstring>  & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;
    virtual XPCFErrorCode  wrapStructureVector(const char * name, std::vector<SRef<IPropertyMap>> & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) = 0;

};

#include "xpcf/xpcf_api_define.h"

XPCF_EXPORT_API SRef<IPropertyMap> getPropertyMapInstance();

}}}


#endif
