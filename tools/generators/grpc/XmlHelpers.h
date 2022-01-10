#ifndef XMLHELPERS_H
#define XMLHELPERS_H
#include <iostream>
#include <fstream>
#include <stack>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
#include <boost/filesystem/path.hpp>

typedef enum {
    REGISTRY = 0,
    CONFIG = 1,
    MODULE = 2,
    COMPONENT = 3,
    INTERFACE = 4,
    ALIASES = 5,
    FACTORY = 6,
    BINDINGS = 7,
    BIND = 8,
    PROPERTIES = 9,
    CONFIGURE = 10,
    PROPERTY = 11,
} XML;


//template <typename S = std::ostream>
class XmlBlockManager {
public:

    XmlBlockManager(std::ostream& out):m_out(out) {};
    virtual ~XmlBlockManager() = default;

    void newline() {
        m_out << '\n';
    }

    template< XML t = REGISTRY>
    void enter(std::initializer_list<std::pair<std::string,std::string>> attributes = {}) {
        switch (t) {
        case REGISTRY :
            enter("xpcf-registry", attributes);
            break;
        case CONFIG :
            enter("xpcf-configuration", attributes);
            break;
        case MODULE :
            enter("module", attributes);
            break;
        case COMPONENT:
            enter("component", attributes);
            break;
        case INTERFACE:
            enter("interface", attributes);
            break;
        case ALIASES:
            enter("aliases");
            break;
        case FACTORY:
            enter("factory");
            break;
        case BINDINGS:
            enter("bindings");
            break;
        case BIND:
            enter("bind",attributes);
            break;
        case PROPERTIES:
            enter("properties");
            break;
        case CONFIGURE:
            enter("configure", attributes);
            break;
        case PROPERTY:
            enter("property", attributes);
            break;
        }
    }

    template< XML t = MODULE>
    void node(std::initializer_list<std::pair<std::string,std::string>> attributes = {}) {
        // assert for non coherent nodes : registry, etc must be multiline blocks
        switch (t) {
        case MODULE :
            node("module", attributes);
            break;
        case COMPONENT:
            node("component", attributes);
            break;
        case INTERFACE:
            node("interface", attributes);
            break;
        case BIND:
            node("bind", attributes);
            break;
        case CONFIGURE:
            node("configure", attributes);
            break;
        case PROPERTY:
            node("property", attributes);
            break;
        }
    }

    void enter(const std::string & nodeName, std::initializer_list<std::pair<std::string,std::string>> attributes = {}) {
        m_blockNameStack.push(nodeName);
        std::string b;
        b = indent() + "<" + nodeName;
        for (auto & [attrName, attrValue] : attributes) {
            b += " " + attrName  + "=\"" + attrValue + "\"";
        }
        b += ">\n";
        m_indentLevel ++;
        m_out << b;
    }

    void addValue(const std::string & value) {
        m_out << indent() + "<value>" + value + "</value>\n";
    }

    void node(const std::string & nodeName, std::initializer_list<std::pair<std::string,std::string>> attributes = {}) {
        std::string b;
        b = indent() + "<" + nodeName;
        for (auto & [attrName, attrValue] : attributes) {
            b += " " + attrName  + "=\"" + attrValue + "\"";
        }
        b += "/>\n";
        m_out << b;
    }

    void leave() {
        std::string blkName = m_blockNameStack.top();
        m_blockNameStack.pop();
        leave(blkName);
    }

    std::ostream& out() { return m_out << indent(); }

    void xmlOpenRootNode(const std::string & nodeName = "xpcf-registry", std::initializer_list<std::pair<std::string,std::string>> attributes = {{"autoAlias","true"}}) {
        out() << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n";
        m_rootName = nodeName;
        enter(m_rootName);
    }

    void xmlCloseOpenedNodes() {
        while (!m_blockNameStack.empty()) {
            leave();
        }
    }

    void leave(const std::string & nodeName) {
        std::string b;
        if (m_indentLevel > 0) {
            m_indentLevel --;
        }
        b = indent() + "</" + nodeName + ">\n";
        m_out << b;
    }

    std::string indent() {
        std::string ind;
        for (uint32_t i = 0; i<m_indentLevel; i++) {
            ind += m_indentStr;
        }
        return ind;
    }

    virtual void release() {
        xmlCloseOpenedNodes();
    }


private:
    const std::string m_indentStr = "  ";
    uint32_t m_indentLevel = 0;
    std::ostream & m_out;
    std::string m_rootName;
    std::stack<std::string> m_blockNameStack;
};

class XmlFileManager : public XmlBlockManager {
public:
    XmlFileManager(std::unique_ptr<std::ofstream> xmlFile):m_xmlFile(std::move(xmlFile)),
        XmlBlockManager(*xmlFile) {}

    ~XmlFileManager() override {
        release();
    }

    void release() override {
        XmlBlockManager::release();
        m_xmlFile->close();
    }

    static std::unique_ptr<XmlFileManager> createXmlFileManager(const boost::filesystem::path & fileName)
    {
        boost::filesystem::detail::utf8_codecvt_facet utf8;
        return std::make_unique<XmlFileManager>(std::make_unique<std::ofstream>(fileName.generic_string(utf8).c_str(), std::ios::out));
    }

private:
    std::unique_ptr<std::ofstream> m_xmlFile;
};

class XmlStringStreamManager : public XmlBlockManager {
public:
    XmlStringStreamManager(std::unique_ptr<std::stringstream> stream):m_stream(std::move(stream)),
        XmlBlockManager(*stream) {}

    ~XmlStringStreamManager() override {
        release();
    }

private:
    std::unique_ptr<std::stringstream> m_stream;
};

template< XML t = REGISTRY>
class xml_block_guard {
public:
    xml_block_guard(XmlBlockManager & mgr, std::initializer_list<std::pair<std::string,std::string>> attributes = {}):m_blockMgr(mgr) { m_blockMgr.enter<t>(attributes); }
    ~xml_block_guard() { m_blockMgr.leave(); }

private:
    XmlBlockManager & m_blockMgr;
};

class fstream_guard {
public:
    fstream_guard(std::fstream & stream):m_stream(stream) {}
    ~fstream_guard() { if (m_stream.is_open()) { m_stream.close(); } }

private:
    std::fstream & m_stream;
};


#endif // CPPHELPERS_H
