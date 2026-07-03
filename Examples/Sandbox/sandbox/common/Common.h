#ifndef __SANDBOX_COMMON_H__
#define __SANDBOX_COMMON_H__

// Propagated by PreVEngine's CMake. MAX_VIEW_COUNT_VALUE = total XR eyes (2 for XR, else 1);
// MAX_PER_PASS_VIEW_COUNT_VALUE = views rendered per pass (2 for multiview, else 1). Fall back to mono so a
// translation unit still compiles if a definition is missing (mirrors the shaders' own #ifndef guards).
#ifndef MAX_VIEW_COUNT_VALUE
#define MAX_VIEW_COUNT_VALUE 1
#endif
#ifndef MAX_PER_PASS_VIEW_COUNT_VALUE
#define MAX_PER_PASS_VIEW_COUNT_VALUE 1
#endif

#endif // !__SANDBOX_COMMON_H__
