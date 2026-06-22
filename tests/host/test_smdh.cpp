#include "smdh.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>

namespace {

void SetShortDescription(std::uint8_t *smdh, std::uint8_t slot, const char *ascii) {
    std::uint8_t *p = smdh + smdh::kTitlesOff + slot * smdh::kSlotSize + smdh::kShortOff;
    const std::size_t len = std::strlen(ascii);
    const std::size_t copyLen = len < smdh::kShortBytes / 2 ? len : smdh::kShortBytes / 2;
    for (std::size_t i = 0; i < copyLen; ++i) {
        p[i * 2] = static_cast<std::uint8_t>(ascii[i]);
        p[i * 2 + 1] = 0;
    }
}

} // namespace

int main() {
    using namespace smdh;

    assert(PickSlot(0) == 0);
    assert(PickSlot(1) == 1);
    assert(PickSlot(11) == 11);
    assert(PickSlot(12) == kEnglishSlot);
    assert(PickSlot(255) == kEnglishSlot);

    std::string buffer(kTotalSize, '\0');
    auto *data = reinterpret_cast<std::uint8_t *>(buffer.data());
    std::memcpy(data, "SMDH", 4);

    SetShortDescription(data, kEnglishSlot, "Zelda: Ocarina of Time 3D");
    assert(ExtractName(data, kTotalSize, kEnglishSlot) == "Zelda: Ocarina of Time 3D");
    assert(ExtractName(data, kTotalSize, 0) == "Zelda: Ocarina of Time 3D");

    std::string spaced(kTotalSize, '\0');
    auto *spacedData = reinterpret_cast<std::uint8_t *>(spaced.data());
    SetShortDescription(spacedData, kEnglishSlot, "Game   ");
    assert(ExtractName(spacedData, kTotalSize, kEnglishSlot) == "Game");

    assert(ExtractName(data, 4, kEnglishSlot).empty());
}
