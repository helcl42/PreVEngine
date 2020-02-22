#ifndef __EVENT_CHANNEL_H__
#define __EVENT_CHANNEL_H__

#include <algorithm>
#include <functional>
#include <mutex>
#include <vector>

#include "Patterns.h"

namespace PreVEngine {
namespace Internal {
    class IEventChannelQueue {
    public:
        virtual void DispatchAll() = 0;

    public:
        virtual ~IEventChannelQueue() = default;
    };

    class EventChannelQueueManager final : public Singleton<EventChannelQueueManager> {
    private:
        friend class Singleton<EventChannelQueueManager>;

    private:
        std::mutex m_mutex;

        std::vector<IEventChannelQueue*> m_eventChannelQueues;

    private:
        EventChannelQueueManager(EventChannelQueueManager&& other) = delete;

        EventChannelQueueManager& operator=(EventChannelQueueManager&& other) = delete;

        EventChannelQueueManager(const EventChannelQueueManager& other) = delete;

        EventChannelQueueManager& operator=(const EventChannelQueueManager& other) = delete;

    private:
        EventChannelQueueManager() = default;

        ~EventChannelQueueManager() = default;

    public:
        void Add(IEventChannelQueue& queue)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            m_eventChannelQueues.emplace_back(&queue);
        }

        void Remove(IEventChannelQueue& queue)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            auto it = std::find(m_eventChannelQueues.cbegin(), m_eventChannelQueues.cend(), &queue);
            m_eventChannelQueues.erase(it);
        }

        void BroadcastAll()
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
    };

    template <typename EventType>
    class EventChannelQueue final : public Singleton<EventChannelQueue<EventType> >, public IEventChannelQueue {
    private:
        friend class Singleton<EventChannelQueue<EventType> >;

    private:
        std::mutex m_mutex;

        std::vector<std::function<void(const EventType&)> > m_handlers;

        std::vector<void*> m_originalPointers;

        std::vector<EventType> m_unsedMessages;

    private:
        EventChannelQueue(EventChannelQueue&& other) = delete;

        EventChannelQueue& operator=(EventChannelQueue&& other) = delete;

        EventChannelQueue(const EventChannelQueue& other) = delete;

        EventChannelQueue& operator=(const EventChannelQueue& other) = delete;

    private:
        EventChannelQueue()
            : Singleton<EventChannelQueue<EventType> >()
        {
            EventChannelQueueManager::GetInstance().Add(*this);
        }

        ~EventChannelQueue()
        {
            EventChannelQueueManager::GetInstance().Remove(*this);
        }

    public:
        template <typename EventHandlerType>
        void Add(EventHandlerType& handler)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            m_handlers.emplace_back(CreateHandler(handler));
            m_originalPointers.emplace_back(&handler);
        }

        template <typename EventHandlerType>
        void Remove(EventHandlerType& handler)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            auto it = std::find(m_originalPointers.begin(), m_originalPointers.end(), &handler);
            if (it == m_originalPointers.end())
                throw std::runtime_error("Tried to remove a handler that is not in the list");

            auto idx = (it - m_originalPointers.begin());

            m_handlers.erase(m_handlers.begin() + idx);
            m_originalPointers.erase(it);
        }

        void Broadcast(const EventType& message)
        {
            std::vector<std::function<void(const EventType&)> > currentHandlersCopy(m_handlers.size());

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                currentHandlersCopy = m_handlers;
            }

            for (const auto& handler : currentHandlersCopy) {
                handler(message);
            }
        }

        void BroadcastWithDispatch(const EventType& message)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            m_unsedMessages.emplace_back(message);
        }

        void DispatchAll() override
        {
            std::vector<std::function<void(const EventType&)> > currentHandlersCopy(m_handlers.size());
            std::vector<EventType> currentUnsendMessages(m_unsedMessages.size());

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                currentHandlersCopy = m_handlers;

                currentUnsendMessages = m_unsedMessages;
                m_unsedMessages.clear();
            }

            for (const auto& message : currentUnsendMessages) {
                for (const auto& handler : currentHandlersCopy) {
                    handler(message);
                }
            }
        }

    private:
        template <typename EventHandlerType>
        static std::function<void(const EventType&)> CreateHandler(EventHandlerType& handler)
        {
            return [&handler](const EventType& message) { handler(message); };
        }
    };
} // namespace Internal

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
        Internal::EventChannelQueue<MessageType>::GetInstance().Add(handler);
    }

    template <typename MessageType, typename EventHandlerType>
    static void Remove(EventHandlerType& handler)
    {
        Internal::EventChannelQueue<MessageType>::GetInstance().Remove(handler);
    }

    // Should I add new function PostToDispatch instead of enum ??
    template <typename MessageType>
    static void Broadcast(const MessageType& message)
    {
        Internal::EventChannelQueue<MessageType>::GetInstance().Broadcast(message);
    }

    template <typename MessageType>
    static void BroadcastWithDispatch(const MessageType& message)
    {
        Internal::EventChannelQueue<MessageType>::GetInstance().BroadcastWithDispatch(message);
    }

    // rename it BroadcastDispatched
    static void DispatchAll()
    {
        Internal::EventChannelQueueManager::GetInstance().BroadcastAll();
    }
};

template <typename EventHandlerType, typename EventType>
class EventHandler final {
private:
    EventHandlerType& m_handlerInstance;

public:
    EventHandler(EventHandlerType& instance)
        : m_handlerInstance(instance)
    {
        EventChannel::Add<EventType>(m_handlerInstance);
    }

    virtual ~EventHandler()
    {
        EventChannel::Remove<EventType>(m_handlerInstance);
    }

public:
    EventHandler(const EventHandler& other) = default;

    EventHandler& operator=(const EventHandler& other) = default;

    EventHandler(EventHandler&& other) = default;

    EventHandler& operator=(EventHandler&& other) = default;
};
} // namespace PreVEngine

#endif
