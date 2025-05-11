#include <iostream>
#include <string>
#include <sqlite3.h>
#include <unicode/unistr.h>
#include <vector>

struct LcsRow {
  int tokno;
  icu::UnicodeString lcs_unicode;
  int32_t num_chars;
};

int main() {
  sqlite3* DB;

  if(!sqlite3_open("chu.db", &DB)) {

    sqlite3_exec(DB, "DROP TABLE IF EXISTS lcs_trigrams;CREATE TABLE lcs_trigrams (tokno INTEGER, trigram TEXT)", nullptr, nullptr, nullptr);

    sqlite3_stmt* stmt_select;
    const char* sql_select = "SELECT tokno, autoreconstructed_lcs FROM corpus";

    sqlite3_prepare_v2(DB, sql_select, -1, &stmt_select, NULL);

    std::vector<LcsRow> lcs_rows_vec;
    lcs_rows_vec.reserve(524288);

    while(sqlite3_step(stmt_select) == SQLITE_ROW) {
      int tokno = sqlite3_column_int(stmt_select, 0);

      const unsigned char* lcs_raw = sqlite3_column_text(stmt_select, 1);
      if(lcs_raw != nullptr) {
        icu::UnicodeString lcs_unicode = (const char*)lcs_raw;
        
        int32_t num_chars = lcs_unicode.countChar32();

        LcsRow row = {tokno, lcs_unicode, num_chars};
        lcs_rows_vec.emplace_back(row);
        
      }
    }
    sqlite3_finalize(stmt_select);

    sqlite3_exec(DB, "BEGIN IMMEDIATE", nullptr, nullptr, nullptr);
    const char* sql_insert = "INSERT INTO lcs_trigrams (tokno, trigram) VALUES (?, ?)";
    sqlite3_stmt* stmt_insert;

    sqlite3_prepare_v2(DB, sql_insert, -1, &stmt_insert, NULL);
    for(const auto& row : lcs_rows_vec) {
  
      std::string trigram;
      if(row.num_chars < 4) {
        sqlite3_bind_int(stmt_insert, 1, row.tokno);
        row.lcs_unicode.toUTF8String(trigram);
        sqlite3_bind_text(stmt_insert, 2, trigram.c_str(), -1, SQLITE_STATIC);

        sqlite3_step(stmt_insert);
        sqlite3_reset(stmt_insert);
        sqlite3_clear_bindings(stmt_insert);

        continue;
      }

      int32_t trigram_start_offset = 0;
      while(trigram_start_offset + 2 < row.num_chars) {
        row.lcs_unicode.tempSubString(trigram_start_offset, 3).toUTF8String(trigram);
        sqlite3_bind_int(stmt_insert, 1, row.tokno);
        sqlite3_bind_text(stmt_insert, 2, trigram.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt_insert);
        sqlite3_reset(stmt_insert);
        sqlite3_clear_bindings(stmt_insert);

        trigram.clear();
        trigram_start_offset++;

      }
      
      // std::string lcs_str;
      // row.lcs_unicode.toUTF8String(lcs_str);
      // std::cout << row.tokno << " " << lcs_str << " " << row.num_chars << "\n";
    }

    sqlite3_finalize(stmt_insert);

    sqlite3_exec(DB, "CREATE INDEX trigram_index ON lcs_trigrams (trigram COLLATE BINARY)", nullptr, nullptr, nullptr);//I perhaps should use a join index here; I took out the tokno index because it prevents my queries from using the more important trigram index, since my WHERE clause filters by both
    sqlite3_exec(DB, "COMMIT", nullptr, nullptr, nullptr);
    sqlite3_close(DB);
  }

  else {
    std::cout << "Problem opening database\n";
    return 0;
  }

}