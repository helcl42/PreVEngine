#include "EventChannelQueueManager.h"

namespace PreVEngine {
namespace Internal {
    void EventChannelQueueManager::Add(IEventChannelQueue& queue)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_eventChannelQueues.emplace_back(&queue);
    }

    void EventChannelQueueManager::Remove(IEventChannelQueue& queue)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = std::find(m_eventChannelQueues.cbegin(), m_eventChannelQueues.cend(), &queue);
        m_eventChannelQueues.erase(it);
    }

    void EventChannelQueueManager::BroadcastAll()
    {
        std::vector<IEventChannelQueue*> queues(m_eventChannelQueues.size());

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            queues = m_eventChannelQueues;
        }

        for (auto& queue : queues) {
            queue->DispatchAll();
        }
    }
} // namespace Internal
} // namespace PreVEngine
