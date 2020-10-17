#ifndef __DEFAULT_RENDER_CONTEXT_USER_DATA_H__
#define __DEFAULT_RENDER_CONTEXT_USER_DATA_H__

namespace prev::render {
struct DefaultRenderContextUserData // inherit this in case you need any special data while rendering scene graph
{
    virtual ~DefaultRenderContextUserData() = default;
};
} // namespace prev::render

#endif // !__DEFAULT_RENDER_CONTEXT_USER_DATA_H__
