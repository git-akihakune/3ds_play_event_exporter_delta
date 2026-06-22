#include "outputlog.h"

#include <cstddef>
#include <cstdint>

namespace {

constexpr char kOutputPath[] = "/play_events.log";

} // namespace

std::string_view OutputLogPath() {
    return kOutputPath;
}

#ifdef __3DS__

#include <fmt/base.h>
#include <3ds.h>

namespace {

struct ArchiveHandle {
    FS_Archive archive = 0;

    ~ArchiveHandle() noexcept {
        if (archive != 0) {
            FSUSER_CloseArchive(archive);
        }
    }
};

struct FileHandle {
    Handle file = 0;

    ~FileHandle() noexcept {
        if (file != 0) {
            FSFILE_Close(file);
        }
    }
};

bool WriteAll(Handle file, std::string_view contents) {
    std::size_t offset = 0;
    while (offset < contents.size()) {
        const std::size_t remaining = contents.size() - offset;
        const u32 chunk = remaining > UINT32_MAX ? UINT32_MAX : static_cast<u32>(remaining);
        u32 written = 0;
        const u32 flags = (offset + chunk == contents.size()) ? (FS_WRITE_FLUSH | FS_WRITE_UPDATE_TIME) : 0;
        const Result res = FSFILE_Write(file, &written, offset, contents.data() + offset, chunk, flags);
        if (R_FAILED(res) || written != chunk) {
            fmt::print("FSFILE_Write failure: {:08X} ({} of {} bytes)\n", static_cast<u32>(res), written, chunk);
            return false;
        }
        offset += written;
    }

    return true;
}

} // namespace

bool WriteLog(std::string_view contents) {
    const FS_Path archivePath = fsMakePath(PATH_EMPTY, "");
    const FS_Path filePath = fsMakePath(PATH_ASCII, kOutputPath);

    ArchiveHandle archive;
    Result res = FSUSER_OpenArchive(&archive.archive, ARCHIVE_SDMC, archivePath);
    if (R_FAILED(res)) {
        fmt::print("FSUSER_OpenArchive sdmc failure: {:08X}\n", static_cast<u32>(res));
        return false;
    }

    FSUSER_DeleteFile(archive.archive, filePath);

    res = FSUSER_CreateFile(archive.archive, filePath, 0, static_cast<u64>(contents.size()));
    if (R_FAILED(res)) {
        fmt::print("FSUSER_CreateFile sdmc:{} failure: {:08X}\n", kOutputPath, static_cast<u32>(res));
        return false;
    }

    FileHandle file;
    res = FSUSER_OpenFile(&file.file, archive.archive, filePath, FS_OPEN_WRITE, 0);
    if (R_FAILED(res)) {
        fmt::print("FSUSER_OpenFile sdmc:{} failure: {:08X}\n", kOutputPath, static_cast<u32>(res));
        return false;
    }

    if (!WriteAll(file.file, contents)) {
        return false;
    }

    res = FSFILE_Close(file.file);
    file.file = 0;
    if (R_FAILED(res)) {
        fmt::print("FSFILE_Close sdmc:{} failure: {:08X}\n", kOutputPath, static_cast<u32>(res));
        return false;
    }

    return true;
}

#else

bool WriteLog(std::string_view contents) {
    (void)contents;
    return false;
}

#endif
