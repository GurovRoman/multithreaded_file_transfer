#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdexcept>
#include "FileMapping.h"


FileMapping::FileMapping(const std::string &file_path, size_t size) {
    int file_des;

    if (size == -1) {
        file_des = open(file_path.c_str(), O_RDONLY, 0664);
        struct stat file_stat{};
        fstat(file_des, &file_stat);

        mapping_length_ = file_stat.st_size;
        if (mapping_length_ > 0) {
            mapping_ = (char *) mmap(nullptr, mapping_length_, PROT_READ, MAP_PRIVATE, file_des, 0);
        }
    } else {
        file_des = open(file_path.c_str(), O_RDWR | O_TRUNC | O_CREAT | O_EXCL, 0664);
        if (file_des == -1) {
            if (errno == EEXIST) {
                throw std::runtime_error("File already exists: " + file_path);
            }
        }
        ftruncate(file_des, size);

        mapping_length_ = size;
        if (mapping_length_ > 0) {
            mapping_ = (char *) mmap(nullptr, size, PROT_WRITE, MAP_SHARED, file_des, 0);
        }
    }

    if (mapping_ == MAP_FAILED) {
        throw std::runtime_error("Couldn't open file: " + file_path);
    }

    filename_ = basename(file_path.c_str());

    close(file_des);
}

FileMapping::FileMapping(FileMapping &&other) noexcept {
    std::swap(mapping_, other.mapping_);
    std::swap(mapping_length_, other.mapping_length_);
}

FileMapping &FileMapping::operator=(FileMapping &&other) noexcept {
    munmap(mapping_, mapping_length_);

    mapping_ = nullptr;
    mapping_length_ = 0;

    std::swap(mapping_, other.mapping_);
    std::swap(mapping_length_, other.mapping_length_);

    return *this;
}

FileMapping::~FileMapping() {
    munmap(mapping_, mapping_length_);
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
