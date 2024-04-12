#include <not_implemented.h>

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
size_t allocator_boundary_tags::get_meta_size (
    allocator *parent_allocator,
    logger *logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode) const noexcept
{
    return (sizeof(logger) + sizeof(parent_allocator) + sizeof(allocate_fit_mode) +
            2 * sizeof(size_t) + sizeof(void *) + sizeof(unsigned char));
}
//

//
allocator_boundary_tags::allocator_boundary_tags(
    size_t space_size,
    allocator *parent_allocator,
    logger *logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    debug_with_guard(get_typename() + "constructor: Launched");
    void *trusted_segment;

    try
    {
        trusted_segment = (parent_allocator == nullptr)
            ? (::operator new(get_meta_size(parent_allocator, logger, allocate_fit_mode) + space_size))
            : parent_allocator->allocate(get_meta_size(parent_allocator, logger, allocate_fit_mode) + space_size, 1);
    }
    catch(std::exception const &ex)
    {
        error_with_guard(get_typename() + "constructor: Failed due to memory fault.");
    }
    
    allocator **alloc = reinterpret_cast<allocator**>(_trusted_memory);

}
//


[[nodiscard]] void *allocator_boundary_tags::allocate(
    size_t value_size,
    size_t values_count)
{
    throw not_implemented("[[nodiscard]] void *allocator_boundary_tags::allocate(size_t, size_t)", "your code should be here...");
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

inline allocator *allocator_boundary_tags::get_allocator() const
{
    throw not_implemented("inline allocator *allocator_boundary_tags::get_allocator() const", "your code should be here...");
}

std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const noexcept
{
    throw not_implemented("std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const noexcept", "your code should be here...");
}

inline logger *allocator_boundary_tags::get_logger() const
{
    throw not_implemented("inline logger *allocator_boundary_tags::get_logger() const", "your code should be here...");
}

inline std::string allocator_boundary_tags::get_typename() const noexcept
{
    return "Allocator_boundary_tags";
}