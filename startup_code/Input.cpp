#include "Input.h"

// Echo input if ECHO is defined and stdin is not a terminal.
// Useful for debugging.
#ifdef ECHO

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#endif // ECHO

InvalidNumberException::InvalidNumberException(std::string invalid_number)
    : std::invalid_argument(invalid_number)
{
}

Input::Input(std::istream &in)
    : in(in)
{
}

std::istream &Input::getline(std::string &line)
{
    if (!std::getline(in, line))
    {
        echo("<EOF>");
        return in;
    }
    str_trim(line);

    // support comments, ignore lines starting with '#'.
    // this is useful for creating descriptive test cases
    if (line.size() && line.front() == '#')
    {
        return getline(line);
    }

    echo(line);

    if (line.size() == 0)
    {
        throw EmptyLineException();
    }
    return in;
}

std::istream &Input::prompt_int(int &num)
{
    std::string line;
    try
    {
        if (getline(line))
        {
            num = std::stoi(line);
        }
    }
    catch (std::invalid_argument &)
    {
        throw InvalidNumberException(line);
    }

    return in;
}

void Input::echo(const std::string &line)
{
    (void)line;
#ifdef ECHO
#ifdef _WIN32
    if (!_isatty(_fileno(stdin)))
#else
    if (!isatty(fileno(stdin)))
#endif
    {
        std::cout << line << std::endl;
    }
#endif // ECHO
}

std::string &str_trim(std::string &str)
{
    // remove trailing whitespace
    while (str.size() && isspace(str.back()))
    {
        str.pop_back();
    }

    // remove leading whitespace
    size_t i = 0;
    while (i < str.size() && isspace(str[i]))
    {
        ++i;
    }
    str = str.substr(i);

    return str;
}

bool is_valid_number(const std::string &str)
{
    for (char c : str)
    {
        if (!std::isdigit(c))
        {
            return false;
        }
    }
    return true;
}

std::vector<std::string> tokenize(const std::string &str, const std::string &delims)
{
    std::vector<std::string> tokens;
    std::string token;
    for (char c : str)
    {
        if (delims.find(c) != std::string::npos)
        {
            if (token.size())
            { // found a token
                tokens.push_back(token);
                token.clear();
            }
        }
        else
        {
            token.push_back(c);
        }
    }
    if (token.size())
    { // the very last token
        tokens.push_back(token);
    }
    return tokens;
}
