#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H


#include <map>
#include <unordered_map>
#include <set>
#include <fstream>
#include <logger.h>
#include "../include/client_logger_builder.h"

class client_logger final:
    public logger
    
{
    public:
    
        friend class client_logger_builder;

    private:

        std::map<std::string, std::pair<std::ostream*, std::set<logger::severity>>> _log_streams;
        static std::unordered_map<std::string, std::pair<std::ostream*, size_t>> _loggers_count;
        std::string _log_format;

    private: 

        [[nodiscard]] std::string const log_parser(const std::string &text, logger::severity severity) const;

        client_logger(std::string const& _format, std::map<std::string, std::set<logger::severity>> const& _streams);

        void client_logger_expel(std::map<std::string, std::pair<std::ostream *, std::set<logger::severity>>>::iterator &iter) noexcept;

    public:

        client_logger(
            client_logger const &other);

        client_logger &operator=(
            client_logger const &other);

        client_logger(
            client_logger &&other) noexcept;

        client_logger &operator=(
            client_logger &&other) noexcept;

        ~client_logger() noexcept final;

    public:

        [[nodiscard]] logger const *log(
            const std::string &message,
            logger::severity severity) const noexcept override;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H