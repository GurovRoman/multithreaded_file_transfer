#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>

class LongOptionParser {
public:
    LongOptionParser() = default;

    LongOptionParser(char **argv, const std::initializer_list<std::string> &options,
                     const std::initializer_list<std::string> &options_with_argument);

    bool contains(const std::string &option);

    std::string operator[](const std::string &option);

private:

    std::unordered_set<std::string> parsed_options_;
    std::unordered_map<std::string, std::string> parsed_arguments_;
};

