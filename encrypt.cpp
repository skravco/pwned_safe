#include "encrypt.h"
#include <crypto++/config_int.h>
#include <crypto++/cryptlib.h>
#include <crypto++/filters.h>
#include <cryptopp/osrng.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "cryptopp/aes.h"
#include "cryptopp/modes.h"

using namespace std;
using namespace CryptoPP;

// initialize vector
void
InitializeVector()
{
    AutoSeededRandomPool prng;
    prng.GenerateBlock(iv, sizeof(iv));
}

// prompt user for symmetric key
bool
PromptForSymmetricKey()
{
    cout << "Enter symmetric key: ";
    cin >> key;
    return true; // error handling
}

// encrypt password using key
std::string
EncryptPassword(const std::string& password)
{
    if (!PromptForSymmetricKey()) {
        cerr << "Failed to get symmetric key." << endl;
        return ""; // handle error
    }

    string encrypted;
    try {
        CBC_Mode<AES>::Encryption encryption(key, AES::DEFAULT_KEYLENGTH, iv);
        StringSource(password,
                     true,
                     new StreamTransformationFilter(encryption,
                                                    new StringSink(encrypted)));
    } catch (const CryptoPP::Exception& e) {
        cerr << "Encryption error: " << e.what() << endl;
    }
    return encrypted;
}

// Prompt symmetric key for decryption
bool
PromptForDecryptionKey()
{
    cout << "Enter symmetric key for decryption: ";
    cin >> key;
    return true; // error handling
}

// decrypt password using key
std::string
DecryptPassword(const std::string& encryptedPassword)
{

    if (!PromptForDecryptionKey()) {
        cerr << "Failed to get symmetric key for decryption." << endl;
        return ""; // handle error
    }

    string decrypted;
    try {
        CBC_Mode<AES>::Decryption decryption(key, AES::DEFAULT_KEYLENGTH, iv);
        StringSource(encryptedPassword,
                     true,
                     new StreamTransformationFilter(decryption,
                                                    new StringSink(decrypted)));
    } catch (const CryptoPP::Exception& e) {
        cerr << "Decryption error: " << e.what() << endl;
    }
    return decrypted;
}

