#include <vector>
#include <array>
#include <unordered_set>
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <string_view>
#include <cstdint>

typedef std::unordered_map<int, std::string> inner_map;

#include "verb_map.cpp"
#include "outer_verb_map.cpp"
#include "nom_map.cpp"
#include "outer_nom_map.cpp"


consteval std::uint64_t compileTimeHashString(std::string_view sv) {
    uint64_t hash = 1469598103934665603ULL;
    for(char c : sv) {
        hash = hash ^ static_cast<unsigned char>(c);
        hash = hash * 1099511628211ULL;
    }
    return hash;
}

std::uint64_t runTimeHashString(std::string_view sv) {
    uint64_t hash = 1469598103934665603ULL;
    for(char c : sv) {
        hash = hash ^ static_cast<unsigned char>(c);
        hash = hash * 1099511628211ULL;
    }
    return hash;
}

void replaceAll(std::string &source, const std::string& yeeted, const std::string& replacement) {
    
  size_t yeeted_length = yeeted.length();
  if(yeeted_length == 0) return;
  size_t replacement_length = replacement.length();

  size_t yeeted_pos = source.find(yeeted);
  while(yeeted_pos != std::string::npos) {
      source.replace(yeeted_pos, yeeted_length, replacement); 
      yeeted_pos = source.find(yeeted, yeeted_pos + replacement_length);
  }
}
void replaceEnd(std::string &source, const std::string yeeted, const std::string replacement) {
    
  size_t yeeted_length = yeeted.length();
  if(yeeted_length == 0) return;
  size_t replacement_length = replacement.length();

  size_t yeeted_pos = source.find(yeeted);
  if(yeeted_pos + yeeted_length == source.length()) {
      source.replace(yeeted_pos, yeeted_length, replacement); 
  }
}

struct Lemma
{

  uint64_t lemma_id{};
  std::string lemma_form{};
  std::string morph_replace{};
  //std::string second_velar{};
  std::string poss_doublet{};
  short int loan_place{};
  short int long_adj{};
  //bool non_assim{};
  //std::string eng_trans{};
  std::string pre_jot{};
  //std::string etym_disc{};
  //bool bad_etym{};
  //std::string loan_source{};
  std::string conj_type{};
  short int noun_verb{};

  // friend bool operator==(const Lemma &l1, const Lemma &l2);
  // friend bool operator<(const Lemma &l1, const Lemma &l2);

  Lemma(
      uint64_t lemma_id_, 
      std::string lemma_form_ = "", 
      std::string morph_replace_ = "", 
      //std::string second_velar_ = "", 
      std::string poss_doublet_ = "", 
      short int loan_place_ = 0, 
      short int long_adj_ = 0, 
      //bool non_assim_ = false, 
      //std::string eng_trans_ = "", 
      std::string pre_jot_ = "", 
      //std::string etym_disc_ = "", 
      //bool bad_etym_ = false, 
      //std::string loan_source_ = "", 
      std::string conj_type_ = "", 
      short int noun_verb_ = 0) : 
      
      lemma_id{lemma_id_}, 
      lemma_form{lemma_form_}, 
      morph_replace{morph_replace_}, 
      //second_velar{second_velar_}, 
      poss_doublet{poss_doublet_}, 
      loan_place{loan_place_}, 
      long_adj{long_adj_}, 
      //non_assim{non_assim_}, 
      //eng_trans{eng_trans_}, 
      pre_jot{pre_jot_}, 
      //etym_disc{etym_disc_}, 
      //bad_etym{bad_etym_}, 
      //loan_source{loan_source_}, 
      conj_type{conj_type_}, 
      noun_verb{noun_verb_}
  {
  }
};
// bool operator==(const Lemma &l1, const Lemma &l2)
// {
//   return (l1.lemma_id == l2.lemma_id);
// }
// bool operator<(const Lemma &l1, const Lemma &l2)
// {
//   return (l1.lemma_id < l2.lemma_id);
// }

std::string str_Truncate(std::string str1, int lop_off)
{

  int end_pos = str1.length();
  int cut_pos = end_pos - lop_off;
  std::string str_cut = str1.replace(cut_pos, lop_off, "");
  return str_cut;
}

//#include "lemlist_2.h"
std::unordered_map<int, Lemma> lemma_list;
#include "conj_type_enum.h"
#include "verb_cleaning.h"
#include "noun_cleaning.h"
#include "functions.h"

#include "noun_Flect.h"
#include "changeLemma_field.h"

/*std::string Dejotate(std::string cs_word)
{

  std::string str1, str2;
  int x, y;
  for (int letno = -1; letno < 38; letno++)
  {

    switch (letno)
    {

    case -1:
      str1.assign("stvj");
      str2.assign("šћvĺ");
      break;
    case 0:
      str1.assign("strj");
      str2.assign("šћŕ");
      break;
    case 1:
      str1.assign("stj");
      str2.assign("šћ");
      break;
    case 2:
      str1.assign("zdj");
      str2.assign("žђ");
      break;
    case 3:
      str1.assign("slj");
      str2.assign("šĺ");
      break;
    case 4:
      str1.assign("zlj");
      str2.assign("žĺ");
      break;
    case 5:
      str1.assign("znj");
      str2.assign("žń");
      break;
    case 6:
      str1.assign("snj");
      str2.assign("šń");
      break;
    case 7:
      str1.assign("trj");
      str2.assign("ћŕ");
      break;
    case 8:
      str1.assign("drj");
      str2.assign("ђŕ");
      break;
    case 9:
      str1.assign("tvj");
      str2.assign("ћvĺ");
      break;
    case 10:
      str1.assign("bj");
      str2.assign("bĺ");
      break;
    case 11:
      str1.assign("pj");
      str2.assign("pĺ");
      break;
    case 12:
      str1.assign("mj");
      str2.assign("mĺ");
      break;
    case 13:
      str1.assign("vj");
      str2.assign("vĺ");
      break;
    case 14:
      str1.assign("kt");
      str2.assign("ћ");
      break;
    case 15:
      str1.assign("gt");
      str2.assign("ћ");
      break;
    case 16:
      str1.assign("tj");
      str2.assign("ћ");
      break;
    case 17:
      str1.assign("dj");
      str2.assign("ђ");
      break;
    case 18:
      str1.assign("nj");
      str2.assign("ń");
      break;
    case 19:
      str1.assign("lj");
      str2.assign("ĺ");
      break;
    case 20:
      str1.assign("rj");
      str2.assign("ŕ");
      break;
    case 21:
      str1.assign("sj");
      str2.assign("š");
      break;
    case 22:
      str1.assign("zj");
      str2.assign("ž");
      break;
    case 23:
      str1.assign("čj");
      str2.assign("č");
      break;
    case 24:
      str1.assign("šj");
      str2.assign("š");
      break;
    case 25:
      str1.assign("žj");
      str2.assign("ž");
      break;
    case 26:
      str1.assign("zž");
      str2.assign("žǯ");
      break;
    case 27:
      str1.assign("jj");
      str2.assign("j");
      break;
    case 28:
      str1.assign("gj");
      str2.assign("ž");
      break;
    case 29:
      str1.assign("kj");
      str2.assign("č");
      break;
    case 30:
      str1.assign("xj");
      str2.assign("š");
      break;
    case 31:
      str1.assign("ђj");
      str2.assign("ђ");
      break;
    case 32:
      str1.assign("ńj");
      str2.assign("ń");
      break;
    case 33:
      str1.assign("ћj");
      str2.assign("ћ");
      break;
    case 34:
      str1.assign("ĺj");
      str2.assign("ĺ");
      break;
    case 35:
      str1.assign("ŕj");
      str2.assign("ŕ");
      break;
    case 36:
      str1.assign("ћъ");
      str2.assign("ћь");
      break;
    case 37:
      str1.assign("ђъ");
      str2.assign("ђь");
      break;
    }

    do
    {
      x = cs_word.find(str1);
      if (x == -1)
      {
        break;
      }
      y = str1.length();
      cs_word.replace(x, y, str2);
    } while (x != -1);
  }
  return cs_word;
}*/
void Dejotate(std::string& jotated_form) {
  replaceAll(jotated_form, "stvj", "šћvĺ");
  replaceAll(jotated_form, "strj", "šћŕ");
  replaceAll(jotated_form, "stj", "šћ");
  replaceAll(jotated_form, "skj", "šč");
  replaceAll(jotated_form, "zdj", "žђ");
  replaceAll(jotated_form, "zgj", "žǯ");
  replaceAll(jotated_form, "slj", "šĺ");
  replaceAll(jotated_form, "zlj", "žĺ");
  replaceAll(jotated_form, "znj", "žń");
  replaceAll(jotated_form, "snj", "šń");
  replaceAll(jotated_form, "trj", "ћŕ");
  replaceAll(jotated_form, "drj", "ђŕ");
  replaceAll(jotated_form, "tvj", "ћvĺ");
  replaceAll(jotated_form, "bj", "bĺ");
  replaceAll(jotated_form, "pj", "pĺ");
  replaceAll(jotated_form, "mj", "mĺ");
  replaceAll(jotated_form, "vj", "vĺ");
  replaceAll(jotated_form, "kt", "ћ");
  replaceAll(jotated_form, "gt", "ћ");
  replaceAll(jotated_form, "tj", "ћ");
  replaceAll(jotated_form, "dj", "ђ");
  replaceAll(jotated_form, "nj", "ń");
  replaceAll(jotated_form, "lj", "ĺ");
  replaceAll(jotated_form, "rj", "ŕ");
  replaceAll(jotated_form, "sj", "š");
  replaceAll(jotated_form, "zj", "ž");
  replaceAll(jotated_form, "čj", "č");
  replaceAll(jotated_form, "šj", "š");
  replaceAll(jotated_form, "žj", "ž");
  replaceAll(jotated_form, "ǯj", "ǯ");
  replaceAll(jotated_form, "zž", "žǯ");
  replaceAll(jotated_form, "jj", "j");
  replaceAll(jotated_form, "gj", "ž");
  replaceAll(jotated_form, "kj", "č");
  replaceAll(jotated_form, "xj", "š");
  replaceAll(jotated_form, "ђj", "ђ");
  replaceAll(jotated_form, "ńj", "ń");
  replaceAll(jotated_form, "ћj", "ћ");
  replaceAll(jotated_form, "ĺj", "ĺ");
  replaceAll(jotated_form, "ŕj", "ŕ");
  replaceAll(jotated_form, "ћъ", "ћь");
  replaceAll(jotated_form, "ђъ", "ђь");

  //this is a bullshit hack to get around the problem of jo-stem displacement of masc_i-stem nouns, which is only possible in my system on stems ending with /nlr/ since they have corresponding palatal phonemes. гю га etc. break everything, so they just get dumbly reconstructed with /da/
  replaceAll(jotated_form, "rǢ", "ŕǢ");
  replaceAll(jotated_form, "nǢ", "ńǢ");
  replaceAll(jotated_form, "lǢ", "ĺǢ");
}

std::string deep_clean_Cyr(std::string form_cell)
{

  std::string str1;
  std::string str2;
  int x = 0;
  //  int q = 0;
  int y;

#include "deep_clean_cyr_rep.h"

  do
  {
    x = form_cell.find(str1);
    if (x == -1)
    {
      break;
    }

    y = str1.length();
    form_cell.replace(x, y, str2);
  } while (x != -1);
  //   cout << q << " times." << "\n";
}
return form_cell;
}

std::string clean_Cyr(std::string form_cell)
{

  std::string str1;
  std::string str2;
  int x = 0;
  //  int q = 0;
  int y;

#include "clean_cyr_rep.h"

  do
  {
    x = form_cell.find(str1);
    if (x == -1)
    {
      break;
    }

    y = str1.length();
    form_cell.replace(x, y, str2);
  } while (x != -1);
  //   cout << q << " times." << "\n";
}
return form_cell;
}

std::string glagoliticise(std::string form_cell)
{

  std::string str1;
  std::string str2;
  int x = 0;
  //  int q = 0;
  int y;

#include "nonKF_glag_rep.h"

  do
  {
    x = form_cell.find(str1);
    if (x == -1)
    {
      break;
    }

    y = str1.length();
    form_cell.replace(x, y, str2);
  } while (x != -1);
  //   cout << q << " times." << "\n";
}
return form_cell;
}


std::string boolToStr(bool non_assim)
{
  if (non_assim == false)
    return "";
  else
    return "W";
}

std::string Reconstruct(std::string cyr_id, std::string morph_tag, int lemma_id)
{  
  std::string lcs_reconstruction = "";
  std::unordered_map<int, Lemma>::iterator it;

  it = lemma_list.find(lemma_id);
  if (it != lemma_list.end())
  {
    Lemma lemma = it->second; // retrieve the object stored in the memory address of the iterator

    changeLemma_field(lemma, morph_tag, deep_clean_Cyr(cyr_id)); // pass object by reference to a function so I can change one of its members directly


    Dejotate(lemma.lemma_form);
    return lemma.lemma_form;
  }
  return lcs_reconstruction;
}

std::array<std::string, 2> ReconstructMorphReplace(std::string cyr_id, std::string morph_tag, int lemma_id)
{  
  std::string lcs_reconstruction = "";
  std::string lcs_morph_replace = "";
  std::unordered_map<int, Lemma>::iterator it;

  it = lemma_list.find(lemma_id);
  if (it != lemma_list.end())
  {
    Lemma lemma = it->second; // retrieve the object stored in the memory address of the iterator

    changeLemma_field(lemma, morph_tag, cyr_id); // pass object by reference to a function so I can change one of its members directly

    if(lemma.lemma_form.substr(0, 7) != "corrupt"){
      Dejotate(lemma.lemma_form);
      Dejotate(lemma.morph_replace);
      // lcs_reconstruction = Dejotate(lemma.lemma_form);
      // lcs_morph_replace = Dejotate(lemma.morph_replace);
      
      return std::array<std::string, 2>{lemma.lemma_form, lemma.morph_replace};
    }
  }
  return std::array<std::string, 2>{"", ""};

}

std::string ReconstructLine(std::string text_line)
{  
  std::string cyr_id = "";
  std::string morph_tag = "";
  int lemma_id;

  std::stringstream ss_text_line(text_line);
  std::string field;
  int field_count = 0;
  while(std::getline(ss_text_line, field, ',')) {
    switch(field_count) {
      case 0:
        cyr_id = field;
        break;
      case 2:
        morph_tag = field;
        break;
      case 4:
        lemma_id = std::stoi(field);
        break;
      default:
        ;
    }
    field_count++;
  }

  std::string lcs_reconstruction = cyr_id + "|";
  std::unordered_map<int, Lemma>::iterator it;

  it = lemma_list.find(lemma_id);
  if (it != lemma_list.end())
  {
    Lemma lemma = it->second; // retrieve the object stored in the memory address of the iterator

    changeLemma_field(lemma, morph_tag, cyr_id); // pass object by reference to a function so I can change one of its members directly

    Dejotate(lemma.lemma_form);
    lcs_reconstruction += lemma.lemma_form;
  }
  return lcs_reconstruction;

}

// int main(int argc, char *argv[]) {
    
//   if(argc < 2) {
//       std::cout << "Need to supply a text file\n";
//       return 0;
//   }
//   std::string text_file = argv[1];
//   std::string input_line;
//   std::ifstream text_file_stream(text_file);
//   while(std::getline(text_file_stream, input_line)) {
//     std::cout << Reconstruct(input_line) << "\n"; 
//   }
//   text_file_stream.close();

//   return 0;
// }
