#include <fstream>
#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <string>

using namespace std;

sqlite3* db = nullptr;

// Open Db or Create if Not Exist
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

    // Create SQL statement to create the table if it doesn't exist
    string createTableSQL = "CREATE TABLE IF NOT EXISTS passwords ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "website TEXT NOT NULL,"
                            "password TEXT NOT NULL)";

    // Execute SQL statement to create the table
    int status = sqlite3_exec(db, createTableSQL.c_str(), nullptr, 0, &errorMessage);
    if (status != SQLITE_OK) {
        cerr << "Error creating table: " << errorMessage << endl;
        sqlite3_free(errorMessage);
        return;
    }

    // Create SQL statement to insert data into the table
    string insertSQL = "INSERT INTO passwords (website, password) VALUES (?, ?)";

    // Prepare the SQL statement
    sqlite3_stmt *stmt;
    status = sqlite3_prepare_v2(db, insertSQL.c_str(), -1, &stmt, nullptr);
    if (status != SQLITE_OK) {
        cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Bind parameters to the prepared statement
    status = sqlite3_bind_text(stmt, 1, website.c_str(), -1, SQLITE_STATIC);
    if (status != SQLITE_OK) {
        cerr << "Error binding website parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return;
    }
    status = sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    if (status != SQLITE_OK) {
        cerr << "Error binding password parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return;
    }

    // Execute the SQL statement
    status = sqlite3_step(stmt);
    if (status != SQLITE_DONE) {
        cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return;
    }

    // Finalize the prepared statement
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
    // Create SQL statement to select all records from the table
    string selectSQL = "SELECT * FROM passwords";
    // Prepare the SQL statement
    sqlite3_stmt* stmt;
    int status = sqlite3_prepare_v2(db, selectSQL.c_str(), -1, &stmt, nullptr);
    if (status != SQLITE_OK) {
        cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << endl;
        return;
    }
    // Execute the SQL statement and display the results
    cout << "ID\tWebsite\t\tPassword" << endl;
    cout << "--------------------------------" << endl;
    while ((status = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* website = sqlite3_column_text(stmt, 1);
        const unsigned char* password = sqlite3_column_text(stmt, 2);
        cout << id << "\t" << website << "\t" << password << endl;
    }
    // Check for errors or end of rows
    if (status != SQLITE_DONE) {
        cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << endl;
    }
    // Finalize the prepared statement
    sqlite3_finalize(stmt);
}

// display record by ID
void displayRecordById(int id) {
    if (!db) {
        cerr << "Database is not open." << endl;
        return;
    }
    char* errorMessage = nullptr;

    // Create SQL statement to select record with the given ID
    stringstream selectSQL;
    selectSQL << "SELECT * FROM passwords WHERE id = " << id;

    // Prepare the SQL statement
    sqlite3_stmt* stmt;
    int status = sqlite3_prepare_v2(db, selectSQL.str().c_str(), -1, &stmt, nullptr);
    if (status != SQLITE_OK) {
        cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Execute the SQL statement and display the result
    cout << "ID\tWebsite\t\tPassword" << endl;
    cout << "--------------------------------" << endl;
    while ((status = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* website = sqlite3_column_text(stmt, 1);
        const unsigned char* password = sqlite3_column_text(stmt, 2);
        cout << id << "\t" << website << "\t" << password << endl;
    }

    // Check for errors or end of rows
    if (status != SQLITE_DONE) {
        cerr << "Error executing SQL statement: " << sqlite3_errmsg(db) << endl;
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);
}

// Function to update record by ID
void updateRecordById(int id) {
    if (!db) {
        cerr << "Database is not open." << endl;
        return;
    }
    string password;

    cout << "Enter new password: " << endl;
    getline(cin, password);

    char* errorMessage = nullptr;

    // Create SQL statement to update password for the given ID
    stringstream updateSQL;
    updateSQL << "UPDATE passwords SET password = ? WHERE id = ?";

    // Prepare the SQL statement
    sqlite3_stmt* statement;
    int status = sqlite3_prepare_v2(db, updateSQL.str().c_str(), -1, &statement, nullptr);
    if (status != SQLITE_OK) {
        cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Bind parameters to the prepared statement
    sqlite3_bind_text(statement, 1, password.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(statement, 2, id);

    // Execute the SQL statement
    status = sqlite3_step(statement);
    if (status != SQLITE_DONE) {
        cerr << "Error updating record: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Record updated successfully" << endl;
    }

    // Finalize the statement
    sqlite3_finalize(statement);
}

// delete record by ID
void deleteRecordById(int id) {
    if (!db) {
        cerr << "Database is not open." << endl;
        return;
    }
    char* errorMessage = nullptr;

    // Create SQL statement to delete record with the given ID
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


// close the database connection
void closeDB() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

int main() {
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
    cout << "6. Quit" << endl;
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
            keepRunning = false;
            break;
        default:
            cout << "Invalid choice" << endl;
            break;
    }
    }
// Close Db Connaction
    closeDB();

    return 0;
}

