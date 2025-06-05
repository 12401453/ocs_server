#include <iostream>
#include <sqlite3.h>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <unicode/unistr.h>
#include <vector>

#include "main_stripped.cpp"

int conj_type_Trunc(std::string conj_type) {

  if (conj_type == "byti")
    return 4;
  if (conj_type == "pref_byti")
    return 4;
  if (conj_type == "nebyti")
    return 6;
  if (conj_type == "dati")
    return 4;
  if (conj_type == "have")
    return 6;
  if (conj_type == "eat")
    return 5;
  if (conj_type == "pref_eat")
    return 4;
  if (conj_type == "wote")
    return 6;
  if (conj_type == "will")
    return 6;
  if (conj_type == "stati")
    return 5;
  if (conj_type == "dovleti")
    return 8;
  if (conj_type == "dedj")
    return 4;
  if (conj_type == "sleep")
    return 6;

  if (conj_type == "čuti")
    return 2;
  if (conj_type == "17")
    return 3;
  if (conj_type == "18")
    return 3;
  if (conj_type == "19")
    return 3;
  if (conj_type == "jьti")
    return 4;
  if (conj_type == "13")
    return 4;

  if (conj_type == "21")
    return 4;
  if (conj_type == "22")
    return 3;
  if (conj_type == "ьt")
    return 4;
  if (conj_type == "ьz")
    return 4;
  if (conj_type == "uti")
    return 3;
  if (conj_type == "rti")
    return 4;
  if (conj_type == "rěsti")
    return 5;

  if (conj_type == "31")
    return 3;
  if (conj_type == "viděti")
    return 3;
  if (conj_type == "jaxati")
    return 4;
  if (conj_type == "32")
    return 3;
  if (conj_type == "40")
    return 3;
  if (conj_type == "51")
    return 3;
  if (conj_type == "51_abl")
    return 3;
  if (conj_type == "52_abl")
    return 3;
  if (conj_type == "iskati")
    return 3;
  if (conj_type == "52")
    return 3;
  if (conj_type == "53")
    return 3;
  if (conj_type == "53_abl")
    return 3;
  if (conj_type == "54")
    return 2;
  if (conj_type == "61")
    return 5;
  if (conj_type == "62")
    return 5;


  if (conj_type == "adj_soft")
    return 1;
  if (conj_type == "adj_hard")
    return 1;
  if (conj_type == "adj_soft_ord")
    return 1;
  if (conj_type == "adj_hard_ord")
    return 1;
  if (conj_type == "adj_ij")
    return 1;
  if (conj_type == "masc_o")
    return 1;
  if (conj_type == "masc_u")
    return 1;
  if (conj_type == "masc_i")
    return 1;
  if (conj_type == "masc_jo")
    return 1;
  if (conj_type == "masc_jo_foreign")
    return 1;
  if (conj_type == "masc_ju")
    return 1;
  if (conj_type == "masc_o_u")
    return 1;
  if (conj_type == "masc_a")
    return 1;
  if (conj_type == "masc_ja")
    return 1;
  if (conj_type == "masc_ji")
    return 1;
  if (conj_type == "masc_N")
    return 1;
  if (conj_type == "masc_tel")
    return 5;
  if (conj_type == "masc_ar")
    return 1;
  if (conj_type == "masc_o_PV3")
    return 1;
  if (conj_type == "fem_a")
    return 1;
  if (conj_type == "fem_a_PV3")
    return 1;
  if (conj_type == "masc_a_PV3")
    return 1;
  if (conj_type == "fem_ja")
    return 1;
  if (conj_type == "fem_ji")
    return 1;
  if (conj_type == "fem_R")
    return 1;
  if (conj_type == "fem_uu")
    return 1;
  if (conj_type == "fem_i")
    return 1;
  if (conj_type == "tri")
    return 1;
  if (conj_type == "nt_o")
    return 1;
  if (conj_type == "nt_S")
    return 1;
  if (conj_type == "nt_o_S")
    return 1;
  if (conj_type == "four")
    return 1;
  if (conj_type == "nt_jo")
    return 1;
  if (conj_type == "nt_N")
    return 1;
  if (conj_type == "nt_NT")
    return 1;
  if (conj_type == "nt_o_PV3")
    return 1;
  if (conj_type == "kamene")
    return 3;
  if (conj_type == "oko")
    return 1;

  if (conj_type == "den")
    return 4;   //extend to include all the conj_types
  if (conj_type == "masc_anin")
    return 4;
  if (conj_type == "pron_soft")
    return 1;
  if (conj_type == "pron_hard")
    return 1;
  if (conj_type == "kъto")
    return 4;
  if (conj_type == "kъtože")
    return 6;
  if (conj_type == "čьto")
    return 4;
  if (conj_type == "čьtože")
    return 6;
  if (conj_type == "kъjь")
    return 3;
  if (conj_type == "kъjьže")
    return 5;
  if (conj_type == "vьxь")
    return 1;
  if (conj_type == "sь")
    return 2;
  if (conj_type == "jь")
    return 2;
  if (conj_type == "jьže")
    return 4;
  if (conj_type == "pron_hard_ђe")
    return 4;
  if (conj_type == "pron_hard_že")
    return 3;
  if (conj_type == "kъžьdo")
    return 5;
  if (conj_type == "long_adj_noun")
    return 1;
  if (conj_type == "azъ")
    return 3;
  if (conj_type == "ty")
    return 2;
  if (conj_type == "sę")
    return 2;
  if (conj_type == "adj_ьj")
    return 1;
  if (conj_type == "dъva")
    return 1;
  if (conj_type == "adj_soft_comp")
    return 1;
  if (conj_type == "ten")
    return 1;
  if (conj_type == "1.1_PRAP")
    return 1;

  else return 0;
}

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

std::unordered_map <int, std::string> text_id_map = {
  {1, "Treatise on the letters"},
  {2, "Euchologium Sinaiticum"},
  {3, "Kiev Missal"},
  {4, "Codex Marianus"},
  {5, "Psalterium Sinaiticum"},
  {6, "Codex Suprasliensis"},
  {7, "Vita Constantini"},
  {8, "Vita Methodii"},
  {9, "Codex Zographensis"}
};

int main () {
    //std::cout.setstate(std::ios_base::failbit);
    sqlite3* DB;
    if(!sqlite3_open("chu_corpus_untagged.db", &DB)) {

        sqlite3_stmt* statement;

        std::ifstream chu_words_file("chu_words_full_with_titles_untagged.csv");
        std::ifstream lemma_spreadsheet("lemmas_with_text_occurence_gdrive.csv");
        std::ifstream chu_lemmas_file("chu_lemmas.csv");
        std::ifstream chu_subtitles_file("chu_subtitles.csv");

        std::string subtitles_line;
        std::vector<std::vector<std::string>> subtitles_vector;
        std::string text_id_str = "";
        std::string subtitle_text = "";
        std::vector<std::string> text_subtitles;
        while(std::getline(chu_subtitles_file, subtitles_line)) {
          
          int sep_pos = subtitles_line.find('|');
          if(text_id_str != subtitles_line.substr(0, sep_pos) && !text_subtitles.empty()) {
            subtitles_vector.push_back(text_subtitles);
            text_subtitles.clear();
          }
          subtitle_text = subtitles_line.substr(sep_pos + 1);
          text_subtitles.push_back(subtitle_text);
          text_id_str = subtitles_line.substr(0, sep_pos);
        }
        subtitles_vector.push_back(text_subtitles);

        const char* sql_BEGIN = "BEGIN IMMEDIATE";
        const char* sql_COMMIT = "COMMIT";

        sqlite3_exec(DB, sql_BEGIN, nullptr, nullptr, nullptr);

        sqlite3_exec(DB, "DROP TABLE IF EXISTS corpus;CREATE TABLE corpus (tokno INTEGER PRIMARY KEY, chu_word_torot TEXT, chu_word_normalised TEXT, morph_tag TEXT, lemma_id INTEGER, sentence_no INTEGER, presentation_before TEXT, presentation_after TEXT, autoreconstructed_lcs TEXT, inflexion_class_id INTEGER, autoreconstructed_morph_replace TEXT);CREATE INDEX lemma_id_index on corpus(lemma_id) WHERE lemma_id IS NOT NULL;CREATE INDEX sentence_no_index on corpus(sentence_no);CREATE INDEX inflexion_class_corpus_index on corpus(inflexion_class_id) WHERE inflexion_class_id IS NOT NULL", nullptr, nullptr, nullptr);
        sqlite3_exec(DB, "DROP TABLE IF EXISTS lemmas;CREATE TABLE lemmas (lemma_id INTEGER PRIMARY KEY, pos INTEGER, lemma_lcs TEXT, lemma_ocs TEXT, stem_lcs TEXT, inflexion_class_id INTEGER);CREATE INDEX inflexion_class_index ON lemmas(inflexion_class_id) WHERE inflexion_class_id IS NOT NULL", nullptr, nullptr, nullptr);
        sqlite3_exec(DB, "DROP TABLE IF EXISTS inflexion_classes;CREATE TABLE inflexion_classes (inflexion_class_id INTEGER PRIMARY KEY, inflexion_class_name TEXT)", nullptr, nullptr, nullptr);
        sqlite3_exec(DB, "DROP TABLE IF EXISTS texts;CREATE TABLE texts (text_id INTEGER PRIMARY KEY, text_title TEXT, tokno_start INTEGER, tokno_end INTEGER)", nullptr, nullptr, nullptr);
        sqlite3_exec(DB, "DROP TABLE IF EXISTS subtitles;CREATE TABLE subtitles (subtitle_id INTEGER PRIMARY KEY, subtitle_text TEXT, text_id INTEGER, tokno_start INTEGER, tokno_end INTEGER)", nullptr, nullptr, nullptr);
        
        std::unordered_map<std::string, int> inflexion_class_map;
        std::unordered_map<std::string, int> lemma_id_map;

        const char *sql_text = "INSERT INTO corpus (chu_word_torot, chu_word_normalised, morph_tag, lemma_id, sentence_no, presentation_before, presentation_after, autoreconstructed_lcs, autoreconstructed_morph_replace) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";

        const char* sql_text_table = "INSERT INTO texts (text_title, tokno_start, tokno_end) VALUES (?, ?, ?)";
        const char* sql_subtitle_table = "INSERT INTO subtitles (subtitle_text, text_id, tokno_start, tokno_end) VALUES (?, ?, ?, ?)";

        sqlite3_stmt* statement_texts;
        sqlite3_stmt* statement_subtitles;

        sqlite3_prepare_v2(DB, sql_text, -1, &statement, nullptr);
        sqlite3_prepare_v2(DB, sql_text_table, -1, &statement_texts, nullptr);
        sqlite3_prepare_v2(DB, sql_subtitle_table, -1, &statement_subtitles, nullptr);

        int tokno_count = 1;
        int current_main_title_id = 1;
        int current_subtitle_id = 1;
        int main_tokno_start = 1;
        int main_tokno_end;
        int subtitle_tokno_start = 1;
        int subtitle_tokno_end;                
        std::string line;
        std::string field;
        int pos_key = 0;
        while(std::getline(chu_words_file, line)) {
            std::stringstream ss_line(line);

            std::string chu_word_torot, chu_word_normalised, morph_tag, presentation_before, presentation_after, autoreconstructed_lcs, autoreconstructed_morph_replace;
            int lemma_id, sentence_no, main_title_id, subtitle_id;


            int row_no = 1;
            while(std::getline(ss_line, field, '|')) {
                switch(row_no) {
                    case 1:
                        chu_word_torot = field;	
                        break;
                    case 4:
                        chu_word_normalised = field;
                        break;
                    case 3:
                        morph_tag = field;
                        break;
                    case 5:
                        //std::cout << "stoi 01\n";
                        lemma_id = std::stoi(field);
                        break;
                    case 6:
                        //std::cout << "stoi 02\n";
                        sentence_no = std::stoi(field);
                        break;
                    case 7:
                        presentation_before = field;
                        break;
                    case 8:
                        presentation_after = field;
                        break;
                    case 9:
                        //std::cout << "stoi 03\n";
                        main_title_id = std::stoi(field);
                        break;
                    case 10:
                        //std::cout << "stoi 04\n";
                        subtitle_id = std::stoi(field);
                        break;
                    default:
                        ;
		            }
                row_no++;
            }
            std::array<std::string, 2> autoreconstructed_array = ReconstructMorphReplace(chu_word_torot, morph_tag, lemma_id);
            autoreconstructed_lcs = autoreconstructed_array[0];
            if(autoreconstructed_lcs.empty()) {
              sqlite3_bind_null(statement, 8);
              sqlite3_bind_null(statement, 9);
            }
            else {
              sqlite3_bind_text(statement, 8, autoreconstructed_lcs.c_str(), -1, SQLITE_TRANSIENT);
              autoreconstructed_morph_replace = autoreconstructed_array[1];
              if(autoreconstructed_morph_replace.empty()) sqlite3_bind_null(statement, 9);
              else sqlite3_bind_text(statement, 9, autoreconstructed_morph_replace.c_str(), -1, SQLITE_TRANSIENT);
            }

            if(lemma_id > 0) sqlite3_bind_int(statement, 4, lemma_id);
            else sqlite3_bind_null(statement, 4);
            sqlite3_bind_int(statement, 5, sentence_no);
            sqlite3_bind_text(statement, 1, chu_word_torot.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, chu_word_normalised.c_str(), -1, SQLITE_TRANSIENT);
            if(!morph_tag.empty()) sqlite3_bind_text(statement, 3, morph_tag.c_str(), -1, SQLITE_TRANSIENT);
            else sqlite3_bind_null(statement, 3);
            sqlite3_bind_text(statement, 6, presentation_before.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 7, presentation_after.c_str(), -1, SQLITE_TRANSIENT);

            sqlite3_step(statement);
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            if(current_subtitle_id != subtitle_id || current_main_title_id < main_title_id) {
              subtitle_tokno_end = tokno_count - 1;
              
              sqlite3_bind_text(statement_subtitles, 1, subtitles_vector[current_main_title_id - 1][current_subtitle_id - 1].c_str(), -1, SQLITE_TRANSIENT);
              sqlite3_bind_int(statement_subtitles, 2, current_main_title_id); 
              sqlite3_bind_int(statement_subtitles, 3, subtitle_tokno_start);
              sqlite3_bind_int(statement_subtitles, 4, subtitle_tokno_end);

              sqlite3_step(statement_subtitles);
              sqlite3_reset(statement_subtitles);
              sqlite3_clear_bindings(statement_subtitles);

              current_subtitle_id = subtitle_id;
              subtitle_tokno_start = tokno_count;
            }
            if(current_main_title_id < main_title_id) {
              main_tokno_end = tokno_count - 1;
              
              sqlite3_bind_text(statement_texts, 1, text_id_map.at(current_main_title_id).c_str(), -1, SQLITE_TRANSIENT);
              sqlite3_bind_int(statement_texts, 2, main_tokno_start);
              sqlite3_bind_int(statement_texts, 3, main_tokno_end);

              sqlite3_step(statement_texts);
              sqlite3_reset(statement_texts);
              sqlite3_clear_bindings(statement_texts);

              current_main_title_id = main_title_id;
              main_tokno_start = tokno_count;
            }
            tokno_count++;
        }
        main_tokno_end = tokno_count - 1;
        subtitle_tokno_end = tokno_count - 1;
        sqlite3_bind_text(statement_subtitles, 1, subtitles_vector[current_main_title_id - 1][current_subtitle_id - 1].c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(statement_subtitles, 2, current_main_title_id); 
        sqlite3_bind_int(statement_subtitles, 3, subtitle_tokno_start);
        sqlite3_bind_int(statement_subtitles, 4, subtitle_tokno_end);
        sqlite3_step(statement_subtitles);

        sqlite3_bind_text(statement_texts, 1, text_id_map.at(current_main_title_id).c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(statement_texts, 2, main_tokno_start);
        sqlite3_bind_int(statement_texts, 3, main_tokno_end);
        sqlite3_step(statement_texts);


        sqlite3_finalize(statement);
        sqlite3_finalize(statement_subtitles);
        sqlite3_finalize(statement_texts);


        while(std::getline(chu_lemmas_file, line)) {
            std::stringstream ss_line(line);
            int row_no = 1;
            std::string pos_lemma_combo = "";
            int lemma_id = 0;
            while(std::getline(ss_line, field, ',')) {
                switch(row_no){
                    case 1:
                        //std::cout << "stoi 05\n";
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
        chu_lemmas_file.close();

        sql_text = "INSERT INTO lemmas (lemma_id, pos, lemma_lcs, lemma_ocs, stem_lcs, inflexion_class_id) VALUES (?,?,?,?,?,?)";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, nullptr);

        int verb_class_count = 1;
        int noun_class_count = 1;

        //this is going to miss out lemmas which are in the corpus but not in my lemmas_spreadsheet, e.g. the adjective видимъ from Supr. (8162)
        while(std::getline(lemma_spreadsheet, line)) {
            std::stringstream ss_line(line);
            int parameter_no = 1;
            int row_no = 1;

            int lemma_id, pos, inflexion_class_id, verb_noun;
            std::string lemma_lcs, lemma_ocs, pre_jot, stem_lcs, pos_lemma_combo, inflexion_class, root_1, root_2;
            icu::UnicodeString unicode_lemma_lcs;

            while(std::getline(ss_line, field, '|')) {
                switch(row_no) {
                    case 1:
                        lemma_ocs = field;
                        pos_lemma_combo = field;
                        break;
                    case 2:
                    //assign first to the id used in my spreadsheet incase the lemma doesnt exist in the 2023 version of the torot files (which it definitely won't for my added ones, but also some may have been deleted since 2020)
                        //std::cout << "stoi 06\n";
                        lemma_id = std::stoi(field);
                        break;
                    case 3:
                        if(pos_map.contains(field)) {
                            pos = pos_map.at(field);                        
                        }
                        else pos = 0;
                        pos_lemma_combo = field + pos_lemma_combo;
                        break;
                    case 4:
                        lemma_lcs = field;
                        break;
                    case 12:
                        pre_jot = field;
                        break;
                    case 19:
                        root_1 = field;
                        break;
                    case 20:
                        root_2 = field;
                        break;
                    case 21:
                        inflexion_class = field;
                        break;
                    case 22:
                        //std::cout << "stoi 07\n";
                        verb_noun = std::stoi(field);
                        break;
                }
                row_no++;
            }
            if(lemma_id_map.contains(pos_lemma_combo)) {
              lemma_id = lemma_id_map.at(pos_lemma_combo);
              lemma_id_map.erase(pos_lemma_combo);
            }
            sqlite3_bind_int(statement, 1, lemma_id);
            sqlite3_bind_int(statement, 2, pos);
            sqlite3_bind_text(statement, 3, lemma_lcs.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 4, lemma_ocs.c_str(), -1, SQLITE_TRANSIENT);

            if(inflexion_class == "11" || inflexion_class == "12" || inflexion_class == "15" || inflexion_class == "16" || inflexion_class == "infix_11" || inflexion_class == "infix_12") {
                stem_lcs = root_2;
            }
            else if(inflexion_class == "14") {
                stem_lcs = root_1;
            }
            else if(pre_jot == "") {
                stem_lcs = "";
                unicode_lemma_lcs = unicode_lemma_lcs.fromUTF8(lemma_lcs);
                unicode_lemma_lcs.truncate(unicode_lemma_lcs.length() - conj_type_Trunc(inflexion_class));
                unicode_lemma_lcs.toUTF8String(stem_lcs);
            }
            else {
                stem_lcs = "";
                unicode_lemma_lcs = unicode_lemma_lcs.fromUTF8(pre_jot);
                unicode_lemma_lcs.truncate(unicode_lemma_lcs.length() - conj_type_Trunc(inflexion_class));
                unicode_lemma_lcs.toUTF8String(stem_lcs);
            }

            sqlite3_bind_text(statement, 5, stem_lcs.c_str(), -1, SQLITE_TRANSIENT);
            
            if(verb_noun == 1) {
                if(inflexion_class_map.contains(inflexion_class)) {
                    inflexion_class_id = inflexion_class_map.at(inflexion_class);
                }
                else {
                    inflexion_class_id = verb_class_count;
                    inflexion_class_map.emplace(inflexion_class, inflexion_class_id);
                    verb_class_count++;
                }     
            }
            else if(verb_noun == 2) {
               if(inflexion_class_map.contains(inflexion_class)) {
                    inflexion_class_id = inflexion_class_map.at(inflexion_class);
                }
                else {
                    inflexion_class_id = noun_class_count + 1000;
                    inflexion_class_map.emplace(inflexion_class, inflexion_class_id);
                    noun_class_count++;
                }     
            }
            else if(verb_noun == 0) inflexion_class_id = 0;
            else inflexion_class_id = 999999;
            
            if(inflexion_class_id == 999999) sqlite3_bind_null(statement, 6);
            else sqlite3_bind_int(statement, 6, inflexion_class_id);
            sqlite3_step(statement);
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);
        }
        

        for(const auto leftover_lemma_map_entry : lemma_id_map) {
          std::string leftover_pos_lemma_combo = leftover_lemma_map_entry.first;
          int leftover_lemma_id = leftover_lemma_map_entry.second;

          std::string leftover_pos_tag = leftover_pos_lemma_combo.substr(0, 2);
          std::cout << "leftover_pos_tag: " << leftover_pos_tag << " ";
          int leftover_pos = pos_map.at(leftover_pos_tag);
          std::string leftover_lemma_ocs = leftover_pos_lemma_combo.substr(2, leftover_pos_lemma_combo.length() - 2);
          std::cout << " " << leftover_lemma_ocs<< "\n";

          sqlite3_bind_int(statement, 1, leftover_lemma_id);
          sqlite3_bind_int(statement, 2, leftover_pos);
          sqlite3_bind_null(statement, 3);
          sqlite3_bind_text(statement, 4, leftover_lemma_ocs.c_str(), -1, SQLITE_TRANSIENT);
          sqlite3_bind_null(statement, 5);
          sqlite3_bind_null(statement, 6);

          sqlite3_step(statement);
          sqlite3_reset(statement);
          sqlite3_clear_bindings(statement);
          
        }
        sqlite3_finalize(statement);

        sql_text = "INSERT INTO inflexion_classes (inflexion_class_id, inflexion_class_name) VALUES (?, ?)";
        sqlite3_prepare_v2(DB, sql_text, -1, &statement, nullptr);
        for(const auto& pair : inflexion_class_map) {
            sqlite3_bind_int(statement, 1, pair.second);
            sqlite3_bind_text(statement, 2, pair.first.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(statement);
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);
        }
        sqlite3_finalize(statement);

        const char* sql_select_lemma_text = "SELECT lemma_id, inflexion_class_id FROM lemmas WHERE inflexion_class_id IS NOT NULL";
        sqlite3_stmt* stmt_select_lemma;
        sqlite3_prepare_v2(DB, sql_select_lemma_text, -1, &stmt_select_lemma, nullptr);
        const char* sql_update_infl_class_text = "UPDATE corpus SET inflexion_class_id = ? WHERE lemma_id = ?";
        sqlite3_stmt* stmt_update_corpus;
        sqlite3_prepare_v2(DB, sql_update_infl_class_text, -1, &stmt_update_corpus, nullptr);
        while(sqlite3_step(statement) == SQLITE_ROW) {
          int lemma_id = sqlite3_column_int(statement, 0);
          int inflexion_class_id = sqlite3_column_int(statement, 1);
          sqlite3_bind_int(stmt_update_corpus, 1, inflexion_class_id);
          sqlite3_bind_int(stmt_update_corpus, 2, lemma_id);
          sqlite3_step(stmt_update_corpus);
          sqlite3_reset(stmt_update_corpus);
          sqlite3_clear_bindings(stmt_update_corpus);
        }
        sqlite3_finalize(stmt_select_lemma);
        sqlite3_finalize(stmt_update_corpus);


        chu_words_file.close();
        lemma_spreadsheet.close();
        chu_subtitles_file.close();

        std::cout << sqlite3_exec(DB, sql_COMMIT, nullptr, nullptr, nullptr);

        sqlite3_close(DB);

        return 0;
    }
    else std::cout << "DB opening failed\n";

    
    return 0;
}
