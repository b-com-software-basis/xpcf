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
 * @date 2018-07-03
 */

#include <stdio.h>
#include <boost/uuid/uuid_io.hpp>
#include <boost/dll.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/function.hpp>
#include <boost/system/system_error.hpp>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include "TestDefines.h"

using namespace std;


class TestPropertyWrapper {
public:
    TestPropertyWrapper();
    ~TestPropertyWrapper() = default;

    inline uint64_t getIntNumber() { return m_intNumber;}
    inline double getFloatNumber() { return m_floatNumber;}
    inline const char * getCharStr() { return m_charStr.c_str();}
    inline const wchar_t * getWcharStr() { return m_wcharStr.c_str();}
    const std::vector<int64_t> & getIntParams() { return m_intParams;}
    inline SRef<xpcf::IPropertyMap> getPropertyRootNode() const {return m_property;}
    inline SRef<xpcf::IProperty> getProperty(const char * name) const {return m_property->at(name);}

private:
    uint64_t m_intNumber;
    float m_floatNumber;
    std::string m_charStr;
    std::wstring m_wcharStr;
    std::vector<int64_t> m_intParams;
    SRef<xpcf::IPropertyMap> m_property;
};

TestPropertyWrapper::TestPropertyWrapper() {
    m_property = xpcf::getPropertyMapInstance();
    m_property->wrapUnsignedLong("intNumber",m_intNumber);
    m_property->wrapFloat("floatNumber",m_floatNumber);
    m_property->wrapString("charstr",m_charStr);
    m_property->wrapUnicodeString("wcharstr",m_wcharStr);
    m_property->wrapLongVector("vectWrapper",m_intParams);
    m_intNumber = 10;
    m_floatNumber = 0.5;
    m_charStr = "in constructor";
    m_wcharStr = L"in constructor";

    m_intParams.resize(10);

    for (uint32_t i = 0; i<m_intParams.size() ; i++) {
        m_intParams[i]=i*2;
    }

}

void displayParameter(SRef<xpcf::IProperty> p)
{
    BOOST_TEST_MESSAGE("Property name ="<<p->getName());
    for (uint32_t i=0 ; i < p->size() ; i++ ) {
        switch (p->getType()) {
        case xpcf::IProperty::IProperty_NONE :
            break;
        case xpcf::IProperty::IProperty_INTEGER :
            BOOST_TEST_MESSAGE("=> Int property = "<<p->getIntegerValue(i));
            break;
        case xpcf::IProperty::IProperty_UINTEGER :
            BOOST_TEST_MESSAGE("=> Uint property = "<<p->getUnsignedIntegerValue(i));
            break;
        case xpcf::IProperty::IProperty_LONG :
            BOOST_TEST_MESSAGE("=> Long property = "<<p->getLongValue(i));
            break;
        case xpcf::IProperty::IProperty_ULONG :
            BOOST_TEST_MESSAGE("=> ULong property = "<<p->getUnsignedLongValue(i));
            break;
        case xpcf::IProperty::IProperty_CHARSTR :
            BOOST_TEST_MESSAGE("=> String property = "<<p->getStringValue(i));
            break;
        case xpcf::IProperty::IProperty_UNICODESTR :
            break;
        case xpcf::IProperty::IProperty_FLOAT :
            BOOST_TEST_MESSAGE("=> Float property = "<<p->getFloatingValue(i));
            break;
        case xpcf::IProperty::IProperty_DOUBLE :
            BOOST_TEST_MESSAGE("=> Double property = "<<p->getDoubleValue(i));
            break;
        case xpcf::IProperty::IProperty_STRUCTURE :
        {
            SRef<xpcf::IPropertyMap> propertyMap = p->getStructureValue(i);
            SRef<xpcf::IEnumerator<SRef<xpcf::IProperty>>> propertyEnum =
                    propertyMap->getProperties().getEnumerator();
            BOOST_TEST_MESSAGE("Accessing class values for structure from IProperty/IPropertyMap interfaces");
            while (propertyEnum->moveNext()) {
                displayParameter(propertyEnum->current());
            }
            break;
        }
        default:
            break;
        }
    }
}

BOOST_AUTO_TEST_SUITE( test_properties )

BOOST_AUTO_TEST_CASE( test_propertyWrapper )
{
    TestPropertyWrapper w;
    BOOST_TEST_MESSAGE("Wrapped class values int="<<w.getIntNumber());
    BOOST_TEST_MESSAGE("Wrapped class values double="<<w.getFloatNumber());
    BOOST_TEST_MESSAGE("Wrapped class values str="<<w.getCharStr());
    BOOST_TEST_MESSAGE("Wrapped class values wstr="<<w.getWcharStr());
    for (uint32_t i=0 ; i<w.getIntParams().size() ; i++ ) {
        BOOST_TEST_MESSAGE("Wrapped class values intParams["<<i<<"]="<<w.getIntParams().at(i));
    }
    /* BOOST_CHECK_THROW(w.getIntParams().at(w.getIntParams().size()),std::out_of_range);*/

    SRef<xpcf::IEnumerator<SRef<xpcf::IProperty>>> propertyEnum =
            w.getPropertyRootNode()->getProperties().getEnumerator();
    BOOST_TEST_MESSAGE("Accessing class values from IProperty/IPropertyMap interfaces");
    try {
        /*xpcf::Iterator<SRef<xpcf::IProperty>> itend = w.getPropertyRootNode()->getProperties().end();
        for (xpcf::Iterator<SRef<xpcf::IProperty>> it = w.getPropertyRootNode()->getProperties().begin() ; it != itend ; ++it) {*/
        for(auto p : w.getPropertyRootNode()->getProperties()) {
            //while (propertyEnum->moveNext()) {
            //    SRef<xpcf::IProperty> p = *it;
            displayParameter(p);
        }
    }
    catch (std::out_of_range & e) {
        BOOST_TEST_MESSAGE("Catched : "<<e.what());
    }
    catch (boost::bad_weak_ptr & e) {
        BOOST_TEST_MESSAGE("Catched : "<<e.what());
    }
}

bool pred(const SRef<xpcf::IProperty> & elem)
{
    if (!strcmp(elem->getName(),"charstr")) {
        return true;
    }
    return false;
}

bool falsePred(const SRef<xpcf::IProperty> & elem)
{
    return false;
}

BOOST_AUTO_TEST_CASE( test_blockEnumerator )
{
    TestPropertyWrapper w;
    auto & props = w.getPropertyRootNode()->getProperties();
    BOOST_TEST_MESSAGE("Iterating with blockenum(0,2)");
    SRef<xpcf::IEnumerator<SRef<xpcf::IProperty>>> propertyEnum = props.getEnumerator(0,2);
    for(auto p : propertyEnum) {
        displayParameter(p);
    }
     BOOST_TEST_MESSAGE("Iterating with blockenum(2,2)");
    for(auto p : props.getEnumerator(2,2)) {
        displayParameter(p);
    }
    propertyEnum = props.getEnumerator(4,1);
     BOOST_TEST_MESSAGE("Iterating with blockenum(4,1)");
    for(auto p : propertyEnum) {
        displayParameter(p);
    }
    // NOTE : missing movable & comparable upon iterator to check for end iterator upon return
    auto result1 = std::find_if(xpcf::begin(props), xpcf::end(props), pred );
    // SHOULD BE ABLE TO WRITE if (result1 != end(props) ) ... play with endReached inside iterator construction ?
    BOOST_TEST_MESSAGE("Testing std::algorithm upon enumerators with valid result");
    auto endIt = xpcf::end(props);
    if (result1 != endIt) {
        displayParameter(*result1);
    }

    // NOTE : missing movable & comparable upon iterator to check for end iterator upon return
    result1 = std::find_if(xpcf::begin(props), xpcf::end(props), falsePred );
    // SHOULD BE ABLE TO WRITE if (result1 != end(props) ) ... play with endReached inside iterator construction ?
    BOOST_TEST_MESSAGE("Testing std::algorithm upon enumerators without result");
    if (result1 != endIt) {
        displayParameter(*result1);
    }
    BOOST_TEST_MESSAGE("Checking exception while constructing BlockEnumerator with values above sequence size");
    BOOST_TEST_MESSAGE("1- testing too many elements exception");
    BOOST_CHECK_THROW( props.getEnumerator(4,2),std::out_of_range);
    BOOST_TEST_MESSAGE("2- testing offset at end of sequence");
    BOOST_CHECK_THROW( props.getEnumerator(5,1),std::out_of_range);
}

BOOST_AUTO_TEST_SUITE_END()



