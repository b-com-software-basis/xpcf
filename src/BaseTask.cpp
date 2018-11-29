#include "xpcf/threading/BaseTask.h"

#include "boost/fiber/fiber.hpp"
#include "boost/fiber/operations.hpp"
#include "boost/fiber/future.hpp"

using namespace std;
namespace bfibers = boost::fibers;

namespace org { namespace bcom { namespace xpcf {

void BaseTask::start()
{
    m_stop = false;
    m_thread = thread([this] { this->process(); });
}

void BaseTask::stop()
{
    m_stop = true;
    m_thread.join();
}


void BaseTask::awaitStart()
{
     std::future<void> startedFuture = m_startedPromise.get_future();
     startedFuture.wait();
}

void BaseTask::signalStarted()
{
    m_startedPromise.set_value();
}

void BaseTask::yield()
{
    std::this_thread::yield();
}

void DelegateTask::yield()
{
    BaseTask::yield();
}

void AbstractDelegateTask::process()
{   signalStarted();
    while(!stopped()) {
        m_processFunc();
    }
}

class FiberTask::FiberTaskImpl {
public:
    bfibers::fiber m_fiber;
    bfibers::promise<void> m_startedPromise;
};

FiberTask::FiberTask(std::function<void(void)> processingFunction):AbstractDelegateTask(processingFunction),m_pimpl(new FiberTaskImpl())
{
}

FiberTask::~FiberTask()
{
}

void FiberTask::start()
{
    m_stop = false;
    m_pimpl->m_fiber = bfibers::fiber([this] { this->process(); });
}

void FiberTask::stop()
{
    m_stop = true;
    m_pimpl->m_fiber.join();
}

void FiberTask::signalStarted()
{
    m_pimpl->m_startedPromise.set_value();
}

void FiberTask::awaitStart()
{
     bfibers::future<void> startedFuture = m_pimpl->m_startedPromise.get_future();
     startedFuture.wait();
}

void FiberTask::yield()
{
    boost::this_fiber::yield();
}

}}}
