#ifndef __EVENT_CHANNEL_H__
#define __EVENT_CHANNEL_H__

#include "internal/EventChannelQueue.h"
#include "internal/EventChannelQueueManager.h"

namespace prev::event {
class EventChannel final {
private:
    EventChannel() = default;

    ~EventChannel() = default;

private:
    EventChannel(const EventChannel& other) = delete;

    EventChannel& operator=(const EventChannel& other) = delete;

    EventChannel(const EventChannel&& other) = delete;

    EventChannel& operator=(const EventChannel&& other) = delete;

public:
    template <typename MessageType, typename EventHandlerType>
    static void Add(EventHandlerType& handler)
    {
        internal::EventChannelQueue<MessageType>::Instance().Add(handler);
    }

    template <typename MessageType, typename EventHandlerType>
    static void Remove(EventHandlerType& handler)
    {
        internal::EventChannelQueue<MessageType>::Instance().Remove(handler);
    }

    // Should I add new function PostToDispatch instead of enum ??
    template <typename MessageType>
    static void Broadcast(const MessageType& message)
    {
        internal::EventChannelQueue<MessageType>::Instance().Broadcast(message);
    }

    template <typename MessageType>
    static void BroadcastWithDispatch(const MessageType& message)
    {
        internal::EventChannelQueue<MessageType>::Instance().BroadcastWithDispatch(message);
    }

    // rename it BroadcastDispatched
    static void DispatchAll()
    {
        internal::EventChannelQueueManager::Instance().BroadcastAll();
    }
};
} // namespace prev::event

#endif
