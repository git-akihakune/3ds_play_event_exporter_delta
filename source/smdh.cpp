#include "smdh.h"

namespace smdh {
namespace {

std::uint16_t Read16(const std::uint8_t *p) {
    return static_cast<std::uint16_t>(p[0] | (static_cast<std::uint16_t>(p[1]) << 8));
}

void AppendUtf8(std::string &out, std::uint16_t codeUnit) {
    if (codeUnit < 0x80) {
        out.push_back(static_cast<char>(codeUnit));
    } else if (codeUnit < 0x800) {
        out.push_back(static_cast<char>(0xC0 | (codeUnit >> 6)));
        out.push_back(static_cast<char>(0x80 | (codeUnit & 0x3F)));
    } else {
        out.push_back(static_cast<char>(0xE0 | (codeUnit >> 12)));
        out.push_back(static_cast<char>(0x80 | ((codeUnit >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (codeUnit & 0x3F)));
    }
}

std::string Utf16ToUtf8(const std::uint8_t *p, std::size_t maxCodeUnits) {
    std::string out;
    for (std::size_t i = 0; i < maxCodeUnits; ++i) {
        const std::uint16_t codeUnit = Read16(p + i * 2);
        if (codeUnit == 0) {
            break;
        }
        AppendUtf8(out, codeUnit);
    }
    return out;
}

std::string SlotName(const std::uint8_t *smdh, std::size_t len, std::uint8_t slot) {
    if (slot >= kSlotCount) {
        return {};
    }

    const std::size_t off = kTitlesOff + slot * kSlotSize + kShortOff;
    if (off + kShortBytes > len) {
        return {};
    }

    std::string name = Utf16ToUtf8(smdh + off, kShortBytes / 2);
    while (!name.empty() && (name.back() == ' ' || name.back() == '\0')) {
        name.pop_back();
    }
    return name;
}

} // namespace

std::uint8_t PickSlot(std::uint8_t langSlot) {
    constexpr std::uint8_t kLanguageSlotCount = 12;
    return langSlot < kLanguageSlotCount ? langSlot : kEnglishSlot;
}

std::string ExtractName(const std::uint8_t *smdh, std::size_t len, std::uint8_t langSlot) {
    if (smdh == nullptr || len < kTitlesOff + kSlotSize) {
        return {};
    }

    const std::uint8_t slots[] = {
        PickSlot(langSlot),
        kEnglishSlot,
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
    };

    for (const std::uint8_t slot : slots) {
        std::string name = SlotName(smdh, len, slot);
        if (!name.empty()) {
            return name;
        }
    }

    return {};
}

} // namespace smdh
