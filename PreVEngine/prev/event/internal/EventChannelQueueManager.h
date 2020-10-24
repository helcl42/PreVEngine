#ifndef __EVENT_CHANNEL_QUEUE_MANAGER_H__
#define __EVENT_CHANNEL_QUEUE_MANAGER_H__

#include "../../common/pattern/Singleton.h"
#include "IEventChannelQueue.h"

#include <mutex>
#include <vector>

namespace prev::event::internal {
class EventChannelQueueManager final : public prev::common::pattern::Singleton<EventChannelQueueManager> {
public:
    void Add(IEventChannelQueue& queue);

    void Remove(IEventChannelQueue& queue);

    void DispatchAll();

private:
    EventChannelQueueManager() = default;

    ~EventChannelQueueManager() = default;

private:
    EventChannelQueueManager(EventChannelQueueManager&& other) = delete;

    EventChannelQueueManager& operator=(EventChannelQueueManager&& other) = delete;

    EventChannelQueueManager(const EventChannelQueueManager& other) = delete;

    EventChannelQueueManager& operator=(const EventChannelQueueManager& other) = delete;

private:
    friend class prev::common::pattern::Singleton<EventChannelQueueManager>;

private:
    std::mutex m_mutex;

    std::vector<IEventChannelQueue*> m_eventChannelQueues;
};
} // namespace prev::event::internal

#endif