#pragma once
#include "onut/Dispatcher.h"

ODispatcherRef oDispatcher;

namespace onut
{
    ODispatcherRef Dispatcher::create()
    {
        return OMake<Dispatcher>();
    }

    Dispatcher::Dispatcher()
    {
    }

    void Dispatcher::processQueue()
    {
        m_mutex.lock();
        while (!m_callbackQueue.empty())
        {
            auto pCallback = m_callbackQueue.front();
            m_callbackQueue.pop();

            // We unlock during the call. Because the call might add new callbacks!
            m_mutex.unlock();
            pCallback->call();
            m_mutex.lock();
        }
        m_mutex.unlock();
    }

    size_t Dispatcher::size()
    {
        m_mutex.lock();
        auto ret = m_callbackQueue.size();
        m_mutex.unlock();
        return ret;
    }

    void Dispatcher::syncCallback(const ICallbackRef& pCallback)
    {
        m_callbackQueue.push(pCallback);
    }
}
