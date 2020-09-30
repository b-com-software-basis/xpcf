#ifndef CPPHELPERS_H
#define CPPHELPERS_H
#include <iostream>
#include <stack>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

const std::string cppBeginBlock = "{\n";
const std::string cppEndBlock = "}\n\n";
const std::string classEndBlock = "};\n\n";
const std::string indentStr = "  ";

typedef enum {
    STD = 0,
    CLASS = 1,
    PUBLIC = 2,
    PROTECTED = 3,
    PRIVATE = 4,
    NSPACE = 5
} CPP;

template< CPP t = STD>
class ScopeHolder {
public:
    ScopeHolder(std::function<void ()> leaveFunc):m_leaveFunc(leaveFunc) {} ;
    ~ScopeHolder() { m_leaveFunc(); }

private:
    std::function<void ()> m_leaveFunc;
};

class CppBlockManager {
public:

    CppBlockManager(std::ostream& out):m_out(out) {};
    ~CppBlockManager() = default;

    void newline() {
        m_out << '\n';
    }

    template< CPP t = STD>
    ScopeHolder<t> enter() {
        m_blockStack.push(t);
        std::string b;
        switch (t) {
        case PUBLIC :
            b = indent() + "public:\n";
            break;
        case PROTECTED :
            b = indent() + "protected:\n";
            break;
        case PRIVATE :
            b = indent() + "private:\n";
            break;
        default:
            b = indent() + cppBeginBlock;
            break;
        }
        if (t != NSPACE) {
            m_indentLevel ++;
        }
        m_out << b;
        return ScopeHolder<t>([&]() { this->leave(); });
    }

    std::ostream& out() { return m_out << indent(); }

    std::string indent() {
        std::string ind;
        for (uint32_t i = 0; i<m_indentLevel; i++) {
            ind += indentStr;
        }
        return ind;
    }

private:
    void leave() {
        CPP t = m_blockStack.top();
        m_blockStack.pop();
        std::string b;
        if (t != NSPACE && m_indentLevel > 0) {
            m_indentLevel --;
        }
        switch (t) {
        case PUBLIC :
        case PROTECTED :
        case PRIVATE :
            b = "\n";
            break;
        case CLASS :
            b = indent() + classEndBlock;
            break;
        default:
            b = indent() + cppEndBlock;
            break;
        }
        m_out << b;
    }
    uint32_t m_indentLevel = 0;
    std::ostream& m_out;
    std::stack<CPP> m_blockStack;
};

inline void newline(std::ostream& out)
{
    out << '\n';
}

inline void includeGuardsStart(const std::string & name, std::ostream& out)
{
    std::string upcasedName =  boost::to_upper_copy(name);
    out << "#ifndef " + upcasedName + "_H" ;
    newline(out);
    out << "#define " + upcasedName + "_H" ;
    newline(out);
}

inline void includeGuardsEnd(std::ostream& out)
{
    out << "#endif";
}

#endif // CPPHELPERS_H
