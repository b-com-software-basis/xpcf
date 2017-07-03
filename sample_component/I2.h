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

#ifndef I2_H
#define I2_H
#include "I1.h"

class I2 : public I1
{
public:
    virtual ~I2() {}
    virtual void I2_opname () = 0;

    virtual void I2_opname2 () = 0;
    static constexpr const char * UUID = "41348765-1017-47a7-ab9F-6b59d39e4b95";
};


#endif // I2_H
