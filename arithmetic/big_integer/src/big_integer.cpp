#include <cstring>
#include <string>
#include <limits>
#include <algorithm>
#include <cmath>

#include "../include/big_integer.h"

#pragma region multiplication implementation

big_integer &big_integer::trivial_multiplication::multiply(
    big_integer &first_multiplier,
    big_integer const &second_multiplier) const
{
    if (&first_multiplier == &second_multiplier)
    {
        return multiply(first_multiplier, big_integer(second_multiplier));
    }
    
    if (second_multiplier.is_equal_to_zero())
    {
        return first_multiplier = second_multiplier;
    }
    
    if (first_multiplier.is_equal_to_zero())
    {
        return first_multiplier;
    }
    
    if (second_multiplier.is_equal_to_one())
    {
        return first_multiplier;
    }
    
    if (first_multiplier.is_equal_to_one())
    {
        return first_multiplier = second_multiplier;
    }
    
    if (first_multiplier.sign() == -1)
    {
        return multiply(first_multiplier.change_sign(), second_multiplier).change_sign();
    }
    
    if (second_multiplier.sign() == -1)
    {
        return multiply(first_multiplier, -second_multiplier).change_sign();
    }
    
    auto const first_value_digits_amount = first_multiplier.get_digits_amount();
    auto const second_value_digits_amount = second_multiplier.get_digits_amount();
    auto const max_digits_amount = first_value_digits_amount + second_value_digits_amount;
    
    constexpr int shift = sizeof(unsigned int) << 2;
    constexpr int mask = (1 << shift) - 1;
    
    std::vector<int> half_digits(2 * max_digits_amount, 0);
    size_t pos_shift = 0;
    
    auto first_iter = first_multiplier.half_cbegin();
    auto first_iter_end = first_multiplier.half_cend();
    
    for (; first_iter != first_iter_end; ++first_iter)
    {
        if (*first_iter == 0)
        {
            ++pos_shift;
            continue;
        }
        
        size_t pos = 0;
        unsigned int operation_result = 0;  
        
        auto second_iter = second_multiplier.half_cbegin();
        auto second_iter_end = second_multiplier.half_cend();
        
        for (; second_iter != second_iter_end; ++second_iter)
        {
            operation_result += *first_iter * *second_iter + half_digits[pos_shift + pos];
            half_digits[pos_shift + pos++] = operation_result & mask;
            
            operation_result >>= shift;
        }
        
        for (; operation_result && (pos_shift + pos) < half_digits.size(); ++pos)
        {
            operation_result += half_digits[pos_shift + pos];
            half_digits[pos_shift + pos++] = operation_result & mask;
            
            operation_result >>= shift;
        }
        
        if (operation_result)
        {
            // TODO logger log error
            throw std::logic_error("too much digit carrying while mupltiplicating");
        }
        
        ++pos_shift;
    }
    
    std::vector<int> result_digits(max_digits_amount);
    
    for (size_t i = 0; i < max_digits_amount; ++i)
    {
        result_digits[i] = (half_digits[2*i + 1] << shift) + half_digits[2*i];
    }
    
    return first_multiplier.clear().initialize_from(result_digits, get_significant_digits_cnt(result_digits, true));
}

big_integer &big_integer::Karatsuba_multiplication::multiply(
    big_integer &first_multiplier,
    big_integer const &second_multiplier) const
{
    throw not_implemented("big_integer &big_integer::Karatsuba_multiplication::multiply(big_integer &, big_integer const &)", "your code should be here...");
}

big_integer &big_integer::Schonhage_Strassen_multiplication::multiply(
    big_integer &first_multiplier,
    big_integer const &second_multiplier) const
{
    throw not_implemented("big_integer &big_integer::Schonhage_Strassen_multiplication::multiply(big_integer &, big_integer const &)", "your code should be here...");
}

#pragma endregion multiplication implementation



#pragma region division implementation

std::pair<std::optional<big_integer>, big_integer> big_integer::trivial_division::divide_with_remainder(
    big_integer const &dividend,
    big_integer const &divisor,
    bool eval_quotient,
    big_integer::multiplication_rule multiplication_rule) const
{
    if (&dividend == &divisor)
    {
        return divide_with_remainder(dividend, big_integer(divisor), eval_quotient, multiplication_rule);
    }
    
    if (divisor.is_equal_to_zero())
    {
        // todo log logger log custom error
        throw std::logic_error("attempt to divide by zero");
    }
    
    if (dividend.is_equal_to_zero())
    {
        return std::make_pair(std::optional(big_integer(0)), big_integer(0));
    }
    
    if (divisor.is_equal_to_one())
    {
        return std::make_pair(std::optional(dividend), big_integer(0));
    }
    
    if (dividend.sign() == -1)
    {
        auto [quotient, remainder] = divide_with_remainder(-dividend, divisor, eval_quotient, multiplication_rule);
        
        if (quotient.has_value())
        {
            return std::make_pair(std::optional(-quotient.value()), -remainder);
        }
        else
        {
            return std::make_pair(std::optional<big_integer>(), -remainder);
        }
    }
    
    if (divisor.sign() == -1)
    {
        auto [quotient, remainder] = divide_with_remainder(dividend, -divisor, eval_quotient, multiplication_rule);
        
        if (quotient.has_value())
        {
            return std::make_pair(std::optional(-quotient.value()), -remainder);
        }
        else
        {
            return std::make_pair(std::optional<big_integer>(), -remainder);
        }
    }
    
    auto const dividend_digits_amount = dividend.get_digits_amount();
    
    std::vector<int> result_digits(1, 0);
    big_integer minuend(0);
    
    for (size_t i = 0; i < dividend_digits_amount; ++i)
    {
        unsigned int cur_digit = dividend.get_digit(dividend_digits_amount - i - 1);
        
        minuend <<= 8 * sizeof(unsigned int);
        minuend += big_integer(std::vector<int>( {*reinterpret_cast<int *>(&cur_digit), 0} ));
        
        if (minuend < divisor)
        {
            result_digits.push_back(0);
        }
        else
        {
            unsigned int digit = 0;
            big_integer subtrahend(0);
            
            for (unsigned int multiplier = 1 << (8*sizeof(unsigned int) - 1); multiplier > 0; multiplier >>= 1)
            {
                int multiplier_int = *reinterpret_cast<int *>(&multiplier);
                big_integer tmp = multiply(divisor, big_integer(std::vector<int>( {multiplier_int, 0} )));
                
                if (minuend >= subtrahend + tmp)
                {
                    subtrahend += tmp;
                    digit += multiplier;
                }
            }
            
            minuend -= subtrahend;
            
            if (eval_quotient)
            {
                result_digits.push_back(digit);
            }
        }
    }
    
    if (eval_quotient)
    {
        std::reverse(result_digits.begin(), result_digits.end());
        
        return std::make_pair(std::optional(big_integer(result_digits)), minuend);
    }
    
    return std::make_pair(std::optional<big_integer>(), minuend);
}

big_integer &big_integer::trivial_division::divide(
    big_integer &dividend,
    big_integer const &divisor,
    big_integer::multiplication_rule multiplication_rule) const
{
    return dividend = divide_with_remainder(dividend, divisor, true, multiplication_rule).first.value();
}

big_integer &big_integer::trivial_division::modulo(
    big_integer &dividend,
    big_integer const &divisor,
    big_integer::multiplication_rule multiplication_rule) const
{
    return dividend = divide_with_remainder(dividend, divisor, true, multiplication_rule).second;
}

big_integer &big_integer::Newton_division::divide(
    big_integer &dividend,
    big_integer const &divisor,
    big_integer::multiplication_rule multiplication_rule) const
{
    throw not_implemented("big_integer &big_integer::Newton_division::divide(big_integer &, big_integer const &, big_integer::multiplication_rule)", "your code should be here...");
}

big_integer &big_integer::Newton_division::modulo(
    big_integer &dividend,
    big_integer const &divisor,
    big_integer::multiplication_rule multiplication_rule) const
{
    throw not_implemented("big_integer &big_integer::Newton_division::modulo(big_integer &, big_integer const &, big_integer::multiplication_rule)", "your code should be here...");
}

big_integer &big_integer::Burnikel_Ziegler_division::divide(
    big_integer &dividend,
    big_integer const &divisor,
    big_integer::multiplication_rule multiplication_rule) const
{
    throw not_implemented("big_integer &big_integer::Burnikel_Ziegler_division::divide(big_integer &, big_integer const &, big_integer::multiplication_rule)", "your code should be here...");
}

big_integer &big_integer::Burnikel_Ziegler_division::modulo(
    big_integer &dividend,
    big_integer const &divisor,
    big_integer::multiplication_rule multiplication_rule) const
{
    throw not_implemented("big_integer &big_integer::Burnikel_Ziegler_division::modulo(big_integer &, big_integer const &, big_integer::multiplication_rule)", "your code should be here...");
}

#pragma endregion multiplication implementation



#pragma region iteration tools implementation


big_integer::const_iterator::const_iterator(
    big_integer const *holder,
    int pos):
        _holder(holder),
        _pos(pos)
{ }
    
bool big_integer::const_iterator::operator==(
    const_iterator const &other)
{
    return _holder == other._holder && _pos == other._pos;
}

bool big_integer::const_iterator::operator!=(
    const_iterator const &other)
{
    return !(*this == other);
}

unsigned int big_integer::const_iterator::operator*()
{
    return _holder->get_digit(_pos);
}

big_integer::const_iterator big_integer::const_iterator::operator++()
{
    if (_pos == _holder->get_digits_amount())
    {
        return *this;
    }
    
    ++_pos;
    
    return *this;
}

big_integer::const_iterator big_integer::const_iterator::operator++(
    int unused)
{
    const_iterator it = *this;
    ++*this;
    return it;
}



big_integer::const_reverse_iterator::const_reverse_iterator(
    big_integer const *holder,
    int pos):
        _holder(holder),
        _pos(pos)
{ }
    
bool big_integer::const_reverse_iterator::operator==(
    const_reverse_iterator const &other)
{
    return _holder == other._holder && _pos == other._pos;
}

bool big_integer::const_reverse_iterator::operator!=(
    const_reverse_iterator const &other)
{
    return !(*this == other);
}

unsigned int big_integer::const_reverse_iterator::operator*()
{
    return _holder->get_digit(_pos);
}

big_integer::const_reverse_iterator big_integer::const_reverse_iterator::operator++()
{
    if (_pos == -1)
    {
        return *this;
    }
    
    --_pos;
    
    return *this;
}

big_integer::const_reverse_iterator big_integer::const_reverse_iterator::operator++(
    int unused)
{
    const_reverse_iterator it = *this;
    ++*this;
    return it;
}



big_integer::half_const_iterator::half_const_iterator(
    big_integer const *holder,
    int pos,
    bool is_oldest):
        _holder(holder),
        _pos(pos),
        _is_oldest(is_oldest)
{ }
    
bool big_integer::half_const_iterator::operator==(
    half_const_iterator const &other)
{
    return _holder == other._holder && _pos == other._pos && _is_oldest == other._is_oldest;
}

bool big_integer::half_const_iterator::operator!=(
    half_const_iterator const &other)
{
    return !(*this == other);
}

unsigned int big_integer::half_const_iterator::operator*()
{
    constexpr int shift = sizeof(unsigned int) << 2;
    constexpr int mask = (1 << shift) - 1;
    
    unsigned int number = _holder->get_digit(_pos);
    
    if (_is_oldest)
    {
        return number >> shift;
    }
    else
    {
        return number & mask;
    }
}

big_integer::half_const_iterator big_integer::half_const_iterator::operator++()
{
    if (_pos == _holder->get_digits_amount())
    {
        return *this;
    }
    
    if (!_is_oldest)
    {
        _is_oldest = true;
        return *this;
    }
    
    ++_pos;
    _is_oldest = false;
    
    return *this;
}

big_integer::half_const_iterator big_integer::half_const_iterator::operator++(
    int unused)
{
    half_const_iterator it = *this;
    ++*this;
    return it;
}

bool big_integer::half_const_iterator::is_oldest() noexcept
{
    return _is_oldest;
}

big_integer::const_iterator big_integer::cbegin() const
{
    return const_iterator(this);
}

big_integer::const_iterator big_integer::cend() const
{
    return const_iterator(this, get_digits_amount());
}

big_integer::const_reverse_iterator big_integer::crbegin() const
{
    return const_reverse_iterator(this, get_digits_amount() - 1);
}

big_integer::const_reverse_iterator big_integer::crend() const
{
    return const_reverse_iterator(this, -1);
}

big_integer::half_const_iterator big_integer::half_cbegin() const
{
    return half_const_iterator(this);
}

big_integer::half_const_iterator big_integer::half_cend() const
{
    return half_const_iterator(this, get_digits_amount());
}


#pragma endregion iteration tools implementation



#pragma region big_int basics implementation

big_integer::big_integer(
    int digit,
    allocator *allocator):
        _oldest_digit(digit),
        _other_digits(nullptr),
        _allocator(allocator)
{ }

big_integer::big_integer(
    int const *digits,
    size_t digits_amount,
    allocator *allocator):
        _allocator(allocator)
{
    initialize_from(digits, get_significant_digits_cnt(digits, digits_amount));
}

big_integer::big_integer(
    std::vector<int> const &digits,
    allocator *allocator):
        _allocator(allocator)
{
    initialize_from(digits, get_significant_digits_cnt(digits));
}

big_integer::big_integer(
    std::string const &value_as_string,
    size_t base,
    allocator *allocator):
        _allocator(allocator)
{
    initialize_from(value_as_string, base);
}

big_integer::big_integer(
    big_integer const &other,
    allocator *allocator)
{
    copy_from(other, allocator);
}



big_integer::~big_integer()
{
    clear();
}

big_integer::big_integer(
    big_integer const &other)
{
    copy_from(other, other._allocator);
}

big_integer &big_integer::operator=(
    big_integer const &other)
{
    if (this != &other)
    {
        clear();
        copy_from(other, other._allocator);
    }
    
    return *this;
}

big_integer::big_integer(
    big_integer &&other) noexcept
{
    move_from(std::move(other));
}

big_integer &big_integer::operator=(
    big_integer &&other) noexcept
{
    if (this != &other)
    {
        clear();
        move_from(std::move(other));
    }
    
    return *this;
}

[[nodiscard]] allocator *big_integer::get_allocator() const noexcept
{
    return _allocator;
}

#pragma endregion big_int basics implementation



#pragma region provided operations implementation


bool big_integer::operator==(
    big_integer const &other) const
{
    if (get_digits_amount() != other.get_digits_amount())
    {
        return false;
    }
    
    auto iter = cbegin();
    auto iter_end = cend();
    auto other_iter = other.cbegin();
    auto other_iter_end = other.cend();
    
    while (iter != iter_end)
    {
        if (*iter != *other_iter)
        {
            return false;
        }
        
        ++iter;
        ++other_iter;
    }
    
    return true;
}


bool big_integer::operator!=(
    big_integer const &other) const
{
    return !(*this == other);
}


bool big_integer::operator<(
    big_integer const &other) const
{
    if (sign() == -1 ^ other.sign() == -1)
    {
        return sign() == -1;
    }
    
    if (get_digits_amount() != other.get_digits_amount())
    {
        return get_digits_amount() < other.get_digits_amount() ^ sign() == -1;
    }
    
    auto iter = crbegin();
    auto iter_end = crend();
    auto other_iter = other.crbegin();
    auto other_iter_end = other.crend();
    
    while (iter != iter_end)
    {
        if (*iter != *other_iter)
        {
            return *iter < *other_iter ^ sign() == -1;
        }
        
        ++iter;
        ++other_iter;
    }
    
    return false;
}


bool big_integer::operator>(
    big_integer const &other) const
{
    if (sign() == -1 ^ other.sign() == -1)
    {
        return sign() == -1;
    }
    
    if (get_digits_amount() != other.get_digits_amount())
    {
        return get_digits_amount() > other.get_digits_amount() ^ sign() == -1;
    }
    
    auto iter = crbegin();
    auto iter_end = crend();
    auto other_iter = other.crbegin();
    auto other_iter_end = other.crend();
    
    while (iter != iter_end)
    {
        if (*iter != *other_iter)
        {
            return *iter > *other_iter ^ sign() == -1;
        }
        
        ++iter;
        ++other_iter;
    }
    
    return false;
}


bool big_integer::operator<=(
    big_integer const &other) const
{
    return !(*this > other);
}


bool big_integer::operator>=(
    big_integer const &other) const
{
    return !(*this < other);
}


big_integer big_integer::operator-() const
{
    return big_integer(*this).change_sign();
}


big_integer &big_integer::operator+=(
    big_integer const &other)
{
    if (other.is_equal_to_zero())
    {
        return *this;
    }
    
    if (is_equal_to_zero())
    {
        return *this = other;
    }
    
    if (sign() == -1)
    {
        return change_sign()
                .operator+=(-other)
                .change_sign();
    }
    
    if (other.sign() == -1)
    {
        return *this -= -other;
    }
    
    auto const first_value_digits_amount = get_digits_amount();
    auto const second_value_digits_amount = other.get_digits_amount();
    auto const digits_amount = std::max(first_value_digits_amount, second_value_digits_amount);
    
    constexpr int shift = sizeof(unsigned int) << 2;
    constexpr int mask = (1 << shift) - 1;
    
    size_t pos = 0;
    std::vector<int> result_digits(digits_amount, 0);
    
    unsigned int operation_result = 0;
    
    auto first_iter = half_cbegin();
    auto first_iter_end = half_cend();
    auto second_iter = other.half_cbegin();
    auto seocnd_iter_end = other.half_cend();
    
    for (; first_iter != first_iter_end || second_iter != seocnd_iter_end; ++first_iter, ++second_iter)
    {
        operation_result += *first_iter + *second_iter;
        result_digits[pos] |= (operation_result & mask) << (first_iter.is_oldest() ? shift : 0);
        
        operation_result >>= shift;
        if (first_iter.is_oldest())
        {
            ++pos;
        }
    }
    
    if (operation_result)
    {
        result_digits.push_back(*reinterpret_cast<int *>(&operation_result));
    }
    
    clear();
    initialize_from(result_digits, get_significant_digits_cnt(result_digits, true));
    
    return *this;
}


big_integer big_integer::operator+(
    big_integer const &other) const
{
    return big_integer(*this) += other;
}


big_integer big_integer::operator+(
    std::pair<big_integer, allocator *> const &other) const
{
    return big_integer(*this, other.second) += other.first;
}


big_integer &big_integer::operator-=(
    big_integer const &other)
{
    if (other.is_equal_to_zero())
    {
        return *this;
    }
    
    if (is_equal_to_zero())
    {
        return *this = -other;
    }
    
    if (sign() == -1)
    {
        return change_sign()
                .operator-=(-other)
                .change_sign();
    }
    
    if (other.sign() == -1)
    {
        return *this += -other;
    }
    
    if (*this < other)
    {
        return *this = -(other - *this);
    }
    
    auto const first_value_digits_amount = get_digits_amount();
    auto const second_value_digits_amount = other.get_digits_amount();
    auto const digits_amount = std::max(first_value_digits_amount, second_value_digits_amount);
    
    unsigned int borrow_cnt = 0;
    constexpr unsigned int decremented_borrow_value = std::numeric_limits<unsigned int>::max();
    
    size_t pos = 0;
    std::vector<int> result_digits(digits_amount, 0);
    
    auto first_iter = cbegin();
    auto first_iter_end = cend();
    auto second_iter = other.cbegin();
    auto seocnd_iter_end = other.cend();
    
    for (; first_iter != first_iter_end || second_iter != seocnd_iter_end; ++first_iter, ++second_iter)
    {
        unsigned int first_value = *first_iter;
        unsigned int second_value = *second_iter;
        
        if ((first_value != 0 && first_value - borrow_cnt >= second_value) ||
                (second_value == 0 && first_value >= borrow_cnt))
        {
            result_digits[pos++] = first_value - second_value - borrow_cnt;
            borrow_cnt = 0;
        }
        else
        {
            result_digits[pos++] = (decremented_borrow_value - second_value - borrow_cnt) + 1 + first_value;
            borrow_cnt = 1;
        }
    }
    
    if (borrow_cnt)
    {
        // todo log logger error
        throw std::logic_error("Borrowing from zero\n");
    }
    
    clear();
    initialize_from(result_digits, get_significant_digits_cnt(result_digits, true));
    
    return *this;
    
}


big_integer big_integer::operator-(
    big_integer const &other) const
{
    return big_integer(*this) -= other;
}


big_integer big_integer::operator-(
    std::pair<big_integer, allocator *> const &other) const
{
    return big_integer(*this, other.second) -= other.first;
}


big_integer &big_integer::operator*=(
    big_integer const &other)
{
    return trivial_multiplication().multiply(*this, other);
}


big_integer big_integer::operator*(
    big_integer const &other) const
{
    return big_integer(*this) *= other;
}


big_integer big_integer::operator*(
    std::pair<big_integer, allocator *> const &other) const
{
    return big_integer(*this, other.second) *= other.first;
}


big_integer &big_integer::operator/=(
    big_integer const &other)
{
    return trivial_division().divide(*this, other, multiplication_rule::trivial);
}


big_integer big_integer::operator/(
    big_integer const &other) const
{
    return big_integer(*this) /= other;
}


big_integer big_integer::operator/(
    std::pair<big_integer, allocator *> const &other) const
{
    return big_integer(*this, other.second) /= other.first;
}


big_integer &big_integer::operator%=(
    big_integer const &other)
{
    return trivial_division().modulo(*this, other, multiplication_rule::trivial);
}


big_integer big_integer::operator%(
    big_integer const &other) const
{
    return big_integer(*this) %= other;
}


big_integer big_integer::operator%(
    std::pair<big_integer, allocator *> const &other) const
{
    return big_integer(*this, other.second) %= other.first;
}


big_integer big_integer::operator~() const
{
    const_iterator iter = cbegin();
    const_iterator iter_end = cend();
    
    std::vector<int> result_digits;
    result_digits.reserve(get_digits_amount());
    
    for (; iter != iter_end; ++iter)
    {
        result_digits.push_back(~*iter);
    }
    
    return big_integer(result_digits);
    
    return *this;
}


big_integer &big_integer::operator&=(
    big_integer const &other)
{
    const_iterator iter = cbegin();
    const_iterator iter_end = cend();
    const_iterator other_iter = other.cbegin();
    const_iterator other_iter_end = other.cend();
    
    std::vector<int> result_digits;
    result_digits.reserve(std::max(get_digits_amount(), other.get_digits_amount()));
    
    for (; iter != iter_end || other_iter != other_iter_end; ++iter, ++other_iter)
    {
        result_digits.push_back(*iter & *other_iter);
    }
    
    clear();
    initialize_from(result_digits, get_significant_digits_cnt(result_digits));
    
    return *this;
}


big_integer big_integer::operator&(
    big_integer const &other) const
{
    return big_integer(*this) &= other;
}


big_integer big_integer::operator&(
    std::pair<big_integer, allocator *> const &other) const
{
    return big_integer(*this, other.second) &= other.first;
}


big_integer &big_integer::operator|=(
    big_integer const &other)
{
    const_iterator iter = cbegin();
    const_iterator iter_end = cend();
    const_iterator other_iter = other.cbegin();
    const_iterator other_iter_end = other.cend();
    
    std::vector<int> result_digits;
    result_digits.reserve(std::max(get_digits_amount(), other.get_digits_amount()));
    
    for (; iter != iter_end || other_iter != other_iter_end; ++iter, ++other_iter)
    {
        result_digits.push_back(*iter | *other_iter);
    }
    
    clear();
    initialize_from(result_digits, get_significant_digits_cnt(result_digits));
    
    return *this;
}


big_integer big_integer::operator|(
    big_integer const &other) const
{
    return big_integer(*this) |= other;
}


big_integer big_integer::operator|(
    std::pair<big_integer, allocator *> const &other) const
{
    return big_integer(*this, other.second) |= other.first;
}


big_integer &big_integer::operator^=(
    big_integer const &other)
{
    const_iterator iter = cbegin();
    const_iterator iter_end = cend();
    const_iterator other_iter = other.cbegin();
    const_iterator other_iter_end = other.cend();
    
    std::vector<int> result_digits;
    result_digits.reserve(std::max(get_digits_amount(), other.get_digits_amount()));
    
    for (; iter != iter_end || other_iter != other_iter_end; ++iter, ++other_iter)
    {
        result_digits.push_back(*iter ^ *other_iter);
    }
    
    clear();
    initialize_from(result_digits, get_significant_digits_cnt(result_digits));
    
    return *this;
}


big_integer big_integer::operator^(
    big_integer const &other) const
{
    return big_integer(*this) ^= other;
}


big_integer big_integer::operator^(
    std::pair<big_integer, allocator *> const &other) const
{
    return big_integer(*this, other.second) ^= other.first;
}


big_integer &big_integer::operator<<=(
    size_t shift)
{
    if (is_equal_to_zero() || shift == 0)
    {
        return *this;
    }
    
    auto value_sign = sign();
    if (value_sign == -1)
    {
        change_sign();
    }
    
    auto const added_young_zeros = shift / (sizeof(unsigned int) << 3);
    shift %= (sizeof(unsigned int) << 3);
    
    constexpr int half_shift = sizeof(unsigned int) << 2;
    constexpr int half_mask = (1 << half_shift) - 1;
    bool is_shift_bigger_than_half = shift >= half_shift;
    shift %= half_shift;
    
    size_t pos = added_young_zeros;
    std::vector<int> result_digits(get_digits_amount() + added_young_zeros + 1, 0);
    
    auto iter = half_cbegin();
    auto iter_end = half_cend();
    
    for (; iter != iter_end; ++iter)
    {
        bool is_writing_oldest_firstly = iter.is_oldest() ^ is_shift_bigger_than_half;
        
        unsigned int operation_result = *iter << shift;
        result_digits[pos] |= (operation_result & half_mask) << (is_writing_oldest_firstly ? half_shift : 0);
        
        if (is_writing_oldest_firstly)
        {
            ++pos;
        }
        
        result_digits[pos] |= (operation_result >> half_shift) << (is_writing_oldest_firstly ? 0 : half_shift);
    }
    
    clear();
    initialize_from(result_digits, get_significant_digits_cnt(result_digits, true));
    
    if (value_sign == -1)
    {
        change_sign();
    }
    
    return *this;
}


big_integer big_integer::operator<<(
    size_t shift) const
{
    return big_integer(*this) <<= shift;
}


big_integer big_integer::operator<<(
    std::pair<size_t, allocator *> const &shift) const
{
    return big_integer(0, shift.second) += big_integer(*this) <<= shift.first;
}


big_integer &big_integer::operator>>=(
    size_t shift)
{
    if (is_equal_to_zero() || shift == 0)
    {
        return *this;
    }
    
    auto value_sign = sign();
    if (value_sign == -1)
    {
        change_sign();
    }
    
    auto const erased_digits = shift / (sizeof(unsigned int) << 3);
    shift %= (sizeof(unsigned int) << 3);
    
    if (erased_digits >= get_digits_amount())
    {
        *this = big_integer(0);
        if (value_sign == -1)
        {
            change_sign();
        }
        return *this;
    }
    
    constexpr int half_shift = sizeof(unsigned int) << 2;
    constexpr int half_mask = (1 << half_shift) - 1;
    bool is_shift_bigger_than_half = shift >= half_shift;
    shift %= half_shift;
    
    size_t pos = 0;
    std::vector<int> result_digits(get_digits_amount() - erased_digits, 0);
    
    auto iter = half_cbegin();
    auto iter_end = half_cend();
    
    for (size_t i = 0; i < 2*erased_digits; ++i)
    {
        ++iter;
    }
    
    for (; iter != iter_end; ++iter)
    {
        bool is_writing_oldest_firstly = iter.is_oldest() == is_shift_bigger_than_half;
        unsigned int operation_result = *iter << half_shift >> shift;
        
        if (pos > 0)
        {
            result_digits[pos - 1] |= (operation_result & half_mask) << (is_writing_oldest_firstly ? half_shift : 0);
        }
        
        if (is_writing_oldest_firstly)
        {
            ++pos;
        }
        
        if (pos > 0)
        {
            result_digits[pos - 1] |= (operation_result >> half_shift) << (is_writing_oldest_firstly ? 0 : half_shift);
        }
    }
    
    clear();
    initialize_from(result_digits, get_significant_digits_cnt(result_digits, true));
    
    if (value_sign == -1)
    {
        change_sign();
    }
    
    return *this;
}


big_integer big_integer::operator>>(
    size_t shift) const
{
    return big_integer(*this) <<= shift;
}


big_integer big_integer::operator>>(
    std::pair<size_t, allocator *> const &shift) const
{
    return big_integer(0, shift.second) += big_integer(*this) <<= shift.first;
}

#pragma endregion provided operations implementation



#pragma region UI implementation


big_integer &big_integer::multiply(
    big_integer &first_multiplier,
    big_integer const &second_multiplier,
    allocator *allocator,
    big_integer::multiplication_rule multiplication_rule)
{
    switch (multiplication_rule)
    {
        case multiplication_rule::trivial:
            return trivial_multiplication().multiply(first_multiplier, second_multiplier);
        case multiplication_rule::Karatsuba: // not implemented
            return Karatsuba_multiplication().multiply(first_multiplier, second_multiplier);
        case multiplication_rule::SchonhageStrassen: // not implemented
            return Schonhage_Strassen_multiplication().multiply(first_multiplier, second_multiplier);
    }
}


big_integer big_integer::multiply(
    big_integer const &first_multiplier,
    big_integer const &second_multiplier,
    allocator *allocator,
    big_integer::multiplication_rule multiplication_rule)
{
    big_integer first_multiplier_copy(first_multiplier, allocator);
    
    switch (multiplication_rule)
    {
        case multiplication_rule::trivial:
            return trivial_multiplication().multiply(first_multiplier_copy, second_multiplier);
        case multiplication_rule::Karatsuba: // not implemented
            return Karatsuba_multiplication().multiply(first_multiplier_copy, second_multiplier);
        case multiplication_rule::SchonhageStrassen: // not implemented
            return Schonhage_Strassen_multiplication().multiply(first_multiplier_copy, second_multiplier);
            
    }
}


big_integer &big_integer::divide(
    big_integer &dividend,
    big_integer const &divisor,
    allocator *allocator,
    big_integer::division_rule division_rule,
    big_integer::multiplication_rule multiplication_rule)
{
    switch (division_rule)
    {
        case division_rule::trivial:
            return trivial_division().divide(dividend, divisor, multiplication_rule);
        case division_rule::Newton: // not implemented
            return Newton_division().divide(dividend, divisor, multiplication_rule);
        case division_rule::BurnikelZiegler: // not implemented
            return Burnikel_Ziegler_division().divide(dividend, divisor, multiplication_rule);
    }
}


big_integer big_integer::divide(
    big_integer const &dividend,
    big_integer const &divisor,
    allocator *allocator,
    big_integer::division_rule division_rule,
    big_integer::multiplication_rule multiplication_rule)
{
    big_integer dividend_copy(dividend, allocator);
    
    switch (division_rule)
    {
        case division_rule::trivial:
            return trivial_division().divide(dividend_copy, divisor, multiplication_rule);
        case division_rule::Newton: // not implemented
            return Newton_division().divide(dividend_copy, divisor, multiplication_rule);
        case division_rule::BurnikelZiegler: // not implemented
            return Burnikel_Ziegler_division().divide(dividend_copy, divisor, multiplication_rule);
    }
}


big_integer &big_integer::modulo(
    big_integer &dividend,
    big_integer const &divisor,
    allocator *allocator,
    big_integer::division_rule division_rule,
    big_integer::multiplication_rule multiplication_rule)
{
    switch (division_rule)
    {
        case division_rule::trivial:
            return trivial_division().modulo(dividend, divisor, multiplication_rule);
        case division_rule::Newton: // not implemented
            return Newton_division().modulo(dividend, divisor, multiplication_rule);
        case division_rule::BurnikelZiegler: // not implemented
            return Newton_division().modulo(dividend, divisor, multiplication_rule);
    }
}


big_integer big_integer::modulo(
    big_integer const &dividend,
    big_integer const &divisor,
    allocator *allocator,
    big_integer::division_rule division_rule,
    big_integer::multiplication_rule multiplication_rule)
{
    big_integer dividend_copy(dividend, allocator);
    
    switch (division_rule)
    {
        case division_rule::trivial:
            return trivial_division().modulo(dividend_copy, divisor, multiplication_rule);
        case division_rule::Newton: // not implemented
            return Newton_division().modulo(dividend_copy, divisor, multiplication_rule);
        case division_rule::BurnikelZiegler: // not implemented
            return Newton_division().modulo(dividend_copy, divisor, multiplication_rule);
    }
}


#pragma endregion UI implementation




#pragma region I/O implementation


std::ostream &operator<<(
    std::ostream &stream,
    big_integer const &value)
{
    return stream << value.to_string();
}


std::istream &operator>>(
    std::istream &stream,
    big_integer &value)
{
    std::string value_as_string;
    
    stream >> value_as_string;
    
    value.clear();
    value.initialize_from(value_as_string, 10);
    
    return stream;
}


#pragma endregion IO implementation




#pragma region time savers implementation


int big_integer::get_digits_amount() const noexcept
{
    return static_cast<int>(_other_digits == nullptr
         ? 1
         : *_other_digits);
}

inline int big_integer::sign() const noexcept
{
    if (is_equal_to_zero())
    {
        return 0;
    }

    return 1 - (static_cast<int>((*reinterpret_cast<unsigned int const *>(&_oldest_digit) >> ((sizeof(int) << 3) - 1))) << 1);
}

inline bool big_integer::is_equal_to_zero() const noexcept
{
    return _oldest_digit == 0 && _other_digits == nullptr;
}

inline bool big_integer::is_equal_to_one() const noexcept
{
    return _oldest_digit == 1 && _other_digits == nullptr;
}

inline unsigned int big_integer::get_digit(
    int position) const noexcept
{
    if (_other_digits == nullptr)
    {
        return position == 0 ? _oldest_digit : 0;
    }

    int const digits_amount = get_digits_amount();
    if (position < digits_amount - 1)
    {
        return _other_digits[position + 1];
    }

    if (position == digits_amount - 1)
    {
        return _oldest_digit;
    }

    return 0;
}

void big_integer::dump_value(
    std::ostream &stream) const
{
    for (auto i = 0; i < get_digits_amount(); ++i)
    {
        auto digit = get_digit(i);
        dump_int_value(stream, *reinterpret_cast<int *>(&digit));
        stream << ' ';
    }
}

std::string big_integer::to_string() const
{
    std::string str;
    str.reserve(11 * get_digits_amount());
    
    big_integer big_number(*this);
    int sign = big_number.sign();
    
    if (sign == -1)
    {
        big_number.change_sign();
    }
    
    size_t const big_modulus_zeros_cnt = static_cast<size_t>(std::log10(std::numeric_limits<unsigned int>::max()));
    size_t big_modulus = 1;
    
    for (size_t i = 0; i < big_modulus_zeros_cnt; ++i)
    {
        big_modulus *= 10;
    }
    
    big_integer modulus(big_modulus);
    
    while (!big_number.is_equal_to_zero())
    {
        auto remainder = big_number % modulus;
        big_number /= modulus;
        
        unsigned int tmp = remainder.get_digit(0);
        for (size_t i = 0; i < big_modulus_zeros_cnt && (tmp || !big_number.is_equal_to_zero()); ++i)
        {
            str.push_back('0' + tmp % 10);
            tmp /= 10;
        }
    }
    
    if (is_equal_to_zero())
    {
        str.push_back('0');
    }
    
    if (sign == -1)
    {
        str.push_back('-');
    }
    
    std::reverse(str.begin(), str.end());
    
    return str;
}

big_integer &big_integer::clear()
{
    deallocate_with_guard(_other_digits);
    
    _oldest_digit = 0;
    _other_digits = nullptr;
    _allocator = nullptr;
    
    return *this;
}

big_integer &big_integer::copy_from(
    big_integer const &other,
    allocator *allocator)
{
    _oldest_digit = other._oldest_digit;
    _other_digits = nullptr;
    _allocator = allocator;
    
    if (other._other_digits == nullptr)
    {
        return *this;
    }
    
    try
    {
        _other_digits = reinterpret_cast<unsigned int *>(allocate_with_guard(sizeof(unsigned int), other.get_digits_amount()));
    }
    catch (std::bad_alloc const &)
    {
        clear();
        // TODO log logger error
        throw;
    }
    
    std::memcpy(_other_digits, other._other_digits, sizeof(unsigned int) * other.get_digits_amount());
    
    return *this;
}

big_integer &big_integer::move_from(
    big_integer &&other)
{
    _oldest_digit = other._oldest_digit;
    _other_digits = other._other_digits;
    _allocator = other._allocator;
    
    other._oldest_digit = 0;
    other._other_digits = nullptr;
    other._allocator = nullptr;
    
    return *this;
}

big_integer &big_integer::initialize_from(
    int const *digits,
    size_t digits_amount)
{
    if (digits == nullptr)
    {
        // TODO log logger error
        throw std::logic_error("pointer to digits array must not be nullptr");
    }

    if (digits_amount == 0)
    {
        // TODO log logger error
        throw std::logic_error("digits array length must be greater than 0");
    }
    
    if (digits_amount > std::numeric_limits<unsigned char>::max())
    {
        // TODO log logger error
        throw std::logic_error("digits array length must be not greater than unsigned int max value");
    }

    _oldest_digit = digits[digits_amount - 1];
    _other_digits = nullptr;
    
    if (digits_amount == 1)
    {
        return *this;
    }
    
    try
    {
        _other_digits = reinterpret_cast<unsigned int *>(allocate_with_guard(sizeof(unsigned int), digits_amount));
    }
    catch (std::bad_alloc const &)
    {
        clear();
        // TODO log logger error
        throw;
    }
    
    *_other_digits = static_cast<unsigned int>(digits_amount);
    
    std::memcpy(_other_digits + 1, digits, sizeof(unsigned int) * (digits_amount - 1));
    
    return *this;
}

big_integer &big_integer::initialize_from(
    std::vector<int> const &digits,
    size_t digits_amount)
{
    if (digits.empty() || digits_amount == 0)
    {
        // TODO log logger error
        throw std::logic_error("std::vector<int> of digits should not be empty");
    }
    
    _oldest_digit = digits_amount <= digits.size() ? digits[digits_amount - 1] : 0;
    _other_digits = nullptr;
    
    if (digits_amount == 1)
    {
        return *this;
    }
    
    try
    {
        _other_digits = reinterpret_cast<unsigned int *>(allocate_with_guard(sizeof(unsigned int), digits_amount));
    }
    catch (std::bad_alloc const &)
    {
        clear();
        // TODO log logger error
        throw;
    }
    
    *_other_digits = static_cast<unsigned int>(digits_amount);
    
    for (auto i = 0; i < digits_amount - 1; ++i)
    {
        _other_digits[1 + i] = (i < digits.size()
                ? *reinterpret_cast<unsigned int const *>(&digits[i])
                : 0);
    }
    
    return *this;
}

big_integer &big_integer::initialize_from(
    std::string const &value_as_string,
    size_t base)
{
    _oldest_digit = 0;
    _other_digits = nullptr;
    
    bool negative_flag = false;
    size_t pos = 0;
    
    if (value_as_string[pos] == '-')
    {
        negative_flag = true;
        ++pos;
    }
    
    for (; pos < value_as_string.size(); ++pos)
    {
        unsigned int digit = char_to_int(value_as_string[pos]);
        
        if (digit >= base)
        {
            throw std::logic_error("Invalid digit");
        }
        
        *this *= big_integer(base, this->_allocator);
        *this += big_integer(digit, this->_allocator);
    }
    
    if (negative_flag)
    {
        change_sign();
    }
    
    return *this;
}

big_integer &big_integer::change_sign()
{
    _oldest_digit ^= (1 << ((sizeof(int) << 3) - 1));

    return *this;
}

void big_integer::print_byte(
    std::ostream &stream,
    unsigned char byte_value)
{
    for (int i = 0; i < 8; i++)
    {
        stream << ((byte_value >> (7 - i)) & 1);
    }
}

void big_integer::dump_int_value(
    std::ostream &stream,
    int value)
{
    auto *p = reinterpret_cast<unsigned char *>(&value);
    for (int i = 0; i < sizeof(int); i++)
    {
        print_byte(stream, *p++);
        stream << ' ';
    }
}

size_t big_integer::get_significant_digits_cnt(
    int const *digits,
    size_t digits_amount,
    bool forced_to_be_positive)
{
    size_t significant_digits_cnt = digits_amount;
    
    while (significant_digits_cnt > 2 && digits[significant_digits_cnt - 1] == 0 && digits[significant_digits_cnt - 2] == 0)
    {
        --significant_digits_cnt;
    }
    
    if (significant_digits_cnt > 1 && digits[significant_digits_cnt - 1] == 0)
    {
        auto maybe_overflowed_digit_ptr = reinterpret_cast<unsigned int const *>(&digits[significant_digits_cnt - 2]);
        if (*maybe_overflowed_digit_ptr >> ((sizeof(unsigned int) << 3) - 1) == 0)
        {
            --significant_digits_cnt;
        }
    }
    
    if (forced_to_be_positive && digits[significant_digits_cnt - 1] != 0)
    {
        auto maybe_overflowed_digit_ptr = reinterpret_cast<unsigned int const *>(&digits[significant_digits_cnt - 1]);
        if (*maybe_overflowed_digit_ptr >> ((sizeof(unsigned int) << 3) - 1) == 1)
        {
            ++significant_digits_cnt;
        }
    }
    
    return significant_digits_cnt;
}

size_t big_integer::get_significant_digits_cnt(
    std::vector<int> const &digits,
    bool forced_to_be_positive)
{
    size_t significant_digits_cnt = digits.size();
    
    while (significant_digits_cnt > 2 && digits[significant_digits_cnt - 1] == 0 && digits[significant_digits_cnt - 2] == 0)
    {
        --significant_digits_cnt;
    }
    
    if (significant_digits_cnt > 1 && digits[significant_digits_cnt - 1] == 0)
    {
        auto maybe_overflowed_digit_ptr = reinterpret_cast<unsigned int const *>(&digits[significant_digits_cnt - 2]);
        if (*maybe_overflowed_digit_ptr >> ((sizeof(unsigned int) << 3) - 1) == 0)
        {
            --significant_digits_cnt;
        }
    }
    
    if (forced_to_be_positive && digits[significant_digits_cnt - 1] != 0)
    {
        auto maybe_overflowed_digit_ptr = reinterpret_cast<unsigned int const *>(&digits[significant_digits_cnt - 1]);
        if (*maybe_overflowed_digit_ptr >> ((sizeof(unsigned int) << 3) - 1) == 1)
        {
            ++significant_digits_cnt;
        }
    }
    
    return significant_digits_cnt;
}

unsigned int big_integer::char_to_int(
    char ch)
{
    if ('0' <= ch && ch <= '9')
    {
        return ch - '0';
    }
    if ('A' <= ch && ch <= 'Z')
    {
        return ch - 'A' + 10;
    }
    if ('a' <= ch && ch <= 'z')
    {
        return ch - 'a' + 36;
    }
    
    // todo logger log error
    throw std::logic_error("Invalid digit");
}

static void swap(
    big_integer &lhs,
    big_integer &rhs)
{
    big_integer tmp = std::move(lhs);
    lhs = std::move(rhs);
    rhs = std::move(lhs);
}


#pragma endregion time savers implementation




#pragma region MA(E)TH implementation


big_integer big_integer::gcd(big_integer const &a, big_integer const &b)
{
    big_integer a_tmp = a.abs();
    big_integer b_tmp = b.abs();
    
    while (!a_tmp.is_equal_to_zero())
    {
        b_tmp %= a_tmp;
        big_integer tmp = std::move(a_tmp);
        a_tmp = std::move(b_tmp);
        b_tmp = std::move(tmp);
    }
    
    return b_tmp;
}
    
big_integer big_integer::abs() const
{
    if (this->sign() == -1)
    {
        return big_integer(*this).change_sign();
    }
    
    return big_integer(*this);
}


#pragma endregion MATH

