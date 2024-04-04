#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H


#include <map>
#include <set>
#include <logger_builder.h>
#include "../include/client_logger.h"

class client_logger_builder final:
    public logger_builder
{

private:

    std::map<std::string, std::set<logger::severity>> _settings;
    std::string _formated_data;
    

public:

    client_logger_builder();
    client_logger_builder(std::string format);
    
    client_logger_builder(
        client_logger_builder const &other) = delete;

    client_logger_builder &operator=(
        client_logger_builder const &other) = delete;

    client_logger_builder(
        client_logger_builder &&other) noexcept = delete;

    client_logger_builder &operator=(
        client_logger_builder &&other) noexcept = delete;

    ~client_logger_builder() noexcept override;

public:

    logger_builder *add_file_stream(
        std::string const &stream_file_path,
        logger::severity severity) noexcept override;

    logger_builder *add_console_stream(
        logger::severity severity) noexcept override;

    logger_builder *transform_with_configuration(
        std::string const &configuration_file_path,
        std::string const &configuration_path) throw() override;
    
    logger_builder *clear() noexcept override;

    [[nodiscard]] logger *build() const override;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H