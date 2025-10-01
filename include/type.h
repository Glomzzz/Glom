//
// Created by glom on 9/29/25.
//

#ifndef GLOM_TYPE_H
#define GLOM_TYPE_H
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <variant>
typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

#include <vector>
#include <iostream>

using std::variant;
using std::pair;

constexpr auto VOID = 0;
constexpr auto BOOLEAN = 1;
constexpr auto NUMBER_INT = 2;
constexpr auto NUMBER_RAT = 3;
constexpr auto NUMBER_REAL = 4;
constexpr auto STRING = 5;
constexpr auto SYMBOL = 6;
constexpr auto LAMBDA = 7;
constexpr auto PRIMITIVE = 8;
constexpr auto PAIR = 9;


class rational;
using real = long double;

real from_string(const std::string& str);

class UBigInt
{
    std::vector<uint64_t> data;

    void normalize();
    void shift_left(size_t bits);
    void shift_right(size_t bits);

public:
    UBigInt();
    UBigInt(const UBigInt& other);
    explicit UBigInt(const std::string& value);
    explicit UBigInt(uint64_t value);
    explicit UBigInt(std::vector<uint64_t> values);
    UBigInt(UBigInt&& other) noexcept;

    static UBigInt from_decimal_string(const std::string& str);

    UBigInt& operator=(const UBigInt& other);
    UBigInt& operator=(UBigInt&& other) noexcept ;

    [[nodiscard]] bool is_zero() const;
    [[nodiscard]] bool is_one() const;

    [[nodiscard]] size_t bit_length() const;

    [[nodiscard]] const std::vector<uint64_t>& get_data() const;

    [[nodiscard]] std::pair<UBigInt, UBigInt> divide(const UBigInt& divisor) const;

    UBigInt operator+(const UBigInt& other) const;

    UBigInt operator-(const UBigInt& other) const;

    UBigInt operator*(const UBigInt& other) const;

    UBigInt operator/(const UBigInt& other) const;

    UBigInt operator%(const UBigInt& other) const;

    [[nodiscard]] UBigInt pow(const UBigInt& exponent) const;

    bool operator==(const UBigInt& other) const;

    bool operator!=(const UBigInt& other) const;

    bool operator<(const UBigInt& other) const;

    bool operator<=(const UBigInt& other) const;

    bool operator>(const UBigInt& other) const;

    bool operator>=(const UBigInt& other) const;

    [[nodiscard]] std::string to_hex_string() const;


    [[nodiscard]] std::string to_decimal_string() const;

    bool operator<(uint64_t other) const;


private:
    [[nodiscard]] std::pair<UBigInt, UBigInt> divide_single_word(uint64_t divisor) const;

    [[nodiscard]] std::pair<UBigInt, UBigInt> divide_long_division(const UBigInt& divisor) const;
};

class BigInt {
    UBigInt magnitude;
    bool is_negative;

    void normalize_zero();

public:
    BigInt();

    BigInt(const BigInt& other);

    BigInt(BigInt&& other) noexcept;

    explicit BigInt(UBigInt value, bool negative = false);

    explicit BigInt(const std::string& value);

    explicit BigInt(int64_t value);

    explicit BigInt(uint64_t value);

    static BigInt from_decimal_string(const std::string& str);

    BigInt& operator=(const BigInt& other);
    BigInt& operator=(BigInt&& other) noexcept;


    [[nodiscard]] UBigInt abs() const;

    [[nodiscard]] bool is_positive() const;

    [[nodiscard]] bool is_negative_sign() const;

    [[nodiscard]] bool is_zero() const;
    [[nodiscard]] bool is_one() const;

    [[nodiscard]] BigInt operator-() const;

    BigInt operator+(const BigInt& other) const;

    BigInt operator-(const BigInt& other) const;

    BigInt operator*(const BigInt& other) const;

    BigInt operator/(const BigInt& other) const;

    BigInt operator%(const BigInt& other) const;

    [[nodiscard]] BigInt pow(const UBigInt& exponent) const;
    bool operator==(const BigInt& other) const;

    bool operator!=(const BigInt& other) const;

    bool operator<(const BigInt& other) const;

    bool operator<=(const BigInt& other) const;

    bool operator>(const BigInt& other) const;

    bool operator>=(const BigInt& other) const;
    bool operator<(int64_t other) const;

    bool operator==(int64_t other) const;
    [[nodiscard]] std::string to_decimal_string() const;

    [[nodiscard]] std::string to_hex_string() const;
    [[nodiscard]] size_t bit_length() const;
    [[nodiscard]] const std::vector<uint64_t>& get_data() const;

    [[nodiscard]] real to_real() const;
};

class DecimalConverter
{
    static constexpr uint64_t POW10_19 = 10000000000000000000ULL;
    static constexpr uint64_t POW10_18 = 1000000000000000000ULL;
    static constexpr uint64_t POW10_9 = 1000000000ULL;
    static constexpr uint64_t POW10_8 = 100000000ULL;

    static int char_to_digit(char c);

    static char digit_to_char(size_t d);

public:
    static UBigInt from_string(const std::string& decimal_str);

    static std::string to_string(const UBigInt& num);

private:
    static UBigInt from_string_fast(const std::string& str);

    static std::string to_string_fast(const UBigInt& n);

    static std::string to_string_simple(const UBigInt& n);

    static std::pair<UBigInt, UBigInt> split_decimal(const UBigInt& n);
};

using ExprType = std::size_t;


class integer
{
    variant<int64_t, std::unique_ptr<BigInt>> value;

public:
    integer();
    explicit integer(int64_t val);
    explicit integer(BigInt val);
    explicit integer(const std::string& str);

    integer(const integer& other);
    integer(integer&& other) noexcept;

    ~integer();

    integer& operator=(const integer& other);
    integer& operator=(integer&& other) noexcept;

    [[nodiscard]] bool is_int64() const;
    [[nodiscard]] bool is_bigint() const;
    [[nodiscard]] int64_t as_int64() const;
    [[nodiscard]] const BigInt& as_bigint() const;

    integer operator+(const integer& other) const;
    integer operator-(const integer& other) const;
    integer operator*(const integer& other) const;
    integer operator/(const integer& other) const;
    integer operator-() const;

    [[nodiscard]] integer remainder(const integer& mod) const;
    [[nodiscard]] integer modulo(const integer& mod) const;
    [[nodiscard]] integer pow(const integer& exponent) const;

    bool operator==(const integer& other) const;
    bool operator!=(const integer& other) const;
    bool operator<(const integer& other) const;
    bool operator<=(const integer& other) const;
    bool operator>(const integer& other) const;
    bool operator>=(const integer& other) const;

    [[nodiscard]] bool is_zero() const;
    [[nodiscard]] bool is_one() const;
    [[nodiscard]] bool is_positive() const;
    [[nodiscard]] bool is_negative() const;
    [[nodiscard]] integer abs() const;

    [[nodiscard]] std::string to_decimal_string() const;

    [[nodiscard]] real to_real() const;

    [[nodiscard]] rational to_rational() const;

    [[nodiscard]] BigInt to_bigint() const;

};

class rational {
public:
    integer num;
    integer den;

    void normalize();

    static integer gcd(const integer& a, const integer& b);

    rational();
    explicit rational(integer value);
    explicit rational(int64_t value);
    rational(integer num, integer den);
    rational(int64_t num, int64_t den);
    explicit rational(const std::string& str);

    rational(const rational& other) = default;
    rational(rational&& other) noexcept = default;

    rational& operator=(const rational& other) = default;
    rational& operator=(rational&& other) noexcept = default;

    ~rational() = default;

    [[nodiscard]] const integer& numerator() const { return num; }
    [[nodiscard]] const integer& denominator() const { return den; }

    [[nodiscard]] integer to_integer() const;
    [[nodiscard]] bool is_integer() const;

    rational operator+(const rational& other) const;
    rational operator-(const rational& other) const;
    rational operator*(const rational& other) const;
    rational operator/(const rational& other) const;
    rational operator-() const;

    bool operator==(const rational& other) const;
    bool operator!=(const rational& other) const;
    bool operator<(const rational& other) const;
    bool operator<=(const rational& other) const;
    bool operator>(const rational& other) const;
    bool operator>=(const rational& other) const;

    bool operator==(const integer& other) const;
    bool operator<(const integer& other) const;

    [[nodiscard]] bool is_zero() const;
    [[nodiscard]] bool is_positive() const;
    [[nodiscard]] bool is_negative() const;
    [[nodiscard]] rational abs() const;
    [[nodiscard]] rational reciprocal() const;

    [[nodiscard]] rational pow(const integer& exponent) const;

    [[nodiscard]] std::string to_rational_string() const;

    [[nodiscard]] real to_inexact() const;

    static rational exact_rational(const integer& num, const integer& den);
};



#endif //GLOM_TYPE_H