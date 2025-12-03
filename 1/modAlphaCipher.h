// modAlphaCipher.h
#pragma once
#include <vector>
#include <string>
#include <map>
#include <stdexcept>

// Новый класс исключения
class cipher_error : public std::invalid_argument {
public:
    explicit cipher_error(const std::string& message) : std::invalid_argument(message) {}
};

class modAlphaCipher
{
private:
    std::wstring alphabet;
    std::map<wchar_t, int> charToIndex;
    std::vector<int> key;
    
    std::vector<int> textToIndices(const std::wstring& text) const;
    std::wstring indicesToText(const std::vector<int>& indices) const;
    std::wstring normalizeText(const std::wstring& text) const;

public:
    explicit modAlphaCipher(const std::wstring& key);
    std::wstring encrypt(const std::wstring& plaintext) const;
    std::wstring decrypt(const std::wstring& ciphertext) const;
};
