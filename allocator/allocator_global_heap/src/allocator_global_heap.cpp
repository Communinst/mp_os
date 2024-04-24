#include "not_implemented.h"
#include "../include/allocator_global_heap.h"

//
allocator_global_heap::allocator_global_heap(
    logger *logger) : _logger(logger)
{
    trace_with_guard(get_typename() + "constructor: Successfully called and completed.");
}
//

//
allocator_global_heap::~allocator_global_heap()
{
    trace_with_guard(get_typename() + "destructor: Successfully called and completed.");
}
// 

//
allocator_global_heap::allocator_global_heap(
    allocator_global_heap &&other) noexcept
{
    trace_with_guard(get_typename() + "move_constructor: Move constructor has been launched.");
    _logger = other._logger;
    other._logger = nullptr;
    trace_with_guard(get_typename() + "move_constructor: Move constructor succeed.");
}
//

//
allocator_global_heap &allocator_global_heap::operator=(
    allocator_global_heap &&other) noexcept
{
    if (this != &other)
    {
        trace_with_guard(get_typename() + "move_operator=: Move operator= has been launched.\n");
        _logger = other._logger;
        other._logger = nullptr;
    }
    trace_with_guard(get_typename() + "move_operator=: Move operator= has been launched.\n");
    return *this;
}
//

//
[[nodiscard]] void *allocator_global_heap::allocate(
    size_t value_size,
    size_t values_count)
{
    debug_with_guard(get_typename() + "allocate: Allocating segment of approapriate size...");

    void *mem_seg = nullptr;
    try
    {
        mem_seg = ::operator new(value_size * values_count + sizeof(size_t) + sizeof(allocator_global_heap*));
    }
    catch(const std::exception& ex)
    {
        error_with_guard(get_typename() + "allocate: Allocation failed.");
        throw ex;
    }

    auto **allocator = reinterpret_cast<allocator_global_heap**>(mem_seg);
    *allocator = this;
    auto *size = reinterpret_cast<size_t*>(allocator + 1);
    *size = (values_count * value_size);
    mem_seg = reinterpret_cast<void*>(size + 1);

    debug_with_guard(get_typename() + "allocate: Allocating succeed.");
    return mem_seg;

}
//

//
allocator_global_heap* allocator_global_heap::get_allocator(void *const mem_seg) const noexcept
{
    trace_with_guard(get_typename() + "get_allocator: Handling allocator byte...");

    auto *size = reinterpret_cast<size_t*>(mem_seg);
    auto **allocator = reinterpret_cast<allocator_global_heap**>(size - 1);

    trace_with_guard(get_typename() + "get_allocator: Allocator obtained.");

    return (*(allocator - 1));

}
//

//
size_t allocator_global_heap::get_size(void *const mem_seg) const noexcept
{
    trace_with_guard(get_typename() + "get_size: Handling size byte...");

    auto *size = reinterpret_cast<size_t*>(mem_seg);

    trace_with_guard(get_typename() + "get_byte_state: Size obtained.");

    auto buff = *(size - 1);

    return (buff);

}
//

//
std::string allocator_global_heap::get_byte_data(void *const mem_seg) const noexcept
{

    trace_with_guard(get_typename() + "get_byte_state: Handling bytes flow...");
    std::string memory_state;
    size_t size = get_size(mem_seg);

    for (auto *raw_mem = reinterpret_cast<unsigned char *>(mem_seg);
        raw_mem < raw_mem + size--; raw_mem++)
    {
        memory_state += (*raw_mem);
        memory_state += ' ';
    } 
    trace_with_guard(get_typename() + "get_byte_state: Memory state obtained and wraped.");

    return memory_state;

}
//

//
void allocator_global_heap::deallocate(
    void *at)
{
    debug_with_guard(get_typename() + "deallocate: Deallocating transfered segment...");

    std::string byte_state = get_byte_data(at);

    debug_with_guard(get_typename() + "deallocate: Current memory state:" + byte_state);

    if (get_allocator(at) == this)
    {
        ::operator delete(reinterpret_cast<unsigned char *>(at) - sizeof(size_t*) - sizeof(allocator_global_heap**));
        debug_with_guard(get_typename() + "deallocate: Deallocation succeed.");
    }
    else
    {
        debug_with_guard(get_typename() + "deallocate: Segment was transfered to the wrong allocator.");
        throw std::logic_error("Wrong allocator");
    }
    
}
//

//
inline logger *allocator_global_heap::get_logger() const
{
    return _logger;
}
//

// GBG
inline std::string allocator_global_heap::get_typename() const noexcept
{
    return "Allocator_global_heap::";
}
//
