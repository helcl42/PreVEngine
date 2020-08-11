#ifndef __IEVENT_CHANNEL_QUEUE_H__
#define __IEVENT_CHANNEL_QUEUE_H__

namespace PreVEngine {
namespace Internal {
    class IEventChannelQueue {
    public:
        virtual void DispatchAll() = 0;

    public:
        virtual ~IEventChannelQueue() = default;
    };
} // namespace Internal
} // namespace PreVEngine

#endif