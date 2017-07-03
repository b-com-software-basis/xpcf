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

#ifndef C0_h
#define C0_h 1

// I0
#include "I0.h"
// I1
#include "I1.h"
#include "I2.h"
// XPCF_ComponentBase
#include "ComponentBase.h"

class C0 : public org::bcom::xpcf::ComponentBase,
           public I0,
//           public I1,
           public I2
{
 public:
      C0 ();
      ~C0();
      void unloadComponent () override final;
      void I0_opname () override;
      void I0_opname2 () override;
      void I0_opname3 () override;
      void I0_opname4 () override;
      void I0_opname5 () override;

      void I1_opname () override;
      void I1_opname2 () override;
      void I1_opname3 () override;
      void I1_opname4 () override;
      void I1_opname5 () override;

      void I2_opname () override {};

      void I2_opname2 () override {};

      static constexpr const char * UUID = "ae9d10bb-4bd1-462b-8c01-43f1815c6ae0";

    // Data Members for Class Attributes
};

#endif
