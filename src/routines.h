#pragma once

#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <algorithm>
#include "FileMapping.h"
#include "TCPConnection.h"
#include "TCPListener.h"


enum MessageType : uint8_t {
    FileInfo = 0,
    FilePart = 1,
};


void send(const std::string &address, const std::string &port, const std::string &filename, size_t threads = 1) {
    FileMapping file_mapping(filename);
    {
        TCPConnection connection(address, port);
        connection.write(MessageType::FileInfo);
        connection.write((uint16_t) threads);
        connection.write((uint64_t) file_mapping.size());
        connection.write(file_mapping.filename().c_str(), file_mapping.filename().size() + 1);
        connection.read<char>();
    }

    std::atomic<bool> error = false;
    std::exception_ptr writer_exception;

    auto writer_routine = [&](size_t start, size_t size) {
        try {
            TCPConnection connection(address, port);
            connection.write(MessageType::FilePart);
            connection.write((uint64_t) start);
            connection.write((uint64_t) size);
            connection.write(file_mapping.start() + start, size);
            connection.read<char>();
        } catch (...) {
            if (!error.exchange(true)) {
                writer_exception = std::current_exception();
            }
        }
    };

    std::vector<std::thread> workers;
    workers.reserve(threads);

    for (size_t i = 0; i < threads; ++i) {
        auto part_size = file_mapping.size() / threads + 1;
        auto start = part_size * i;
        auto end = std::min(part_size * (i + 1), file_mapping.size());

        workers.emplace_back(writer_routine, start, end - start);
    }

    for (auto &worker : workers) {
        worker.join();
    }

    if (error) {
        std::rethrow_exception(writer_exception);
    }
}


void receive(const std::string &port) {
    TCPListener server(port);

    uint16_t threads;
    uint64_t size;
    std::string filename;
    FileMapping file_mapping;

    std::atomic<bool> error = false;
    std::exception_ptr reader_exception;

    auto read_file_info = [&](TCPConnection& client) {
        threads = client.read<uint16_t>();
        size = client.read<uint64_t>();
        while (auto c = client.read<char>()) {
            filename += c;
        }
        file_mapping = FileMapping(filename, size);
        client.write("", 1);
    };

    auto read_file_part_routine = [&](TCPConnection client) {
        try {
            auto start = client.read<uint64_t>();
            auto size = client.read<uint64_t>();
            client.read(file_mapping.start() + start, size);
            client.write("", 1);
        } catch (...) {
            if (!error.exchange(true)) {
                reader_exception = std::current_exception();
            }
        }
    };

    std::vector<std::thread> workers;
    workers.reserve(threads);

    while (true) {
        auto client = server.listen();

        auto type = client.read<MessageType>();

        switch (type) {
            case MessageType::FileInfo:
                read_file_info(client);
                break;
            case MessageType::FilePart:
                workers.emplace_back(read_file_part_routine, std::move(client));
                break;
        }

        if (workers.size() >= threads) {
            break;
        }
    }

    for (auto &worker : workers) {
        worker.join();
    }

    if (error) {
        std::rethrow_exception(reader_exception);
    }
}
