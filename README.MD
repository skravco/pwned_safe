# PWNED SAFE

Program provides a simple password security system that allows users to manage their passwords securely. It uses SQLite for database storage and Crypto++ library for encryption and decryption of passwords.

## Functionality

The password security system provides the following functionalities:

1. **Create a new record**: Allows users to create a new record by entering the website name and password.

2. **Display all records**: Displays all existing records stored in the database.

3. **Display record by ID**: Displays a specific record based on the provided ID.

4. **Update record by ID**: Allows users to update an existing record by providing the ID.

5. **Delete record by ID**: Allows users to delete an existing record by providing the ID.

6. **Decrypt record by ID**: Decrypts the password of a specific record based on the provided ID.

7. **Quit**: Exits the program.

## External Dependencies

The program relies on the following external dependencies:

- **SQLite3**: For database storage.
sudo apt-get install libsqlite3-dev

- **Crypto++**: For encryption and decryption of passwords.
sudo apt-get install libcrypto++-dev

## Compilation

To compile the program, make sure you have `g++` installed on your system. Then, run the following command in the terminal:

```bash
g++ -std=c++11 -o ps main.cpp database.cpp encrypt.cpp -lsqlite3 -lcryptopp```
This command compiles the program and links it with the SQLite3 and Crypto++ libraries.

## Usage

After compiling the program, execute the generated executable (ps) to run the password security system. Follow the on-screen prompts to utilize the various functionalities provided by the system.

