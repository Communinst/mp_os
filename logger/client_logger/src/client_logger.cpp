
#include <iostream>
#include <fstream> 
#include <exception>
#include "../include/client_logger.h"

//
client_logger::client_logger(std::string const& format, 
                            std::map<std::string, std::set<logger::severity>> const& streams) :
    _log_format(format) 
{
   for (auto iter = streams.begin();
            iter != streams.end(); iter++)
    {
        std::pair<std::ostream*, std::set<logger::severity>> to_add;
        to_add.second = (*iter).second;
        auto is_open = _loggers_count.find((*iter).first);

        if (is_open != _loggers_count.end())
        {
            to_add.first = (*is_open).second.first;
        }
        else 
        {
            if ((*iter).first == "Console")
            {
                to_add.first = &(std::cout);
                _loggers_count[(*iter).first].first = to_add.first;
            }
            else
            {
                std::ofstream *new_stream = nullptr;
                try
                {
                    new_stream = new std::ofstream((*iter).first);
                    if (!new_stream->is_open())
                    {
                        throw std::exception();
                    }
                }
                catch(const std::exception& e)
                {
                    //if it was allocated but the file turned out to be inappropriate
                    delete new_stream;

                    for (auto emerg_iter = _log_streams.begin();
                            emerg_iter != _log_streams.end(); emerg_iter++)
                    {
                        client_logger_expel(emerg_iter);
                        _log_streams.erase(emerg_iter);
                    }
                    throw;
                }
                to_add.first = dynamic_cast<std::ostream*>(new_stream);
                _loggers_count[(*iter).first].first = to_add.first;
            }
        }

        _log_streams[(*iter).first] = to_add;
        _loggers_count[(*iter).first].second++;


    }
}
//

//
void client_logger::client_logger_expel(std::map<std::string, std::pair<std::ostream *, 
                                        std::set<logger::severity>>>::iterator &iter) noexcept
{
    auto current_elem = _loggers_count.find((*iter).first);
    auto& descryptor = current_elem->second.first;
    auto& amount = current_elem->second.second;

    if (amount-- <= 1 and (descryptor != &(std::cout)))
    {
        delete (descryptor);
        _loggers_count.erase(current_elem);
    }
}
//  

//
client_logger::client_logger(
    client_logger const &other) :
    _log_format(other._log_format)
{

    for (auto iter = other._log_streams.begin(); 
            iter++ != other._log_streams.end(); )
    {
        std::pair<std::ostream*, std::set<logger::severity>> buff_copy;
        buff_copy = iter->second;
        _log_streams[iter->first] = buff_copy;

        _loggers_count[iter->first].second++;
    }
}
//

//
client_logger &client_logger::operator=(
    client_logger const &other)
{
    return *this = client_logger(other);
}
//

//
client_logger::client_logger(
    client_logger &&other) noexcept :
    _log_format(std::move(other._log_format)),
    _log_streams(std::move(other._log_streams))
    {}
//

//
client_logger &client_logger::operator=(
    client_logger &&other) noexcept
{
    if (this != &other)
    {
        _log_format = std::move(other._log_format);
        _log_streams = std::move(other._log_streams);
    }
    return *this;
}   
//

//
client_logger::~client_logger() noexcept
{
    for (auto iter = _log_streams.begin(); 
            iter != _log_streams.end(); iter++)
    {
        client_logger::client_logger_expel(iter);
    }
}
//

//
logger const *client_logger::log(
    const std::string &text,
    logger::severity severity) const noexcept
{
    std::string log = (client_logger::log_parser(text, severity));
    for (auto iter = _log_streams.begin(); 
            iter != _log_streams.end(); iter++)
    {
        std::pair<std::ostream*, std::set<logger::severity>> buff_data = iter->second;

        if ((buff_data.second).find(severity) != (buff_data.second).end())
        {
            *(buff_data.first) << log;
        }

    }
    return this;
}
//

//
[[nodiscard]] std::string const client_logger::log_parser(const std::string &text, logger::severity severity) const
{

    std::string log_mes;

    for (auto format_iter = _log_format.begin(); 
            format_iter != _log_format.end(); format_iter++)
    {   

        switch (*format_iter)
        {
            case 't':
                log_mes += "[" + current_time_to_string() + ']';
                break;

            case 'd':
                log_mes += "[" + current_date_to_string() + ']';
                break;

            case 's':   
                log_mes += "[" + severity_to_string(severity) + ']';
                break;

            case 'm':
                log_mes += "\tMessage: " + text;
                break;

            default:
                break;
        }
    }
    return log_mes;

}
//
