#include "database.h"
#include "encrypt.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

int
main()
{
    // InitializeVector();

    db = openDB();
    if (!db) {
        cerr << "Failed to open Db." << endl;
        return 1;
    }

    int choice;
    int id;
    string website, password;

    bool keepRunning = true;

    while (keepRunning) {
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
                createNewRecord(db, website, password);
                break;
            case 2:
                displayAllRecords(db);
                break;
            case 3:
                cout << "Enter ID: ";
                cin >> id;
                cin.ignore();
                displayRecordById(db, id);
                break;
            case 4:
                cout << "Enter ID: ";
                cin >> id;
                cin.ignore();
                updateRecordById(db, id);
                break;
            case 5:
                cout << "Enter ID: ";
                cin >> id;
                cin.ignore();
                deleteRecordById(db, id);
                break;
            case 6:
                cout << "Enter ID: ";
                cin >> id;
                cin.ignore();
                decryptRecordById(db, id);
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

