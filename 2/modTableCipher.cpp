#include "modTableCipher.h"
#include <algorithm>
#include <vector>
#include <cwctype>

using namespace std;

int Table::getValidKey(const int key)
{
    if (key <= 0)
        throw cipher_error("Invalid key: cannot be zero");
    if (key < 0)
        throw cipher_error("Invalid key: cannot be negative");
    if (key > 100)
        throw cipher_error("Invalid key: too large");
    return key;
}

std::wstring Table::getValidOpenText(const std::wstring& s)
{
    std::wstring tmp;
    std::wstring lower = L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    std::wstring upper = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    
    for (auto c : s) {
        if (upper.find(c) != std::wstring::npos) {
            tmp.push_back(c);
        } else {
            size_t pos = lower.find(c);
            if (pos != std::wstring::npos) {
                tmp.push_back(upper[pos]); 
            }
        }
    }
    if (tmp.empty())
        throw cipher_error("Empty text: no valid Russian letters");
    return tmp;
}

std::wstring Table::getValidCipherText(const std::wstring& s)
{
    if (s.empty())
        throw cipher_error("Empty cipher text");
    
    std::wstring upper = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    for (auto c : s) {
        if (upper.find(c) == std::wstring::npos)
            throw cipher_error("Invalid cipher text");
    }
    return s;
}

Table::Table(int key)
{
    cols = getValidKey(key);
}

std::wstring Table::encrypt(const std::wstring& plain)
{
    std::wstring validText = getValidOpenText(plain);
    int n = static_cast<int>(validText.length());
    
    if (n == 0) return L"";
    
    int rows = (n + cols - 1) / cols;
    
    vector<vector<wchar_t>> table(rows, vector<wchar_t>(cols, L' '));
    int index = 0;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if (index < n) {
                table[row][col] = validText[index++];
            }
        }
    }
    
    std::wstring result;
    for (int col = cols - 1; col >= 0; col--) {
        for (int row = 0; row < rows; row++) {
            if (table[row][col] != L' ') {
                result += table[row][col];
            }
        }
    }
    
    return result;
}

std::wstring Table::decrypt(const std::wstring& cipher)
{
    std::wstring validText = getValidCipherText(cipher);
    int n = static_cast<int>(validText.length());
    
    if (n == 0) return L"";
    
    int rows = (n + cols - 1) / cols;
    
    int fullCols = n % cols;
    if (fullCols == 0) fullCols = cols;
    
    vector<vector<wchar_t>> table(rows, vector<wchar_t>(cols, L' '));
    int index = 0;
    
    for (int col = cols - 1; col >= 0; col--) {
        int rowsInCol = rows;
        if (col >= fullCols) {
            rowsInCol = rows - 1;
        }
        
        for (int row = 0; row < rowsInCol; row++) {
            if (index < n) {
                table[row][col] = validText[index++];
            }
        }
    }
    
    std::wstring result;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if (table[row][col] != L' ') {
                result += table[row][col];
            }
        }
    }
    
    return result;
}