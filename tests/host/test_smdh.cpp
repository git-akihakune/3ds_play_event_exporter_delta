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

void SetShortDescriptionUtf16(std::uint8_t *smdh, std::uint8_t slot, const std::uint16_t *utf16, std::size_t len) {
    std::uint8_t *p = smdh + smdh::kTitlesOff + slot * smdh::kSlotSize + smdh::kShortOff;
    const std::size_t copyLen = len < smdh::kShortBytes / 2 ? len : smdh::kShortBytes / 2;
    for (std::size_t i = 0; i < copyLen; ++i) {
        p[i * 2] = static_cast<std::uint8_t>(utf16[i] & 0xFF);
        p[i * 2 + 1] = static_cast<std::uint8_t>(utf16[i] >> 8);
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

    std::string surrogate(kTotalSize, '\0');
    auto *surrogateData = reinterpret_cast<std::uint8_t *>(surrogate.data());
    const std::uint16_t rocketTitle[] = {'R', 'o', 'c', 'k', 'e', 't', ' ', 0xD83D, 0xDE80};
    SetShortDescriptionUtf16(surrogateData, kEnglishSlot, rocketTitle, sizeof(rocketTitle) / sizeof(rocketTitle[0]));
    assert(ExtractName(surrogateData, kTotalSize, kEnglishSlot) == "Rocket \xF0\x9F\x9A\x80");

    std::string spaced(kTotalSize, '\0');
    auto *spacedData = reinterpret_cast<std::uint8_t *>(spaced.data());
    SetShortDescription(spacedData, kEnglishSlot, "Game   ");
    assert(ExtractName(spacedData, kTotalSize, kEnglishSlot) == "Game");

    assert(ExtractName(data, 4, kEnglishSlot).empty());
}
