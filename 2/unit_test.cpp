#include <UnitTest++/UnitTest++.h>
#include <string>
#include <locale>
#include <codecvt>
#include "modTableCipher.h"

using namespace std;

string wideToUtf8(const wstring& ws) {
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(ws);
}

wstring utf8ToWide(const string& s) {
    wstring_convert<codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes(s);
}

#define CHECK_WIDE_EQUAL(expected, actual) \
    CHECK_EQUAL(wideToUtf8(expected), wideToUtf8(actual))

SUITE(ConstructorTest)
{
    TEST(ValidKey) {
        Table cipher(3);
        CHECK_WIDE_EQUAL(L"ИТРРЕИПВМ", cipher.encrypt(L"ПРИВЕТМИР"));
    }
    
    TEST(LongKey) {
        Table cipher(10);
        CHECK_WIDE_EQUAL(L"ТЕВИРП", cipher.encrypt(L"ПРИВЕТ"));
    }

    TEST(KeyEqualsMessageLength) {
        Table cipher(9);
        CHECK_WIDE_EQUAL(L"РИМТЕВИРП", cipher.encrypt(L"ПРИВЕТМИР"));
    }
    
    TEST(NegativeKey) {
        CHECK_THROW(Table cipher(-3), cipher_error);
    }

    TEST(ZeroKey) {
        CHECK_THROW(Table cipher(0), cipher_error);
    }
}

struct Key3Fixture {
    Table* cipher;
    
    Key3Fixture() {
        cipher = new Table(3);
    }
    
    ~Key3Fixture() {
        delete cipher;
    }
};

struct Key1Fixture {
    Table* cipher;
    
    Key1Fixture() {
        cipher = new Table(1);
    }
    
    ~Key1Fixture() {
        delete cipher;
    }
};

SUITE(EncryptTest)
{
    TEST_FIXTURE(Key3Fixture, UpperCaseString) {
        CHECK_WIDE_EQUAL(L"ИТРРЕИПВМ", cipher->encrypt(L"ПРИВЕТМИР"));
    }
    
    TEST_FIXTURE(Key3Fixture, LowerCaseString) {
        CHECK_WIDE_EQUAL(L"ИТРРЕИПВМ", cipher->encrypt(L"приветмир"));
    }

    TEST_FIXTURE(Key3Fixture, StringWithWhitespace) {
        CHECK_WIDE_EQUAL(L"ИТРРЕИПВМ", cipher->encrypt(L"ПРИВЕТ МИР"));
    }
    
    TEST_FIXTURE(Key3Fixture, StringWithNumbers) {
        CHECK_WIDE_EQUAL(L"ИТРЕПВ", cipher->encrypt(L"ПРИВЕТ2232"));
    }

    TEST_FIXTURE(Key3Fixture, NoLetters) {
        CHECK_THROW(cipher->encrypt(L"1234"), cipher_error);
    }
    
    TEST_FIXTURE(Key3Fixture, EmptyString) {
        CHECK_THROW(cipher->encrypt(L""), cipher_error);
    }
    
    TEST_FIXTURE(Key1Fixture, KeyEqualsOne) {
        CHECK_WIDE_EQUAL(L"ПРИВЕТМИР", cipher->encrypt(L"ПРИВЕТМИР"));
    }

    TEST_FIXTURE(Key3Fixture, StringWithPunctuation) {
        CHECK_WIDE_EQUAL(L"ИТРРЕИПВМ", cipher->encrypt(L"ПРИВЕТ, МИР"));
    }
    
    TEST(NonMultipleKey) {
        Table cipher(4);
        wstring original = L"АБВГД";
        wstring encrypted = cipher.encrypt(original);
        wstring decrypted = cipher.decrypt(encrypted);
        CHECK_WIDE_EQUAL(original, decrypted);
    }

    TEST(ShortText) {
        Table cipher(3);
        CHECK_WIDE_EQUAL(L"А", cipher.encrypt(L"А"));
    }
}

SUITE(DecryptTest)
{
    TEST_FIXTURE(Key3Fixture, UpperCaseString) {
        CHECK_WIDE_EQUAL(L"ПРИВЕТМИР", cipher->decrypt(L"ИТРРЕИПВМ"));
    }

    TEST_FIXTURE(Key3Fixture, LowerCaseString) {
        CHECK_THROW(cipher->decrypt(L"итереиПВМ"), cipher_error);
    }

    TEST_FIXTURE(Key3Fixture, WhitespaceString) {
        CHECK_THROW(cipher->decrypt(L"ИТР РЕИ ПВМ"), cipher_error);
    }

    TEST_FIXTURE(Key3Fixture, DigitsString) {
        CHECK_THROW(cipher->decrypt(L"ИТРЕПВ2232"), cipher_error);
    }

    TEST_FIXTURE(Key3Fixture, NoLettersDecrypt) {
        CHECK_THROW(cipher->decrypt(L"1234"), cipher_error);
    }

    TEST_FIXTURE(Key3Fixture, EmptyStringDecrypt) {
        CHECK_THROW(cipher->decrypt(L""), cipher_error);
    }

    TEST_FIXTURE(Key1Fixture, KeyEqualsOneDecrypt) {
        CHECK_WIDE_EQUAL(L"ПРИВЕТМИР", cipher->decrypt(L"ПРИВЕТМИР"));
    }

    TEST_FIXTURE(Key3Fixture, ValidCipherText) {
        CHECK_WIDE_EQUAL(L"ПРИВЕТМИР", cipher->decrypt(L"ИТРРЕИПВМ"));
    }

    TEST(ShortString) {
        Table cipher(5);
        wstring original = L"А";
        wstring encrypted = cipher.encrypt(original);
        wstring decrypted = cipher.decrypt(encrypted);
        CHECK_WIDE_EQUAL(original, decrypted);
    }

    TEST(ShortCipherText) {
        Table cipher(3);
        CHECK_WIDE_EQUAL(L"А", cipher.decrypt(L"А"));
    }
}

int main(int argc, char** argv)
{
    locale::global(locale(""));
    return UnitTest::RunAllTests();
}