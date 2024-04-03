#include "../include/client_logger.h"
#include "../include/client_logger_builder.h"


std::unordered_map<std::string, std::pair<std::ostream*, size_t>> client_logger::_loggers_count;

int main(
    int argc,
    char *argv[])
{

    auto builder = new client_logger_builder("\%t \%d \%s \%m");
    logger *lggr1 = nullptr;
    try 
    {
        lggr1 = builder
                    ->add_console_stream(logger::severity::trace)
                    ->add_file_stream("X:\\Coding\\CPP\\mp_os\\logger\\client_logger\\tests\\idiot_savant.txt", 
                                    logger::severity::warning)
                    ->build();
    }
    catch (std::exception& ex)
    {   
        std::cerr << "Error occuried. Description: " << ex.what() << std::endl;
        delete builder;
        delete lggr1;
        return 1;
    }

    lggr1->log("Moron GAY!\n", logger::severity::trace);
    lggr1->log("Hi! I'm that famous savant!\n", logger::severity::warning);
    lggr1->log("Hi! I'm that famous savant!\n", logger::severity::error);
    
    delete lggr1;
    builder->clear();

    try
    {
        /* code */
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    builder->client_logger_builder::transform_with_configuration(
                        "X:\\Coding\\CPP\\mp_os\\logger\\client_logger\\tests\\config.json", 
                        "/loggers/1");
    lggr1 = builder
            ->build();

    lggr1->log("Slavery was great0!\n", logger::severity::trace);
    lggr1->log("Slavery was great1!\n", logger::severity::debug);
    lggr1->log("Slavery was great2!\n", logger::severity::information);
    lggr1->log("Slavery was great3!\n", logger::severity::warning);
    lggr1->log("Slavery was great4!\n", logger::severity::error);
    lggr1->log("Slavery was great5!\n", logger::severity::critical);

    delete lggr1;
    builder->clear();

    lggr1 = builder->build();
    lggr1->log("Slavery was great5!\n", logger::severity::critical);

    delete lggr1;
    delete builder;

}