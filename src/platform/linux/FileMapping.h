#pragma once

#include <string>


class FileMapping {
public:
    FileMapping() = default;

    explicit FileMapping(const std::string &file_path, size_t size = -1);

    FileMapping(const FileMapping &) = delete;

    FileMapping(FileMapping &&) noexcept;

    FileMapping &operator=(const FileMapping &) = delete;

    FileMapping &operator=(FileMapping &&) noexcept;

    ~FileMapping();

    char *start();

    size_t size();

    const std::string &filename();

private:
    char *mapping_{nullptr};
    size_t mapping_length_{0};
    std::string filename_;
};

