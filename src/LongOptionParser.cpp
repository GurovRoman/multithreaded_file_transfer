#include "LongOptionParser.h"

LongOptionParser::LongOptionParser(char **argv, const std::initializer_list<std::string> &options,
                                   const std::initializer_list<std::string> &options_with_argument) {

    std::unordered_set option_set(options);
    std::unordered_set option_arg_set(options_with_argument);
    ++argv;
    while (*argv != nullptr) {
        std::string arg = *argv;
        if (option_set.count(arg)) {
            parsed_options_.insert(arg);
        } else if (option_arg_set.count(arg)) {
            if (argv[1] == nullptr) {
                throw std::runtime_error("Argument expected for option: " + arg);
            }
            ++argv;
            parsed_arguments_[arg] = *argv;
        } else {
            throw std::runtime_error("Unexpected option: " + arg);
        }
        ++argv;
    }
}

bool LongOptionParser::contains(const std::string &option) {
    return parsed_options_.count(option) || parsed_arguments_.count(option);
}

std::string LongOptionParser::operator[](const std::string &option) {
    if (!parsed_arguments_.count(option)) {
        return "";
    }
    return parsed_arguments_[option];
}
