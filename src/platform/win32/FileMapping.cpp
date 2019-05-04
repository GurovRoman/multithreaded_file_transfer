#include <stdexcept>
#include <windows.h>
#include "FileMapping.h"


FileMapping::FileMapping(const std::string &file_path, size_t size) {
    HANDLE file_handle;

    if (size == -1) {
        file_handle = CreateFileA(file_path.c_str(), GENERIC_READ, 0, nullptr,
                                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

        LARGE_INTEGER file_size;
        GetFileSizeEx(file_handle, &file_size);
        mapping_length_ = file_size.QuadPart;

        if (mapping_length_ > 0) {
            HANDLE mapping = CreateFileMappingA(file_handle, nullptr, PAGE_READONLY, 0, 0, nullptr);
            mapping_ = (char *) MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
        }
    } else {
        file_handle = CreateFileA(file_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                                  CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (file_handle == INVALID_HANDLE_VALUE) {
            if (GetLastError() == ERROR_FILE_EXISTS) {
                throw std::runtime_error("File already exists: " + file_path);
            }
        }

        SetFilePointer(file_handle, size, nullptr, FILE_BEGIN);
        SetEndOfFile(file_handle);

        mapping_length_ = size;
        if (mapping_length_ > 0) {
            HANDLE mapping = CreateFileMappingA(file_handle, nullptr, PAGE_READWRITE, 0, 0, nullptr);
            mapping_ = (char *) MapViewOfFile(mapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
        }
    }

    if (mapping_ == nullptr) {
        throw std::runtime_error("Couldn't open file: " + file_path);
    }

    auto slash_pos = file_path.rfind('\\');
    if (slash_pos == std::string::npos) {
        slash_pos = 0;
    }
    filename_ = file_path.c_str() + slash_pos + 1;

    CloseHandle(file_handle);
}

FileMapping::FileMapping(FileMapping &&other) noexcept {
    std::swap(mapping_, other.mapping_);
    std::swap(mapping_length_, other.mapping_length_);
}

FileMapping &FileMapping::operator=(FileMapping &&other) noexcept {
    UnmapViewOfFile(mapping_);

    mapping_ = nullptr;
    mapping_length_ = 0;

    std::swap(mapping_, other.mapping_);
    std::swap(mapping_length_, other.mapping_length_);

    return *this;
}

FileMapping::~FileMapping() {
    UnmapViewOfFile(mapping_);
}

char *FileMapping::start() {
    return mapping_;
}

size_t FileMapping::size() {
    return mapping_length_;
}

const std::string &FileMapping::filename() {
    return filename_;
}
