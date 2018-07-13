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
#include "C0Bis.h"
#include "InterfaceMetadata.h"
#include <iostream>

template<> XPCF_EXPORT_API C0* org::bcom::xpcf::ComponentFactory::createInstance() {
    return new C0();
}

C0::C0 ():m_name("C0Bis")
{
    setMetadata<C0>();
    addInterface<I1>(this);
    addInterface<I2>(this);
    std::cout<<m_name<<"::"<<"Constructor C0::C0 () called!"<<std::endl;
}

C0::~C0 ()
{
    std::cout<<m_name<<"::"<<"Destructor C0::C0 () called!"<<std::endl;
}

void C0::unloadComponent ()
{
    std::cout<<m_name<<"::"<<"C0::unload () called!"<<std::endl;

    delete this;
    return;
}

void C0::I1_opname ()
{

    std::cout<<m_name<<"::"<<" C0::I1_opname ()  called!"<<std::endl;

}

void C0::I1_opname2 ()
{

    std::cout<<m_name<<"::"<<" C0::I1_opname2 ()  called!"<<std::endl;

}

void C0::I1_opname3 ()
{

    std::cout<<m_name<<"::"<<" C0::I1_opname3 ()  called!"<<std::endl;

}

void C0::I1_opname4 ()
{

    std::cout<<m_name<<"::"<<" C0::I1_opname4 ()  called!"<<std::endl;

}

void C0::I1_opname5 ()
{

    std::cout<<m_name<<"::"<<" C0::I1_opname5 ()  called!"<<std::endl;
}

void C0::I2_opname ()
{

    std::cout<<m_name<<"::"<<" C0::I2_opname ()  called!"<<std::endl;

}

void C0::I2_opname2 ()
{

    std::cout<<m_name<<"::"<<" C0::I2_opname2 ()  called!"<<std::endl;

}
