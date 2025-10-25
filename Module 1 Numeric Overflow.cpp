// NumericOverflows.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>       // std::cout
#include <limits>         // std::numeric_limits
#include <type_traits>    // std::is_integral_v, std::is_floating_point_v, std::is_signed_v
#include <typeinfo>       // typeid
#include <cmath>          // std::isfinite, std::isnormal

// A small return type to communicate both the numeric value and whether the operation was safe.
// ok == true means no overflow or underflow occurred.
// ok == false means we detected an impending overflow/underflow and stopped before it happened.
// value is the last safe value we could compute.
template <typename T>
struct Checked {
    T value{};
    bool ok{ true };
};

/*
    Helper: check if a + b would overflow or underflow for integral T.
    This uses classic "add will overflow if a > max - b" style rules.
*/
template <typename T>
inline bool will_add_overflow_integral(T a, T b) {
    static_assert(std::is_integral_v<T>, "integral only");
    using Lim = std::numeric_limits<T>;
    if constexpr (std::is_signed_v<T>) {
        if (b > 0 && a > Lim::max() - b) return true;
        if (b < 0 && a < Lim::min() - b) return true;
        return false;
    }
    else {
        // unsigned
        return a > Lim::max() - b; // b cannot be negative for unsigned
    }
}

/*
    Helper: check if a - b would overflow or underflow for integral T.
    Implemented via addition check by negating b when signed.
*/
template <typename T>
inline bool will_sub_overflow_integral(T a, T b) {
    static_assert(std::is_integral_v<T>, "integral only");
    if constexpr (std::is_signed_v<T>) {
        return will_add_overflow_integral<T>(a, static_cast<T>(-b));
    }
    else {
        // For unsigned, a - b underflows if b > a
        return b > a;
    }
}

/*
    Helper: for floating point we treat overflow if the next value is not finite
    or its magnitude exceeds the type's max. Tiny subnormal values are allowed;
    if you want to treat loss of normal as underflow, we mark ok=false but still
    provide the computed value (which may be 0 due to flush-to-zero on some targets).
*/
template <typename T>
inline bool add_would_overflow_floating(T a, T b) {
    static_assert(std::is_floating_point_v<T>, "floating only");
    long double trial = static_cast<long double>(a) + static_cast<long double>(b);
    if (!std::isfinite(trial)) return true;
    long double m = static_cast<long double>(std::numeric_limits<T>::max());
    return std::fabsl(trial) > m;
}

template <typename T>
inline bool sub_would_overflow_floating(T a, T b) {
    static_assert(std::is_floating_point_v<T>, "floating only");
    long double trial = static_cast<long double>(a) - static_cast<long double>(b);
    if (!std::isfinite(trial)) return true;
    long double m = static_cast<long double>(std::numeric_limits<T>::max());
    return std::fabsl(trial) > m;
}

/// <summary>
/// Template function to compute: start + (increment * steps)
/// Now returns Checked<T> so callers know if an overflow would have occurred.
/// We prevent overflow by stopping before the unsafe step and setting ok=false.
/// </summary>
template <typename T>
Checked<T> add_numbers(T const& start, T const& increment, unsigned long int const& steps)
{
    Checked<T> out{ start, true };

    // Iterate step by step so we can preflight each addition regardless of type.
    for (unsigned long int i = 0; i < steps; ++i)
    {
        // Integral path: use bounds checks
        if constexpr (std::is_integral_v<T>) {
            if (will_add_overflow_integral<T>(out.value, increment)) {
                out.ok = false;            // signal overflow would have happened
                break;                     // prevent it
            }
            out.value = static_cast<T>(out.value + increment);
        }
        // Floating path: check finiteness and max magnitude
        else if constexpr (std::is_floating_point_v<T>) {
            if (add_would_overflow_floating<T>(out.value, increment)) {
                out.ok = false;
                break;
            }
            T next = static_cast<T>(out.value + increment);
            // Optional: treat loss of normal as underflow warning
            if (!std::isfinite(next)) { out.ok = false; break; }
            out.value = next;
        }
        else {
            // For unexpected numeric-like types, do a conservative attempt
            out.value = static_cast<T>(out.value + increment);
        }
    }

    return out;
}

/// <summary>
/// Template function to compute: start - (decrement * steps)
/// Returns Checked<T> to communicate safety. Prevents underflow/overflow.
/// </summary>
template <typename T>
Checked<T> subtract_numbers(T const& start, T const& decrement, unsigned long int const& steps)
{
    Checked<T> out{ start, true };

    for (unsigned long int i = 0; i < steps; ++i)
    {
        if constexpr (std::is_integral_v<T>) {
            if (will_sub_overflow_integral<T>(out.value, decrement)) {
                out.ok = false;            // signal underflow or overflow would occur
                break;                     // prevent it
            }
            out.value = static_cast<T>(out.value - decrement);
        }
        else if constexpr (std::is_floating_point_v<T>) {
            if (sub_would_overflow_floating<T>(out.value, decrement)) {
                out.ok = false;
                break;
            }
            T next = static_cast<T>(out.value - decrement);
            if (!std::isfinite(next)) { out.ok = false; break; }
            out.value = next;
        }
        else {
            out.value = static_cast<T>(out.value - decrement);
        }
    }

    return out;
}

//  NOTE:
//    You will see the unary ('+') operator used in front of the variables in the test_XXX methods.
//    This forces the output to be a number for cases where cout would assume it is a character. 

template <typename T>
void test_overflow()
{
    // START DO NOT CHANGE
    const unsigned long int steps = 5;
    const T increment = std::numeric_limits<T>::max() / steps;
    const T start = 0;

    std::cout << "Overflow Test of Type = " << typeid(T).name() << std::endl;
    // END DO NOT CHANGE

    std::cout << "\tAdding Numbers Without Overflow (" << +start << ", " << +increment << ", " << steps << ") => ";
    auto r1 = add_numbers<T>(start, increment, steps);
    std::cout << "ok=" << std::boolalpha << r1.ok << ", result=" << +r1.value << std::endl;

    std::cout << "\tAdding Numbers With Overflow (" << +start << ", " << +increment << ", " << (steps + 1) << ") => ";
    auto r2 = add_numbers<T>(start, increment, steps + 1);
    std::cout << "ok=" << std::boolalpha << r2.ok << ", result=" << +r2.value << std::endl;
}

template <typename T>
void test_underflow()
{
    // START DO NOT CHANGE
    const unsigned long int steps = 5;
    const T decrement = std::numeric_limits<T>::max() / steps;
    const T start = std::numeric_limits<T>::max();

    std::cout << "Underflow Test of Type = " << typeid(T).name() << std::endl;
    // END DO NOT CHANGE

    std::cout << "\tSubtracting Numbers Without Overflow (" << +start << ", " << +decrement << ", " << steps << ") => ";
    auto r1 = subtract_numbers<T>(start, decrement, steps);
    std::cout << "ok=" << std::boolalpha << r1.ok << ", result=" << +r1.value << std::endl;

    std::cout << "\tSubtracting Numbers With Overflow (" << +start << ", " << +decrement << ", " << (steps + 1) << ") => ";
    auto r2 = subtract_numbers<T>(start, decrement, steps + 1);
    std::cout << "ok=" << std::boolalpha << r2.ok << ", result=" << +r2.value << std::endl;
}

void do_overflow_tests(const std::string& star_line)
{
    std::cout << std::endl << star_line << std::endl;
    std::cout << "*** Running Overflow Tests ***" << std::endl;
    std::cout << star_line << std::endl;

    // signed integers
    test_overflow<char>();
    test_overflow<wchar_t>();
    test_overflow<short int>();
    test_overflow<int>();
    test_overflow<long>();
    test_overflow<long long>();

    // unsigned integers
    test_overflow<unsigned char>();
    test_overflow<unsigned short int>();
    test_overflow<unsigned int>();
    test_overflow<unsigned long>();
    test_overflow<unsigned long long>();

    // real numbers
    test_overflow<float>();
    test_overflow<double>();
    test_overflow<long double>();
}

void do_underflow_tests(const std::string& star_line)
{
    std::cout << std::endl << star_line << std::endl;
    std::cout << "*** Running Undeflow Tests ***" << std::endl;
    std::cout << star_line << std::endl;

    // signed integers
    test_underflow<char>();
    test_underflow<wchar_t>();
    test_underflow<short int>();
    test_underflow<int>();
    test_underflow<long>();
    test_underflow<long long>();

    // unsigned integers
    test_underflow<unsigned char>();
    test_underflow<unsigned short int>();
    test_underflow<unsigned int>();
    test_underflow<unsigned long>();
    test_underflow<unsigned long long>();

    // real numbers
    test_underflow<float>();
    test_underflow<double>();
    test_underflow<long double>();
}

/// <summary>
/// Entry point into the application
/// </summary>
/// <returns>0 when complete</returns>
int main()
{
    const std::string star_line = std::string(50, '*');

    std::cout << "Starting Numeric Underflow / Overflow Tests!" << std::endl;

    do_overflow_tests(star_line);
    do_underflow_tests(star_line);

    std::cout << std::endl << "All Numeric Underflow / Overflow Tests Complete!" << std::endl;

    return 0;
}
