#ifndef SQLITE2DICT_H
#define SQLITE2DICT_IMPLEMENTATION
#define SQLITE2DICT_H

#include <sqlite3.h>
#include <stdio.h>  // fprintf
#include <stdlib.h> // malloc, calloc, realloc()

typedef struct
{
    char* column_name;
    char** values;
    int values_amount;
} dict_sqlite_entry;

typedef struct
{
    dict_sqlite_entry* entries;
    int entries_amount;
} dict_sqlite;

void
dict_sqlite_get_from_stmt(dict_sqlite* d, sqlite3_stmt* stmt);

int
dict_sqlite_get_from_file(char* db_filename,
                          char* sql_query,
                          dict_sqlite* dict);

void
dict_sqlite_print(dict_sqlite* dict);

#endif // !SQLITE2DICT_H