#include "xpcf/threading/BaseTask.h"

#ifdef XPCF_USE_BOOST
#include "boost/fiber/fiber.hpp"
#include "boost/fiber/operations.hpp"
#include "boost/fiber/future.hpp"

namespace bfibers = boost::fibers;
#endif

using namespace std;
namespace org { namespace bcom { namespace xpcf {


void AbstractTask::cleanup()
{
    for (SRef<IFinalize> & finalizer:m_finalizers) {
        finalizer->release();
    }
}

void AbstractTask::declareFinalizer(const SRef<IFinalize> & fifo)
{
    m_finalizers.push_back(fifo);
}

BaseTask::~BaseTask()
{
   stop();
}

void BaseTask::start()
{
    m_stop = false;
    m_thread = thread([this] { this->process(); });
    if (!m_joinable) {
        m_thread.detach();
    }
}

void BaseTask::stop()
{
    m_stop = true;
    cleanup();
    if (m_joinable && m_thread.joinable()) {
        m_thread.join();
    }
}

void BaseTask::awaitRunning()
{
    if (!m_stop) {
        std::future<void> startedFuture = m_startedPromise.get_future();
        startedFuture.wait();
    }
}

void BaseTask::signalRunning()
{
    m_startedPromise.set_value();
}

void BaseTask::yield()
{
    std::this_thread::yield();
}

void AbstractDelegateTask::process()
{
    signalRunning();
    while(!stopped()) {
        m_processFunc();
    }
}

void DelegateTask::start()
{
    m_stop = false;
    m_startedPromise = std::promise<void>();
    m_startedFuture = m_startedPromise.get_future();
    m_thread = thread([this] { this->process(); });
    if (!m_joinable) {
        m_thread.detach();
    }
}

void DelegateTask::stop()
{
    m_stop = true;
    cleanup();
    if (m_joinable && m_thread.joinable()) {
        m_thread.join();
    }
}


void DelegateTask::awaitRunning()
{
    if (!m_stop) {
        m_startedFuture.wait();
    }
}

void DelegateTask::signalRunning()
{
    m_startedPromise.set_value();
}

void DelegateTask::yield()
{
    BaseTask::yield();
}

#ifdef XPCF_USE_BOOST

class FiberTask::FiberTaskImpl {
public:
    bfibers::fiber m_fiber;
    bfibers::promise<void> m_startedPromise;
    bfibers::future<void> m_startedFuture;
};

FiberTask::FiberTask(std::function<void(void)> processingFunction, bool joinable):
    AbstractDelegateTask(processingFunction, joinable),m_pimpl(new FiberTaskImpl())
{
}

FiberTask::~FiberTask()
{
}

void FiberTask::start()
{
    m_stop = false;
    m_pimpl->m_fiber = bfibers::fiber([this] { this->process(); });
    m_pimpl->m_startedPromise = bfibers::promise<void>();
    m_pimpl->m_startedFuture = m_pimpl->m_startedPromise.get_future();
    if (!m_joinable) {
         m_pimpl->m_fiber.detach();
    }
}

void FiberTask::stop()
{
    m_stop = true;
    cleanup();
    if (m_joinable && m_pimpl->m_fiber.joinable()) {
        m_pimpl->m_fiber.join();
    }
}

void FiberTask::signalRunning()
{
    m_pimpl->m_startedPromise .set_value();
}

void FiberTask::awaitRunning()
{
    if (!m_stop) {
        m_pimpl->m_startedFuture.wait();
    }
}

void FiberTask::yield()
{
    boost::this_fiber::yield();
}
#endif

}}}
