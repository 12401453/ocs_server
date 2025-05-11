#include <iostream>
#include <string>
#include <sqlite3.h>
#include <unicode/unistr.h>

int main() {
  sqlite3* DB;

  if(!sqlite3_open("chu.db", &DB)) {
    
  }

  else {
    std::cout << "Problem opening database\n";
    return 0;
  }

}