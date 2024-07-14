#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_BIGINT_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_BIGINT_H

#include <iostream>
#include <map>
#include <vector>
#include <optional>

#include <allocator.h>
#include <allocator_guardant.h>
#include <not_implemented.h>

class big_integer final:
    allocator_guardant
{

#pragma region multiplication properties

public:
    
    enum class multiplication_rule
    {
        trivial,
        Karatsuba,
        SchonhageStrassen
    };

private:
    
    class multiplication
    {
    
    public:
        
        virtual ~multiplication() noexcept = default;
    
    public:
        
        virtual big_integer &multiply(
            big_integer &first_multiplier,
            big_integer const &second_multiplier) const = 0;
        
    };
    
    class trivial_multiplication final:
        public multiplication
    {
    
    public:
        
        big_integer &multiply(
            big_integer &first_multiplier,
            big_integer const &second_multiplier) const override;
        
    };
    
    class Karatsuba_multiplication final:
        public multiplication
    {

    public:
        
        big_integer &multiply(
            big_integer &first_multiplier,
            big_integer const &second_multiplier) const override;
        
    };
    
    class Schonhage_Strassen_multiplication final:
        public multiplication
    {

    public:
        
        big_integer &multiply(
            big_integer &first_multiplier,
            big_integer const &second_multiplier) const override;
        
    };

#pragma endregion multiplication properties



#pragma region division properties

public:
    
    enum class division_rule
    {
        trivial,
        Newton,
        BurnikelZiegler
    };

private:
    
    class division
    {
    
    public:
        
        virtual ~division() noexcept = default;
    
    public:
        
        virtual big_integer &divide(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const = 0;
        
        virtual big_integer &modulo(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const = 0;
        
    };
    
    class trivial_division final:
        public division
    {
    
    public:
    
        big_integer &divide(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const override;
        
        big_integer &modulo(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const override;
    
    private:
        
        std::pair<std::optional<big_integer>, big_integer> divide_with_remainder(
            big_integer const &dividend,
            big_integer const &divisor,
            bool eval_quotient,
            big_integer::multiplication_rule multiplication_rule) const;
    
    
    };
    
    class Newton_division final:
        public division
    {
    
    public:
        
        big_integer &divide(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const override;
        
        big_integer &modulo(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const override;
        
    };
    
    class Burnikel_Ziegler_division final:
        public division
    {
    
    public:
        
        big_integer &divide(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const override;
        
        big_integer &modulo(
            big_integer &dividend,
            big_integer const &divisor,
            big_integer::multiplication_rule multiplication_rule) const override;
        
    };

#pragma endregion division properties



#pragma region iteration_tools

private:

    class const_iterator final
    {
        
    private:
        
        big_integer const *_holder;
        int _pos;
        
    public:
    
        const_iterator(
            big_integer const *holder,
            int pos = 0);
        
    public:
    
        bool operator==(
            const_iterator const &other);
        
        bool operator!=(
            const_iterator const &other);
        
        unsigned int operator*();
        
        const_iterator operator++();
        
        const_iterator operator++(
            int unused);
    
    };
    
    class const_reverse_iterator final
    {
        
    private:
        
        big_integer const *_holder;
        int _pos;
        
    public:
    
        const_reverse_iterator(
            big_integer const *holder,
            int pos = 0);
        
    public:
    
        bool operator==(
            const_reverse_iterator const &other);
        
        bool operator!=(
            const_reverse_iterator const &other);
        
        unsigned int operator*();
        
        const_reverse_iterator operator++();
        
        const_reverse_iterator operator++(
            int unused);
    
    };
    
    class half_const_iterator final
    {
        
    private:
        
        big_integer const *_holder;
        int _pos;
        bool _is_oldest;
        
    public:
    
        half_const_iterator(
            big_integer const *holder,
            int pos = 0,
            bool is_oldest = false);
        
    public:
    
        bool operator==(
            half_const_iterator const &other);
        
        bool operator!=(
            half_const_iterator const &other);
        
        unsigned int operator*();
        
        half_const_iterator operator++();
        
        half_const_iterator operator++(
            int unused);
    
    public:
    
        inline bool is_oldest() noexcept;
    
    };

private:

    const_iterator cbegin() const;
    
    const_iterator cend() const;
    
    const_reverse_iterator crbegin() const;
    
    const_reverse_iterator crend() const;
    
    half_const_iterator half_cbegin() const;
    
    half_const_iterator half_cend() const;

#pragma endregion iteration_tools



#pragma region basic properties

private:

    int _oldest_digit;
    unsigned int *_other_digits;
    allocator *_allocator;

public:
    
    big_integer(
        int digit,
        allocator *allocator = nullptr);
    
    big_integer(
        int const *digits,
        size_t digits_amount,
        allocator *allocator = nullptr);

    explicit big_integer(
        std::vector<int> const &digits,
        allocator *allocator = nullptr);

    explicit big_integer(
        std::string const &value_as_string,
        size_t base = 10,
        allocator *allocator = nullptr);
    
    explicit big_integer(
        big_integer const &other,
        allocator *allocator);

public:
    
    ~big_integer() noexcept;
    
    big_integer(
        big_integer const &other);

    big_integer &operator=(
        big_integer const &other);
    
    big_integer(
        big_integer &&other) noexcept;
    
    big_integer &operator=(
        big_integer &&other) noexcept;

private:

    [[nodiscard]] allocator *get_allocator() const noexcept override;

#pragma endregion basic properties



#pragma region provided operations

public:

    
    bool operator==(
        big_integer const &other) const;

    bool operator!=(
        big_integer const &other) const;

public:
    
    bool operator<(
        big_integer const &other) const;

    bool operator>(
        big_integer const &other) const;

    bool operator<=(
        big_integer const &other) const;

    bool operator>=(
        big_integer const &other) const;

public:

    big_integer operator-() const;
    
    big_integer &operator+=(
        big_integer const &other);

    big_integer operator+(
        big_integer const &other) const;

    big_integer operator+(
        std::pair<big_integer, allocator *> const &other) const;
    
    big_integer &operator-=(
        big_integer const &other);

    big_integer operator-(
        big_integer const &other) const;

    big_integer operator-(
        std::pair<big_integer, allocator *> const &other) const;
    
    big_integer &operator*=(
        big_integer const &other);

    big_integer operator*(
        big_integer const &other) const;

    big_integer operator*(
        std::pair<big_integer, allocator *> const &other) const;
    
    big_integer &operator/=(
        big_integer const &other);

    big_integer operator/(
        big_integer const &other) const;

    big_integer operator/(
        std::pair<big_integer, allocator *> const &other) const;

    big_integer &operator%=(
        big_integer const &other);

    big_integer operator%(
        big_integer const &other) const;

    big_integer operator%(
        std::pair<big_integer, allocator *> const &other) const;

public:
    
    big_integer operator~() const;

    big_integer &operator&=(
        big_integer const &other);

    big_integer operator&(
        big_integer const &other) const;

    big_integer operator&(
        std::pair<big_integer, allocator *> const &other) const;

    big_integer &operator|=(
        big_integer const &other);

    big_integer operator|(
        big_integer const &other) const;

    big_integer operator|(
        std::pair<big_integer, allocator *> const &other) const;

    big_integer &operator^=(
        big_integer const &other);

    big_integer operator^(
        big_integer const &other) const;

    big_integer operator^(
        std::pair<big_integer, allocator *> const &other) const;

    big_integer &operator<<=(
        size_t shift);

    big_integer operator<<(
        size_t shift) const;

    big_integer operator<<(
        std::pair<size_t, allocator *> const &shift) const;

    big_integer &operator>>=(
        size_t shift);

    big_integer operator>>(
        size_t shift) const;

    big_integer operator>>(
        std::pair<size_t, allocator *> const &shift) const;

#pragma endregion provided operations



#pragma region UI

public:
    
    static big_integer &multiply(
        big_integer &first_multiplier,
        big_integer const &second_multiplier,
        allocator *allocator = nullptr,
        big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer multiply(
        big_integer const &first_multiplier,
        big_integer const &second_multiplier,
        allocator *allocator = nullptr,
        big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer &divide(
        big_integer &dividend,
        big_integer const &divisor,
        allocator *allocator = nullptr,
        big_integer::division_rule division_rule = big_integer::division_rule::trivial,
        big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer divide(
        big_integer const &dividend,
        big_integer const &divisor,
        allocator *allocator = nullptr,
        big_integer::division_rule division_rule = big_integer::division_rule::trivial,
        big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer &modulo(
        big_integer &dividend,
        big_integer const &divisor,
        allocator *allocator = nullptr,
        big_integer::division_rule division_rule = big_integer::division_rule::trivial,
        big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

    static big_integer modulo(
        big_integer const &dividend,
        big_integer const &divisor,
        allocator *allocator = nullptr,
        big_integer::division_rule division_rule = big_integer::division_rule::trivial,
        big_integer::multiplication_rule multiplication_rule = big_integer::multiplication_rule::trivial);

#pragma endregion UI



#pragma region I/O

public:
    
    friend std::ostream &operator<<(
        std::ostream &stream,
        big_integer const &value);
    
    friend std::istream &operator>>(
        std::istream &stream,
        big_integer &value);

#pragma endregion I/O 



#pragma region time savers

public:
    
    int get_digits_amount() const noexcept;

    inline int sign() const noexcept;

    inline bool is_equal_to_zero() const noexcept;
    
    inline bool is_equal_to_one() const noexcept;

    inline unsigned int get_digit(
        int position) const noexcept;

    void dump_value(
        std::ostream &stream) const;
    
    std::string to_string() const;

private:
    
    big_integer &clear();
    
    big_integer &copy_from(
        big_integer const &other,
        allocator *allocator);
        
    big_integer &move_from(
        big_integer &&other);
    
    big_integer &initialize_from(
        int const *digits,
        size_t digits_amount);

    big_integer &initialize_from(
        std::vector<int> const &digits,
        size_t digits_amount);

    big_integer &initialize_from(
        std::string const &value,
        size_t base);
    
    big_integer &change_sign();
    
    static void print_byte(
        std::ostream &stream,
        unsigned char byte_value);

    static void dump_int_value(
        std::ostream &stream,
        int value);
    
    static size_t get_significant_digits_cnt(
        int const *digits,
        size_t digits_amount,
        bool forced_to_be_positive = false);
    
    static size_t get_significant_digits_cnt(
        std::vector<int> const &digits,
        bool forced_to_be_positive = false);
    
    static std::pair<std::optional<big_integer>, big_integer> divide_with_remainder(
        big_integer const &dividend,
        big_integer const &divisor,
        bool eval_quotient,
        big_integer::multiplication_rule multiplication_rule = multiplication_rule::trivial);
    
    static unsigned int char_to_int(
        char ch);

#pragma endregion time savers



#pragma region MA(E)TH

public:
    
    static big_integer gcd(big_integer const &a, big_integer const &b);
    
    big_integer abs() const;

#pragma endregion MA(E)TH

};





#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_BIGINT_H