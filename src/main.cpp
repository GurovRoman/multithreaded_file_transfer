#include <iostream>
#include <string>
#include "routines.h"
#include "LongOptionParser.h"

static const std::string PORT = "2288";


int main(int argc, char **argv) {
    LongOptionParser opts;

    try {
        opts = LongOptionParser(argv, {"-receive", "-send"}, {"-ip", "-path", "-threads"});
    } catch (std::exception &ex) {
        std::cerr << ex.what() << '\n';
    }

    bool receive = opts.contains("-receive");
    bool send = opts.contains("-send");
    std::string ip = opts["-ip"];
    std::string file_path = opts["-path"];
    size_t threads = 1;
    if (opts.contains("-threads")) {
        threads = std::stoul(opts["-threads"]);
        if (threads <= 0) {
            std::cerr << "Option -threads must be greater than zero\n";
            return 1;
        }
    }

    if (receive && send) {
        std::cerr << "Options -send and -receive are mutually exclusive\n";
        return 1;
    }

    if (send) {
        if (ip.empty() || file_path.empty()) {
            std::cerr << "Options -ip and -path required to send\n";
            return 1;
        }
        try {
            ::send(ip, PORT, file_path, threads);
        } catch (std::exception &ex) {
            std::cerr << "File send failed: " << ex.what() << '\n';
            return 1;
        }
    }

    if (receive) {
        try {
            ::receive(PORT);
        } catch (std::exception &ex) {
            std::cerr << "File receive failed: " << ex.what() << '\n';
            return 1;
        }
    }


    return 0;
}