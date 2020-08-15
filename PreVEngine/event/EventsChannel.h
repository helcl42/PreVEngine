#ifndef __EVENT_CHANNEL_H__
#define __EVENT_CHANNEL_H__

#include "internal/EventChannelQueue.h"
#include "internal/EventChannelQueueManager.h"

namespace prev {
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
        Internal::EventChannelQueue<MessageType>::Instance().Add(handler);
    }

    template <typename MessageType, typename EventHandlerType>
    static void Remove(EventHandlerType& handler)
    {
        Internal::EventChannelQueue<MessageType>::Instance().Remove(handler);
    }

    // Should I add new function PostToDispatch instead of enum ??
    template <typename MessageType>
    static void Broadcast(const MessageType& message)
    {
        Internal::EventChannelQueue<MessageType>::Instance().Broadcast(message);
    }

    template <typename MessageType>
    static void BroadcastWithDispatch(const MessageType& message)
    {
        Internal::EventChannelQueue<MessageType>::Instance().BroadcastWithDispatch(message);
    }

    // rename it BroadcastDispatched
    static void DispatchAll()
    {
        Internal::EventChannelQueueManager::Instance().BroadcastAll();
    }
};
} // namespace prev

#endif
