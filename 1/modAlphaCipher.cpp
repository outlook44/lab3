//modAlphaCipher.cpp
#include "modAlphaCipher.h"
#include <stdexcept>
#include <algorithm>
#include <cctype>

modAlphaCipher::modAlphaCipher(const std::wstring& inputKey)
    : alphabet(L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ")
{
    for (size_t i = 0; i < alphabet.size(); ++i) {
        charToIndex[alphabet[i]] = static_cast<int>(i);
    }

    if (inputKey.empty()) {
        throw cipher_error("Empty key");
    }

    bool hasInvalid = false;
    for (wchar_t ch : inputKey) {
        if (ch >= L'а' && ch <= L'я') {
            continue; 
        } else if (ch == L'ё') {
            continue;
        } else if (ch >= L'А' && ch <= L'Я') {
            continue;
        } else if (ch == L'Ё') {
            continue;
        } else {
            hasInvalid = true;
            break;
        }
    }

    if (hasInvalid) {
        throw cipher_error("Invalid key: non-alphabetic character");
    }

    std::wstring normalizedKey = normalizeText(inputKey);
    if (normalizedKey.empty()) {
        throw cipher_error("Empty key");
    }

    key = textToIndices(normalizedKey);
}

std::wstring modAlphaCipher::normalizeText(const std::wstring& text) const
{
    std::wstring result;
    for (wchar_t ch : text) {
        if (ch >= L'а' && ch <= L'я') {
            ch = ch - L'а' + L'А';
        } else if (ch == L'ё') {
            ch = L'Ё';
        }

        if (charToIndex.count(ch) > 0) {
            result += ch;
        }
    }
    return result;
}

std::vector<int> modAlphaCipher::textToIndices(const std::wstring& text) const
{
    std::vector<int> indices;
    for (wchar_t ch : text) {
        auto it = charToIndex.find(ch);
        if (it != charToIndex.end()) {
            indices.push_back(it->second);
        }
    }
    return indices;
}

std::wstring modAlphaCipher::indicesToText(const std::vector<int>& indices) const
{
    std::wstring result;
    for (int idx : indices) {
        if (idx >= 0 && static_cast<size_t>(idx) < alphabet.size()) {
            result += alphabet[idx];
        }
    }
    return result;
}

std::wstring modAlphaCipher::encrypt(const std::wstring& plaintext) const
{
    std::wstring cleanText = normalizeText(plaintext);
    if (cleanText.empty()) {
        throw cipher_error("Empty text");
    }

    std::vector<int> textIndices = textToIndices(cleanText);
    size_t alphabetSize = alphabet.size();

    for (size_t i = 0; i < textIndices.size(); ++i) {
        textIndices[i] = (textIndices[i] + key[i % key.size()]) % static_cast<int>(alphabetSize);
    }

    return indicesToText(textIndices);
}

std::wstring modAlphaCipher::decrypt(const std::wstring& ciphertext) const
{

    for (wchar_t ch : ciphertext) {
        if (charToIndex.find(ch) == charToIndex.end()) {
            throw cipher_error("Invalid ciphertext: non-alphabetic character");
        }
    }

    if (ciphertext.empty()) {
        throw cipher_error("Empty text, no letters");
    }

    std::vector<int> textIndices = textToIndices(ciphertext);
    size_t alphabetSize = alphabet.size();

    for (size_t i = 0; i < textIndices.size(); ++i) {
        textIndices[i] = (textIndices[i] + static_cast<int>(alphabetSize) - key[i % key.size()]) % static_cast<int>(alphabetSize);
    }

    return indicesToText(textIndices);
}
