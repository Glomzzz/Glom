//
// Created by glom on 9/30/25.
//

#include <algorithm>
#include <charconv>
#include <cmath>
#include <iomanip>

#include "type.h"
#include <limits>
#include <ranges>
integer::integer() : value(static_cast<int64_t>(0)) {}

integer::integer(int64_t val) : value(val) {}

integer::integer(BigInt val) : value(std::make_unique<BigInt>(std::move(val))) {}


integer::integer(const std::string& str) {
    int64_t i64 = 0;
    const auto [_, ec] = std::from_chars(
        str.data(), str.data() + str.size(),i64
    );
    if (ec == std::errc::result_out_of_range)
    {
        value = std::make_unique<BigInt>(BigInt::from_decimal_string(str));
    }
    else if (ec != std::errc())
    {
        throw std::runtime_error("Invalid integer format: " + str);
    }
    else
    {
        value = i64;
    }
}

integer::integer(const integer& other) {
    if (other.is_int64()) {
        value = other.as_int64();
    } else {
        value = std::make_unique<BigInt>(other.as_bigint());
    }
}

integer::integer(integer&& other) noexcept : value(std::move(other.value)) {}

integer::~integer() = default;

integer& integer::operator=(const integer& other) {
    if (this != &other) {
        if (other.is_int64()) {
            value = other.as_int64();
        } else {
            value = std::make_unique<BigInt>(other.as_bigint());
        }
    }
    return *this;
}

integer& integer::operator=(integer&& other) noexcept {
    if (this != &other) {
        value = std::move(other.value);
    }
    return *this;
}

bool integer::is_int64() const {
    return std::holds_alternative<int64_t>(value);
}

bool integer::is_bigint() const {
    return std::holds_alternative<std::unique_ptr<BigInt>>(value);
}

int64_t integer::as_int64() const {
    if (is_int64()) {
        return std::get<int64_t>(value);
    }
    throw std::runtime_error("Not an int64_t");
}

const BigInt& integer::as_bigint() const {
    if (is_bigint()) {
        return *std::get<std::unique_ptr<BigInt>>(value);
    }
    throw std::runtime_error("Not a BigInt");
}

integer integer::operator+(const integer& other) const {
    if (is_int64() && other.is_int64()) {
        const int64_t a = as_int64();
        const int64_t b = other.as_int64();
        // Check for overflow
        if ((b > 0 && a > std::numeric_limits<int64_t>::max() - b) ||
            (b < 0 && a < std::numeric_limits<int64_t>::min() - b)) {
            // Convert to BigInt
            return integer(BigInt(a) + BigInt(b));
        }
        return integer(a + b);
    }
    return integer(to_bigint() + other.to_bigint());
}

integer integer::operator-(const integer& other) const {
    if (is_int64() && other.is_int64()) {
        const int64_t a = as_int64();
        const int64_t b = other.as_int64();
        // Check for overflow
        if ((b > 0 && a < std::numeric_limits<int64_t>::min() + b) ||
            (b < 0 && a > std::numeric_limits<int64_t>::max() + b)) {
            // Convert to BigInt
            return integer(BigInt(a) - BigInt(b));
        }
        return integer(a - b);
    }
    return integer(to_bigint() - other.to_bigint());
}

integer integer::operator*(const integer& other) const {
    if (is_int64() && other.is_int64()) {
        const int64_t a = as_int64();
        const int64_t b = other.as_int64();

        // Check for overflow
        if (a > 0) {
            if (b > 0) {
                if (a > std::numeric_limits<int64_t>::max() / b) {
                    return integer(BigInt(a) * BigInt(b));
                }
            } else {
                if (b < std::numeric_limits<int64_t>::min() / a) {
                    return integer(BigInt(a) * BigInt(b));
                }
            }
        } else {
            if (b > 0) {
                if (a < std::numeric_limits<int64_t>::min() / b) {
                    return integer(BigInt(a) * BigInt(b));
                }
            } else {
                if (a != 0 && b < std::numeric_limits<int64_t>::max() / a) {
                    return integer(BigInt(a) * BigInt(b));
                }
            }
        }
        return integer(a * b);
    }
    return integer(to_bigint() * other.to_bigint());
}

integer integer::operator/(const integer& other) const {
    if (other.is_zero()) {
        throw std::runtime_error("Division by zero");
    }

    if (is_int64() && other.is_int64()) {
        const int64_t a = as_int64();
        const int64_t b = other.as_int64();

        // Special case for minimum value divided by -1
        if (a == std::numeric_limits<int64_t>::min() && b == -1) {
            return integer(BigInt(a) / BigInt(b));
        }

        return integer(a / b);
    }
    return integer(to_bigint() / other.to_bigint());
}

integer integer::operator-() const {
    if (is_int64()) {
        const int64_t val = as_int64();
        if (val == std::numeric_limits<int64_t>::min()) {
            return integer(-BigInt(val));
        }
        return integer(-val);
    }
    return integer(-to_bigint());
}

integer integer::remainder(const integer& mod) const {
    if (mod.is_zero()) {
        throw std::runtime_error("Division by zero");
    }

    if (is_int64() && mod.is_int64()) {
        const int64_t a = as_int64();
        const int64_t b = mod.as_int64();
        return integer(a % b);
    }
    return integer(to_bigint() % mod.to_bigint());
}

integer integer::modulo(const integer& mod) const {
    if (mod.is_zero()) {
        throw std::runtime_error("Division by zero");
    }

    integer rem = remainder(mod);
    if (rem.is_negative() != mod.is_negative()) {
        return -rem;
    }
    return rem;
}

integer integer::pow(const integer& exponent) const {
    if (is_int64() && exponent.is_int64()) {
        const int64_t base_val = as_int64();
        int64_t exp_val;
        if (exponent.is_negative())
        {
            exp_val = -exponent.as_int64();
        }
        else
        {
            exp_val = exponent.as_int64();
        }

        // Handle simple cases
        if (exp_val == 0) return integer(1);
        if (exp_val == 1) return *this;
        if (base_val == 0) return integer(0);
        if (base_val == 1) return integer(1);
        if (base_val == -1) return integer(exp_val % 2 == 0 ? 1 : -1);

        // Check if result fits in int64_t
        try {
            int64_t result = 1;
            int64_t base = base_val;
            int64_t exp = exp_val;

            while (exp > 0) {
                if (exp % 2 == 1) {
                    // Check for overflow
                    if (result > std::numeric_limits<int64_t>::max() / base ||
                        result < std::numeric_limits<int64_t>::min() / base) {
                        throw std::overflow_error("Overflow in exponentiation");
                    }
                    result *= base;
                }
                // Check for overflow in base squaring
                if (base > std::numeric_limits<int64_t>::max() / base ||
                    base < std::numeric_limits<int64_t>::min() / base) {
                    throw std::overflow_error("Overflow in exponentiation");
                }
                base *= base;
                exp /= 2;
            }
            return integer(result);
        } catch (const std::overflow_error&) {
            // Fall back to BigInt
            return integer(BigInt(base_val).pow(UBigInt(llabs(exp_val))));
        }
    }

    // Use BigInt for large numbers
    auto result = to_bigint().pow(exponent.to_bigint().abs());
    return integer(std::move(result));
}

BigInt integer::to_bigint() const {
    if (is_int64()) {
        return BigInt(as_int64());
    }
    return as_bigint();
}

bool integer::operator==(const integer& other) const {
    if (is_int64() && other.is_int64()) {
        return as_int64() == other.as_int64();
    }
    return to_bigint() == other.to_bigint();
}

bool integer::operator!=(const integer& other) const {
    return !(*this == other);
}

bool integer::operator<(const integer& other) const {
    if (is_int64() && other.is_int64()) {
        return as_int64() < other.as_int64();
    }
    return to_bigint() < other.to_bigint();
}

bool integer::operator<=(const integer& other) const {
    if (is_int64() && other.is_int64()) {
        return as_int64() <= other.as_int64();
    }
    return to_bigint() <= other.to_bigint();
}

bool integer::operator>(const integer& other) const {
    if (is_int64() && other.is_int64()) {
        return as_int64() > other.as_int64();
    }
    return to_bigint() > other.to_bigint();
}

bool integer::operator>=(const integer& other) const {
    if (is_int64() && other.is_int64()) {
        return as_int64() >= other.as_int64();
    }
    return to_bigint() >= other.to_bigint();
}

bool integer::is_zero() const {
    if (is_int64()) {
        return as_int64() == 0;
    }
    return as_bigint().is_zero();
}

bool integer::is_one() const {
    if (is_int64()) {
        return as_int64() == 1;
    }
    return as_bigint().is_one();
}

bool integer::is_positive() const {
    if (is_int64()) {
        return as_int64() > 0;
    }
    return as_bigint().is_positive();
}

bool integer::is_negative() const {
    if (is_int64()) {
        return as_int64() < 0;
    }
    return as_bigint().is_negative_sign();
}

integer integer::abs() const {
    if (is_negative()) {
        return -(*this);
    }
    return *this;
}

std::string integer::to_decimal_string() const {
    if (is_int64()) {
        return std::to_string(as_int64());
    }
    return as_bigint().to_decimal_string();
}

real integer::to_real() const {
    if (is_int64()) {
        return static_cast<real>(as_int64());
    }
    // BigInt to real conversion
    return as_bigint().to_real();
}

rational integer::to_rational() const {
    return {*this, integer(1)};
}

real from_string(const std::string& str)
{
    real val = 0;
    if (auto [_,err] = std::from_chars(str.data(), str.data() + str.size(), val); err != std::errc())
    {
        throw std::runtime_error("BigInt to real conversion failed");
    }
    return val;
}

void rational::normalize() {
    if (den.is_zero()) {
        throw std::runtime_error("Zero denominator in rational");
    }

    if (num.is_zero()) {
        den = integer(1);
        return;
    }

    // Make denominator positive
    if (den.is_negative()) {
        num = -num;
        den = -den;
    }

    // Reduce by GCD
    if (const integer g = gcd(num.abs(), den); !g.is_zero() && !g.is_one()) {
        num = num / g;
        den = den / g;
    }
}

integer rational::gcd(const integer& a, const integer& b) {
    if (b.is_zero()) {
        return a;
    }
    return gcd(b, a.modulo(b));
}

rational::rational() : num(0), den(1) {}
rational::rational(integer value) : num(std::move(value)), den(1) {}
rational::rational(const int64_t value) : num(value), den(1) {}

rational::rational(integer num, integer den)
    : num(std::move(num)), den(std::move(den)) {
    normalize();
}

rational::rational(const int64_t num, const int64_t den)
    : num(num), den(den) {
    normalize();
}

rational::rational(const std::string& str) {
    if (const size_t slash_pos = str.find('/'); slash_pos == std::string::npos) {
        // It's just an integer
        num = integer(str);
        den = integer(1);
    } else {
        const std::string num_str = str.substr(0, slash_pos);
        const std::string den_str = str.substr(slash_pos + 1);
        num = integer(num_str);
        den = integer(den_str);
        normalize();
    }
}

integer rational::to_integer() const {
    if (!is_integer()) {
        throw std::runtime_error("Rational is not an integer");
    }
    return num / den;
}

bool rational::is_integer() const {
    return den == integer(1);
}

rational rational::operator+(const rational& other) const {
    const integer num1 = num * other.den;
    const integer num2 = other.num * den;
    integer new_den = den * other.den;
    return {num1 + num2, std::move(new_den)};
}

rational rational::operator-(const rational& other) const {
    const integer num1 = num * other.den;
    const integer num2 = other.num * den;
    integer new_den = den * other.den;
    return {num1 - num2, std::move(new_den)};
}

rational rational::operator*(const rational& other) const {
    return {num * other.num,
                   den * other.den};
}

rational rational::operator/(const rational& other) const {
    if (other.is_zero()) {
        throw std::runtime_error("Division by zero");
    }
    return {num * other.den,
                   den * other.num};
}

rational rational::operator-() const {
    return {-num, den};
}

bool rational::operator==(const rational& other) const {
    return num * other.den == other.num * den;
}

bool rational::operator!=(const rational& other) const {
    return !(*this == other);
}

bool rational::operator<(const rational& other) const {
    const integer lhs = num * other.den;
    const integer rhs = other.num * den;
    return lhs < rhs;
}

bool rational::operator<=(const rational& other) const {
    const integer lhs = num * other.den;
    const integer rhs = other.num * den;
    return lhs <= rhs;
}

bool rational::operator>(const rational& other) const {
    const integer lhs = num * other.den;
    const integer rhs = other.num * den;
    return lhs > rhs;
}

bool rational::operator>=(const rational& other) const {
    const integer lhs = num * other.den;
    const integer rhs = other.num * den;
    return lhs >= rhs;
}

bool rational::operator==(const integer& other) const {
    return *this == rational(other);
}

bool rational::operator<(const integer& other) const {
    return *this < rational(other);
}

bool rational::is_zero() const {
    return num.is_zero();
}

bool rational::is_positive() const {
    return num.is_positive();
}

bool rational::is_negative() const {
    return num.is_negative();
}

rational rational::abs() const {
    return {num.abs(), den};
}

rational rational::reciprocal() const {
    if (num.is_zero()) {
        throw std::runtime_error("Zero has no reciprocal");
    }
    return {den, num};
}

rational rational::pow(const integer& exponent) const {
    if (exponent.is_zero()) {
        return rational(1);
    }

    if (exponent.is_negative()) {
        const integer abs_exp = exponent.abs();
        const rational base_raised = this->pow(abs_exp);
        return base_raised.reciprocal();
    }

    // Positive exponent
    return {num.pow(exponent), den.pow(exponent)};
}

std::variant<integer, real> integer::sqrt() const {
    if (is_negative()) {
        throw std::domain_error("Cannot compute square root of negative number");
    }

    if (is_zero()) {
        return integer(0);
    }

    if (is_one()) {
        return integer(1);
    }

    if (is_int64()) {
        if (const int64_t val = as_int64(); val <= 1LL << 53) {
            real root = std::sqrt(static_cast<real>(val));
            if (const real nearest_int = std::round(root); std::abs(root - nearest_int) < 1e-10) {
                return integer(static_cast<int64_t>(nearest_int));
            }
            return root;
        }
    }

    const BigInt big_val = to_bigint();
    const UBigInt n = big_val.abs();

    if (n.is_one()) {
        return integer(1);
    }

    UBigInt low(1);
    UBigInt high = n;

    while (low <= high) {
        UBigInt mid = (low + high) / UBigInt(2);

        if (UBigInt square = mid * mid; square == n) {
            return integer(BigInt(std::move(mid)));
        } else if (square < n) {
            low = mid + UBigInt(1);
        } else {
            high = mid - UBigInt(1);
        }
    }

    real real_n = 0.0;
    real factor = 1.0;

    for (const auto& data = n.get_data(); const unsigned long it : std::ranges::reverse_view(data)) {
        real_n += static_cast<real>(it) * factor;
        factor *= static_cast<real>(18446744073709551615.0) + 1.0; // 2^64
    }

    return std::sqrt(real_n);
}

integer integer::isqrt() const { // integer square root (floor)
    if (is_negative()) {
        throw std::domain_error("Cannot compute square root of negative number");
    }

    if (is_zero() || is_one()) {
        return *this;
    }

    // 使用二分查找法计算整数平方根（向下取整）
    const BigInt big_val = to_bigint();
    const UBigInt n = big_val.abs();

    UBigInt low(1);
    UBigInt high = n;
    UBigInt result(0);

    while (low <= high) {
        UBigInt mid = (low + high) / UBigInt(2);
        UBigInt square = mid * mid;

        if (square == n) {
            return integer(BigInt(mid, false));
        }
        if (square < n) {
            result = mid;
            low = mid + UBigInt(1);
        } else {
            high = mid - UBigInt(1);
        }
    }

    return integer(BigInt(result, false));
}

std::string rational::to_rational_string() const {
    if (is_integer()) {
        return num.to_decimal_string();
    }
    return num.to_decimal_string() + "/" + den.to_decimal_string();
}

real rational::to_inexact() const {
    const real num_real = from_string(num.to_decimal_string());
    const real den_real = from_string(den.to_decimal_string());
    return num_real / den_real;
}

rational rational::exact_rational(const integer& num, const integer& den) {
    return {num, den};
}

rational rational::from_real(const real r) {
    static constexpr int max_iterations = 100;
    if (r == 0.0) {
        return rational(0);
    }

    bool negative = false;
    real abs_r = r;
    if (r <  0.0) {
        negative = true;
        abs_r = -r;
    }

    auto a0 = integer(0);
    auto a1 = integer(1);
    auto b0 = integer(1);
    auto b1 = integer(0);

    real x = abs_r;

    for (int i = 0; i < max_iterations; ++i) {
        auto integer_part = integer(static_cast<int64_t>(x));
        real fractional_part = x - integer_part.to_real();

        integer a2 = integer_part * a1 + a0;
        integer b2 = integer_part * b1 + b0;

        a0 = a1;
        a1 = a2;
        b0 = b1;
        b1 = b2;

        if (fractional_part == 0.0) {
            break;
        }

        x = 1.0 / fractional_part;

        if (x > 1e18) {
            break;
        }
    }

    rational result(a1, b1);
    if (negative) {
        result = -result;
    }

    return result;
}

bool is_close_enough(const rational& rat, const real& r, const double tolerance = 1e-10) {
    real diff = rat.to_inexact() - r;
    if (diff < 0.0) {
        diff = -diff;
    }
    return diff < tolerance;
}