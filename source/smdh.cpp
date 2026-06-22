#include "smdh.h"

namespace smdh {
namespace {

std::uint16_t Read16(const std::uint8_t *p) {
    return static_cast<std::uint16_t>(p[0] | (static_cast<std::uint16_t>(p[1]) << 8));
}

bool IsHighSurrogate(std::uint16_t codeUnit) {
    return codeUnit >= 0xD800 && codeUnit <= 0xDBFF;
}

bool IsLowSurrogate(std::uint16_t codeUnit) {
    return codeUnit >= 0xDC00 && codeUnit <= 0xDFFF;
}

void AppendUtf8(std::string &out, std::uint32_t codePoint) {
    if (codePoint < 0x80) {
        out.push_back(static_cast<char>(codePoint));
    } else if (codePoint < 0x800) {
        out.push_back(static_cast<char>(0xC0 | (codePoint >> 6)));
        out.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
    } else if (codePoint < 0x10000) {
        out.push_back(static_cast<char>(0xE0 | (codePoint >> 12)));
        out.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
    } else {
        out.push_back(static_cast<char>(0xF0 | (codePoint >> 18)));
        out.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
    }
}

std::string Utf16ToUtf8(const std::uint8_t *p, std::size_t maxCodeUnits) {
    constexpr std::uint32_t kReplacement = 0xFFFD;
    std::string out;
    for (std::size_t i = 0; i < maxCodeUnits; ++i) {
        const std::uint16_t codeUnit = Read16(p + i * 2);
        if (codeUnit == 0) {
            break;
        }

        if (IsHighSurrogate(codeUnit)) {
            if (i + 1 < maxCodeUnits) {
                const std::uint16_t low = Read16(p + (i + 1) * 2);
                if (IsLowSurrogate(low)) {
                    const std::uint32_t codePoint = 0x10000 +
                        ((static_cast<std::uint32_t>(codeUnit) - 0xD800) << 10) +
                        (static_cast<std::uint32_t>(low) - 0xDC00);
                    AppendUtf8(out, codePoint);
                    ++i;
                    continue;
                }
            }
            AppendUtf8(out, kReplacement);
            continue;
        }

        if (IsLowSurrogate(codeUnit)) {
            AppendUtf8(out, kReplacement);
            continue;
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
