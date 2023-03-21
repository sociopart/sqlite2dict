#include <sqlite2dict.h>

void
dict_sqlite_get_from_stmt(dict_sqlite* d, sqlite3_stmt* stmt)
{
    int num_columns = sqlite3_column_count(stmt);
    int row_count = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char* current_column_name = NULL;
        int found_entry = 0;

        for (int i = 0; i < num_columns; i++) {
            current_column_name = strdup(sqlite3_column_name(stmt, i));
            if (current_column_name == NULL) {
                fprintf(stderr, "Error duplicating column name string\n");
                break;
            }

            char* value = strdup((const char*)sqlite3_column_text(stmt, i));
            if (value == NULL) {
                fprintf(stderr, "Error duplicating value string\n");
                break;
            }

            int column_index = -1;

            for (int j = 0; j < row_count; j++) {
                if (strcmp(d->entries[j].column_name, current_column_name) ==
                    0) {
                    found_entry = 1;
                    column_index = j;
                    break;
                }
            }

            if (!found_entry) {
                dict_sqlite_entry* new_entry = realloc(
                  d->entries, (row_count + 1) * sizeof(dict_sqlite_entry));
                if (new_entry == NULL) {
                    fprintf(stderr,
                            "Error allocating memory for dict_sqlite_entry\n");
                    break;
                }
                d->entries = new_entry;
                column_index = row_count;
                d->entries[column_index].column_name = current_column_name;
                d->entries[column_index].values =
                  calloc(num_columns, sizeof(char*));
                if (d->entries[column_index].values == NULL) {
                    fprintf(
                      stderr,
                      "Error allocating memory for dict_sqlite_entry values\n");
                    break;
                }
                d->entries[column_index].values_amount = 0;
                row_count++;
            } else {
                free(current_column_name);
            }

            char** new_values = realloc(
              d->entries[column_index].values,
              (d->entries[column_index].values_amount + 1) * sizeof(char*));
            if (new_values == NULL) {
                fprintf(
                  stderr,
                  "Error reallocating memory for dict_sqlite_entry values\n");
                break;
            }
            d->entries[column_index].values = new_values;
            d->entries[column_index]
              .values[d->entries[column_index].values_amount] = value;
            d->entries[column_index].values_amount++;
        }
    }

    d->entries_amount = row_count;
    sqlite3_finalize(stmt);
}

int
dict_sqlite_get_from_file(char* db_filename, char* sql_query, dict_sqlite* dict)
{
    sqlite3* db;
    char* err_msg = 0;
    if (sqlite3_open(db_filename, &db) != SQLITE_OK) {
        fprintf(stderr, "Error opening database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql_query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing query: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    dict_sqlite_get_from_stmt(dict, stmt);
    sqlite3_close(db);
}

void
dict_sqlite_print(dict_sqlite* dict)
{
    for (int i = 0; i < dict->entries_amount; i++) {
        fprintf(stdout, "Column name: %s\n", dict->entries[i].column_name);
        fprintf(stdout, "Column vals: \n");
        for (int j = 0; j < dict->entries[i].values_amount; j++) {
            fprintf(stdout, "[%d] %s \n", j, dict->entries[i].values[j]);
        }
    }
}