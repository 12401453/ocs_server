//compile: g++ -O3 -std=c++20 orv_words_sqlite.cpp -lsqlite3 -licuuc -licudata -licui18n -o orv_words_sqlite
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
    return 2;
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
  if (conj_type == "masc_o_in")
    return 1;

  else return 0;
}

void removeShit(std::string& dirty_word) {
  replaceAll(dirty_word, "꙽", "");
  replaceAll(dirty_word, "\uF002", "");
  replaceAll(dirty_word, "\uF102", "");
  replaceAll(dirty_word, "$", ""); //these four are characters which I may or may not use to indicate supralinears in the database
  replaceAll(dirty_word, "@", "");
  replaceAll(dirty_word, "—", "");
  replaceAll(dirty_word, "·", "");
  replaceAll(dirty_word, "̇", "");
  replaceAll(dirty_word, "\u0308", "");
  replaceAll(dirty_word, "⁛", "");
  replaceAll(dirty_word, "῾", "");
  replaceAll(dirty_word, "᾽", "");
  replaceAll(dirty_word, "̅", "");
  replaceAll(dirty_word, "̄", "");
  replaceAll(dirty_word, "̀", "");
  replaceAll(dirty_word, ">", "");
  replaceAll(dirty_word, "/", "");
  replaceAll(dirty_word, "\"", "");
  replaceAll(dirty_word, "͡", "");
  replaceAll(dirty_word, "·", "");
  replaceAll(dirty_word, "̏", "");
    //" " : "",
  replaceAll(dirty_word, "+", "");
  replaceAll(dirty_word, "⁜", "");
  replaceAll(dirty_word, "͠", "");
  replaceAll(dirty_word, "!", "");
  replaceAll(dirty_word, "҅", "");
  replaceAll(dirty_word, "҆", "");
  replaceAll(dirty_word, "҄", "");
  replaceAll(dirty_word, "͑", "");
  replaceAll(dirty_word, "͗", "");
  replaceAll(dirty_word, "̆", "");
  replaceAll(dirty_word, "̈", "");
  replaceAll(dirty_word, "̑", "");
  replaceAll(dirty_word, "̒", "");
  replaceAll(dirty_word, "̓", "");
  replaceAll(dirty_word, "̔", "");
  replaceAll(dirty_word, "̕", "");
  replaceAll(dirty_word, "͆", "");
  replaceAll(dirty_word, "͛", "");
  replaceAll(dirty_word, "͞", "");
  replaceAll(dirty_word, "ͨ", "");
  replaceAll(dirty_word, "҃", "");
  replaceAll(dirty_word, "҇", "");
  replaceAll(dirty_word, "ꙿ", "");
  replaceAll(dirty_word, "꙯", "");
  replaceAll(dirty_word, "'", "");
  replaceAll(dirty_word, "(", "");
  replaceAll(dirty_word, ")", "");
  replaceAll(dirty_word, "-", "");
  replaceAll(dirty_word, ".", "");
  replaceAll(dirty_word, ":", "");
  replaceAll(dirty_word, "=", "");
  replaceAll(dirty_word, "?", "");
  replaceAll(dirty_word, "[", "");
  replaceAll(dirty_word, "]", "");
  replaceAll(dirty_word, "{", "");
  replaceAll(dirty_word, "}", "");
  replaceAll(dirty_word, "̂", "");
  replaceAll(dirty_word, "ʼ", "");
  replaceAll(dirty_word, "ⸯ", "");
  replaceAll(dirty_word, "’", "");
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

std::unordered_map<int, std::string> text_id_map = { 
  {1, "Afanasij Nikitin’s journey beyond three seas"},
  {2, "The Life of Avvakum"},
  {3, "Birchbark letters"},
  {4, "Burtsov’s alphabet"},
  {5, "The tale of the fall of Constantinople"},
  {6, "Domostroj"},
  {7, "The Tale of Dracula"},
  {8, "Testament of Ivan Jurievich Grjaznoj"},
  {9, "Letter of F. I. Golitsyna to V. V. Golitsyn"},
  {10, "Letters of V. B. Golitsyn to Vl. B. Golitsyn"},
  {11, "The Kiev Chronicle, Codex Hypatianus"},
  {12, "Letter of E. Klementiev to F. M. Chelishchev"},
  {13, "Letter of M. D. Kurakina to B. I. Kurakin"},
  {14, "The Primary Chronicle, Codex Laurentianus"},
  {15, "The Tale of Luka Koločskij"},
  {16, "Letters of D. V. Mikhalkov to M. I. Mikhalkova and P. D. Mikhalkov"},
  {17, "Mstislav’s letter"},
  {18, "Colophon to Mstislav’s Gospel book"},
  {19, "Treaty of Alexander Nevskij and Novgorod with the Germans"},
  {20, "The list of the Novgorodians' losses"},
  {21, "Novgorod service book marginalia"},
  {22, "The First Novgorod Chronicle, Synodal manuscript"},
  {23, "Novgorod’s treaty with Grand Prince Jaroslav Jaroslavich, 1266"},
  {24, "Colophon to the Ostromir Codex"},
  {25, "Letter of U. S. Pazukhina to S. I. Pazukhin"},
  {26, "Correspondence of Peter the Great"},
  {27, "Missive from Prince Ivan of Pskov, 1463–1465"},
  {28, "The taking of Pskov"},
  {29, "The Primary Chronicle, Codex Hypatianus"},
  {30, "Missive from the Archbishop of Riga to the Prince of Smolensk"},
  {31, "The 1229 Treaty between Smolensk, Riga and Gotland (version A)"},
  {32, "Russkaja pravda"},
  {33, "Materials for the history of the schism"},
  {34, "Life of Sergij of Radonezh"},
  {35, "Letter of M. M. Shcherbatov to D. M. Shcherbatov"},
  {36, "Charter of Prince Jurij Svjatoslavich of Smolensk on the alliance with Poland and Lithuania, 1386"},
  {37, "The tale of Igor’s campaign"},
  {38, "Life of Stefan of Perm"},
  {39, "The Suzdal Chronicle, Codex Laurentianus"},
  {40, "Evidence of A. Turcheninov on fire of 29 May 1737"},
  {41, "Uspenskij sbornik"},
  {42, "Statute of Prince Vladimir"},
  {43, "Varlaam’s donation charter to the Xutyn monastery"},
  {44, "Vesti-Kuranty"},
  {45, "Zadonshchina"}
};

//needed for the step where we add the lcs_trigrams table
struct TrigrammifiableRow {
  int tokno;
  icu::UnicodeString word_unicode;
  int32_t num_chars;
};

class CsvReader {
  public:
    CsvReader(char separator=',') {
      m_separator = separator;
      m_fields_vec.reserve(32);
    }

    void setHeaders(const std::string& first_line) {
      m_header_index_map.clear();

      std::stringstream first_line_ss(first_line);
      std::string header;
      int header_idx = 0;
      while(std::getline(first_line_ss, header, m_separator)) {
        m_header_index_map.emplace(header, header_idx);
        ++header_idx;
      }
    }
   
    void setLine(const std::string& line) {
      m_fields_vec.clear();

      m_raw_line = line;
      std::stringstream line_ss(line);
      std::string field;
      while(std::getline(line_ss, field, m_separator)){
        m_fields_vec.emplace_back(field);
      }
    }
  
    std::string getField(const std::string& header) {
      return m_fields_vec[m_header_index_map.at(header)];
    }

  private:
    char m_separator;
    std::string m_raw_line;
    std::vector<std::string> m_fields_vec;
    std::unordered_map<std::string, int> m_header_index_map;
};

struct LemmaDBInfo {
  //"INSERT INTO lemmas (lemma_id, pos, lemma_lcs, lemma_ocs, stem_lcs, inflexion_class_id) VALUES (?,?,?,?,?,?)";
  //torot_pos and lemma_form are included in the key of the map which this struct is the value of
  int lemma_id;
  std::string lemma_lcs;
  std::string stem_lcs;
  std::string inflexion_class;
  std::string pv2_3_lemma;
  short int noun_verb;
};
struct CorpusDBInfo {    
  bool auto_tagged;
  bool pv2_3_exists;
  int sentence_no;
  int main_title_id;
  int subtitle_id;
  int lemma_id;
  std::string presentation_before;
  std::string presentation_after;
  std::string word_normalised;
  std::string morph_tag;
  std::string word_torot;
  std::string autoreconstructed_lcs;
  std::string autoreconstructed_morph_replace;
};

void writeTrigramsTable(sqlite3* DB, const std::string& corpus_column_name, const std::string& trigrams_table_name, bool remove_punct=false) {
  std::cout << "Writing " << corpus_column_name << " trigrams table into database...\n";

  sqlite3_exec(DB, std::string("DROP TABLE IF EXISTS "+trigrams_table_name+";CREATE TABLE "+trigrams_table_name+" (tokno INTEGER, trigram TEXT)").c_str(), nullptr, nullptr, nullptr);
  sqlite3_stmt* stmt_select;
  std::string sql_select = "SELECT tokno, "+corpus_column_name+" FROM corpus";
  
  sqlite3_prepare_v2(DB, sql_select.c_str(), -1, &stmt_select, NULL);
  
  std::vector<TrigrammifiableRow> rows_vec;
  rows_vec.reserve(524288);

  std::string word_str;
  
  while(sqlite3_step(stmt_select) == SQLITE_ROW) {
      int tokno = sqlite3_column_int(stmt_select, 0);
      
      const unsigned char* word_raw = sqlite3_column_text(stmt_select, 1);
      if(word_raw != nullptr) {
          word_str = (const char*)word_raw;
          if(remove_punct) {
            removeShit(word_str);
          }
          icu::UnicodeString word_unicode = word_str.c_str();
          
          int32_t num_chars = word_unicode.countChar32();
          
          rows_vec.emplace_back(tokno, word_unicode, num_chars);
          
      }
  }
  sqlite3_finalize(stmt_select);
  
  std::string sql_insert = "INSERT INTO "+trigrams_table_name+" (tokno, trigram) VALUES (?, ?)";
  sqlite3_stmt* stmt_insert;
  
  sqlite3_prepare_v2(DB, sql_insert.c_str(), -1, &stmt_insert, NULL);
  for(const auto& row : rows_vec) {
      
      std::string trigram;
      
      int32_t trigram_start_offset = 0;
      while(trigram_start_offset /*+ 2*/ < row.num_chars) {
          row.word_unicode.tempSubString(trigram_start_offset, 3).toUTF8String(trigram);
          sqlite3_bind_int(stmt_insert, 1, row.tokno);
          sqlite3_bind_text(stmt_insert, 2, trigram.c_str(), -1, SQLITE_STATIC);
          sqlite3_step(stmt_insert);
          sqlite3_reset(stmt_insert);
          sqlite3_clear_bindings(stmt_insert);      
          
          trigram.clear();
          trigram_start_offset++;
      }
  }
  sqlite3_finalize(stmt_insert);
  
  sqlite3_exec(DB, std::string("CREATE INDEX "+corpus_column_name+"_index ON "+trigrams_table_name+" (trigram COLLATE BINARY)").c_str(), nullptr, nullptr, nullptr);//I perhaps should use a join index here; I took out the tokno index because it prevents my queries from using the more important trigram index, since my WHERE clause filters by both
}

void buildDataStructures(std::string lemma_filename, std::string words_filename, std::unordered_map<int, Lemma>& lemma_list, std::unordered_map<std::string, LemmaDBInfo>& all_lemmas_map, std::unordered_map<std::string, int>& inflexion_class_map, std::vector<CorpusDBInfo>& corpus_vec) {
  
  lemma_list.clear();
  all_lemmas_map.reserve(16384);
  corpus_vec.reserve(262144);

  std::ifstream words_file(words_filename);
  std::ifstream lemmas_file(lemma_filename);

  if(!words_file.good()) {
    std::cout << "corpus file not found, exiting\n";
    exit(0);
  }
  if(!lemmas_file.good()){
    std::cout << "lemmas file not found, exiting\n";
    exit(0);
  }

  CsvReader csv_reader('|');

  std::string line;

  std::cout << "Reading lemma file...\n";
  std::getline(lemmas_file, line);
  csv_reader.setHeaders(line);
  int lemma_id_count = 1;
  inflexion_class_map.emplace("non_infl", 0);
  int inflexion_class_id = 1;
  while(std::getline(lemmas_file, line)) {
    csv_reader.setLine(line);
    std::string lcs_lemma = csv_reader.getField("lcs_lemma");
    if(lcs_lemma.empty()) continue; //this line here prevents lemmas which are present in my orv_lemmas_master.csv file, but not present in any of the texts, from getting stored in the database, in distinction to what happened under the previous regime. I suspect that these lemmas are junk that has been cleared out of the database between 2020 and 2023, so I'm not sure if keeping them is worth it or not.

    std::string orv_lemma = csv_reader.getField("orv_lemma");
    std::string torot_pos = csv_reader.getField("pos");
    short int noun_verb = std::stoi(csv_reader.getField("noun_verb"));
    std::string loan_place_str = csv_reader.getField("loan_place");
    short int loan_place = 0;
    if(!loan_place_str.empty()) loan_place = std::stoi(loan_place_str);
    std::string morph_replace = csv_reader.getField("morph_replace");
    std::string poss_doublet = csv_reader.getField("doublet");
    std::string pre_jot = csv_reader.getField("pre_jot");
    std::string root_1 = csv_reader.getField("stem1");
    std::string root_2 = csv_reader.getField("stem2");
    std::string inflexion_class = csv_reader.getField("conj_type");
    std::string pv2_3_lemma = csv_reader.getField("PV2/3");
    if(noun_verb == 0) inflexion_class = "non_infl";

    std::string stem_lcs = "";
    icu::UnicodeString unicode_lemma_lcs;

    if(inflexion_class == "11" || inflexion_class == "12" || inflexion_class == "15" || inflexion_class == "16" || inflexion_class == "infix_11" || inflexion_class == "infix_12") {
                stem_lcs = root_2;
    }
    else if(inflexion_class == "14") {
        stem_lcs = root_1;
    }
    else if(pre_jot == "") {
        stem_lcs = "";
        unicode_lemma_lcs = unicode_lemma_lcs.fromUTF8(lcs_lemma);
        unicode_lemma_lcs.truncate(unicode_lemma_lcs.length() - conj_type_Trunc(inflexion_class));
        unicode_lemma_lcs.toUTF8String(stem_lcs);
    }
    else {
        stem_lcs = "";
        unicode_lemma_lcs = unicode_lemma_lcs.fromUTF8(pre_jot);
        unicode_lemma_lcs.truncate(unicode_lemma_lcs.length() - conj_type_Trunc(inflexion_class));
        unicode_lemma_lcs.toUTF8String(stem_lcs);
    }
    
    
    std::string pos_lemma_combo = torot_pos + orv_lemma;

    lemma_list.emplace(lemma_id_count, Lemma{runTimeHashString(pos_lemma_combo), stem_lcs, morph_replace, poss_doublet, loan_place, 0, pre_jot, inflexion_class, noun_verb});
    
    all_lemmas_map.emplace(pos_lemma_combo, LemmaDBInfo{lemma_id_count, lcs_lemma, stem_lcs, inflexion_class, pv2_3_lemma, noun_verb});
    if(!inflexion_class.empty() && inflexion_class != "non_infl" && !inflexion_class_map.contains(inflexion_class)) {
      inflexion_class_map.emplace(inflexion_class, inflexion_class_id);
      inflexion_class_id++;
    }
    lemma_id_count++;
  }
  lemmas_file.close();

  std::cout << "Reading words file...\n";
  std::getline(words_file, line);
  csv_reader.setHeaders(line);
  while(std::getline(words_file, line)) {
    
    csv_reader.setLine(line);

    std::string word_torot = csv_reader.getField("torot_word");
    std::string word_normalised = csv_reader.getField("deep_cleaned");
    std::string morph_tag = csv_reader.getField("morph_tag");
    int sentence_no = std::stoi(csv_reader.getField("sentence_no"));
    std::string presentation_before = csv_reader.getField("pres_before");
    std::string presentation_after = csv_reader.getField("pres_after");
    int main_title_id = std::stoi(csv_reader.getField("text_id"));
    int subtitle_id = std::stoi(csv_reader.getField("subtitle_id"));
    bool auto_tagged = (bool)std::stoi(csv_reader.getField("autotagged"));

    std::string orv_lemma = csv_reader.getField("lemma");
    std::string torot_pos = csv_reader.getField("pos");
    std::string pos_lemma_combo = torot_pos + orv_lemma;

    if(!orv_lemma.empty() && !all_lemmas_map.contains(pos_lemma_combo)){
      all_lemmas_map.emplace(pos_lemma_combo, LemmaDBInfo{lemma_id_count, "", "", ""});
      lemma_id_count++;
    }

    int lemma_id = 0;
    bool pv2_3_exists = false;
    std::string autoreconstructed_lcs = "";
    std::string autoreconstructed_morph_replace = "";
    if(all_lemmas_map.contains(pos_lemma_combo)) {
      lemma_id = all_lemmas_map.at(pos_lemma_combo).lemma_id;
      //std::cout << word_torot << "\n";
      std::array<std::string, 2> autoreconstructed_array = ReconstructMorphReplace(word_normalised, morph_tag, lemma_id);
      autoreconstructed_lcs = autoreconstructed_array[0];
      autoreconstructed_morph_replace = autoreconstructed_array[1];
      if(!all_lemmas_map.at(pos_lemma_combo).pv2_3_lemma.empty()) {
        pv2_3_exists = true;
      }
    }

    corpus_vec.emplace_back(CorpusDBInfo{auto_tagged, pv2_3_exists, sentence_no, main_title_id, subtitle_id, lemma_id, presentation_before, presentation_after, word_normalised, morph_tag, word_torot, autoreconstructed_lcs, autoreconstructed_morph_replace});

  }
  words_file.close();
  
}


int main () {
    //std::cout.setstate(std::ios_base::failbit);
    sqlite3* DB;
    if(!sqlite3_open("orv.db", &DB)) {

        sqlite3_stmt* statement;

        // std::ifstream words_file("orv_words_full_with_titles_untagged.csv");
        // std::ifstream lemmas_file("orv_lemmas_master.csv");
        std::ifstream subtitles_file("orv_subtitles.csv");

       
        std::unordered_map<std::string, LemmaDBInfo> all_lemmas_map;
        std::unordered_map<std::string, int> inflexion_class_map;
        std::vector<CorpusDBInfo> corpus_vec;
        buildDataStructures("orv_lemmas_master.csv", "orv_words_full_with_titles_untagged.csv", lemma_list, all_lemmas_map, inflexion_class_map, corpus_vec);

        std::cout << "Reconstruction complete\n\nreconstructed lemma count: " << lemma_list.size() << "\n";
        std::cout << "all orv lemma count: " << all_lemmas_map.size() << "\n";
        std::cout << "corpus_vec size: " << corpus_vec.size() << "\n";

       
        std::string subtitles_line;
        std::vector<std::vector<std::string>> subtitles_vector;
        std::string text_id_str = "";
        std::string subtitle_text = "";
        std::vector<std::string> text_subtitles;
        while(std::getline(subtitles_file, subtitles_line)) {
          
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

        std::cout << "subtitles_vector size: " << subtitles_vector.size() << "\n";

        std::cout << "Writing into database...\n";

        const char* sql_BEGIN = "BEGIN IMMEDIATE";
        const char* sql_COMMIT = "COMMIT";

        sqlite3_exec(DB, sql_BEGIN, nullptr, nullptr, nullptr);

        sqlite3_exec(DB, "DROP TABLE IF EXISTS corpus;CREATE TABLE corpus (tokno INTEGER PRIMARY KEY, chu_word_torot TEXT, chu_word_normalised TEXT, morph_tag TEXT, lemma_id INTEGER, sentence_no INTEGER, presentation_before TEXT, presentation_after TEXT, autoreconstructed_lcs TEXT, inflexion_class_id INTEGER, autoreconstructed_morph_replace TEXT, auto_tagged INTEGER, inflexion_class TEXT, pv2_3_exists INTEGER);CREATE INDEX lemma_id_index on corpus(lemma_id) WHERE lemma_id IS NOT NULL;CREATE INDEX sentence_no_index on corpus(sentence_no);CREATE INDEX inflexion_class_corpus_index on corpus(inflexion_class_id) WHERE inflexion_class_id IS NOT NULL", nullptr, nullptr, nullptr);
        sqlite3_exec(DB, "DROP TABLE IF EXISTS lemmas;CREATE TABLE lemmas (lemma_id INTEGER PRIMARY KEY, pos INTEGER, lemma_lcs TEXT, lemma_ocs TEXT, stem_lcs TEXT, inflexion_class_id INTEGER, pv2_3_lemma TEXT);CREATE INDEX inflexion_class_index ON lemmas(inflexion_class_id) WHERE inflexion_class_id IS NOT NULL", nullptr, nullptr, nullptr);
        sqlite3_exec(DB, "DROP TABLE IF EXISTS inflexion_classes;CREATE TABLE inflexion_classes (inflexion_class_id INTEGER PRIMARY KEY, inflexion_class_name TEXT)", nullptr, nullptr, nullptr);
        sqlite3_exec(DB, "DROP TABLE IF EXISTS texts;CREATE TABLE texts (text_id INTEGER PRIMARY KEY, text_title TEXT, tokno_start INTEGER, tokno_end INTEGER)", nullptr, nullptr, nullptr);
        sqlite3_exec(DB, "DROP TABLE IF EXISTS subtitles;CREATE TABLE subtitles (subtitle_id INTEGER PRIMARY KEY, subtitle_text TEXT, text_id INTEGER, tokno_start INTEGER, tokno_end INTEGER)", nullptr, nullptr, nullptr);
        /*sqlite3_exec(DB, "DROP TABLE IF EXISTS lcs_trigrams;CREATE TABLE lcs_trigrams (tokno INTEGER, trigram TEXT)", nullptr, nullptr, nullptr);
        sqlite3_exec(DB, "DROP TABLE IF EXISTS chu_clean_trigrams;CREATE TABLE chu_clean_trigrams (tokno INTEGER, trigram TEXT)", nullptr, nullptr, nullptr);*/


        const char *sql_text = "INSERT INTO corpus (chu_word_torot, chu_word_normalised, morph_tag, lemma_id, sentence_no, presentation_before, presentation_after, autoreconstructed_lcs, autoreconstructed_morph_replace, auto_tagged, pv2_3_exists) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

        const char* sql_text_table = "INSERT INTO texts (text_title, tokno_start, tokno_end) VALUES (?, ?, ?)";
        const char* sql_subtitle_table = "INSERT INTO subtitles (subtitle_text, text_id, tokno_start, tokno_end) VALUES (?, ?, ?, ?)";

        std::cout << "Writing main corpus to database...\n";
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

        for(const auto& corpus_row : corpus_vec) {

            

            if(corpus_row.autoreconstructed_lcs.empty()) {
              sqlite3_bind_null(statement, 8);
              sqlite3_bind_null(statement, 9);
            }
            else {
              sqlite3_bind_text(statement, 8, corpus_row.autoreconstructed_lcs.c_str(), -1, SQLITE_TRANSIENT);
              if(corpus_row.autoreconstructed_morph_replace.empty()) sqlite3_bind_null(statement, 9);
              else sqlite3_bind_text(statement, 9, corpus_row.autoreconstructed_morph_replace.c_str(), -1, SQLITE_TRANSIENT);
            }

            sqlite3_bind_int(statement, 4, corpus_row.lemma_id);
            sqlite3_bind_int(statement, 5, corpus_row.sentence_no);
            sqlite3_bind_text(statement, 1, corpus_row.word_torot.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, corpus_row.word_normalised.c_str(), -1, SQLITE_TRANSIENT);
            if(!corpus_row.morph_tag.empty()) sqlite3_bind_text(statement, 3, corpus_row.morph_tag.c_str(), -1, SQLITE_TRANSIENT);
            else sqlite3_bind_null(statement, 3);
            sqlite3_bind_text(statement, 6, corpus_row.presentation_before.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 7, corpus_row.presentation_after.c_str(), -1, SQLITE_TRANSIENT);
            if(corpus_row.auto_tagged) sqlite3_bind_int(statement, 10, 1);
            else sqlite3_bind_null(statement, 10);
            if(corpus_row.pv2_3_exists) sqlite3_bind_int(statement, 11, 1);
            else sqlite3_bind_null(statement, 11);

            sqlite3_step(statement);
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);
            if(current_subtitle_id != corpus_row.subtitle_id || current_main_title_id < corpus_row.main_title_id) {
              subtitle_tokno_end = tokno_count - 1;
              sqlite3_bind_text(statement_subtitles, 1, subtitles_vector[current_main_title_id - 1][current_subtitle_id - 1].c_str(), -1, SQLITE_TRANSIENT);
              sqlite3_bind_int(statement_subtitles, 2, current_main_title_id); 
              sqlite3_bind_int(statement_subtitles, 3, subtitle_tokno_start);
              sqlite3_bind_int(statement_subtitles, 4, subtitle_tokno_end);

              sqlite3_step(statement_subtitles);
              sqlite3_reset(statement_subtitles);
              sqlite3_clear_bindings(statement_subtitles);

              current_subtitle_id = corpus_row.subtitle_id;
              subtitle_tokno_start = tokno_count;
            }
            if(current_main_title_id < corpus_row.main_title_id) {
              main_tokno_end = tokno_count - 1;
              
              sqlite3_bind_text(statement_texts, 1, text_id_map.at(current_main_title_id).c_str(), -1, SQLITE_TRANSIENT);
              sqlite3_bind_int(statement_texts, 2, main_tokno_start);
              sqlite3_bind_int(statement_texts, 3, main_tokno_end);

              sqlite3_step(statement_texts);
              sqlite3_reset(statement_texts);
              sqlite3_clear_bindings(statement_texts);

              current_main_title_id = corpus_row.main_title_id;
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


        //lemmas table
        std::cout << "Writing lemmas table into database...\n";
        sqlite3_stmt* lemma_stmt;
        sql_text = "INSERT INTO lemmas (lemma_id, pos, lemma_lcs, lemma_ocs, stem_lcs, inflexion_class_id, pv2_3_lemma) VALUES (?,?,?,?,?,?,?)";
        sqlite3_prepare_v2(DB, sql_text, -1, &lemma_stmt, nullptr);

        //INFLEXION_CLASS SHIT TODO
        sqlite3_stmt* stmt_update_corpus;
        sqlite3_prepare_v2(DB, "UPDATE corpus SET inflexion_class_id = ?, inflexion_class = ? WHERE lemma_id = ?", -1, &stmt_update_corpus, nullptr);

        std::ofstream lemmas_json_file("../HTML_DOCS/orv_lemmas_json.json");
        lemmas_json_file << "[";
        for(const auto& lemma_row : all_lemmas_map) {

          sqlite3_bind_int(lemma_stmt, 1, lemma_row.second.lemma_id);
          sqlite3_bind_int(lemma_stmt, 2, pos_map.at(lemma_row.first.substr(0, 2))); //torot part-of-speech code as defined by the map at the top of the file
          sqlite3_bind_text(lemma_stmt, 3, lemma_row.second.lemma_lcs.c_str(), -1, SQLITE_TRANSIENT);
          sqlite3_bind_text(lemma_stmt, 4, lemma_row.first.substr(2).c_str(), -1, SQLITE_TRANSIENT); //torot lemma-form extracted from pos_lemma_combo string

          sqlite3_bind_text(lemma_stmt, 5, lemma_row.second.stem_lcs.c_str(), -1, SQLITE_TRANSIENT);

          if(!inflexion_class_map.contains(lemma_row.second.inflexion_class)) {
            sqlite3_bind_null(lemma_stmt, 6);
          }
          else {
            sqlite3_bind_int(lemma_stmt, 6, inflexion_class_map.at(lemma_row.second.inflexion_class));
            
            sqlite3_bind_int(stmt_update_corpus, 1, inflexion_class_map.at(lemma_row.second.inflexion_class));
            sqlite3_bind_text(stmt_update_corpus, 2, lemma_row.second.inflexion_class.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt_update_corpus, 3, lemma_row.second.lemma_id);

            sqlite3_step(stmt_update_corpus);
            sqlite3_reset(stmt_update_corpus);
            sqlite3_clear_bindings(stmt_update_corpus);

            if(lemma_row.second.noun_verb != 0) {
              lemmas_json_file << "\n  [" << lemma_row.second.lemma_id << ", \"" << lemma_row.second.stem_lcs << "\",\"" << lemma_row.second.noun_verb << "\",\"" << lemma_row.second.inflexion_class << "\",\"" << lemma_row.second.lemma_lcs << "\",\"" << lemma_row.second.pv2_3_lemma << "\"],";
            }
          }

          if(!lemma_row.second.pv2_3_lemma.empty()) {
            sqlite3_bind_text(lemma_stmt, 7, lemma_row.second.pv2_3_lemma.c_str(), -1, SQLITE_TRANSIENT);
          }
          else sqlite3_bind_null(lemma_stmt, 7);

          sqlite3_step(lemma_stmt);
          sqlite3_reset(lemma_stmt);
          sqlite3_clear_bindings(lemma_stmt);
        }
        lemmas_json_file.seekp(-1, std::ios_base::cur);
        lemmas_json_file << "\n]";
        lemmas_json_file.close();
        sqlite3_finalize(stmt_update_corpus);
        sqlite3_finalize(lemma_stmt);

        sqlite3_stmt* inflexion_class_stmt;
        sql_text = "INSERT INTO inflexion_classes (inflexion_class_id, inflexion_class_name) VALUES (?, ?)";
        sqlite3_prepare_v2(DB, sql_text, -1, &inflexion_class_stmt, nullptr);
        for(const auto& pair : inflexion_class_map) {
            sqlite3_bind_int(inflexion_class_stmt, 1, pair.second);
            sqlite3_bind_text(inflexion_class_stmt, 2, pair.first.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(inflexion_class_stmt);
            sqlite3_reset(inflexion_class_stmt);
            sqlite3_clear_bindings(inflexion_class_stmt);
        }
        sqlite3_finalize(inflexion_class_stmt);

        

        /*std::cout << "Writing trigrams table into database...\n";
        //add the lcs_trigrams and an index on the trigrams table to make searches faster (does the same as the separate trigram_index.cpp program)
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

        const char* sql_insert = "INSERT INTO lcs_trigrams (tokno, trigram) VALUES (?, ?)";
        sqlite3_stmt* stmt_insert;

        sqlite3_prepare_v2(DB, sql_insert, -1, &stmt_insert, NULL);
        for(const auto& row : lcs_rows_vec) {
      
          std::string trigram;
          // if(row.num_chars < 4) {
          //   sqlite3_bind_int(stmt_insert, 1, row.tokno);
          //   row.lcs_unicode.toUTF8String(trigram);
          //   sqlite3_bind_text(stmt_insert, 2, trigram.c_str(), -1, SQLITE_STATIC);

          //   sqlite3_step(stmt_insert);
          //   sqlite3_reset(stmt_insert);
          //   sqlite3_clear_bindings(stmt_insert);

          //   continue;
          // }

          int32_t trigram_start_offset = 0;
          while(trigram_start_offset < row.num_chars) {
            //int32_t substr_length = row.num_chars - trigram_start_offset > 2 ? 3 : 
            row.lcs_unicode.tempSubString(trigram_start_offset, 3).toUTF8String(trigram);
            sqlite3_bind_int(stmt_insert, 1, row.tokno);
            sqlite3_bind_text(stmt_insert, 2, trigram.c_str(), -1, SQLITE_STATIC);
            sqlite3_step(stmt_insert);
            sqlite3_reset(stmt_insert);
            sqlite3_clear_bindings(stmt_insert);

            trigram.clear();
            trigram_start_offset++;

          }
        }

        sqlite3_finalize(stmt_insert);

        sqlite3_exec(DB, "CREATE INDEX trigram_index ON lcs_trigrams (trigram COLLATE BINARY)", nullptr, nullptr, nullptr);//I perhaps should use a join index here; I took out the tokno index because it prevents my queries from using the more important trigram index, since my WHERE clause filters by both */
        
        writeTrigramsTable(DB, "autoreconstructed_lcs", "lcs_trigrams");
        writeTrigramsTable(DB, "chu_word_torot", "chu_word_clean_trigrams", true);
        writeTrigramsTable(DB, "chu_word_normalised", "chu_normalised_trigrams");

        
        std::cout << sqlite3_exec(DB, sql_COMMIT, nullptr, nullptr, nullptr);
        sqlite3_close(DB);

        return 0;
    }
    else std::cout << "DB opening failed\n";

    
    return 0;
}
