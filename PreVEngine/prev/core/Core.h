#ifndef __CORE_H__
#define __CORE_H__

#include "../common/Common.h"
#include "../common/Logger.h"

#include <gfx/gfx.h>

#ifdef NDEBUG
#define GFXERRCHECK(FN) \
    {                   \
        (void)(FN);     \
    }
#else
#define GFXERRCHECK(FN)                                               \
    {                                                                 \
        GfxResult GFXRESULT = (FN);                                   \
        if (GFXRESULT != GFX_RESULT_SUCCESS) {                        \
            LOGE("%s failed: %s", #FN, gfxResultToString(GFXRESULT)); \
            assert(GFXRESULT == GFX_RESULT_SUCCESS);                  \
        }                                                             \
    }
#endif

#endif // !__CORE_H__
