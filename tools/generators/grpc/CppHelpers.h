#ifndef CPPHELPERS_H
#define CPPHELPERS_H
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

const std::string cppBeginBlock = "{\n";
const std::string cppEndBlock = "}\n\n";

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
