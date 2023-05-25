#ifndef __INPUT_H__
#define __INPUT_H__

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Exception thrown when the user enters an empty line.
//! DO NOT HANDLE THIS EXCEPTION IN ANY OTHER FUNCTION EXCEPT mainMenu().
class EmptyLineException
{
};

// Exception thrown when the user enters a line that is not a valid integer.
class InvalidNumberException : public std::invalid_argument
{
  public:
    InvalidNumberException(std::string invalid_number);
};

/**
 * @brief Handle user input.
 *
 * @note that all the input from the user should be done through this class.
 *
 * @example
 * Input input(std::cin);
 * std::string line;
 * if(!input.getline(line)) {
 *     EOF reached
 * }
 *
 * @example
 * Input input(std::cin);
 * int id;
 * try{
 *     if(!input.prompt_int(id)) {
 *        EOF reached
 *     }
 * } catch(InvalidNumberException &e) {
 *     std::cout << e.what() << " is not a valid number" << std::endl;
 * }
 */
class Input
{
  public:
    Input(std::istream &in);

    // Read a line from the input stream.
    // throw EmptyLineException if the line is empty.
    std::istream &getline(std::string &line);

    // Read a line from the input stream and convert it to an integer.
    // throw InvalidNumberException if the line is not a valid integer.
    // throw EmptyLineException if the line is empty.
    std::istream &prompt_int(int &num);

  private:
    void echo(const std::string &line);

  private:
    std::istream &in;
};

// Remove leading and trailing whitespace from a string.
std::string &str_trim(std::string &str);

// Check if a string contains only digits.
bool is_valid_number(const std::string &str);

// Split a string into tokens using the given delimiters.
std::vector<std::string> tokenize(const std::string &str, const std::string &delims);

#endif // __INPUT_H__
