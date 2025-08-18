#include <vector>
#include <array>
#include <unordered_set>
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cstdint>
#include <string_view>


consteval std::uint64_t compileTimehashString(std::string_view sv) {
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
std::string DejotateOld(std::string cs_word)
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
}
void Dejotate(std::string& jotated_form) {
    replaceAll(jotated_form, "stvj", "šћvĺ");
    replaceAll(jotated_form, "strj", "šћŕ");
    replaceAll(jotated_form, "stj", "šћ");
    replaceAll(jotated_form, "zdj", "žђ");
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
}


int main() {

    std::string jotated = "rodjьstvo vъzmogti avjǢti ljubъ oržǯjьše";
    //Dejotate(jotated);
    std::cout << DejotateOld(jotated) << "\n";

    // for(int i = 0; i < 1000000; i++) {
    //     Dejotate(jotated);
    // }
    return 0;
}