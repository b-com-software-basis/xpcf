/**
 * @copyright Copyright (c) 2015 All Right Reserved, B-com http://www.b-com.com/
 *
 * This file is subject to the B<>Com License.
 * All other rights reserved.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * @author Loïc Touraine
 *
 * @file
 * @brief description of file
 * @date 2015-09-18
 */

#include "IComponentManager.h"
#include "ComponentLibrary.h"

namespace org {
namespace bcom {
namespace xpcf {

ComponentManager::ComponentManager()
{
  m_pRegistry = 0L;
}

ComponentManager::~ComponentManager()
{
  if (m_pRegistry != 0L) {
    delete m_pRegistry;
  }
}

unsigned long ComponentManager::Load()
{
  initRegistry();
  return m_pRegistry->load();
}
 
bool ComponentManager::IsLoaded()
{
  initRegistry();
  return m_pRegistry->isLoaded();
}

template <class T>
unsigned long ComponentManager::CreateInstance(boost::uuids::uuid& rclsid,boost::uuids::uuid& riid, SRef<T>& aRef)
{
  initRegistry();
  return m_pRegistry->createComponent<T>(rclsid,riid,aRef);
}
 
/*unsigned long XPCF_IComponentManager::GetContainerObject(boost::uuids::uuid& rclsid,boost::uuids::uuidList& aList)
{ 
  initRegistry();
  return m_pRegistry->GetContainerObject(rclsid,aList);
}

unsigned long XPCF_IComponentManager::GetContainerObject(const char* libPath ,boost::uuids::uuidList& aList)
{
  initRegistry();
  return m_pRegistry->GetContainerObject(libPath,aList);
}
*/
void ComponentManager::initRegistry()
{
  if (m_pRegistry == 0L) {
    m_pRegistry = new ComponentLibrary();
  }
}

}}} //namespace org::bcom::xpcf
