// Exceptions.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdexcept>
#include <exception>
#include <string>

// A custom exception class derived from std::exception
class CustomException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Custom Exception Thrown!";
    }
};

bool do_even_more_custom_application_logic() {
    std::cout << "Running Even More Custom Application Logic." << std::endl;
    throw std::runtime_error("Something went wrong in even more custom logic.");
    return true;
}

void do_custom_application_logic() {
    std::cout << "Running Custom Application Logic." << std::endl;

    try {
        if (do_even_more_custom_application_logic()) {
            std::cout << "Even More Custom Application Logic Succeeded." << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Caught an exception: " << e.what() << std::endl;
    }

    throw CustomException();

    std::cout << "Leaving Custom Application Logic." << std::endl;
}

float divide(float num, float den) {
    if (den == 0) {
        throw std::invalid_argument("Division by zero is not allowed.");
    }
    return (num / den);
}

void do_division() noexcept {
    float numerator = 10.0f;
    float denominator = 0;

    try {
        auto result = divide(numerator, denominator);
        std::cout << "divide(" << numerator << ", " << denominator << ") = " << result << std::endl;
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Caught a specific exception: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "Exceptions Tests!" << std::endl;

    try {
        do_division();
        do_custom_application_logic();
    }
    catch (const CustomException& e) {
        std::cerr << "Caught my custom exception: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Caught a standard exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Caught an unknown exception." << std::endl;
    }

    return 0;
}