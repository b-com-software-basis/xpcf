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

#include <stdio.h>
#include "C0.h"
#include "InterfaceMetadata.h"

template<> XPCF_EXPORT_API C0* org::bcom::xpcf::ComponentFactory::createInstance() {
    return new C0();
}


constexpr const char * C0::UUID;

C0::C0 ()
{
   setUUID(C0::UUID);
   addInterface<I0>(this,I0::UUID, "interface I0");
   addInterface<I1>(this,I1::UUID, "interface I1");
   addInterface<I2>(this,I2::UUID, "interface I2");
   printf("Constructor C0::C0 () called!\n");

}

C0::~C0 ()
{
    printf("Destructor C0::C0 () called!\n");
}

void C0::unloadComponent ()
{
  printf("C0::unload () called!\n");

  delete this;
  return;
}

void C0::I0_opname ()
{

	printf(" C0::I0_opname ()  called!\n");

}

void C0::I0_opname2 ()
{

	printf(" C0::I0_opname2 ()  called!\n");

}

void C0::I0_opname3 ()
{

	printf(" C0::I0_opname3 ()  called!\n");


}

void C0::I0_opname4 ()
{


	printf(" C0::I0_opname4 ()  called!\n");


}

void C0::I0_opname5 ()
{

	printf(" C0::I0_opname5 ()  called!\n");

}

void C0::I1_opname ()
{

	printf(" C0::I1_opname ()  called!\n");

}

void C0::I1_opname2 ()
{

	printf(" C0::I1_opname2 ()  called!\n");

}

void C0::I1_opname3 ()
{

	printf(" C0::I1_opname3 ()  called!\n");

}

void C0::I1_opname4 ()
{

	printf(" C0::I1_opname4 ()  called!\n");

}

void C0::I1_opname5 ()
{

	printf(" C0::I1_opname5 ()  called!\n");

}
