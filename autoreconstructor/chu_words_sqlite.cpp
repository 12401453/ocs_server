#include <iostream>
#include <sqlite3.h>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>

std::unordered_map <std::string, int> pos_map = {
  {"A-", 1},
  {"Df", 2},
  {"S-", 3},
  {"Ma", 4},
  {"Nb", 5},
  {"C-", 6},
  {"Pd", 7},
  {"F-", 8},
  {"Px", 9},
  {"N-", 10},
  {"I-", 11},
  {"Du", 12},
  {"Pi", 13},
  {"Mo", 14},
  {"Pp", 15},
  {"Pk", 16},
  {"Ps", 17},
  {"Pt", 18},
  {"R-", 19},
  {"Ne", 20},
  {"Py", 21},
  {"Pc", 22},
  {"Dq", 23},
  {"Pr", 24},
  {"G-", 25},
  {"V-", 26},
  {"X-", 27}
};

int main () {
    sqlite3* DB;
    if(!sqlite3_open("chu_corpus.db", &DB)) {

        sqlite3_stmt* statement;

        std::ifstream chu_words_file("chu_words_full.csv");
        std::ifstream lemma_spreadsheet("lemmas_with_text_occurence_gdrive.csv");
        std::ifstream chu_lemmas_file("chu_lemmas.csv");
        const char* sql_BEGIN = "BEGIN IMMEDIATE";
        const char* sql_COMMIT = "COMMIT";

        sqlite3_exec(DB, sql_BEGIN, nullptr, nullptr, nullptr);

        sqlite3_exec(DB, "DROP TABLE IF EXISTS tagged_corpus;CREATE TABLE tagged_corpus (tokno INTEGER PRIMARY KEY, chu_word_torot TEXT, chu_word_normalised TEXT, morph_tag TEXT, lemma_id INTEGER, sentence_no INTEGER);CREATE INDEX lemma_id_index on tagged_corpus(lemma_id);CREATE INDEX sentence_id_index on tagged_corpus(sentence_id)", nullptr, nullptr, nullptr);
        sqlite3_exec(DB, "DROP TABLE IF EXISTS lemmas;CREATE TABLE lemmas (lemma_id INTEGER PRIMARY KEY, pos INTEGER, lemma_lcs TEXT, lemma_ocs TEXT, stem_lcs TEXT, inflexion_class_id INTEGER)", nullptr, nullptr, nullptr);
        sqlite3_exec(DB, "DROP TABLE IF EXISTS inflexion_class;CREATE TABLE inflexion_classes (inflexion_class_id INTEGER PRIMARY KEY, inflexion_class TEXT)", nullptr, nullptr, nullptr);
        
        std::unordered_map<std::string, int> inflexion_class_map;
        std::unordered_map<std::string, int> lemma_id_map;

        const char *sql_text = "INSERT INTO tagged_corpus (chu_word_torot, morph_tag, chu_word_normalised, lemma_id, sentence_no) VALUES (?, ?, ?, ?, ?)";

        sqlite3_prepare_v2(DB, sql_text, -1, &statement, nullptr);
               
        std::string line;
        std::string field;
        int pos_key = 0;
        while(std::getline(chu_words_file, line)) {
            std::stringstream ss_line(line);
            int parameter_no = 1;
            int row_no = 1;
            while(std::getline(ss_line, field, ',')) {
                switch(row_no) {
                    case 2:
                    //the POS will be stored in the lemmas-table and thus accessible from the lemma_id field
                        parameter_no--;		
                        break;
                    case 5:
                        sqlite3_bind_int(statement, parameter_no, std::stoi(field));
                        break;
                    case 6:
                        sqlite3_bind_int(statement, parameter_no, std::stoi(field));
                        break;
                    default:
                        sqlite3_bind_text(statement, parameter_no, field.c_str(), -1, SQLITE_TRANSIENT); //SQLITE_STATIC doesnt work and makes every field get filled with some permutation of sentence_no
		        }
                row_no++;
                parameter_no++;
            } 
            sqlite3_step(statement);
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);
        }
        sqlite3_finalize(statement);

        while(std::getline(chu_lemmas_file, line)) {
            std::stringstream ss_line(line);
            int row_no = 1;
            std::string pos_lemma_combo;
            int lemma_id = 0;
            while(std::getline(ss_line, field, ',')) {
                switch(row_no){
                    case 1:
                        lemma_id = std::stoi(field);
                        break;
                    case 2:
                        pos_lemma_combo.append(field);
                        break;
                    case 3:
                        pos_lemma_combo = field + pos_lemma_combo;
                        break;
                }
                row_no++;
            }
            lemma_id_map.emplace(pos_lemma_combo, lemma_id);
        }

        sql_text = "INSERT INTO lemmas (lemma_id, pos, lemma_lcs, lemma_ocs, stem_lcs, inflexion_class_id) VALUES (?,?,?,?,?,?)";

        while(std::getline(lemma_spreadsheet, line)) {
            std::stringstream ss_line(line);
            int parameter_no = 1;
            int row_no = 1;

            while(std::getline(ss_line, field, '|')) {

            }
        }


        chu_words_file.close();
        chu_lemmas_file.close();
        lemma_spreadsheet.close();

        std::cout << sqlite3_exec(DB, sql_COMMIT, nullptr, nullptr, nullptr);

        sqlite3_close(DB);

        return 0;
    }
    else std::cout << "DB opening failed\n";

    
    return 0;
}
