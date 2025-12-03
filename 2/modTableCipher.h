#pragma once
#include <string>
#include <vector>
#include <stdexcept>

class cipher_error: public std::invalid_argument {
public:
    explicit cipher_error (const std::string& what_arg):
        std::invalid_argument(what_arg) {}
    explicit cipher_error (const char* what_arg):
        std::invalid_argument(what_arg) {}
};

class Table
{
private:
    int cols;
    
    int getValidKey(const int key);
    std::wstring getValidOpenText(const std::wstring& s);
    std::wstring getValidCipherText(const std::wstring& s);
    
public:
    explicit Table(int key);
    std::wstring encrypt(const std::wstring& plain);
    std::wstring decrypt(const std::wstring& cipher);
};