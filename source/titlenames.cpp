#include "titlenames.h"

#include "smdh.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

std::string FormatHexTitleId(std::uint64_t titleId) {
    constexpr char kHex[] = "0123456789ABCDEF";
    std::string out(16, '0');
    for (int i = 15; i >= 0; --i) {
        out[static_cast<std::size_t>(i)] = kHex[titleId & 0x0F];
        titleId >>= 4;
    }
    return out;
}

struct KnownTitleName {
    std::uint32_t titleHigh;
    std::uint32_t titleLow;
    std::string_view name;
};

constexpr KnownTitleName kKnownSystemApplets[] = {
    {0x00040030u, 0x00008202u, "HOME Menu"},
    {0x00040030u, 0x00008F02u, "HOME Menu"},
    {0x00040030u, 0x00009802u, "HOME Menu"},
    {0x00040030u, 0x0000A102u, "HOME Menu"},
    {0x00040030u, 0x0000A902u, "HOME Menu"},
    {0x00040030u, 0x0000B102u, "HOME Menu"},

    {0x00040030u, 0x00008602u, "Instruction Manual"},
    {0x00040030u, 0x00009202u, "Instruction Manual"},
    {0x00040030u, 0x00009B02u, "Instruction Manual"},
    {0x00040030u, 0x0000A402u, "Instruction Manual"},
    {0x00040030u, 0x0000AC02u, "Instruction Manual"},
    {0x00040030u, 0x0000B402u, "Instruction Manual"},

    {0x00040030u, 0x00008702u, "Game Notes"},
    {0x00040030u, 0x00009302u, "Game Notes"},
    {0x00040030u, 0x00009C02u, "Game Notes"},
    {0x00040030u, 0x0000A502u, "Game Notes"},
    {0x00040030u, 0x0000AD02u, "Game Notes"},
    {0x00040030u, 0x0000B502u, "Game Notes"},

    {0x00040030u, 0x00008802u, "Internet Browser"},
    {0x00040030u, 0x00009402u, "Internet Browser"},
    {0x00040030u, 0x00009D02u, "Internet Browser"},
    {0x00040030u, 0x0000A602u, "Internet Browser"},
    {0x00040030u, 0x0000AE02u, "Internet Browser"},
    {0x00040030u, 0x0000B602u, "Internet Browser"},
    {0x00040030u, 0x20008802u, "Internet Browser"},
    {0x00040030u, 0x20009402u, "Internet Browser"},
    {0x00040030u, 0x20009D02u, "Internet Browser"},
    {0x00040030u, 0x2000AE02u, "Internet Browser"},

    {0x00040030u, 0x00008D02u, "Friend List"},
    {0x00040030u, 0x00009602u, "Friend List"},
    {0x00040030u, 0x00009F02u, "Friend List"},
    {0x00040030u, 0x0000A702u, "Friend List"},
    {0x00040030u, 0x0000AF02u, "Friend List"},
    {0x00040030u, 0x0000B702u, "Friend List"},

    {0x00040030u, 0x00008E02u, "Notifications"},
    {0x00040030u, 0x00009702u, "Notifications"},
    {0x00040030u, 0x0000A002u, "Notifications"},
    {0x00040030u, 0x0000A802u, "Notifications"},
    {0x00040030u, 0x0000B002u, "Notifications"},
    {0x00040030u, 0x0000B802u, "Notifications"},

    {0x00040030u, 0x0000BC02u, "Miiverse"},
    {0x00040030u, 0x0000BD02u, "Miiverse"},
    {0x00040030u, 0x0000BE02u, "Miiverse"},

    {0x00040030u, 0x00009502u, "amiibo Settings"},
    {0x00040030u, 0x00009E02u, "amiibo Settings"},
    {0x00040030u, 0x0000B902u, "amiibo Settings"},
    {0x00040030u, 0x00008C02u, "amiibo Settings"},
    {0x00040030u, 0x0000BF02u, "amiibo Settings"},
};

std::string_view FindKnownSystemTitleName(std::uint64_t titleId) {
    const auto titleHigh = static_cast<std::uint32_t>(titleId >> 32);
    const auto titleLow = static_cast<std::uint32_t>(titleId & 0xFFFFFFFFu);

    for (const KnownTitleName &known : kKnownSystemApplets) {
        if (known.titleHigh == titleHigh && known.titleLow == titleLow) {
            return known.name;
        }
    }

    return {};
}

} // namespace

#ifdef __3DS__

#include <fmt/base.h>
#include <3ds.h>

namespace {

struct ServiceGuard {
    bool fs = false;
    bool am = false;
    bool cfgu = false;

    ~ServiceGuard() noexcept {
        if (cfgu) {
            cfguExit();
        }
        if (am) {
            amExit();
        }
        if (fs) {
            fsExit();
        }
    }
};

bool ReadSmdh(std::uint64_t titleId, FS_MediaType media, std::uint8_t *out, std::size_t len) {
    Handle fileHandle = 0;
    const std::uint32_t archivePathData[4] = {
        static_cast<std::uint32_t>(titleId & 0xFFFFFFFFu),
        static_cast<std::uint32_t>(titleId >> 32),
        static_cast<std::uint32_t>(media),
        0u,
    };
    const std::uint32_t filePathData[5] = {0u, 0u, 2u, 0x6E6F6369u, 0u};
    const FS_Path archivePath = {PATH_BINARY, sizeof(archivePathData), archivePathData};
    const FS_Path filePath = {PATH_BINARY, sizeof(filePathData), filePathData};

    Result res = FSUSER_OpenFileDirectly(
        &fileHandle,
        ARCHIVE_SAVEDATA_AND_CONTENT,
        archivePath,
        filePath,
        FS_OPEN_READ,
        0);
    if (R_FAILED(res)) {
        return false;
    }

    u32 bytesRead = 0;
    res = FSFILE_Read(fileHandle, &bytesRead, 0, out, static_cast<u32>(len));
    FSFILE_Close(fileHandle);
    return R_SUCCEEDED(res) && bytesRead == static_cast<u32>(len);
}

} // namespace

TitleNameResolver BuildTitleNameResolver() {
    TitleNameResolver resolver;
    ServiceGuard guard;

    Result res = fsInit();
    if (R_FAILED(res)) {
        fmt::print("fsInit failure: {:08X}\n", static_cast<u32>(res));
        return resolver;
    }
    guard.fs = true;

    res = amInit();
    if (R_FAILED(res)) {
        fmt::print("amInit failure: {:08X}\n", static_cast<u32>(res));
        return resolver;
    }
    guard.am = true;

    std::uint8_t languageSlot = smdh::kEnglishSlot;
    if (R_SUCCEEDED(cfguInit())) {
        guard.cfgu = true;
        u8 systemLanguage = 0;
        if (R_SUCCEEDED(CFGU_GetSystemLanguage(&systemLanguage))) {
            languageSlot = systemLanguage;
        }
    }

    std::uint8_t smdhBuffer[smdh::kTotalSize];
    const FS_MediaType mediaTypes[] = {MEDIATYPE_NAND, MEDIATYPE_SD, MEDIATYPE_GAME_CARD};

    for (const FS_MediaType media : mediaTypes) {
        u32 count = 0;
        if (R_FAILED(AM_GetTitleCount(media, &count)) || count == 0) {
            continue;
        }

        std::vector<u64> titleIds(count);
        u32 read = 0;
        if (R_FAILED(AM_GetTitleList(&read, media, count, titleIds.data())) || read == 0) {
            continue;
        }

        titleIds.resize(read);
        for (const u64 titleId : titleIds) {
            if (!ReadSmdh(titleId, media, smdhBuffer, sizeof(smdhBuffer))) {
                continue;
            }

            std::string name = smdh::ExtractName(smdhBuffer, sizeof(smdhBuffer), languageSlot);
            if (!name.empty()) {
                resolver.byId[titleId] = std::move(name);
            }
        }
    }

    return resolver;
}

#else

TitleNameResolver BuildTitleNameResolver() {
    return {};
}

#endif

std::string ResolveTitleName(const TitleNameResolver &resolver, std::uint64_t titleId) {
    const auto it = resolver.byId.find(titleId);
    if (it != resolver.byId.end()) {
        return it->second;
    }

    if (const std::string_view knownName = FindKnownSystemTitleName(titleId); !knownName.empty()) {
        return std::string{knownName};
    }

    return FormatHexTitleId(titleId);
}
