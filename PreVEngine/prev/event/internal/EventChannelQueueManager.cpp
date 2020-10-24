#include "EventChannelQueueManager.h"

#include <algorithm>

namespace prev::event::internal {
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

void EventChannelQueueManager::DispatchAll()
{
    std::vector<IEventChannelQueue*> queues;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        queues = m_eventChannelQueues;
    }

    for (auto& queue : queues) {
        queue->DispatchAll();
    }
}
} // namespace prev::event::internal
