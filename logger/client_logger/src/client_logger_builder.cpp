
#include <iostream>
#include <nlohmann/json.hpp>
#include "../include/client_logger.h"
#include "../include/client_logger_builder.h"

////
client_logger_builder::client_logger_builder() : 
    _formated_data("\%t \%d \%s \%m") {}
//

////
client_logger_builder::client_logger_builder(std::string format) : 
    _formated_data(format) {}
//

////
client_logger_builder::~client_logger_builder() noexcept
{
}
//

////
logger_builder *client_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity) noexcept
{
    _settings[stream_file_path].insert(severity);
    return this;
}
//

////
logger_builder *client_logger_builder::add_console_stream(
    logger::severity severity) noexcept
{
    _settings["Console"].insert(severity);
    return this;
}
//

////
logger_builder* client_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path) throw()
{
    // Open file as json
    std::ifstream config_fdata(configuration_file_path);
    if (!config_fdata.is_open())
    {
        throw std::runtime_error("File seems to be absent\n");
    }
    nlohmann::json data;
    config_fdata >> data;
    //
    
    // Follow the path
    nlohmann::json nested_elem = data.at(nlohmann::json::json_pointer(configuration_path));
    if (nested_elem == nullptr)
    {
        throw std::runtime_error("Requested config is missing!\n");
    }
    //

    //Set values in the class fields
    _formated_data = nested_elem["format"];
    for (auto elem : nested_elem["streams"])
    {
        std::string const path = elem["path"];
        if (path == "Console")
        {
            for (logger::severity severity : elem["severity"])
            {
                add_console_stream(severity);
            }
        }
        else
        {
            for (logger::severity severity : elem["severity"])
            {
                add_file_stream(path, severity);
            }
        }
    }
    //

    return this;

}
//

////
logger_builder *client_logger_builder::clear() noexcept
{
    _settings.clear();
    _formated_data.clear();

    return this;
}
//

////
logger *client_logger_builder::build() const
{
    return (dynamic_cast<logger*>(new client_logger(_formated_data, _settings)));
}
//

