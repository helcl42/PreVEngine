#ifndef __EXTENSIONS_H__
#define __EXTENSIONS_H__

#include "../../common/PickList.h"

#include "../Core.h"

namespace prev::core::instance {
class Extensions final : public prev::common::PickList {
public:
    Extensions(const char* layerName = nullptr);
};
} // namespace prev::core::instance

#endif