#include <UnitTest++/UnitTest++.h>
#include "modAlphaCipher.h"
#include <iostream>
#include <locale>
#include <codecvt>
#include <string>

using namespace std;

string wideToUtf8(const wstring& ws) {
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(ws);
}

wstring utf8ToWide(const string& s) {
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes(s);
}

#define CHECK_EQUAL_WS(expected, actual) \
    CHECK_EQUAL(wideToUtf8(expected), wideToUtf8(actual))

struct CipherWithLetterV {
    modAlphaCipher* cipher;
    
    CipherWithLetterV() {
        cipher = new modAlphaCipher(L"В");
    }
    
    ~CipherWithLetterV() {
        delete cipher;
    }
};

SUITE(KeyValidationTests)
{
    TEST(RejectsEmptyKey) {
        CHECK_THROW(modAlphaCipher cp(L""), cipher_error);
    }

    TEST(RejectsKeyWithDigits) {
        CHECK_THROW(modAlphaCipher cp(L"МИР123"), cipher_error);
    }

    TEST(RejectsKeyWithPunctuation) {
        CHECK_THROW(modAlphaCipher cp(L"МИР,МИР"), cipher_error);
    }

    TEST(RejectsKeyWithWhitespace) {
        CHECK_THROW(modAlphaCipher cp(L"МИР МИР"), cipher_error);
    }

    TEST(AcceptsValidUppercaseKey) {
        CHECK_EQUAL_WS(L"МИРМИ", modAlphaCipher(L"МИР").encrypt(L"ААААА"));
    }

    TEST(AcceptsValidLowercaseKey) {
        CHECK_EQUAL_WS(L"МИРМИ", modAlphaCipher(L"мир").encrypt(L"ААААА"));
    }

    TEST(AcceptsLongKeyCorrectlyTruncated) {
        CHECK_EQUAL_WS(L"ДЛИНН", modAlphaCipher(L"ДЛИННЫЙКЛЮЧ").encrypt(L"ААААА"));
    }

    TEST(HandlesSingleLetterKeyCorrectly) {
        modAlphaCipher cipher(L"А");
        wstring encrypted = cipher.encrypt(L"ТЕСТ");
        wstring decrypted = cipher.decrypt(encrypted);
        CHECK_EQUAL_WS(L"ТЕСТ", decrypted);
    }
}

SUITE(EncryptionBehavior)
{
    TEST(MaxShiftWithLetterYa) {
        CHECK_EQUAL_WS(L"ОПЗБДС", modAlphaCipher(L"Я").encrypt(L"ПРИВЕТ"));
    }

    TEST(NormalizesUppercaseInput) {
        CipherWithLetterV ctx;
        CHECK_EQUAL_WS(L"СТКДЖФ", ctx.cipher->encrypt(L"ПРИВЕТ"));
    }

    TEST(NormalizesLowercaseInput) {
        CipherWithLetterV ctx;
        CHECK_EQUAL_WS(L"СТКДЖФ", ctx.cipher->encrypt(L"привет"));
    }

    TEST(EncryptsOnlyLetters_IgnoresPunctuationAndSpaces) {
        CipherWithLetterV ctx;
        CHECK_EQUAL_WS(L"СТКДЖФОКТ", ctx.cipher->encrypt(L"ПРИВЕТ, МИР!"));
    }

    TEST(EncryptsOnlyLetters_IgnoresDigits) {
        CipherWithLetterV ctx;
        CHECK_EQUAL_WS(L"ФЖУФ", ctx.cipher->encrypt(L"ТЕСТ123"));
    }

    TEST(ThrowsOnEmptyPlainText) {
        CipherWithLetterV ctx;
        CHECK_THROW(ctx.cipher->encrypt(L""), cipher_error);
    }

    TEST(ThrowsOnPlainTextWithoutLetters) {
        CipherWithLetterV ctx;
        CHECK_THROW(ctx.cipher->encrypt(L"1234+8765=9999"), cipher_error);
    }
}

SUITE(DecryptionBehavior)
{
    TEST(MaxShiftDecryptionWithYa) {
        CHECK_EQUAL_WS(L"ПРИВЕТ", modAlphaCipher(L"Я").decrypt(L"ОПЗБДС"));
    }

    TEST(CorrectlyDecryptsValidCiphertext) {
        CipherWithLetterV ctx;
        CHECK_EQUAL_WS(L"ПРИВЕТ", ctx.cipher->decrypt(L"СТКДЖФ"));
    }

    TEST(RejectsLowercaseInCiphertext) {
        CipherWithLetterV ctx;
        wstring encrypted = ctx.cipher->encrypt(L"ПРИВЕТ");
        encrypted[0] = towlower(encrypted[0]); 
        CHECK_THROW(ctx.cipher->decrypt(encrypted), cipher_error);
    }

    TEST(RejectsTrailingWhitespace) {
        CipherWithLetterV ctx;
        wstring encrypted = ctx.cipher->encrypt(L"ПРИВЕТ");
        wstring corrupted = encrypted + L" ";
        CHECK_THROW(ctx.cipher->decrypt(corrupted), cipher_error);
    }

    TEST(RejectsTrailingDigits) {
        CipherWithLetterV ctx;
        wstring encrypted = ctx.cipher->encrypt(L"ПРИВЕТ");
        wstring corrupted = encrypted + L"123";
        CHECK_THROW(ctx.cipher->decrypt(corrupted), cipher_error);
    }

    TEST(RejectsLeadingPunctuation) {
        CipherWithLetterV ctx;
        wstring encrypted = ctx.cipher->encrypt(L"ПРИВЕТ");
        wstring corrupted = L"!" + encrypted;
        CHECK_THROW(ctx.cipher->decrypt(corrupted), cipher_error);
    }

    TEST(RejectsEmptyCiphertext) {
        CipherWithLetterV ctx;
        CHECK_THROW(ctx.cipher->decrypt(L""), cipher_error);
    }
}

int main() {
    setlocale(LC_ALL, "");
    return UnitTest::RunAllTests();
}