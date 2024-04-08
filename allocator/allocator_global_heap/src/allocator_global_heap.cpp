#include "not_implemented.h"
#include "../include/allocator_global_heap.h"

//
allocator_global_heap::allocator_global_heap(
    logger *logger) : _logger(logger)
{
    debug_with_guard("Allocator_global_heap::constructor: Successfully called and completed.\n");
}
//

//
allocator_global_heap::~allocator_global_heap()
{
    debug_with_guard("Allocator_global_heap::destructor: Successfully called and completed.\n");
}
// 

//
allocator_global_heap::allocator_global_heap(
    allocator_global_heap &&other) noexcept
    
    
{
    _logger = other._logger;
    other._logger = nullptr;
}
//

//
allocator_global_heap &allocator_global_heap::operator=(
    allocator_global_heap &&other) noexcept
{
    if (this != &other)
    {
        _logger = other._logger;
        other._logger = nullptr;
    }
    return *this;
}
//

//
[[nodiscard]] void *allocator_global_heap::allocate(
    size_t value_size,
    size_t values_count)
{
    debug_with_guard("Allocator_global_heap::allocate: Allocating segment of approapriate size.\n");

    void *mem_seg = nullptr;
    try
    {
        mem_seg = ::operator new(value_size * values_count + sizeof(size_t) + sizeof(allocator_global_heap*));
        auto **allocator = reinterpret_cast<allocator_global_heap**>(mem_seg);
        *allocator = this;
        auto *size = reinterpret_cast<size_t*>(mem_seg + 1);
        *size = (values_count * value_size);
        mem_seg = reinterpret_cast<void*>(size + 1);
    }
    catch(const std::exception& ex)
    {
        error_with_guard("Allocator_global_heap::allocate: Allocation failed.\n");
        throw ex;
    }

    debug_with_guard("Allocator_global_heap::allocate: Allocating succeed.\n");
    return mem_seg;

}
//



//
void allocator_global_heap::deallocate(
    void *at)
{
    debug_with_guard("Allocator_global_heap::deallocate: Deallocating handed over segment...\n");

    std::string current_mem_statel = nullptr;
    
}
//

//
inline logger *allocator_global_heap::get_logger() const
{
    
}
//

// GBG
inline std::string allocator_global_heap::get_typename() const noexcept
{
    std::cout << "What are you expecting for?\n" << std::endl;
}
//