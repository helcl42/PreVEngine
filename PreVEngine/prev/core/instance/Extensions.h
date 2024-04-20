#ifndef __EXTENSIONS_H__
#define __EXTENSIONS_H__

#include "../../common/PickList.h"

#include "../Core.h"

namespace prev::core::instance {
class Extensions final : public prev::common::PickList<VkExtensionProperties> {
public:
    Extensions(const char* layerName = nullptr);

    Extensions(const Extensions& other);

    Extensions& operator=(const Extensions& other);

public:
    std::string GetNameByIndex(const uint32_t inx) const override;

    std::string GetName() const override;
};
} // namespace prev::core::instance

#endif