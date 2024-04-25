#include <not_implemented.h>

#include "mutex"
#include "../include/allocator_boundary_tags.h"

//+
allocator_boundary_tags::~allocator_boundary_tags()
{
    trace_with_guard(get_typename() + "destructor: Launched.\n");

    auto *logger = get_logger();

    get_mutex().~mutex();
    deallocate_with_guard(_trusted_memory);

    if (logger != nullptr)
    {
        logger->log(get_typename() + "destructor: Succesfully completed.\n", logger::severity::debug);
    }

}
//

//+
allocator_boundary_tags::allocator_boundary_tags(
    allocator_boundary_tags &&other) noexcept :
    _trusted_memory(&other._trusted_memory)
{
    trace_with_guard(get_typename() + "move_constructor: Move constructor is up.\n");

    other._trusted_memory = nullptr;

    trace_with_guard(get_typename() + "move_constructor: Move constructor succeeded.\n");
}
//

//+
allocator_boundary_tags &allocator_boundary_tags::operator=(
    allocator_boundary_tags &&other) noexcept
{
    trace_with_guard(get_typename() + "move_operator=: Move operator= has been launched.\n");

    if (_trusted_memory != &other._trusted_memory)
    {

        get_mutex().~mutex();
        deallocate_with_guard(_trusted_memory);

        _trusted_memory = &other._trusted_memory;
        other._trusted_memory = nullptr;

    }

    trace_with_guard(get_typename() + "move_constructor: Move operator= succeeded.\n");

    return *this;
}
//

//+
allocator::block_size_t allocator_boundary_tags::get_allocator_meta_size () const noexcept
{
    return (sizeof(logger*) + sizeof(allocator*) + sizeof(allocator_with_fit_mode::fit_mode) +
            2 * sizeof(allocator::block_size_t) + sizeof(allocator::block_pointer_t) + sizeof(std::mutex));
}
//

//+
allocator_boundary_tags::allocator_boundary_tags(
    size_t space_size,
    allocator *parent_allocator,
    logger *parent_logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    if (parent_logger != nullptr)
    {
        parent_logger->log(get_typename() + "constructor: Launched.\n", logger::severity::debug);
    }
    void *trusted_segment;

    try
    {
        trusted_segment = (parent_allocator == nullptr)
            ? (::operator new(get_allocator_meta_size() + space_size))
            : parent_allocator->allocate(get_allocator_meta_size() + space_size, 1);
    }
    catch(std::exception const &ex)
    {
        error_with_guard(get_typename() + "constructor: Failed due to memory fault.\n");
    }

    _trusted_memory = trusted_segment;
    auto *raw = reinterpret_cast<unsigned char *>(trusted_segment);

    *reinterpret_cast<allocator**>(raw) = parent_allocator;
    *reinterpret_cast<logger**>(raw += sizeof(allocator*)) = parent_logger;
    *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(raw += sizeof(logger*)) = allocate_fit_mode;
    new (reinterpret_cast<std::mutex*>(raw += sizeof(allocator_with_fit_mode::fit_mode))) std::mutex();
    *reinterpret_cast<block_size_t*>(raw += sizeof(std::mutex)) = space_size;
    *reinterpret_cast<block_size_t*>(raw += sizeof(block_size_t)) = space_size;
    *reinterpret_cast<block_pointer_t*>(raw += sizeof(block_size_t)) = nullptr;

    debug_with_guard(get_typename() + "constructor: Successfully complete.\n");

}
//

//+
[[nodiscard]] void *allocator_boundary_tags::allocate(
    size_t value_size,
    size_t values_count)
{

    std::lock_guard<std::mutex> guard_mutex(get_mutex());

    debug_with_guard(get_typename() + "allocate: Allocating segment of appropriate size...\n");
    
    allocator_with_fit_mode::fit_mode fmode = get_fit_mode();
    long long int redundant_size = (fmode == allocator_with_fit_mode::fit_mode::the_worst_fit)
        ? LONG_LONG_MIN
        : LONG_LONG_MAX;

    auto allocator_meta_size = get_allocator_meta_size();
    auto block_meta_size = get_block_meta_size();
    allocator::block_size_t required_size = block_meta_size + value_size * values_count;

    auto *raw = reinterpret_cast<unsigned char*>(_trusted_memory);
    auto *edge = raw + allocator_meta_size + get_allocated();
    
    auto *left = raw + allocator_meta_size;

    auto temp1 = edge - left;

    auto *right = reinterpret_cast<unsigned char*>(*(get_void()));

    allocator::block_pointer_t prev = nullptr;

    unsigned char *appropriate_segment = nullptr;

    while (left <= edge)
    {
        long long int buff = 0;
        if (right == nullptr)
        {
            right = edge;
        }

        if (right == edge || right - left >= required_size)
        {
            buff = right - left - required_size;

            if (redundant_size < 0)
            {
                error_with_guard(get_typename() + "allocate: Allocation failed due to shortage of space.\n")
                    ->debug_with_guard(get_typename() + "allocate: Allocation failed due to shortage of space.\n");
                
                throw std::runtime_error("Lack of space.\n");
            }

            buff = (buff >= block_meta_size) 
                ? 0
                : buff;

            redundant_size = buff;



            if (fmode == allocator_with_fit_mode::fit_mode::first_fit)
            {
                appropriate_segment = left;
                break;
            }
            
            if (fmode == allocator_with_fit_mode::fit_mode::the_best_fit && redundant_size > buff
                || fmode == allocator_with_fit_mode::fit_mode::the_worst_fit && redundant_size < buff)
            {
                appropriate_segment = left;
            }
        }
        prev = left;
        left = right + block_meta_size + get_block_allocated(right);
        right = reinterpret_cast<unsigned char*>(*get_block_next_occupied_ptr(right));
    }

    if (prev == nullptr)
    {
        *(get_void()) = appropriate_segment;
    }
    else
    {
        *get_block_next_occupied_ptr(prev)
            = appropriate_segment;
    }

    if (right != edge)
    {   
        *get_block_prev_occupied_ptr(right)
            = appropriate_segment; 
    }

    *reinterpret_cast<allocator**>(appropriate_segment) = this;

    auto temp4 = value_size * values_count + redundant_size;

    *reinterpret_cast<allocator::block_size_t*>(appropriate_segment += sizeof(allocator*)) 
        = temp4;

    if (redundant_size)
    {
        warning_with_guard(get_typename() + "allocate: " + std::to_string(redundant_size) 
            + " bytes were added to requested size.\n");
    }

    *reinterpret_cast<allocator::block_pointer_t*>(appropriate_segment += sizeof(allocator::block_size_t)) = prev;

    *reinterpret_cast<allocator::block_pointer_t*>(appropriate_segment += sizeof(allocator::block_pointer_t)) = 
        (right == edge) 
            ? nullptr
            : *reinterpret_cast<allocator::block_pointer_t*>(right);

    get_available() -= required_size + redundant_size;
    auto temp3 = get_available();

    information_with_guard(get_typename() + "allocate: Amount of available bytes: " + std::to_string(get_available()) + '\n');

    std::vector<allocator_test_utils::block_info> blocks_data = get_blocks_info_with_guard();

    display_blocks(blocks_data);
    
    debug_with_guard('\n' + get_typename() + "allocate: Allocation succeeded.\n");

    return reinterpret_cast<void*>(appropriate_segment += sizeof(allocator::block_pointer_t));;  

}
//

//+
void allocator_boundary_tags::deallocate(
    void *at)
{
    std::lock_guard<std::mutex> guard_mutex(get_mutex());

    debug_with_guard(get_typename() + "deallocate: Deallocating transfered segment...\n");
    if (at == nullptr)
    {
        debug_with_guard(get_typename() + "deallocate: Segment is corrupted.\n");
    }

    std::string byte_state = get_byte_state(at);
    debug_with_guard(get_typename() + "deallocate: Current memory state:" + byte_state + '\n');

    auto *next = *get_block_next_occupied_ptr(at);
    auto *prev = *get_block_prev_occupied_ptr(at);
    
    if (prev != nullptr)
    {
        *get_block_next_occupied_ptr(prev) = next;
    }
    if (next != nullptr)
    {
        *get_block_prev_occupied_ptr(next) = prev;
    }
    
    *reinterpret_cast<allocator**>(get_block_allocator(at)) = nullptr;
    get_block_allocated(at) = 0;
    *get_block_next_occupied_ptr(at) = nullptr;
    *get_block_prev_occupied_ptr(at) = nullptr;

    get_available() -= get_block_allocated(at);

    debug_with_guard(get_typename() + "deallocate: Deallocation succeeded.\n");

}
//

//
std::string allocator_boundary_tags::get_byte_state(void *at) const noexcept
{
    trace_with_guard(get_typename() + "get_byte_state: Launched and ontained.\n");

    auto *raw = reinterpret_cast<unsigned char *>(at);
    allocator::block_size_t mem_size = get_block_allocated(raw - get_block_meta_size());
    std::string result;

    for (int i = 0; i < mem_size; i++)
    {
        result += *(raw + i);
        result += ' ';
    }

    return result;
}
//

//
inline void allocator_boundary_tags::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    trace_with_guard(get_typename() + "set_fit_mode: Launched and completed.");
    get_fit_mode() = mode;

}
//

//
inline allocator *allocator_boundary_tags::get_allocator() const
{
    trace_with_guard(get_typename() + "get_allocator: Launched and obtained.");
    auto *raw = reinterpret_cast<unsigned char *>(_trusted_memory);
    return (*reinterpret_cast<allocator**>(raw));
}
//

//
inline logger *allocator_boundary_tags::get_logger() const
{
    auto *raw = reinterpret_cast<unsigned char *>(_trusted_memory);
    logger* log = *reinterpret_cast<logger**>(raw + sizeof(allocator*));

    if (log != nullptr)
    {
        log->log(get_typename() + "get_logger: Launched and successfully completed.\n", logger::severity::trace);
    }

    return log;
}
//

//
inline allocator_with_fit_mode::fit_mode &allocator_boundary_tags::get_fit_mode() const noexcept
{
    trace_with_guard(get_typename() + "get_fit_mode: Launched and obtained.");
    auto *raw = reinterpret_cast<unsigned char *>(_trusted_memory);
    return (*reinterpret_cast<allocator_with_fit_mode::fit_mode *>(raw + sizeof(logger*) 
                                                                    + sizeof(allocator*)));      
}
//

//
inline std::mutex &allocator_boundary_tags::get_mutex() const noexcept
{
    trace_with_guard(get_typename() + "get_mutex: Launched and obtained.");
    auto *raw = reinterpret_cast<unsigned char *>(_trusted_memory);
    return (*reinterpret_cast<std::mutex*>(raw + sizeof(logger*) + sizeof(allocator*)
                + sizeof(allocator_with_fit_mode::fit_mode)));
}
//

//
inline allocator::block_size_t &allocator_boundary_tags::get_allocated() const noexcept
{
    trace_with_guard(get_typename() + "get_available: Launched and obtained.");
    auto *raw = reinterpret_cast<unsigned char *>(_trusted_memory);
    return (*reinterpret_cast<allocator::block_size_t*>(raw + sizeof(logger*) + sizeof(allocator*)
                + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(std::mutex)));
}
//

//
inline allocator::block_size_t &allocator_boundary_tags::get_available() const noexcept
{
    trace_with_guard(get_typename() + "get_available: Launched and obtained.");
    auto *raw = reinterpret_cast<unsigned char *>(_trusted_memory);
    return (*reinterpret_cast<size_t*>(raw + sizeof(logger*) + sizeof(allocator*)
            + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(std::mutex) + sizeof(size_t)));
}
//

//
inline allocator::block_pointer_t* allocator_boundary_tags::get_void() const noexcept
{
    trace_with_guard(get_typename() + "get_void: Launched and obtained.");
    auto *raw = reinterpret_cast<unsigned char *>(_trusted_memory);
    return (reinterpret_cast<allocator::block_pointer_t*>(raw + sizeof(logger*) + sizeof(allocator*)
            + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(std::mutex) + 2 * sizeof(allocator::block_size_t)));
}
//

//
inline allocator::block_size_t allocator_boundary_tags::get_block_meta_size() const noexcept 
{
    trace_with_guard(get_typename() + "get_block_meta_size: Launched and obtained.");
    return (sizeof(allocator*) + sizeof(allocator::block_size_t) + 2 * sizeof(allocator::block_pointer_t));
}
//

//
inline allocator* allocator_boundary_tags::get_block_allocator(allocator::block_pointer_t at) const noexcept
{
    trace_with_guard(get_typename() + "get_block_allocator: Launched and obtained.");
    return (*reinterpret_cast<allocator**>(at));
}
//

//
inline allocator::block_size_t &allocator_boundary_tags::get_block_allocated(allocator::block_pointer_t at) const noexcept
{
    trace_with_guard(get_typename() + "get_block_allocated: Launched and obtained.");
    auto *raw = reinterpret_cast<unsigned char *>(at);
    return (*reinterpret_cast<allocator::block_size_t*>(raw + sizeof(allocator*)));
}
//

//
inline allocator::block_pointer_t *allocator_boundary_tags::get_block_prev_occupied_ptr(allocator::block_pointer_t at) const noexcept
{
    trace_with_guard(get_typename() + "get_block_prev_occupied_ptr: Launched and obtained.");
    if (at == nullptr)
    {
        return nullptr;
    }
    auto *raw = reinterpret_cast<unsigned char *>(at);
    return (reinterpret_cast<allocator::block_pointer_t*>(raw + sizeof(allocator*) + sizeof(allocator::block_size_t)));
}
//

//
inline allocator::block_pointer_t *allocator_boundary_tags::get_block_next_occupied_ptr(allocator::block_pointer_t at) const noexcept
{
    trace_with_guard(get_typename() + "get_block_next_occupied_ptr: Launched and obtained.");
    if (at == nullptr)
    {
        return nullptr;
    }
    auto *raw = reinterpret_cast<unsigned char *>(at);
    return (reinterpret_cast<allocator::block_pointer_t*>(raw + sizeof(allocator*) 
        + sizeof(allocator::block_size_t) + sizeof(allocator::block_pointer_t)));
}
//

//+
std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const noexcept 
{ 
    std::lock_guard<std::mutex> guard_mutex(get_mutex()); 
    
    debug_with_guard(get_typename() + "get_blocks_info: Launched.\n");

    auto blocks_collection = collect_blocks_info();

    debug_with_guard(get_typename() + "get_blocks_info: succeeded.\n");

    return blocks_collection;

}
//

//
std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info_with_guard() const noexcept 
{
    
    trace_with_guard(get_typename() + "get_blocks_info_with_guard: Launched.\n");

    auto blocks_collection = collect_blocks_info();

    trace_with_guard(get_typename() + "get_blocks_info_with_guard: Succeeded.\n");

    return blocks_collection;

}
//

//
std::vector<allocator_test_utils::block_info> allocator_boundary_tags::collect_blocks_info() const noexcept
{
    trace_with_guard(get_typename() +  "get_blocks_info: Obtaining state of blocks...\n"); 

    std::vector<allocator_test_utils::block_info> blocks_data; 
    allocator::block_size_t block_meta_size = get_block_meta_size(); 

    unsigned char *previous_occupied = reinterpret_cast<unsigned char*>(_trusted_memory) + get_allocator_meta_size(); 
    auto *edge = previous_occupied + get_allocated(); 
    auto *current_occupied = reinterpret_cast<unsigned char*>(*get_void()); 
    bool state = 0; 

    while (previous_occupied != edge) 
    { 
        allocator::block_size_t size = 0; 
        bool state = 0; 
        current_occupied = current_occupied == nullptr 
            ? edge 
            : current_occupied; 
        auto temp1 = current_occupied - previous_occupied;
        if (previous_occupied != current_occupied) 
        { 
            state = 0; 
            size = current_occupied - previous_occupied; 
            blocks_data.push_back({size, state}); 
            previous_occupied = current_occupied; 
        } 
        if (current_occupied != edge)
        {
            state = 1; 
            size = get_block_allocated(current_occupied); 
            current_occupied = reinterpret_cast<unsigned char*>(*get_block_next_occupied_ptr(current_occupied)); 
            previous_occupied += size + block_meta_size; 
            
            blocks_data.push_back({size, state}); 
        }
        
    } 

    trace_with_guard(get_typename() +  "get_blocks_info: Lowdown obtained successfully.\n"); 

    return blocks_data; 
}
//

//
inline void allocator_boundary_tags::display_blocks(std::vector<allocator_test_utils::block_info> blocks_data) const noexcept
{
    std::string string_block_state;
    for (allocator_test_utils::block_info &block : blocks_data)
    {
        std::string state = block.is_block_occupied
            ? "occup"
            : "avail";
        string_block_state += " <" + state + ">: " + std::to_string(block.block_size) + " |";
        
    }
    debug_with_guard(get_typename() + "Current memory state:\n|" + string_block_state);
}
//

//
inline std::string allocator_boundary_tags::get_typename() const noexcept
{
    return "Allocator_boundary_tags:: ";
}
//

//
unsigned char *allocator_boundary_tags::show_edge_address() const noexcept
{
    return (reinterpret_cast<unsigned char*>(_trusted_memory) + get_allocator_meta_size() + get_allocated());
}
//
