TARGET = testxpcf_static_deps

TEMPLATE_LIST_FILE=../version.pri packagedependencies.txt xpcf_registry_test.xml xpcf_xml_test_missing_end_tag.xml xpcf_xml_test_missing_start_tag.xml xpcf_xml_test_module_uuid_format_ko.xml xpcf_xml_test_property_type_invalid.xml xpcf_xml_test_property_unknown_component.xml xpcf_xml_test_unknown_component.xml xpcf_xml_test_unknown_property.xml xpcf_xml_test_valid.xml
PROJECT_ROOT_PATH=$$_PRO_FILE_PWD_/..
include ($${PROJECT_ROOT_PATH}/generate_template_files.pri)

CONFIG += shared
DEPENDENCIESCONFIG = staticlib recurse
DEFINES += XPCF_STATIC_DEPS

include(testxpcf.pri)
