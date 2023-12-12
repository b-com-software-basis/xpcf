TARGET = testxpcf

LIST = "xpcf_registry_test.xml.template" "xpcf_xml_test_missing_end_tag.xml" "xpcf_xml_test_missing_start_tag.xml.template" "xpcf_xml_test_module_uuid_format_ko.xml.template" "xpcf_xml_test_property_type_invalid.xml.template" "xpcf_xml_test_property_unknown_component.xml.template" "xpcf_xml_test_unknown_component.xml.template" "xpcf_xml_test_unknown_property.xml.template" "xpcf_xml_test_valid.xml.template"
for(file, LIST) {
    ! exists(file) {
        win32 {
            system(../scripts/win/update_version.bat)
        }
        linux {
            system(../scripts/unixes/update_version.sh)
        }
        break()
    } 
}

CONFIG += shared
DEPENDENCIESCONFIG = sharedlib recurse
DEFINES += BOOST_ALL_DYN_LINK
DEFINES += XPCF_SHARED_DEPS

include(testxpcf.pri)
