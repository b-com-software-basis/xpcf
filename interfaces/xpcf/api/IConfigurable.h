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
 * @date 2017-11-11
 */

#ifndef ORG_BCOM_XPCF_ICONFIGURABLE_H
#define ORG_BCOM_XPCF_ICONFIGURABLE_H

#include "xpcf/api/IComponentIntrospect.h"
#include "xpcf/properties/IPropertyMap.h"

namespace org { namespace bcom { namespace xpcf {

class IConfigurable : public virtual IComponentIntrospect {
public:
    virtual ~IConfigurable() = default;

    /**
     * Configure the component from the given configuration declared in the xml file @em filepath
     * @param [in] filepath the xml configuration file
     * @return
     */
    virtual XPCFErrorCode configure(const char * filepath) = 0;

    /**
     * Configure the component from the given configuration declared in the node @em xpath from xml file @em filepath
     * @param [in] filepath the xml configuration file
     * @param [in] xpath the xpath to seach for component configuration
     * @return
     */
    virtual XPCFErrorCode configure(const char * filepath, const char * xpath) = 0;

    /**
     *
     * @param [in] filepath
     * @param [in] mode append, create
     * @return
     */
    virtual XPCFErrorCode serialize(const char * filepath, uint32_t mode) = 0;

    /**
     * Checks if the root property node contains properties
     * @return true when the root property node contains properties, false otherwise
     */

    virtual bool hasProperties() const = 0;

    /**
     * Get a property from its name. The property is searched in the property root node.
     * Properties contained in sub-property node are not found with this method (i.e. a property map itself contained in the property root node).
     * Sub-property node properties must be searched from the sub-property node using the @em at() method on this node.
     *
     * @param [in] the property name
     * @return the corresponding property
     */
    virtual SRef<IProperty> getProperty(const char * name) const = 0;

    /**
     * Retrieves the properties root node (the IPropertyMap properties entry point).
     *
     * @return the property root node
     */
    virtual SRef<IPropertyMap> getPropertyRootNode() const = 0;

    /**
     * Retrieves the properties collection contained in the propertyRootNode.
     *
     * @return the properties' collection
     */
    virtual const IEnumerable<SRef<IProperty>> & getProperties() const = 0;

};

template <> struct InterfaceTraits<IConfigurable>
{
    static constexpr const char * UUID = "98DBA14F-6EF9-462E-A387-34756B4CBA80";
    static constexpr const char * DESCRIPTION = "XPCF::IConfigurable interface";
};

}}}

#endif // ICLONEABLE_H
