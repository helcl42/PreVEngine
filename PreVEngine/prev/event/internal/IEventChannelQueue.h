#ifndef __IEVENT_CHANNEL_QUEUE_H__
#define __IEVENT_CHANNEL_QUEUE_H__

namespace prev::event::internal {
class IEventChannelQueue {
public:
    virtual void DispatchAll() = 0;

public:
    virtual ~IEventChannelQueue() = default;
};
} // namespace prev::event::internal

#endif