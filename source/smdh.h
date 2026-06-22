#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace smdh {

constexpr std::size_t kTotalSize = 0x36C0;
constexpr std::size_t kTitlesOff = 0x08;
constexpr std::size_t kSlotSize = 0x200;
constexpr std::size_t kSlotCount = 16;
constexpr std::size_t kShortOff = 0x00;
constexpr std::size_t kShortBytes = 0x80;
constexpr std::uint8_t kEnglishSlot = 1;

std::uint8_t PickSlot(std::uint8_t langSlot);
std::string ExtractName(const std::uint8_t *smdh, std::size_t len, std::uint8_t langSlot);

} // namespace smdh
