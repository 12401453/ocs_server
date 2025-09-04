#include <iostream>
#include <fstream>
#include <sqlite3.h>

void replaceAll(std::string &source, const std::string yeeted, const std::string replacement) {
    
  size_t yeeted_length = yeeted.length();
  if(yeeted_length == 0) return;
  size_t replacement_length = replacement.length();

  size_t yeeted_pos = source.find(yeeted);
  while(yeeted_pos != std::string::npos) {
      source.replace(yeeted_pos, yeeted_length, replacement); 
      yeeted_pos = source.find(yeeted, yeeted_pos + replacement_length);
  }
}

int main() {

  sqlite3* DB;

  if(!sqlite3_open("handwoerterbuch.sqlite", &DB)) {
    
    sqlite3_stmt* select_stmt;
    const char* select_sql = "SELECT col_2_c, col_2, col_3 FROM transliterated_data";

    sqlite3_prepare_v2(DB, select_sql, -1, &select_stmt, nullptr);

    std::ofstream hwb_json_file("handwoerterbuch.json");
    hwb_json_file << "[";
    std::string cyr_header;
    std::string lat_header;
    std::string ger_definition;
    while(sqlite3_step(select_stmt) == SQLITE_ROW) {

      cyr_header = (const char*)sqlite3_column_text(select_stmt, 0);
      lat_header = (const char*)sqlite3_column_text(select_stmt, 1);
      ger_definition = (const char*)sqlite3_column_text(select_stmt, 2);

      replaceAll(cyr_header, "z", "ѕ");
      replaceAll(cyr_header, "к’", "к҄");
      replaceAll(cyr_header, "ꙙ", "ѧ");

      hwb_json_file << "\n  [\"" << cyr_header << "\",\"" << lat_header << "\",\"" << ger_definition << "\"],"; 
    }
    hwb_json_file.seekp(-1, std::ios_base::cur);
    hwb_json_file << "\n]";

    hwb_json_file.close();
    sqlite3_finalize(select_stmt);
    sqlite3_close(DB);
  }
  else {
    std::cout << "Problem opening database-file\n";
  }

  return 0;
}