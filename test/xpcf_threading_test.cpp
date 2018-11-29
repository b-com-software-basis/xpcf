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
 * @date 2018-07-02
 */

#include <stdio.h>
#include <iostream>
#include "xpcf/threading/FiberFifos.h"
#include "xpcf/threading/SharedBuffer.h"
#include "xpcf/threading/BaseTask.h"
#include <boost/test/unit_test.hpp>

#include "TestDefines.h"

using namespace std;

BOOST_AUTO_TEST_SUITE( test_threading )

BOOST_AUTO_TEST_CASE( test_threading_std)
{
    SRef<xpcf::IFifo<int>> inFifo = xpcf::createFifo<xpcf::SharedFifo,int>();
    SRef<xpcf::IFifo<std::string>> inoutFifo = xpcf::createFifo<xpcf::SharedFifo,std::string>();
    SRef<xpcf::IFifo<std::string>> out2Fifo = xpcf::createFifo<xpcf::SharedFifo,std::string>();
    xpcf::SharedBuffer<int> buffer(4);
    std::function<void(void)> providerFunc = [inFifo](){
        inFifo->push(2);
        //std::this_thread::sleep_for(std::chrono::milliseconds(50));
        //xpcf::utils::this_fiber::sleep_for(std::chrono::milliseconds(50));
        inFifo->push(3);
    };

    std::function<void(void)> func = [inFifo,inoutFifo](){
        int valueIn = inFifo->pop();
        valueIn += 2;
        inoutFifo->push(to_string(valueIn));
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //xpcf::utils::this_fiber::sleep_for(std::chrono::milliseconds(1000));
    };

    std::function<void(void)> func2 = [inoutFifo,out2Fifo](){
        string valueIn;
        bool popped= inoutFifo->pop(valueIn,std::chrono::milliseconds(100));
        //valueIn += 2;
        if (popped) {
            BOOST_TEST_MESSAGE("THREADING OK. Out2Fifo element is:"<<valueIn);
            out2Fifo->push(valueIn);
        }
        else {
            BOOST_TEST_MESSAGE("Timeout expired");
        }
    };

    std::function<void(void)> consumerFunc = [out2Fifo](){
        BOOST_TEST_MESSAGE("THREADING OK. OutFifo first element is:"<<out2Fifo->pop());
        BOOST_TEST_MESSAGE("THREADING OK. OutFifo second element is:"<<out2Fifo->pop());
    };
#define TASKTYPE xpcf::DelegateTask
    SRef<xpcf::ITask> task = xpcf::createDelegateTask<TASKTYPE>(func);
    SRef<xpcf::ITask> task2 = xpcf::createDelegateTask<TASKTYPE>(func2);
    SRef<xpcf::ITask> providerTask = xpcf::createDelegateTask<TASKTYPE>(providerFunc);
    SRef<xpcf::ITask> consumerTask = xpcf::createDelegateTask<TASKTYPE>(consumerFunc);
    consumerTask->start();
    task2->start();
    task->start();
    providerTask->start();
    consumerTask->awaitStart();
    //std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    consumerTask->stop();
    task2->stop();
    task->stop();
    providerTask->stop();
}

BOOST_AUTO_TEST_CASE( test_threading_fibers)
{
    SRef<xpcf::IFifo<int>> inFifo = xpcf::createFifo<xpcf::SharedFifo,int,xpcf::FiberThreadedNamespace>();
    SRef<xpcf::IFifo<std::string>> inoutFifo = xpcf::createFifo<xpcf::SharedFifo,std::string,xpcf::FiberThreadedNamespace>();
    SRef<xpcf::IFifo<std::string>> out2Fifo = xpcf::createFifo<xpcf::SharedFifo,std::string,xpcf::FiberThreadedNamespace>();

    std::function<void(void)> func = [inFifo,inoutFifo](){
        int valueIn = inFifo->pop();
        valueIn += 2;
        inoutFifo->push(to_string(valueIn));
        xpcf::utils::this_fiber::sleep_for(std::chrono::milliseconds(1000));
    };
    std::function<void(void)> func2 = [inoutFifo,out2Fifo](){
        string valueIn;
        bool popped= inoutFifo->pop(valueIn,std::chrono::milliseconds(100));
        //valueIn += 2;
        if (popped) {
            BOOST_TEST_MESSAGE("THREADING FIBER OK. Out2Fifo element is:"<<valueIn);
            out2Fifo->push(valueIn);
        }
        else {
            BOOST_TEST_MESSAGE("Timeout expired");
        }
    };
    std::function<void(void)> providerFunc = [inFifo](){
        inFifo->push(2);
        xpcf::utils::this_fiber::sleep_for(std::chrono::milliseconds(50));
        inFifo->push(3);
    };
    std::function<void(void)> consumerFunc = [out2Fifo](){
        BOOST_TEST_MESSAGE("THREADING FIBER OK. OutFifo first element is:"<<out2Fifo->pop());
        BOOST_TEST_MESSAGE("THREADING FIBER OK. OutFifo second element is:"<<out2Fifo->pop());
    };
    xpcf::FiberTask task(func);
    xpcf::FiberTask task2(func2);
    xpcf::FiberTask providerTask(providerFunc);
    xpcf::FiberTask consumerTask(consumerFunc);
    consumerTask.start();
    task2.start();
    task.start();
    providerTask.start();
    consumerTask.awaitStart();
    //std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    consumerTask.stop();
    task2.stop();
    task.stop();
    providerTask.stop();
}

BOOST_AUTO_TEST_SUITE_END()


