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
 * @date 2017-04-28
 */

#ifndef ORG_BCOM_XPCF_ConfigurableBase_H
#define ORG_BCOM_XPCF_ConfigurableBase_H

#include "xpcf/api/IConfigurable.h"
#include "xpcf/component/ComponentBase.h"

namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API ConfigurableBase : public ComponentBase,
        virtual public IConfigurable
{
public:
    ConfigurableBase() = delete;
    ConfigurableBase(const uuids::uuid & uuid, const SRef<IPropertyMap> configuration = nullptr);
    ConfigurableBase(std::map<std::string,std::string> componentTrait, const SRef<IPropertyMap> configuration = nullptr);
    virtual ~ConfigurableBase() override;

    // It is the component that decides if it must or must not unload when all
    // refs are out
    virtual void unloadComponent() override = 0;

    // IConfigurable
    XPCFErrorCode configure(const char * filepath) final;
    XPCFErrorCode configure(const char * filepath, const char * xpath) final;
    inline virtual XPCFErrorCode onConfigured() override { return XPCFErrorCode::_SUCCESS; }
    XPCFErrorCode serialize(const char * filepath, uint32_t mode) final;
    SRef<IPropertyMap> getPropertyRootNode() const final;
    SRef<IProperty> getProperty(const char * name) const final;
    IEnumerable<SRef<IProperty>> & getProperties() const final;
    SRef<IEnumerator<SRef<IProperty>>> getPropertiesEnumerator() const final;
    bool hasProperties() const final;

protected:
    XPCFErrorCode addProperty(SRef<IProperty> property) override final;
    void declarePropertyNode(const char * name, SRef<IPropertyMap> & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT) override final;
    template<typename T>
    XPCFErrorCode declareProperty(const char * name, T & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT);

    template <template<typename, typename> class Sequence, typename T>
    XPCFErrorCode declarePropertySequence(const char * name, Sequence<T, ::std::allocator<T>> & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT);

    template<typename T>
    XPCFErrorCode declareProperty(SRef<IPropertyMap> node, const char * name, T & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT);

    template <template<typename, typename> class Sequence, typename T>
    XPCFErrorCode declarePropertySequence(SRef<IPropertyMap> node, const char * name, Sequence<T, ::std::allocator<T>> & wrappedValue, IProperty::AccessSpecifier iotype = IProperty::AccessSpecifier::IProperty_INOUT);


private:
    class InternalImpl;
    UniqueRef<InternalImpl> m_internalImpl;
};

extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<uint32_t>(const char * name, uint32_t & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<int32_t>(const char * name, int32_t & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<uint64_t>(const char * name, uint64_t & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<int64_t>(const char * name, int64_t & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<float>(const char * name, float & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<double>(const char * name, double & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<std::string>(const char * name, std::string & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<std::wstring>(const char * name, std::wstring & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector, uint32_t>(const char * name, std::vector<uint32_t> & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,int32_t>(const char * name, std::vector<int32_t> & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,uint64_t>(const char * name, std::vector<uint64_t> & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,int64_t>(const char * name, std::vector<int64_t> & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,float>(const char * name, std::vector<float> & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,double>(const char * name, std::vector<double> & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,std::string>(const char * name, std::vector<std::string> & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,std::wstring>(const char * name, std::vector<std::wstring> & wrappedValue, IProperty::AccessSpecifier iotype);

extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<uint32_t>(SRef<IPropertyMap> node, const char * name, uint32_t & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<int32_t>(SRef<IPropertyMap> node, const char * name, int32_t & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<uint64_t>(SRef<IPropertyMap> node, const char * name, uint64_t & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<int64_t>(SRef<IPropertyMap> node, const char * name, int64_t & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<float>(SRef<IPropertyMap> node, const char * name, float & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<double>(SRef<IPropertyMap> node, const char * name, double & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<std::string>(SRef<IPropertyMap> node, const char * name, std::string & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declareProperty<std::wstring>(SRef<IPropertyMap> node, const char * name, std::wstring & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector, uint32_t>(SRef<IPropertyMap> node, const char * name, std::vector<uint32_t> & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,int32_t>(SRef<IPropertyMap> node, const char * name, std::vector<int32_t> & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,uint64_t>(SRef<IPropertyMap> node, const char * name, std::vector<uint64_t> & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,int64_t>(SRef<IPropertyMap> node, const char * name, std::vector<int64_t> & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,float>(SRef<IPropertyMap> node, const char * name, std::vector<float> & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,double>(SRef<IPropertyMap> node, const char * name, std::vector<double> & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,std::string>(SRef<IPropertyMap> node, const char * name, std::vector<std::string> & wrappedValue, IProperty::AccessSpecifier iotype);
extern template XPCF_EXPORT_API XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,std::wstring>(SRef<IPropertyMap> node, const char * name, std::vector<std::wstring> & wrappedValue, IProperty::AccessSpecifier iotype);


}}} //namespace org::bcom::xpcf

#endif
