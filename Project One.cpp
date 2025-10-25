#include <iostream>
#include <string>
#include <limits> // Required for numeric_limits to handle input buffer clearing

// Forward declarations for the other functions
void CheckUserPermissionAccess();
void DisplayInfo();
void ChangeCustomerChoice();

int main() {
    int choice = 0;

    std::cout << "Created by Anthony McCormack\n\nRangers Lead The Way!\n" << std::endl;

    // Main program loop
    do {
        // Display the main menu
        std::cout << "\nWelcome! Please select an option:" << std::endl;
        std::cout << "1. Check User Permission Access" << std::endl;
        std::cout << "2. Display Customer Information" << std::endl;
        std::cout << "3. Change Customer Choice" << std::endl;
        std::cout << "4. Exit" << std::endl;
        std::cout << "Enter your choice: ";

        // --- SECURITY VULNERABILITY FIX: Input Validation / DoS Prevention ---
        // We ensure the input is valid and within the expected range (1-4).
        while (!(std::cin >> choice) || choice < 1 || choice > 4) {

            // FIX: Output error message to the user
            std::cout << "Invalid input. Please enter a number between 1 and 4." << std::endl;

            // FIX: Clear the fail flags on the input stream (handles non-numeric input)
            std::cin.clear();

            // FIX: Discard the invalid input left in the buffer (prevents infinite loop/DoS)
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            // Re-prompt for choice within the loop
            std::cout << "Enter your choice: ";
        }
        // --- END OF SECURITY FIX ---

        // Process the choice based on user input
        if (choice == 1) {
            CheckUserPermissionAccess();
        }
        else if (choice == 2) {
            DisplayInfo();
        }
        else if (choice == 3) {
            ChangeCustomerChoice();
        }
        else if (choice == 4) {
            break; // Exit the loop
        }
        /* The redundant 'else' block for invalid input is now implicitly handled
           by the 'while' loop above, ensuring the input is always 1-4. */
    } while (true); // Loop indefinitely until choice is 4

    return 0;
}

// NOTE: The vulnerabilities below are not fixed as they are outside the scope of the "main" function fix.
void CheckUserPermissionAccess() {
    std::string username, password;

    // SECURITY VULNERABILITY IDENTIFIED: Implied Buffer Overflow Risk (via legacy context)
    std::cout << "Please enter your username: ";
    std::cin >> username;

    std::cout << "Please enter your password: ";
    std::cin >> password;

    // SECURITY VULNERABILITY IDENTIFIED: Hardcoded Credentials
    if (username == "admin" && password == "secure") {
        std::cout << "Access Granted." << std::endl;
    }
    else {
        std::cout << "Access Denied." << std::endl;
    }
}

void DisplayInfo() {
    std::cout << "\n--- Customer Information ---" << std::endl;
    std::cout << "Company: GlobalTech Solutions" << std::endl;
    std::cout << "Customer Name: Jane Doe" << std::endl;
    std::cout << "Customer ID: CUST12345" << std::endl;
    std::cout << "-----------------------------" << std::endl;
}

void ChangeCustomerChoice() {
    int choice;

    std::cout << "Please select an option to change:" << std::endl;
    std::cout << "1. Service Plan" << std::endl;
    std::cout << "2. Billing Address" << std::endl;
    std::cout << "3. Contact Information" << std::endl;
    std::cout << "4. Upgrade Account" << std::endl;
    std::cout << "5. Downgrade Account" << std::endl;
    std::cout << "Enter your choice: ";

    // SECURITY VULNERABILITY IDENTIFIED: Input Validation Failure (Repeat vulnerability)
    std::cin >> choice;

    if (choice == 1) {
        std::cout << "Service Plan updated." << std::endl;
    }
    else if (choice == 2) {
        std::cout << "Billing Address updated." << std::endl;
    }
    else if (choice == 3) {
        std::cout << "Contact Information updated." << std::endl;
    }
    else if (choice == 4) {
        std::cout << "Account upgraded." << std::endl;
    }
    else if (choice == 5) {
        std::cout << "Account downgraded." << std::endl;
    }
    else {
        std::cout << "Invalid choice." << std::endl;
    }
}