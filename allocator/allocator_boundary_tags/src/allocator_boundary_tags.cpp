#include <not_implemented.h>

#include "mutex"
#include "../include/allocator_boundary_tags.h"


allocator_boundary_tags::~allocator_boundary_tags()
{
    throw not_implemented("allocator_boundary_tags::~allocator_boundary_tags()", "your code should be here...");
}

allocator_boundary_tags::allocator_boundary_tags(
    allocator_boundary_tags const &other)
{
    throw not_implemented("allocator_boundary_tags::allocator_boundary_tags(allocator_boundary_tags const &)", "your code should be here...");
}

allocator_boundary_tags &allocator_boundary_tags::operator=(
    allocator_boundary_tags const &other)
{
    throw not_implemented("allocator_boundary_tags &allocator_boundary_tags::operator=(allocator_boundary_tags const &)", "your code should be here...");
}

allocator_boundary_tags::allocator_boundary_tags(
    allocator_boundary_tags &&other) noexcept
{
    throw not_implemented("allocator_boundary_tags::allocator_boundary_tags(allocator_boundary_tags &&) noexcept", "your code should be here...");
}

allocator_boundary_tags &allocator_boundary_tags::operator=(
    allocator_boundary_tags &&other) noexcept
{
    throw not_implemented("allocator_boundary_tags &allocator_boundary_tags::operator=(allocator_boundary_tags &&) noexcept", "your code should be here...");
}

//
size_t allocator_boundary_tags::get_meta_size () const noexcept
{
    return (sizeof(logger*) + sizeof(allocator*) + sizeof(allocator_with_fit_mode::fit_mode) +
            2 * sizeof(size_t) + sizeof(void *) + sizeof(std::mutex));
}
//

//
allocator_boundary_tags::allocator_boundary_tags(
    size_t space_size,
    allocator *parent_allocator,
    logger *parent_logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    debug_with_guard(get_typename() + "constructor: Launched");
    void *trusted_segment;

    try
    {
        trusted_segment = (parent_allocator == nullptr)
            ? (::operator new(get_meta_size() + space_size))
            : parent_allocator->allocate(get_meta_size() + space_size, 1);
    }
    catch(std::exception const &ex)
    {
        error_with_guard(get_typename() + "constructor: Failed due to memory fault.");
    }

    auto *raw = reinterpret_cast<unsigned char *>(trusted_segment);

    *reinterpret_cast<allocator**>(raw) = parent_allocator;
    *reinterpret_cast<logger**>(raw += sizeof(allocator*)) = parent_logger;
    *reinterpret_cast<allocator_with_fit_mode::fit_mode*>(raw += sizeof(logger*)) = allocate_fit_mode;
    new (reinterpret_cast<std::mutex*>(raw += sizeof(allocator_with_fit_mode::fit_mode))) std::mutex();
    *reinterpret_cast<block_size_t*>(raw += sizeof(std::mutex)) = space_size;
    *reinterpret_cast<block_size_t*>(raw += sizeof(block_size_t)) = 0;
    *reinterpret_cast<void**>(raw += sizeof(block_size_t)) = nullptr;

    debug_with_guard(get_typename() + "constructor: Successfully complete.");

}
//


[[nodiscard]] void *allocator_boundary_tags::allocate(
    size_t value_size,
    size_t values_count)
{
    
}

void allocator_boundary_tags::deallocate(
    void *at)
{
    throw not_implemented("void allocator_boundary_tags::deallocate(void *)", "your code should be here...");
}

inline void allocator_boundary_tags::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    throw not_implemented("inline void allocator_boundary_tags::set_fit_mode(allocator_with_fit_mode::fit_mode)", "your code should be here...");
}

//
inline allocator *allocator_boundary_tags::get_allocator() const
{
    trace_with_guard(get_typename() + "get_allocator: Called and obtained.");
    auto *raw = reinterpret_cast<unsigned char *>(_trusted_memory);
    return (*reinterpret_cast<allocator**>(raw));
}
//

//
inline logger *allocator_boundary_tags::get_logger() const
{
    trace_with_guard(get_typename() + "get_logger: Called and obtained.");
    auto *raw = reinterpret_cast<unsigned char *>(_trusted_memory);
    return (*reinterpret_cast<logger**>(raw + sizeof(allocator*)));
}
//

//
inline allocator_with_fit_mode::fit_mode allocator_boundary_tags::get_fit_mode() const
{
    trace_with_guard(get_typename() + "get_fit_mode: Called and obtained.");
    auto *raw = reinterpret_cast<unsigned char *>(_trusted_memory);
    return (*reinterpret_cast<allocator_with_fit_mode::fit_mode *>(raw + sizeof(logger*) 
                                                                    + sizeof(allocator*)));      
}
//

//
inline std::mutex *allocator_boundary_tags::get_mutex() const
{
    trace_with_guard(get_typename() + "get_mutex: Called and obtained.");
    auto *raw = reinterpret_cast<unsigned char *>(_trusted_memory);
    return (reinterpret_cast<std::mutex*>(raw + sizeof(logger*) + sizeof(allocator*)
                + sizeof(allocator_with_fit_mode::fit_mode)));
}
//

//
inline size_t &allocator_boundary_tags::get_available() const
{
    trace_with_guard(get_typename() + "get_available: Called and obtained.");
    auto *raw = reinterpret_cast<unsigned char *>(_trusted_memory);
    return (*reinterpret_cast<size_t*>(raw + sizeof(logger*) + sizeof(allocator*)
                + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(std::mutex)));
}
//

//
inline size_t &allocator_boundary_tags::get_occupied() const
{
    trace_with_guard(get_typename() + "get_occupied: Called and obtained.");
    auto *raw = reinterpret_cast<unsigned char *>(_trusted_memory);
    return (*reinterpret_cast<size_t*>(raw + sizeof(logger*) + sizeof(allocator*)
            + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(std::mutex) + sizeof(size_t)));
}
//

//
inline void *allocator_boundary_tags::get_void() const noexcept
{
    trace_with_guard(get_typename() + "get_void: Called and obtained.");
    auto *raw = reinterpret_cast<unsigned char *>(_trusted_memory);
    return (*reinterpret_cast<void**>(raw + sizeof(logger*) + sizeof(allocator*)
            + sizeof(allocator_with_fit_mode::fit_mode) + sizeof(std::mutex) + 2 * sizeof(size_t)));
}
//


std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const noexcept
{
    throw not_implemented("std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const noexcept", "your code should be here...");
}


//
inline std::string allocator_boundary_tags::get_typename() const noexcept
{
    return "Allocator_boundary_tags";
}
//
