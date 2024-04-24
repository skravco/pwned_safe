#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

extern sqlite3* db;

sqlite3*
openDB();
void
createNewRecord(sqlite3* db,
                const std::string& website,
                const std::string& password);
void
displayAllRecords(sqlite3* db);
void
displayRecordById(sqlite3* db, int id);
void
updateRecordById(sqlite3* db, int id);
void
deleteRecordById(sqlite3* db, int id);
void
decryptRecordById(sqlite3* db, int id);
void
closeDB();

#endif // DATABASE_H

