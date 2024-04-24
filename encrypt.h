#ifndef ENCRYPT_H
#define ENCRYPT_H

#include "cryptopp/aes.h"
#include "cryptopp/modes.h"
#include <string>

extern CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH];
extern CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];

void
InitializeVector();
std::string
EncryptPassword(const std::string& password);
std::string
DecryptPassword(const std::string& encryptedPassword);
bool
PromptForDecryptionKey();
#endif // ENCRYPT_H

