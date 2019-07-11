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

#ifndef C0Bis_H
#define C0Bis_H 1

// I1
#include "I1.h"
#include "I2.h"
// XPCF_ComponentBase
#include "ComponentBase.h"

#include <string>

class C0 : public org::bcom::xpcf::ComponentBase,
           public I2
{
 public:
      C0 ();
      ~C0();
      void unloadComponent () override final;

      void I1_opname () override;
      void I1_opname2 () override;
      void I1_opname3 () override;
      void I1_opname4 () override;
      void I1_opname5 () override;

      void I2_opname () override;

      void I2_opname2 () override;



private:
      std::string m_name;
    // Data Members for Class Attributes
};

template <> struct org::bcom::xpcf::ComponentTraits<C0>
{
    static constexpr const char * UUID = "6B8F2FAD-5DFD-4390-B12E-B600C9FC5B12";
    static constexpr const char * DESCRIPTION = "C0 component";
};


#endif
