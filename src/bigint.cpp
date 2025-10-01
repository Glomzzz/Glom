//
// Created by glom on 9/30/25.
//

#include "type.h"

#include <cstdint>
#include <utility>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <bit>
#include <cassert>
#include <charconv>
#include <iostream>
#include <random>

int DecimalConverter::char_to_digit(const char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    throw std::invalid_argument("Invalid decimal digit of " + std::string(1, c));
}

char DecimalConverter::digit_to_char(const size_t d)
{
    return '0' + d;
}

UBigInt DecimalConverter::from_string(const std::string& decimal_str)
{
    if (decimal_str.empty())
    {
        return UBigInt(0);
    }

    const std::string& str = decimal_str;
    UBigInt result(0);

    if (str.length() <= 40)
    {
        for (const char c : str)
        {
            result = result * UBigInt(10) + UBigInt(char_to_digit(c));
        }
    }
    else
    {
        result = from_string_fast(str);
    }

    return result;
}

std::string DecimalConverter::to_string(const UBigInt& num)
{
    if (num.is_zero())
    {
        return "0";
    }

    // Estimate the length of the resulting string, each digit is about log2(10) ~ 3.32 bits
    const size_t estimated_length = num.bit_length() * 301 / 1000 + 2;
    std::string result;
    result.reserve(estimated_length);

    const UBigInt& n = num;
    UBigInt ten(10);

    return to_string_fast(n);
}

UBigInt DecimalConverter::from_string_fast(const std::string& str)
{
    // Split the string into chunks of up to 19 digits
    // 19 digits fit into uint64_t (max 10^19 < 2^64)
    constexpr size_t CHUNK_SIZE = 19;
    UBigInt result(0);
    UBigInt multiplier(1);

    // From right to left, process each chunk
    for (int128_t i = str.length(); i > 0; i -= CHUNK_SIZE)
    {
        const size_t start = i > CHUNK_SIZE ? i - CHUNK_SIZE : 0;
        const size_t chunk_length = i - start;
        std::string chunk = str.substr(start, chunk_length);

        // Convert chunk to uint64_t
        uint64_t chunk_value = 0;
        for (const char c : chunk)
        {
            chunk_value = chunk_value * 10 + char_to_digit(c);
        }

        // Accumulate the result
        result = result + UBigInt(chunk_value) * multiplier;

        // Update multiplier for the next chunk
        for (size_t j = 0; j < chunk_length; ++j)
        {
            multiplier = multiplier * UBigInt(10);
        }
    }

    return result;
}

std::string DecimalConverter::to_string_fast(const UBigInt& n)
{
    if (n < UBigInt(1000000000))
    {
        // Small number, use simple conversion
        return to_string_simple(n);
    }

    // Divide and conquer approach
    auto [high, low] = split_decimal(n);

    const std::string high_str = to_string_fast(high);
    std::string low_str = to_string_fast(low);

    // Ensure low part is zero-padded to 9 digits
    constexpr size_t target_length = 9;
    while (low_str.length() < target_length)
    {
        low_str.insert(low_str.begin(), '0');
    }

    return high_str + low_str;
}

std::string DecimalConverter::to_string_simple(const UBigInt& n)
{
    if (n.is_zero()) return "0";

    UBigInt num = n;
    std::string result;
    const UBigInt ten(10);

    while (!num.is_zero())
    {
        auto [quotient, remainder] = num.divide(ten);
        result += digit_to_char(remainder.get_data()[0]);
        num = quotient;
    }

    std::ranges::reverse(result);
    return result;
}

std::pair<UBigInt, UBigInt> DecimalConverter::split_decimal(const UBigInt& n)
{
    const UBigInt divisor(POW10_9);
    auto [high, low] = n.divide(divisor);
    return {high, low};
}


void UBigInt::normalize()
{
    while (!data.empty() && data.back() == 0)
    {
        data.pop_back();
    }
    if (data.empty())
    {
        data.push_back(0);
    }
}

void UBigInt::shift_left(const size_t bits)
{
    if (is_zero()) return;

    const size_t word_shift = bits / 64;
    const size_t bit_shift = bits % 64;

    data.resize(data.size() + word_shift + (bit_shift > 0 ? 1 : 0), 0);

    if (word_shift > 0)
    {
        for (size_t i = data.size() - 1; i >= word_shift; i--)
        {
            data[i] = data[i - word_shift];
        }
        for (size_t i = 0; i < word_shift; i++)
        {
            data[i] = 0;
        }
    }

    if (bit_shift > 0)
    {
        uint64_t carry = 0;
        for (size_t i = word_shift; i < data.size(); i++)
        {
            const uint64_t val = data[i];
            data[i] = (val << bit_shift) | carry;
            carry = val >> (64 - bit_shift);
        }
    }

    normalize();
}

void UBigInt::shift_right(const size_t bits)
{
    if (is_zero()) return;

    const size_t word_shift = bits / 64;
    const size_t bit_shift = bits % 64;

    if (word_shift >= data.size())
    {
        data = {0};
        return;
    }

    if (word_shift > 0)
    {
        for (size_t i = 0; i < data.size() - word_shift; i++)
        {
            data[i] = data[i + word_shift];
        }
        data.resize(data.size() - word_shift);
    }

    if (bit_shift > 0)
    {
        uint64_t carry = 0;
        for (size_t i = data.size() - 1; i != static_cast<size_t>(-1); i--)
        {
            const uint64_t val = data[i];
            data[i] = (val >> bit_shift) | carry;
            carry = val << (64 - bit_shift);
        }
    }

    normalize();
}

UBigInt::UBigInt() : data{0}
{
}

UBigInt::UBigInt(const uint64_t value) : data{value}
{
}

UBigInt::UBigInt(std::vector<uint64_t> values) : data(std::move(values)) { normalize(); }

UBigInt::UBigInt(const UBigInt& other) = default;

UBigInt::UBigInt(const std::string& value) : UBigInt(DecimalConverter::from_string(value))
{
}

UBigInt& UBigInt::operator=(const UBigInt& other) = default;

UBigInt::UBigInt(UBigInt&& other) noexcept = default;
UBigInt& UBigInt::operator=(UBigInt&& other) noexcept = default;

bool UBigInt::is_zero() const
{
    return data.size() == 1 && data[0] == 0;
}

bool UBigInt::is_one() const
{
    return data.size() == 1 && data[0] == 1;
}

size_t UBigInt::bit_length() const
{
    if (is_zero()) return 1;
    const size_t bits = (data.size() - 1) * 64;
    const uint64_t top_word = data.back();
    return bits + 64 - std::countl_zero(top_word);
}

const std::vector<uint64_t>& UBigInt::get_data() const { return data; }

UBigInt UBigInt::operator+(const UBigInt& other) const
{
    const auto& a = data;
    const auto& b = other.data;
    const size_t max_size = std::max(a.size(), b.size());

    std::vector<uint64_t> result(max_size + 1, 0);
    uint128_t carry = 0;

    for (size_t i = 0; i < max_size; ++i)
    {
        uint128_t sum = carry;
        if (i < a.size()) sum += a[i];
        if (i < b.size()) sum += b[i];

        result[i] = static_cast<uint64_t>(sum);
        carry = sum >> 64;
    }

    if (carry)
    {
        result[max_size] = static_cast<uint64_t>(carry);
    }
    else
    {
        result.pop_back();
    }

    return UBigInt(result);
}

UBigInt UBigInt::operator-(const UBigInt& other) const
{
    if (*this < other)
    {
        throw std::underflow_error("Subtraction would result in negative value");
    }

    const auto& a = data;
    const auto& b = other.data;
    std::vector<uint64_t> result(a.size(), 0);
    uint64_t borrow = 0;

    for (size_t i = 0; i < a.size(); ++i)
    {
        uint128_t diff = static_cast<uint128_t>(a[i]) - borrow;
        if (i < b.size()) diff -= b[i];

        if (diff > UINT64_MAX)
        {
            borrow = 1;
            diff += static_cast<uint128_t>(1) << 64;
        }
        else
        {
            borrow = 0;
        }

        result[i] = static_cast<uint64_t>(diff);
    }

    return UBigInt(result);
}

UBigInt UBigInt::operator*(const UBigInt& other) const
{
    if (is_zero() || other.is_zero())
    {
        return UBigInt(0);
    }

    const auto& a = data;
    const auto& b = other.data;
    std::vector<uint64_t> result(a.size() + b.size(), 0);

    for (size_t i = 0; i < a.size(); ++i)
    {
        uint128_t carry = 0;
        for (size_t j = 0; j < b.size(); ++j)
        {
            const uint128_t product = static_cast<uint128_t>(a[i]) * b[j] +
                result[i + j] + carry;
            result[i + j] = static_cast<uint64_t>(product);
            carry = product >> 64;
        }
        if (carry)
        {
            result[i + b.size()] = static_cast<uint64_t>(carry);
        }
    }

    return UBigInt(result);
}

std::pair<UBigInt, UBigInt> UBigInt::divide(const UBigInt& divisor) const
{
    if (divisor.is_zero())
    {
        throw std::invalid_argument("Division by zero");
    }

    if (*this < divisor)
    {
        return {UBigInt(0), *this};
    }

    if (divisor.data.size() == 1)
    {
        return divide_single_word(divisor.data[0]);
    }

    return divide_long_division(divisor);
}

UBigInt UBigInt::operator/(const UBigInt& other) const
{
    return divide(other).first;
}

UBigInt UBigInt::operator%(const UBigInt& other) const
{
    return divide(other).second;
}

// Fast exponentiation by squaring
UBigInt UBigInt::pow(const UBigInt& exponent) const
{
    if (exponent.is_zero())
    {
        return UBigInt(1);
    }

    UBigInt result(1);
    UBigInt base = *this;
    UBigInt exp = exponent;

    while (!exp.is_zero())
    {
        if (exp.data[0] & 1)
        {
            result = result * base;
        }
        base = base * base;
        exp = exp / UBigInt(2);
    }

    return result;
}

bool UBigInt::operator==(const UBigInt& other) const
{
    return data == other.data;
}

bool UBigInt::operator!=(const UBigInt& other) const
{
    return !(*this == other);
}

bool UBigInt::operator<(const UBigInt& other) const
{
    if (data.size() != other.data.size())
    {
        return data.size() < other.data.size();
    }

    for (size_t i = data.size(); i > 0; --i)
    {
        if (data[i - 1] != other.data[i - 1])
        {
            return data[i - 1] < other.data[i - 1];
        }
    }

    return false;
}

bool UBigInt::operator<=(const UBigInt& other) const
{
    return *this < other || *this == other;
}

bool UBigInt::operator>(const UBigInt& other) const
{
    return !(*this <= other);
}

bool UBigInt::operator>=(const UBigInt& other) const
{
    return !(*this < other);
}

std::string UBigInt::to_hex_string() const
{
    if (is_zero()) return "0";

    std::string result;
    char buffer[17];

    for (size_t i = data.size(); i > 0; --i)
    {
        if (i == data.size())
        {
            snprintf(buffer, sizeof(buffer), "%lX", data[i - 1]);
        }
        else
        {
            snprintf(buffer, sizeof(buffer), "%016lX", data[i - 1]);
        }
        result += buffer;
    }

    return result;
}

[[nodiscard]] std::pair<UBigInt, UBigInt> UBigInt::divide_single_word(const uint64_t divisor) const
{
    if (divisor == 0) throw std::invalid_argument("Division by zero");

    UBigInt quotient;
    quotient.data.resize(data.size(), 0);

    uint64_t remainder = 0;

    for (size_t i = data.size(); i > 0; --i)
    {
        const uint64_t current = data[i - 1];
        const uint128_t value = static_cast<uint128_t>(remainder) << 64 | current;

        quotient.data[i - 1] = static_cast<uint64_t>(value / divisor);
        remainder = static_cast<uint64_t>(value % divisor);
    }

    quotient.normalize();
    return {quotient, UBigInt(remainder)};
}

[[nodiscard]] std::pair<UBigInt, UBigInt> UBigInt::divide_long_division(const UBigInt& divisor) const
{
    // Handle special cases
    if (*this < divisor)
    {
        return {UBigInt(0), *this};
    }
    if (*this == divisor)
    {
        return {UBigInt(1), UBigInt(0)};
    }

    // Normalize by shifting to make the divisor's MSB set
    int shift = 0;
    uint64_t divisor_msb = divisor.data.back();
    while ((divisor_msb & (static_cast<uint64_t>(1) << 63)) == 0)
    {
        divisor_msb <<= 1;
        shift++;
    }

    UBigInt dividend(*this);
    UBigInt normalized_divisor(divisor);

    if (shift > 0)
    {
        dividend.shift_left(shift);
        normalized_divisor.shift_left(shift);
    }

    const size_t n = normalized_divisor.data.size();
    const size_t m = dividend.data.size() - n;

    UBigInt quotient(0);
    quotient.data.resize(m + 1, 0);

    // Precompute helper values
    const uint64_t divisor_high = normalized_divisor.data[n - 1];
    const uint64_t divisor_high_minus_1 = n > 1 ? normalized_divisor.data[n - 2] : 0;

    for (size_t j = m; j < m + 1; --j)
    {
        // Estimate quotient digit
        const uint64_t dividend_high = j + n < dividend.data.size() ? dividend.data[j + n] : 0;
        const uint64_t dividend_mid = j + n - 1 < dividend.data.size() ? dividend.data[j + n - 1] : 0;
        const uint64_t dividend_low = j + n - 2 < dividend.data.size() ? dividend.data[j + n - 2] : 0;

        const uint128_t value_high = static_cast<uint128_t>(dividend_high) << 64 | dividend_mid;
        auto q_hat = static_cast<uint64_t>(value_high / divisor_high);
        auto r_hat = static_cast<uint64_t>(value_high % divisor_high);

        // Refine quotient estimate
        while (q_hat > UINT64_MAX / divisor_high_minus_1 ||
            (q_hat * divisor_high_minus_1 > ((static_cast<uint128_t>(r_hat) << 64) | dividend_low)))
        {
            q_hat--;
            r_hat += divisor_high;
            if (r_hat < divisor_high) break; // overflow check
        }

        // Multiply and subtract
        UBigInt product = normalized_divisor * UBigInt(q_hat);
        product.shift_left(j * 64);

        if (dividend < product)
        {
            q_hat--;
            product = normalized_divisor * UBigInt(q_hat);
            product.shift_left(j * 64);
        }

        dividend = dividend - product;
        quotient.data[j] = q_hat;
    }

    quotient.normalize();

    // Denormalize remainder
    if (shift > 0)
    {
        dividend.shift_right(shift);
    }

    return {quotient, dividend};
}

UBigInt UBigInt::from_decimal_string(const std::string& str)
{
    return DecimalConverter::from_string(str);
}

std::string UBigInt::to_decimal_string() const
{
    return DecimalConverter::to_string(*this);
}

bool UBigInt::operator<(const uint64_t other) const
{
    if (data.size() > 1) return false;
    return data[0] < other;
}

void BigInt::normalize_zero()
{
    if (magnitude.is_zero())
    {
        is_negative = false;
    }
}

BigInt::BigInt() : magnitude(0), is_negative(false)
{
}

BigInt::BigInt(const BigInt& other) = default;

BigInt::BigInt(BigInt&& other) noexcept = default;

BigInt::BigInt(UBigInt value, const bool negative)
    : magnitude(std::move(value)), is_negative(negative)
{
    normalize_zero();
}

BigInt::BigInt(const std::string& value)
{
    if (value.empty())
    {
        magnitude = UBigInt(0);
        is_negative = false;
        return;
    }

    size_t start = 0;
    if (value[0] == '-')
    {
        is_negative = true;
        start = 1;
    }
    else if (value[0] == '+')
    {
        is_negative = false;
        start = 1;
    }
    else
    {
        is_negative = false;
    }

    magnitude = UBigInt::from_decimal_string(value.substr(start));
    normalize_zero();
}

BigInt::BigInt(const int64_t value)
{
    if (value < 0)
    {
        is_negative = true;
        magnitude = UBigInt(static_cast<uint64_t>(-value));
    }
    else
    {
        is_negative = false;
        magnitude = UBigInt(static_cast<uint64_t>(value));
    }
}

BigInt::BigInt(const uint64_t value) : magnitude(value), is_negative(false)
{
}

BigInt& BigInt::operator=(const BigInt& other) = default;
BigInt& BigInt::operator=(BigInt&& other) noexcept = default;

BigInt BigInt::from_decimal_string(const std::string& str)
{
    return BigInt(str);
}

[[nodiscard]] UBigInt BigInt::abs() const
{
    return magnitude;
}

[[nodiscard]] bool BigInt::is_positive() const
{
    return !is_negative && !magnitude.is_zero();
}

[[nodiscard]] bool BigInt::is_negative_sign() const
{
    return is_negative;
}

[[nodiscard]] bool BigInt::is_zero() const
{
    return magnitude.is_zero();
}

[[nodiscard]] bool BigInt::is_one() const
{
    return magnitude.is_one();
}

[[nodiscard]] BigInt BigInt::operator-() const
{
    if (magnitude.is_zero())
    {
        return *this;
    }
    return BigInt(magnitude, !is_negative);
}

BigInt BigInt::operator+(const BigInt& other) const
{
    if (is_negative == other.is_negative)
    {
        return BigInt(magnitude + other.magnitude, is_negative);
    }
    if (magnitude >= other.magnitude)
    {
        return BigInt(magnitude - other.magnitude, is_negative);
    }
    return BigInt(other.magnitude - magnitude, other.is_negative);
}

BigInt BigInt::operator-(const BigInt& other) const
{
    return *this + -other;
}

BigInt BigInt::operator*(const BigInt& other) const
{
    const bool result_negative = is_negative != other.is_negative;
    return BigInt(magnitude * other.magnitude, result_negative);
}

BigInt BigInt::operator/(const BigInt& other) const
{
    if (other.is_zero())
    {
        throw std::invalid_argument("Division by zero");
    }

    const bool result_negative = is_negative != other.is_negative;
    return BigInt(magnitude / other.magnitude, result_negative);
}

BigInt BigInt::operator%(const BigInt& other) const
{
    if (other.is_zero())
    {
        throw std::invalid_argument("Modulo by zero");
    }

    auto result = BigInt(magnitude % other.magnitude, is_negative);
    if (result.is_negative)
    {
        result = result + BigInt(other.abs());
    }
    return result;
}

[[nodiscard]] BigInt BigInt::pow(const UBigInt& exponent) const
{
    UBigInt result_magnitude = magnitude.pow(exponent);
    const bool negative = is_negative && (exponent.get_data()[0] & 1) == 1 && !magnitude.is_zero();
    return BigInt(std::move(result_magnitude), negative);
}

bool BigInt::operator==(const BigInt& other) const
{
    return is_negative == other.is_negative && magnitude == other.magnitude;
}

bool BigInt::operator!=(const BigInt& other) const
{
    return !(*this == other);
}

bool BigInt::operator<(const BigInt& other) const
{
    if (is_negative != other.is_negative)
    {
        return is_negative;
    }

    if (is_negative)
    {
        return magnitude > other.magnitude;
    }
    return magnitude < other.magnitude;
}

bool BigInt::operator<=(const BigInt& other) const
{
    return *this < other || *this == other;
}

bool BigInt::operator>(const BigInt& other) const
{
    return !(*this <= other);
}

bool BigInt::operator>=(const BigInt& other) const
{
    return !(*this < other);
}

bool BigInt::operator<(const int64_t other) const
{
    return *this < BigInt(other);
}

bool BigInt::operator==(const int64_t other) const
{
    return *this == BigInt(other);
}

[[nodiscard]] std::string BigInt::to_decimal_string() const
{
    std::string mag_str = magnitude.to_decimal_string();
    if (is_negative && !magnitude.is_zero())
    {
        return "-" + mag_str;
    }
    return mag_str;
}

[[nodiscard]] std::string BigInt::to_hex_string() const
{
    std::string mag_str = magnitude.to_hex_string();
    if (is_negative && !magnitude.is_zero())
    {
        return "-" + mag_str;
    }
    return mag_str;
}

[[nodiscard]] size_t BigInt::bit_length() const
{
    return magnitude.bit_length();
}

[[nodiscard]] const std::vector<uint64_t>& BigInt::get_data() const
{
    return magnitude.get_data();
}

real BigInt::to_real() const
{
    return from_string(to_decimal_string());
}
