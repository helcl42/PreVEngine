#ifndef __BUFFER_EXECUTION_MODE_H__
#define __BUFFER_EXECUTION_MODE_H__

namespace prev::render::buffer {

// Selects how a builder runs the upload / layout work (and the resource's destruction).
enum class ExecutionMode {
    // Defer when the frame loop is running, run immediately otherwise (e.g. at init). Default.
    Auto,

    // Always run immediately: submit and wait idle. Safe only for brand-new resources — it does not wait
    // out frames already in flight, so never use it to overwrite/destroy a resource a live frame may read.
    Immediate,
};

} // namespace prev::render::buffer

#endif // !__BUFFER_EXECUTION_MODE_H__
