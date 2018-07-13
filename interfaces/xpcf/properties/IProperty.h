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

#ifndef ORG_BCOM_XPCF_IPROPERTY_H
#define ORG_BCOM_XPCF_IPROPERTY_H

#include "xpcf/core/refs.h"
#include "xpcf/core/XPCFErrorCode.h"

namespace org { namespace bcom { namespace xpcf {

class IPropertyMap;

class IProperty {
public:
    enum PropertyType {
        IProperty_NONE=0,
        IProperty_INTEGER,
        IProperty_UINTEGER,
        IProperty_LONG,
        IProperty_ULONG,
        IProperty_CHARSTR,
        IProperty_UNICODESTR,
        IProperty_FLOAT,
        IProperty_DOUBLE,
        IProperty_STRUCTURE
    };

    typedef enum  {
        IProperty_IN =0x01,
        IProperty_OUT = 0x02,
        IProperty_INOUT= 0x04
    } AccessSpecifier;

    virtual ~IProperty() = default;

    /**
   * To get the name of the property.
   * @return The name of the property. The string must be copied and not deleted by the component user.
   *  The pointer can be null (i.e. if no name is already specified).
   */
    virtual const char* getName () const = 0;

    /**
   * To get the type of the property.
   *
   * @return A value from the enumeration IProperty_PropertyType that correspond to the property's type.
   * @par
   * These values are:
   * <ul>
   * <li>IProperty_INTEGER: for integer value.</li>
   * <li>IProperty_UINTEGER: for unsigned integer value.</li>
   * <li>IProperty_CHARSTR: for string value.</li>
   * <li>IProperty_UNICODESTR: for Unicode string value.</li>
   * <li>IProperty_FLOAT: for floating point value.</li>
   * <li>IProperty_NONE: default value.</li>
   * <li>IProperty_STRUCTURE: a IPropertyMap interface.</li>
   * </ul>
   */
    virtual IProperty::PropertyType getType () = 0;

    /**
   * That method does a copy of the property (property1)
   * in the property (property2) referenced by "dest".
   * A shallow copy is done.
   * The IPropertyMap contained in the source are not explored,
   * only their references will be copied in the destination.
   *
   * @param dest [inout] the caller must provide destination reference.
   * @return @b XPCFErrorCode::_FAIL if the copy fails otherwise @b XPCFErrorCode::_SUCCESS.
   * @throws std::invalid_argument if "dest" is null.
   */
    virtual XPCFErrorCode copyTo (SRef<IProperty> dest) = 0;

    /**
    * To set the name of the property.
   * @param name This is the name of the property.
   * That string is copied and not deleted by the component.
   *
   * @throws std::invalid_argument if "name" is null.
   */
    virtual void setName (const char *name) = 0;

    /**
   * To get the number of values that the property contains (the property can be an array).
   * At least 1 value is present in the property.
   *
   * @return The number of values.
   */
    virtual uint32_t size () = 0;

    /**
   * To set an integer value. The property's type is automatically initialized.
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   * @param val The value.
   *
   */
    virtual void setIntegerValue (int32_t val, uint32_t itemIndex = 0 ) = 0;

    /**
   * To set an unsigned integer value. The property's type is automatically initialised.
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   * @param val The value.
   *
   * @throws std::out_of_range if "anIndex" is out of range (See [BASIC_ERROR]).
   */
    virtual void setUnsignedIntegerValue (uint32_t val, uint32_t itemIndex = 0 ) = 0;

    /**
   * To set an integer value. The property's type is automatically initialized.
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   * @param val The value.
   *
   */

    virtual void setLongValue (int64_t val, uint32_t itemIndex = 0 ) = 0;

    /**
   * To set an unsigned integer value. The property's type is automatically initialised.
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   * @param val The value.
   *
   * @throws std::out_of_range if "anIndex" is out of range (See [BASIC_ERROR]).
   */
    virtual void setUnsignedLongValue (uint64_t val, uint32_t itemIndex = 0 ) = 0;

    /**
   * To set a float value. The property's type is automatically initialized.
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   * @param val The value is a double float.
   *
   */
    virtual void setFloatingValue (float val, uint32_t itemIndex = 0 ) = 0;

    /**
  * To set a double value. The property's type is automatically initialized.
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   * @param val The value is a double float.
   *
   */
    virtual void setDoubleValue (double val, uint32_t itemIndex = 0 ) = 0;

    /**
    * To set a string value. The property's type is automatically initialized. The value will be copied.
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   * @param val The value is an ASCII string ended by a null character. The string is copied.
   *
   * @throws std::out_of_range if "anIndex" is out of range (See [BASIC_ERROR]).
   * @throws std::invalid_argument if "val" is null.
   */
    virtual void setStringValue (const char * val, uint32_t itemIndex = 0 ) = 0;

    /**
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   * @param val The value is a Unicode string ended by a null Unicode character. The string is copied.
   *
   * @throws std::out_of_range if "anIndex" is out of range (See [BASIC_ERROR]).
   */
    virtual void setUnicodeStringValue (const wchar_t * val, uint32_t itemIndex = 0 ) = 0;


    /**
    * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   * @param val The value. The IPropertyMap object is not copied, only the reference on it is copied.
   *
   * @throws std::out_of_range if "anIndex" is out of range (See [BASIC_ERROR]).
   * @throws std::invalid_argument if "val" is null.
   */
    virtual void setStructureValue (SRef<IPropertyMap> val, uint32_t itemIndex = 0 ) = 0;

    /**
   *
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   *
   * @return A reference to a IPropertyMap interface.
   * @throws std::out_of_range if "anIndex" is out of range (See [BASIC_ERROR]).
   */
    virtual SRef<IPropertyMap> getStructureValue (uint32_t itemIndex = 0) const = 0;


    /**
   * To set the name and the number of values that the property contains (the property can be an array).
   * At least 1 value is present in the property. That method can be called more than one time.
   * The method must resize its collection (array, list or whatever collection).
   * The values do not have to be initialised.
   * It is the client's responsibility to set a default value for each index.
   * @param name This is the name of the property. (ASCII, null terminated, 64 characters maximum)
   * @param numberOfValues the number of values.
   * @param iotype A value from the enumeration IProperty_AccessSpecifier that corresponds to the
   * property's access specifier.
   * @par
   * These values are:
   * <ul>
   * <li>@b IProperty_IN: The user application modifies the value </li>
   * <li>@b IProperty_OUT: The interface implementation modifies the value </li>
   * <li>@b IProperty_INOUT: The user application and the interface implementation modify the value </li>
   * </ul>
   * @return
   * <ul>
   * <li>@b XPCFErrorCode::_ERROR_OUT_OF_RANGE if the size of "name" is > 64 characters.</li>
   * <li>@b XPCFErrorCode::_SUCCESS if no error occurs.</li>
   * <li>@b XPCFErrorCode::_FAIL otherwise. </li>
   * </ul>
   */
    virtual XPCFErrorCode setProperties (const char* name, uint32_t numberOfValues = 1L, IProperty::AccessSpecifier iotype = IProperty_INOUT) = 0;

    /**
    * To get an integer value.
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   *
   * @return An integer value.
   * @throws std::out_of_range if "anIndex" is out of range (See [BASIC_ERROR]).
   */
    virtual int32_t getIntegerValue (uint32_t itemIndex = 0) const = 0;

    /**
   * To get an unsigned integer value.
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   *
   * @return An unsigned integer value.
   * @throws std::out_of_range if "anIndex" is out of range (See [BASIC_ERROR]).
   */
    virtual uint32_t getUnsignedIntegerValue (uint32_t itemIndex = 0) const = 0;

    /**
   * To get an integer value.
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   *
   * @return An integer value.
   * @throws std::out_of_range if "anIndex" is out of range (See [BASIC_ERROR]).
   */
    virtual int64_t getLongValue (uint32_t itemIndex = 0) const = 0;

    /**
   * To get an unsigned integer value.
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   *
   * @return An unsigned integer value.
   * @throws std::out_of_range if "anIndex" is out of range (See [BASIC_ERROR]).
   */
    virtual uint64_t getUnsignedLongValue (uint32_t) const = 0;

    /**
   * To get a string value.<b> The string will be delete when the IProperty will be deleted</b>.
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   *
   * @return A string value.
   * @throws std::out_of_range if "anIndex" is out of range (See [BASIC_ERROR]).
   */
    virtual const char* getStringValue (uint32_t itemIndex = 0) const = 0;

    /**
   * To get a Unicode string value. <b>The string will be deleted when the IProperty will be deleted</b>.
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   *
   * @return An Unicode string value.
   * @throws std::out_of_range if "anIndex" is out of range (See [BASIC_ERROR]).
   */
    virtual const wchar_t* getUnicodeStringValue (uint32_t itemIndex = 0) const = 0;

    /**
   * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   *
   * @return A double float value.
   * @throws std::out_of_range if "anIndex" is out of range (See [BASIC_ERROR]).
   */
    virtual float getFloatingValue (uint32_t itemIndex = 0) const = 0;

    /**
    * @param itemIndex the index of the values to retrieve that begin at 0 and end at [size() - 1].
   *
   * @return A double float value.
   * @throws std::out_of_range if "anIndex" is out of range (See [BASIC_ERROR]).
   */
    virtual double getDoubleValue (uint32_t itemIndex = 0) const = 0;

    /**
   * To set the access specifier of the property.
   * That access specifier determines who from the user application and
   * the interface implementation modifies the property value.
   * @param iotype A value from the enumeration IProperty_AccessSpecifier that corresponds to the
   * property's access specifier.
   * @par
   * These values are:
   * <ul>
   * <li>@b IProperty_IN: The user application modifies the value </li>
   * <li>@b IProperty_OUT: The interface implementation modifies the value </li>
   * <li>@b IProperty_INOUT: The user application and the interface implementation modify the value </li>
   * </ul>
   *
   */

    virtual void setAccessSpecifier (IProperty::AccessSpecifier iotype) = 0;

    /**
    * To get the access specifier of the property.
   * That access specifier determines who from the user application and
   * the interface implementation modifies the property value.
   *
   * @return A value from the enumeration IProperty_AccessSpecifier that corresponds to the
   * property's access specifier.
   * @par
   * These values are:
   * <ul>
   * <li>@b IProperty_IN: The user application modifies the value </li>
   * <li>@b IProperty_OUT: The interface implementation modifies the value </li>
   * <li>@b IProperty_INOUT: The user application and the interface implementation modify the value </li>
   * </ul>
   */
    virtual IProperty::AccessSpecifier getAccessSpecifier () const = 0;

};

}}}


#endif
