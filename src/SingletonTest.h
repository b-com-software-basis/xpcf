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

#ifndef SINGLETONTEST_H
#define SINGLETONTEST_H
#include <string>
#include <vector>
#include <tuple>

class SingletonTest
{
public:
    static SingletonTest & instance();
    void initialize(const std::string & configFilePath);
    void release();
    bool isAvailable();

private:
    SingletonTest();
    ~SingletonTest();
    SingletonTest(const SingletonTest&);
    SingletonTest operator =(SingletonTest const &);
    static SingletonTest m_instance;
};

#endif // DATASERVICE_H

