#include <crypto++/config_int.h>
#include <crypto++/cryptlib.h>
#include <crypto++/filters.h>
#include <fstream>
#include <iostream>
#include "cryptopp/aes.h"
#include "cryptopp/modes.h"
#include <cryptopp/osrng.h>
#include <sqlite3.h>
#include <sstream>
#include <string>

using namespace std;
using namespace CryptoPP;

sqlite3* db = nullptr;
byte key[AES::DEFAULT_KEYLENGTH], iv[AES::BLOCKSIZE];

// initialize vector
void InitializeVector(){
    AutoSeededRandomPool  prng;
    prng.GenerateBlock(iv, sizeof(iv));
}

// prompt user for symmetric key
bool PromptForSymmetricKey() {
    cout << "Enter symmetric key: ";
    cin >> key;
    return true; // error handling 
}

// encrypt password using key
string EncryptPassword(const string& password) {
    if (!PromptForSymmetricKey()) {
        cerr << "Failed to get symmetric key." << endl;
        return ""; // handle error 
    }

    string encrypted;
    try {
        CBC_Mode<AES>::Encryption encryption(key, AES::DEFAULT_KEYLENGTH, iv);
        StringSource(password, true, new StreamTransformationFilter(encryption, new StringSink(encrypted)));
    } catch (const CryptoPP::Exception& e) {
        cerr << "Encryption error: " << e.what() << endl;
    }
    return encrypted;
}


// Prompt symmetric key for decryption
bool PromptForDecryptionKey() {
    cout << "Enter symmetric key for decryption: ";
    cin >> key;
    return true; // error handling 
}

// decrypt password using key
string DecryptPassword(const string& encryptedPassword) {
    if (!PromptForDecryptionKey()) {
        cerr << "Failed to get symmetric key for decryption." << endl;
        return ""; // handle error 
    }

    string decrypted;
    try {
        CBC_Mode<AES>::Decryption decryption(key, AES::DEFAULT_KEYLENGTH, iv);
        StringSource(encryptedPassword, true, new StreamTransformationFilter(decryption, new StringSink(decrypted)));
    } catch (const CryptoPP::Exception& e) {
        cerr << "Decryption error: " << e.what() << endl;
    }
    return decrypted;
}


// open db or create if not exist
sqlite3* openDB(){
    sqlite3* db;
    int status = sqlite3_open("passwords.db", &db);
    if (status != SQLITE_OK){
        cerr << "Error opening DB: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }
    return db;
}
 // create a new record
void createNewRecord(const string &website, const string &password) {
    if (!db) {
        cerr << "Database is not open." << endl;
        return;
    }
    char *errorMessage = nullptr;

    // encrypt the password
    string encryptedPassword = EncryptPassword(password);

    // create the table if it doesn't exist
    string createTableSQL = "CREATE TABLE IF NOT EXISTS passwords ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "website TEXT NOT NULL,"
                            "password TEXT NOT NULL)";

    // create the table
    int status = sqlite3_exec(db, createTableSQL.c_str(), nullptr, 0, &errorMessage);
    if (status != SQLITE_OK) {
        cerr << "Error creating table: " << errorMessage << endl;
        sqlite3_free(errorMessage);
        return;
    }

    // insert data into the table
    string insertSQL = "INSERT INTO passwords (website, password) VALUES (?, ?)";

    // prepare the SQL statement
    sqlite3_stmt *stmt;
    status = sqlite3_prepare_v2(db, insertSQL.c_str(), -1, &stmt, nullptr);
    if (status != SQLITE_OK) {
        cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // bind parameters 
    status = sqlite3_bind_text(stmt, 1, website.c_str(), -1, SQLITE_STATIC);
    if (status != SQLITE_OK) {
        cerr << "Error binding website parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return;
    }
    status = sqlite3_bind_text(stmt, 2, encryptedPassword.c_str(), -1, SQLITE_STATIC);
    if (status != SQLITE_OK) {
        cerr << "Error binding password parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return;
    }

    // execute the SQL statement
    status = sqlite3_step(stmt);
    if (status != SQLITE_DONE) {
        cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return;
    }

    // finalize the statement
    sqlite3_finalize(stmt);
    cout << "New record created successfully" << endl;
}

// display all records
void displayAllRecords() {
    if (!db) {
        cerr << "Database is not open." << endl;
        return;
    }
    char* errorMessage = nullptr;
    // select all records from the table
    string selectSQL = "SELECT * FROM passwords";
    // crepare the SQL statement
    sqlite3_stmt* stmt;
    int status = sqlite3_prepare_v2(db, selectSQL.c_str(), -1, &stmt, nullptr);
    if (status != SQLITE_OK) {
        cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << endl;
        return;
    }
    // execute the SQL statement 
    cout << "ID\tWebsite\t\tPassword" << endl;
    cout << "--------------------------------" << endl;
    while ((status = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* website = sqlite3_column_text(stmt, 1);
        const unsigned char* password = sqlite3_column_text(stmt, 2);
        cout << id << "\t" << website << "\t" << password << endl;
    }
    // check for errors or end of rows
    if (status != SQLITE_DONE) {
        cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << endl;
    }
    // Finalize the statement
    sqlite3_finalize(stmt);
}

// display record by id 
void displayRecordById(int id) {
    if (!db) {
        cerr << "Database is not open." << endl;
        return;
    }
    char* errorMessage = nullptr;

    // select record with the given id
    stringstream selectSQL;
    selectSQL << "SELECT * FROM passwords WHERE id = " << id;

    // prepare the SQL statement
    sqlite3_stmt* stmt;
    int status = sqlite3_prepare_v2(db, selectSQL.str().c_str(), -1, &stmt, nullptr);
    if (status != SQLITE_OK) {
        cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // execute the SQL statement 
    cout << "ID\tWebsite\t\tPassword" << endl;
    cout << "--------------------------------" << endl;
    while ((status = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* website = sqlite3_column_text(stmt, 1);
        const unsigned char* password = sqlite3_column_text(stmt, 2);
        cout << id << "\t" << website << "\t" << password << endl;
    }

    // check for errors or end of rows
    if (status != SQLITE_DONE) {
        cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << endl;
    }

    // finalize the statement
    sqlite3_finalize(stmt);
}

// update record by ID
void updateRecordById(int id) {
    if (!db) {
        cerr << "Database is not open." << endl;
        return;
    }
    string newPassword;

    cout << "Enter new password: " << endl;
    getline(cin, newPassword);

    char* errorMessage = nullptr;

    // Encrypt the new password
    string encryptedPassword = EncryptPassword(newPassword);

    // update password for the given ID
    stringstream updateSQL;
    updateSQL << "UPDATE passwords SET password = ? WHERE id = ?";

    // prepare the SQL statement
    sqlite3_stmt* statement;
    int status = sqlite3_prepare_v2(db, updateSQL.str().c_str(), -1, &statement, nullptr);
    if (status != SQLITE_OK) {
        cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Bind parameters 
    status = sqlite3_bind_text(statement, 1, encryptedPassword.c_str(), -1, SQLITE_STATIC);
    if (status != SQLITE_OK) {
	    cerr << "Error binding password parameter: " << sqlite3_errmsg(db) << endl;
	    sqlite3_finalize(statement);
	    return;
    }
    status = sqlite3_bind_int(statement, 2, id);
    if (status != SQLITE_OK) {
	    cerr << "Error binding ID parameter: " << sqlite3_errmsg(db) << endl;
	    sqlite3_finalize(statement);
	    return;
    }


    // execute the SQL statement
    status = sqlite3_step(statement);
    if (status != SQLITE_DONE) {
        cerr << "Error updating record: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Record updated successfully" << endl;
    }

    // finalize the statement
    sqlite3_finalize(statement);
}

// delete record by ID
void deleteRecordById(int id) {
    if (!db) {
        cerr << "Database is not open." << endl;
        return;
    }
    char* errorMessage = nullptr;

    // delete record with the given ID
    stringstream deleteSQL;
    deleteSQL << "DELETE FROM passwords WHERE id = " << id;

    // Execute the SQL statement
    int status = sqlite3_exec(db, deleteSQL.str().c_str(), nullptr, 0, &errorMessage);
    if (status != SQLITE_OK) {
        cerr << "Error deleting record: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    } else {
        cout << "Record deleted successfully" << endl;
    }
}

void decryptRecordById(int id) {
    if (!db) {
        cerr << "Db is not open." << endl;
        return;
    }

    char* errorMessage = nullptr;

    // select record with given id
    stringstream selectSQL;
    selectSQL << "SELECT * FROM passwords WHERE id = " << id;

    // prepare the SQL statement
    sqlite3_stmt* stmt;
    int status = sqlite3_prepare_v2(db, selectSQL.str().c_str(), -1, &stmt, nullptr);
    if (status != SQLITE_OK) {
        cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Execute the SQL statement 
    cout << "ID\tWebsite\t\tPassword" << endl;
    cout << "--------------------------------" << endl;
    while ((status = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* website = sqlite3_column_text(stmt, 1);
        const unsigned char* encryptedPassword = sqlite3_column_text(stmt, 2);

        // Convert const unsigned char* to string
        string encryptedPasswordStr(reinterpret_cast<const char*>(encryptedPassword));

        // Decrypt the password
        // Prompt for symmetric key for decryption
        if (!PromptForDecryptionKey()) {
            cerr << "Failed to get symmetric key for decryption." << endl;
            sqlite3_finalize(stmt);
            return;
        }

        string decryptedPassword;
        try {
            CBC_Mode<AES>::Decryption decryption(key, AES::DEFAULT_KEYLENGTH, iv);
            StringSource(encryptedPasswordStr, true, new StreamTransformationFilter(decryption, new StringSink(decryptedPassword),StreamTransformationFilter::NO_PADDING));
        } catch (const CryptoPP::Exception& e) {
            cerr << "Decryption error: " << e.what() << endl;
            sqlite3_finalize(stmt);
            return;
        }

        // output the decrypted record
        cout << id << "\t" << website << "\t" << decryptedPassword << endl;
    }

    // check for errors or end of rows
    if (status != SQLITE_DONE) {
        cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << endl;
    }

    // finalize
    sqlite3_finalize(stmt);
}




// close the database connection
void closeDB() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

int main() {
    InitializeVector();

    db = openDB();
    if(!db){
        cerr << "Failed to open Db." << endl;
        return 1;
    }

    int choice;
    int id;
    string website, password;

    bool keepRunning = true;

    while(keepRunning){
        cout << "Password Security System" << endl;
        cout << "-----------------------" << endl;
        cout << "1. Create a new record" << endl;
        cout << "2. Display all records" << endl;
        cout << "3. Display record by ID" << endl;
        cout << "4. Update record by ID" << endl;
        cout << "5. Delete record by ID" << endl;
        cout << "6. Decrypt record by ID" << endl;
        cout << "7. Quit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        
        // Clear the newline character from the input buffer
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "Enter website: ";
                getline(cin, website);
                cout << "Enter password: ";
                getline(cin, password);
                createNewRecord(website, password);
                break;
            case 2:
                displayAllRecords();
                break;
            case 3:
                cout << "Enter ID: ";
                cin >> id;
                cin.ignore();
                displayRecordById(id);
                break;
            case 4:
                cout << "Enter ID: ";
                cin >> id;
                cin.ignore();
                updateRecordById(id);
                break;
            case 5:
                cout << "Enter ID: ";
                cin >> id;
                cin.ignore();
                deleteRecordById(id);
                break;
            case 6:
                cout << "Enter ID: ";
                cin >> id;
                cin.ignore();
                decryptRecordById(id);
                break;
            case 7:
                keepRunning = false;
                break;
            default:
                cout << "Invalid choice" << endl;
                break;
        }
    }
    closeDB();

    return 0;
}

