#include "xpcf/threading/BaseTask.h"

using namespace std;
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

void DelegateTask::process()
{
    while(!stopped()) {
        m_processFunc();
    }
}

}}}
