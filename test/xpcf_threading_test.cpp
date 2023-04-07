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
#include "xpcf/threading/DropBuffer.h"
#include "xpcf/threading/MultiConsumer.h"
#include "xpcf/threading/BaseTask.h"
#include <boost/test/unit_test.hpp>

#include "TestDefines.h"

using namespace std;

BOOST_AUTO_TEST_SUITE( test_threading )


BOOST_AUTO_TEST_CASE( test_fifo_instantitation)
{
    SRef<xpcf::IFifo<int>> intBuffer = xpcf::createSharedBuffer<int>(10,5);
    SRef<xpcf::IFifo<int*>> intPointerFifo = xpcf::createFifo<xpcf::SharedFifo,int*>();
    SRef<xpcf::IFifo<std::string>> stringFifo = xpcf::createFifo<xpcf::SharedFifo,std::string>();
    SRef<xpcf::IFifo<SRef<std::string>>> srefStringDropBuffer = xpcf::createDropBuffer<SRef<std::string>>();
    intBuffer->clear();
    intBuffer->push(4);
    intBuffer->push(5);
    intBuffer->clear();
    intBuffer->clear();
    int * i = new int;
    *i = 10;
    intPointerFifo->push(i);
    intPointerFifo->clear();
    stringFifo->clear();
    stringFifo->push("test");
    stringFifo->push("test2");
    stringFifo->clear();
    stringFifo->clear();
    SRef<string> str = xpcf::utils::make_shared<string>("srefString");
    srefStringDropBuffer->push(str);
    srefStringDropBuffer->clear();
}

/*
BOOST_AUTO_TEST_CASE( test_threading_std)
{
    SRef<xpcf::IFifo<int>> inFifo = xpcf::createFifo<xpcf::SharedFifo,int>();
    SRef<xpcf::IFifo<std::string>> inoutFifo = xpcf::createFifo<xpcf::SharedFifo,std::string>();
    SRef<xpcf::IFifo<std::string>> out2Fifo = xpcf::createFifo<xpcf::SharedFifo,std::string>();
    SRef<xpcf::IFifo<int>> buffer = xpcf::createFifo<xpcf::SharedBuffer,int>(4);

    std::function<void(void)> providerFunc = [buffer](){
        try {
            buffer->push(2);
            //std::this_thread::sleep_for(std::chrono::milliseconds(50));
            //xpcf::utils::this_fiber::sleep_for(std::chrono::milliseconds(50));
            buffer->push(3);
        }
        catch (xpcf::Exception & e) {
            BOOST_TEST_MESSAGE("Provider::queue released !!!");
            return;
        }
    };

    std::function<void(void)> func = [buffer,inoutFifo](){
        try {
            int valueIn = buffer->pop();
            valueIn += 2;
            inoutFifo->push(to_string(valueIn));
        }
        catch (xpcf::Exception & e) {
            BOOST_TEST_MESSAGE("Func::queue released !!!");
            return;
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //xpcf::utils::this_fiber::sleep_for(std::chrono::milliseconds(1000));
    };

    std::function<void(void)> func2 = [inoutFifo,out2Fifo](){
        string valueIn;
        bool popped= inoutFifo->pop(valueIn,std::chrono::milliseconds(10000));
        //valueIn += 2;
        if (popped) {
            BOOST_TEST_MESSAGE("THREADING OK. Out2Fifo element is:"<<valueIn);
            out2Fifo->push(valueIn);
        }
        else {
            BOOST_TEST_MESSAGE("Func2::Timeout expired or queue released");
            return;
        }
    };

    std::function<void(void)> consumerFunc = [out2Fifo](){
        std::string result;
        try {
            result = out2Fifo->pop();
            BOOST_TEST_MESSAGE("THREADING OK. OutFifo first element is:"<<result);
            result = out2Fifo->pop();
            BOOST_TEST_MESSAGE("THREADING OK. OutFifo first element is:"<<result);
        }
        catch (xpcf::Exception & e) {
            BOOST_TEST_MESSAGE("Consumer::queue released !!!");
            return;
        }
    };

#define TASKTYPE xpcf::DelegateTask
    bool joinable = true;
    SRef<xpcf::ITask> task = xpcf::createDelegateTask<TASKTYPE>(func,joinable);
    task->declareFinalizer(buffer);
    SRef<xpcf::ITask> task2 = xpcf::createDelegateTask<TASKTYPE>(func2,joinable);
    task2->declareFinalizer(inoutFifo);
    SRef<xpcf::ITask> providerTask = xpcf::createDelegateTask<TASKTYPE>(providerFunc,joinable);
    providerTask->declareFinalizer(buffer);
    SRef<xpcf::ITask> consumerTask = xpcf::createDelegateTask<TASKTYPE>(consumerFunc,joinable);
    consumerTask->declareFinalizer(out2Fifo);
    consumerTask->start();
    task2->start();
    task->start();
    providerTask->start();
    consumerTask->awaitRunning();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    providerTask->stop();
    BOOST_TEST_MESSAGE("providerTask::stop done !!!");
    task->stop();
    BOOST_TEST_MESSAGE("task::stop done !!!");
    task2->stop();
    BOOST_TEST_MESSAGE("task2::stop done !!!");
    consumerTask->stop();
    BOOST_TEST_MESSAGE("consumerTask::stop done !!!");




}

BOOST_AUTO_TEST_CASE( test_threading_multiconsumer)
{
    SRef<xpcf::IFifo<int>> inFifo = xpcf::createFifo<xpcf::SharedFifo,int>();
    SRef<xpcf::IFifo<std::string>> inoutFifo = xpcf::createFifo<xpcf::SharedFifo,std::string>();
    SRef<xpcf::IFifo<std::string>> out2Fifo = xpcf::createFifo<xpcf::SharedFifo,std::string>();
    SRef<xpcf::IFifo<int>> buffer = xpcf::createFifo<xpcf::SharedBuffer,int>(4);
    SRef<xpcf::MultiConsumer<std::string>> multi = xpcf::utils::make_shared<xpcf::MultiConsumer<std::string>>(3,inoutFifo);
    uint64_t consumer1,consumer2,consumer3;
    consumer1 = multi->subscribe();
    consumer2 = multi->subscribe();
    consumer3 = multi->subscribe();

    std::function<void(void)> providerFunc = [buffer](){
        static uint64_t currentIndex = 0;
        try {
            buffer->push(currentIndex++);
            //std::this_thread::sleep_for(std::chrono::milliseconds(50));
            #ifdef XPCF_USE_BOOST
            //xpcf::utils::this_fiber::sleep_for(std::chrono::milliseconds(50));
            #endif
           // BOOST_TEST_MESSAGE("Provider: values pushed !!!");
        }
        catch (xpcf::Exception & e) {
            BOOST_TEST_MESSAGE("Provider::queue released !!!");
            return;
        }
    };

    std::function<void(void)> func = [buffer,inoutFifo](){
        try {
            int valueIn = buffer->pop();
            valueIn += 2;
            inoutFifo->push(to_string(valueIn));
           // BOOST_TEST_MESSAGE("func: values incremented and converted to string : pushed !!!");
        }
        catch (xpcf::Exception & e) {
            BOOST_TEST_MESSAGE("Func::queue released !!!");
            return;
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        #ifdef XPCF_USE_BOOST
        //xpcf::utils::this_fiber::sleep_for(std::chrono::milliseconds(1000));
        #endif
    };

    std::function<void(void)> consumerFunc1 = [&multi, &consumer1,&out2Fifo](){
        try {
            string popped= multi->pop(consumer1);
            out2Fifo->push(popped);
            //BOOST_TEST_MESSAGE("CONSUMER1 inoutFifo element is:"<<popped);

        }
        catch (xpcf::Exception & e) {
            BOOST_TEST_MESSAGE("CONSUMER1::queue released !!!");
            return;
        }
    };

    std::function<void(void)> consumerFunc2 = [&multi, &consumer2,&out2Fifo](){
        try {
            string popped= multi->pop(consumer2);
            out2Fifo->push(popped);
//            BOOST_TEST_MESSAGE("CONSUMER2 inoutFifo element is:"<<popped);
        }
        catch (xpcf::Exception & e) {
            BOOST_TEST_MESSAGE("CONSUMER2::queue released !!!");
            return;
        }

    };

    std::function<void(void)> consumerFunc3 = [&multi, &consumer3, &out2Fifo](){
        try {
            string popped= multi->pop(consumer3);
            out2Fifo->push(popped);

//            BOOST_TEST_MESSAGE("CONSUMER3 inoutFifo element is:"<<popped);
        }
        catch (xpcf::Exception & e) {
            BOOST_TEST_MESSAGE("CONSUMER3::queue released !!!");
            return;
        }
    };

#define TASKTYPE xpcf::DelegateTask
    bool joinable = true;
    SRef<xpcf::ITask> task = xpcf::createDelegateTask<TASKTYPE>(func,joinable);
    task->declareFinalizer(buffer);
    SRef<xpcf::ITask> providerTask = xpcf::createDelegateTask<TASKTYPE>(providerFunc,joinable);
    providerTask->declareFinalizer(buffer);
    SRef<xpcf::ITask> consumerTask1 = xpcf::createDelegateTask<TASKTYPE>(consumerFunc1,joinable);
    consumerTask1->declareFinalizer(multi);
    SRef<xpcf::ITask> consumerTask2 = xpcf::createDelegateTask<TASKTYPE>(consumerFunc2,joinable);
    consumerTask2->declareFinalizer(multi);
    SRef<xpcf::ITask> consumerTask3 = xpcf::createDelegateTask<TASKTYPE>(consumerFunc3,joinable);
    consumerTask3->declareFinalizer(multi);
    consumerTask3->start();
    consumerTask2->start();
    consumerTask1->start();
    task->start();
    providerTask->start();
    consumerTask1->awaitRunning();
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    providerTask->stop();
    BOOST_TEST_MESSAGE("providerTask::stop done !!!");
    task->stop();
    BOOST_TEST_MESSAGE("task::stop done !!!");
    consumerTask1->stop();
    BOOST_TEST_MESSAGE("consumerTask1::stop done !!!");
    consumerTask2->stop();
    BOOST_TEST_MESSAGE("consumerTask2::stop done !!!");
    consumerTask3->stop();
    BOOST_TEST_MESSAGE("consumerTask3::stop done !!!");
    string value;
    string values;
    while (out2Fifo->tryPop(value) ) {
        values += "|";
        values += value;
    }
    BOOST_TEST_MESSAGE("MultiConsumer::out2Fifo values: "<<values);
}

#ifdef XPCF_USE_BOOST
BOOST_AUTO_TEST_CASE( test_threading_fibers, * boost::unit_test::disabled())
{
    SRef<xpcf::IFifo<int>> inFifo = xpcf::createFifo<xpcf::SharedFifo,int,xpcf::FiberThreadedNamespace>();
    SRef<xpcf::IFifo<std::string>> inoutFifo = xpcf::createFifo<xpcf::SharedFifo,std::string,xpcf::FiberThreadedNamespace>();
    SRef<xpcf::IFifo<std::string>> out2Fifo = xpcf::createFifo<xpcf::SharedFifo,std::string,xpcf::FiberThreadedNamespace>();
    SRef<xpcf::IFifo<int>> buffer = xpcf::createFifo<xpcf::SharedBuffer,int,xpcf::FiberThreadedNamespace>(4);

    std::function<void(void)> providerFunc = [buffer](){
        try {
            buffer->push(2);
            //xpcf::utils::this_fiber::sleep_for(std::chrono::milliseconds(50));
            buffer->push(3);
            BOOST_TEST_MESSAGE("THREADING FIBER::PROVIDER. buffer values pushed: 2 3");
        }
        catch (xpcf::Exception & e) {
            BOOST_TEST_MESSAGE("THREADING FIBER::PROVIDER buffer released");
            return;
        }
    };

    std::function<void(void)> func = [buffer,inoutFifo](){
        try {
            int valueIn = buffer->pop();
            valueIn += 2;
            inoutFifo->push(to_string(valueIn));
            BOOST_TEST_MESSAGE("THREADING FIBER::CONVERTER. inoutFifo value pushed:"<<to_string(valueIn));
            //xpcf::utils::this_fiber::sleep_for(std::chrono::milliseconds(1000));
        }
        catch (xpcf::Exception & e) {
            return;
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //xpcf::utils::this_fiber::sleep_for(std::chrono::milliseconds(1000));
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
            BOOST_TEST_MESSAGE("Timeout expired or queue released");
            return;
        }
    };

    std::function<void(void)> consumerFunc = [out2Fifo](){
        std::string result;
        try {
            result = out2Fifo->pop();
            BOOST_TEST_MESSAGE("THREADING FIBER OK. OutFifo first element is:"<<result);
            result = out2Fifo->pop();
            BOOST_TEST_MESSAGE("THREADING FIBER OK. OutFifo first element is:"<<result);
        }
        catch (xpcf::Exception & e) {
            return;
        }
    };

    std::function<void(void)> fiberLauncher = [&]() {

        xpcf::FiberTask task(func);
        xpcf::FiberTask task2(func2);
        xpcf::FiberTask providerTask(providerFunc);
        xpcf::FiberTask consumerTask(consumerFunc);
        task.declareFinalizer(buffer);
        task2.declareFinalizer(inoutFifo);
        consumerTask.declareFinalizer(out2Fifo);
        consumerTask.start();
        task2.start();
        task.start();
        providerTask.start();
        xpcf::FiberTask::yield();
        consumerTask.awaitRunning();
        //std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    BOOST_TEST_MESSAGE("---- CONSUMERTASK::STOP ----");
        consumerTask.stop();
        BOOST_TEST_MESSAGE("---- CONSUMERTASK::STOPPED ----");
        BOOST_TEST_MESSAGE("---- TASK2::STOP ----");
        task2.stop();
        BOOST_TEST_MESSAGE("---- TASK2::STOPPED ----");
        BOOST_TEST_MESSAGE("---- TASK::STOP ----");
        task.stop();
        BOOST_TEST_MESSAGE("---- TASK::STOPPED ----");
        BOOST_TEST_MESSAGE("---- PROVIDERTASK::STOP ----");
        providerTask.stop();
        BOOST_TEST_MESSAGE("---- PROVIDERTASK::STOPPED ----");
    };

    SRef<xpcf::ITask> threadTask = xpcf::createDelegateTask<xpcf::DelegateTask>(fiberLauncher,true);
    threadTask->start();
    threadTask->awaitRunning();
     std::this_thread::sleep_for(std::chrono::milliseconds(5000));
     threadTask->stop();

}
#endif


*/
BOOST_AUTO_TEST_SUITE_END()
