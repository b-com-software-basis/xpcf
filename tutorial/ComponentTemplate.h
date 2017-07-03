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

#ifndef __ComponentTemplate_h__
#define __ComponentTemplate_h__


#include "ITemplateInterface.h"
#include "ComponentBase.h"
namespace client {
class ComponentTemplate : public org::bcom::xpcf::ComponentBase,
           public ITemplateInterface {
 public:
      ComponentTemplate ();
      ~ComponentTemplate();

      // This method must always be overrided and implements the adequate component cleanup
      // It is the component responsibility to handle its unload strategy, accordingly with the creation pattern chosen in ComponentFactory::createInstance specialization
      // A common implementation is provided with the helper macro XPCF_DEFINE_FACTORY_CREATE_INSTANCE
      void unloadComponent () override final;

      // Implement methods of inherited interfaces here
      // ...
      //

      // a new UUID for each component must be created !
      XPCF_DECLARE_UUID("40F7D2F4-523E-4497-A848-9A6F17456019");
};
}

#endif
