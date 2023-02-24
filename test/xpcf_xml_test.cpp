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
 * @author Adrien Schadle
 *
 * @file
 * @brief description of file
 * @date 2023-02-21
 */
#include <exception>

#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
#include <boost/filesystem.hpp>

#define BOOST_TEST_MODULE MyTest
#include <boost/test/unit_test.hpp>

#include "TestDefines.h"

namespace fs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(xml_parsing)

BOOST_AUTO_TEST_CASE(xml_valid)
{
    SRef<xpcf::IComponentManager> xpcfComponentManager = xpcf::getComponentManagerInstance();

    fs::path confPath = "xpcf_xml_test_valid.xml";
    fs::detail::utf8_codecvt_facet utf8;
	try {
	    org::bcom::xpcf::XPCFErrorCode ret = xpcfComponentManager->load(confPath.generic_string(utf8).c_str());
		BOOST_TEST_REQUIRE( ret == org::bcom::xpcf::_SUCCESS, "Xml parsing failed with error " << ret);
	}
	catch (std::exception & e) {
		BOOST_TEST_MESSAGE("Exception occures loading xml file.");
	}
    xpcfComponentManager->clear();
}

BOOST_AUTO_TEST_CASE(xml_missing_start_tag)
{
    SRef<xpcf::IComponentManager> xpcfComponentManager = xpcf::getComponentManagerInstance();

    fs::path confPath = "xpcf_xml_test_missing_start_tag.xml";
    fs::detail::utf8_codecvt_facet utf8;
    org::bcom::xpcf::XPCFErrorCode ret = xpcfComponentManager->load(confPath.generic_string(utf8).c_str());
	BOOST_TEST_REQUIRE(ret == org::bcom::xpcf::_SUCCESS, "Xml parsing failed with error " << ret);
    BOOST_CHECK_THROW(org::bcom::xpcf::XPCFErrorCode ret = xpcfComponentManager->load(confPath.generic_string(utf8).c_str()), xpcf::ConfigurationException);
    xpcfComponentManager->clear();
}

BOOST_AUTO_TEST_CASE(xml_missing_end_tag)
{
    SRef<xpcf::IComponentManager> xpcfComponentManager = xpcf::getComponentManagerInstance();

    fs::path confPath = "xpcf_xml_test_missing_end_tag.xml";
    fs::detail::utf8_codecvt_facet utf8;
   	BOOST_CHECK_THROW(org::bcom::xpcf::XPCFErrorCode ret = xpcfComponentManager->load(confPath.generic_string(utf8).c_str()), xpcf::ModuleNotFoundException);
    xpcfComponentManager->clear();
}

BOOST_AUTO_TEST_SUITE_END()