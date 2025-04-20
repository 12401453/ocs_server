#include "LcsFlecter.h"

const std::unordered_map<int, inner_map> LcsFlecter::m_noun_endings = {
    #include "autoreconstructor/data/noun_inflections.txt"
};

const std::unordered_map<int, inner_map> LcsFlecter::m_verb_endings = {
    #include "autoreconstructor/data/verb_inflections.txt"
};

const std::unordered_map<std::string, int> LcsFlecter::m_conj_type_map {
    #include "autoreconstructor/data/conj_type_map.txt"
};

void LcsFlecter::setConjType(std::string conj_type) {
    try {
        m_outer_map_no = m_conj_type_map.at(conj_type);
        m_conj_type = conj_type;
    }
    catch (std::out_of_range const& ex) {
        
        if(m_noun_verb == NOUN) {
            m_outer_map_no = m_conj_type_map.at("masc_o");
            m_conj_type = "masc_o";
            std::cout << "inflection-class passed in to setConjType not found, defaulting to masc_o\n";
        }
        else {
            m_outer_map_no = m_conj_type_map.at("40");
            m_conj_type = "40";
            std::cout << "inflection-class passed in to setConjType not found, defaulting to class 40\n";
        }
    }
}

void LcsFlecter::setStem(std::string stem) {
    m_stem = stem;
}

std::string LcsFlecter::getEnding(int desinence_ix) {
    try {
        return m_active_endings.at(m_outer_map_no).at(desinence_ix);
    }
    catch (std::out_of_range const& ex) {
        std::cout << "desinence not found, try again\n";
        return "-";
    }
}

std::string LcsFlecter::getEnding(std::string conj_type, int desinence_ix) {
    try {
        return m_active_endings.at(m_conj_type_map.at(conj_type)).at(desinence_ix);
    }
    catch (std::out_of_range const& ex) {
        std::cout << "inflection-class not found, try again\n";
        return "-";
    }
}

Inflection LcsFlecter::addEnding(int desinence_ix) {
    return {desinence_ix, m_stem + getEnding(desinence_ix)};
}

void LcsFlecter::postProcess(std::array<std::vector<Inflection>, 3> &inflected_forms) {
    if(m_noun_verb == NOUN) {
        if(m_outer_map_no == 301) {
            firstVelarClean(inflected_forms[0][6].flected_form);
        }
        else if((m_stem.ends_with('k') || m_stem.ends_with('g') || m_stem.ends_with('x') || m_stem.ends_with("xv")) && (m_conj_type == "masc_o" || m_conj_type == "adj_hard")) {
            firstVelarClean(inflected_forms[0][6].flected_form);
        }
    }
    else if(m_noun_verb == VERB) {
        if(m_conj_type == "51_abl" || m_conj_type == "52_abl" || m_conj_type == "53_abl") {
            //present
            for(int i = 0; i < 9; i++) {
                class5AblautClean(inflected_forms[0][i].flected_form);
                class5AblautClean(inflected_forms[1][i].flected_form);
            }
            //imperatives       
            for(int i = 27; i < 36; i++) {
                class5AblautClean(inflected_forms[0][i].flected_form);
                class5AblautClean(inflected_forms[1][i].flected_form);
            }
        }
        else if(m_conj_type == "infix_11") {
            //present
            for(int i = 0; i < 9; i++) {
                class11InfixClean(inflected_forms[0][i].flected_form);
            }
            //imperatives
            for(int i = 27; i < 36; i++) {
                class11InfixClean(inflected_forms[0][i].flected_form);
            }
        }
        else if(m_conj_type == "byti") {
            //need to add on future-forms, alternative imperfects, alternative aorist/past subjunctives etc., fucking nightmare
        }
        else if(m_outer_map_no == 211) {
            //add the endings at outer_map_no == 2112 to the possible variants alongside those at m_outer_map_no++;
        }

        if(m_conj_type == "11" || m_conj_type == "infix_11" || m_conj_type == "infix_12" || m_conj_type == "14" || m_conj_type == "15" || m_conj_type == "21" || m_conj_type == "31" || m_conj_type == "iskati") {
            for(auto& inflections_vec: inflected_forms){
                for(auto& inflection : inflections_vec) {
                    class1Clean(inflection);
                }
            }
            if(m_conj_type == "14") {
                // change the infinitive and supine to full-grade ablaut
                class14AblautClean(inflected_forms[0][42].flected_form);
                class14AblautClean(inflected_forms[0][43].flected_form);

                //add full-grade aorists as variants

                //give S-aorists full-grade ablauts
                class14AblautClean(inflected_forms[2][9].flected_form);
                for(int i = 12; i < 18; i++) {
                    class14AblautClean(inflected_forms[2][i].flected_form);
                }
                
            }
        }

        if(m_outer_map_no != 1101) {
            //add imperfSheta()'d variants of desinence_ix 22, 23 and 25 as possible variants
        }

    }
};

std::array<std::vector<Inflection>, 3> LcsFlecter::getFullParadigm() {
    const inner_map* desinences = &m_active_endings.at(m_outer_map_no); 
    const inner_map* deviant_desinences = nullptr;
    const inner_map* alternative_desinences = nullptr;

    auto end_iter = m_active_endings.end();
    auto deviances_iter = m_active_endings.find(m_outer_map_no + 1);
    
    int alternative_map_no = m_outer_map_no + 2;

    //should possibly get rid of this check and add an explicit new map at 523 with just those 5.1 forms that actually occur
    if(m_conj_type == "iskati") {
        alternative_map_no = m_outer_map_no - 10;
    }
    auto alternatives_iter = m_active_endings.find(alternative_map_no);

    if(deviances_iter != end_iter) {
        deviant_desinences = &deviances_iter->second;
    }
    if(alternatives_iter != end_iter) {
        alternative_desinences = &alternatives_iter->second;
    }

    auto desinences_iter = desinences->begin();
    auto desinences_iter_end = desinences->end();
    
    short int gender_third = 0; //delete
    std::string suffix = "";
    if(m_noun_verb == NOUN) {

        if(m_conj_type == "masc_ji" || m_conj_type == "masc_a" || m_conj_type == "masc_a_PV3" || m_conj_type == "masc_ja") {
            gender_third = 2;
            std::advance(desinences_iter, 21);
            std::advance(desinences_iter_end, -21);
        }
        else if(c_strStartsWith(m_conj_type.c_str(), "masc")) {
            gender_third = 1;
            std::advance(desinences_iter_end, -42);
        }
        else if(c_strStartsWith(m_conj_type.c_str(), "nt")) {
            gender_third = 3;
            std::advance(desinences_iter, 42);
            
        }
        else if(c_strStartsWith(m_conj_type.c_str(), "fem")) {
            gender_third = 2;
            std::advance(desinences_iter, 21);
            std::advance(desinences_iter_end, -21);
        }

        //requires -std=c++20
        if(m_conj_type.ends_with("že")) {
            suffix = "že";
        }
        else if(m_conj_type.ends_with("ђe")) {
            suffix = "ђe";
        }
        else if(m_conj_type == "kъžьdo") {
            suffix = "žьdo";
        }

    }
    std::array<std::vector<Inflection>, 3> inflected_forms;
    inflected_forms[0].reserve(64);
    inflected_forms[1].reserve(64);
    inflected_forms[2].reserve(64);

    
    //for(const auto& ending_pair : m_active_endings.at(m_outer_map_no)) {
    for(;desinences_iter != desinences_iter_end; ++desinences_iter) {
        Inflection infl = {desinences_iter->first, m_stem + desinences_iter->second};
        //inflected_forms.emplace_back(infl);
        inflected_forms[0].emplace_back(desinences_iter->first, m_stem + desinences_iter->second + suffix);
    }

    if(deviant_desinences != nullptr) {
        for(const auto& base_inflection : inflected_forms[0]) {
            if(deviant_desinences->contains(base_inflection.desinence_ix)) {
                inflected_forms[1].emplace_back(base_inflection.desinence_ix, m_stem + deviant_desinences->at(base_inflection.desinence_ix) + suffix);
            }
            else {
                inflected_forms[1].emplace_back(base_inflection.desinence_ix, "");
            }         
        }
    }
    if(alternative_desinences != nullptr) {
        for(const auto& base_inflection : inflected_forms[0]) {
            if(alternative_desinences->contains(base_inflection.desinence_ix)) {
                inflected_forms[2].emplace_back(base_inflection.desinence_ix, m_stem + alternative_desinences->at(base_inflection.desinence_ix) + suffix);
            }
            else {
                inflected_forms[2].emplace_back(base_inflection.desinence_ix, "");
            }         
        }
    }

    postProcess(inflected_forms);
    return inflected_forms;
}

void replaceAll(std::string &source, const std::string yeeted, const std::string replacement) {
    
    size_t yeeted_length = yeeted.length();
    if(yeeted_length == 0) return;
    size_t replacement_length = replacement.length();

    std::string result;

    int search_pos = 0;
    int yeeted_pos = source.find(yeeted);
    while(yeeted_pos != std::string::npos) {
        //source = source.substr(0, yeeted_pos) + replacement + source.substr(yeeted_pos + yeeted_length);
        source = source.replace(yeeted_pos, yeeted_length, replacement); 
        yeeted_pos = source.find(yeeted, yeeted_pos + replacement_length);
    }
}

bool LcsFlecter::c_strStartsWith(const char *str1, const char *str2) {
    int strcmp_count = 0;
    int str2_len = strlen(str2);
 
    int i = -1;
   
    while ((*str1 == *str2 || *str2 == '\0') && i < str2_len)
    {
        strcmp_count++;
        str1++;
        str2++;
        i++;
    }
 
    if (strcmp_count == str2_len + 1)
    {
        return true;
    }
    else
        return false;
}

void LcsFlecter::class1Clean(Inflection& inflection) {
    if(inflection.desinence_ix < 37 && inflection.desinence_ix > 27) {
        replaceAll(inflection.flected_form, "eki", "ьki");
        replaceAll(inflection.flected_form, "ekě", "ьkě");
        replaceAll(inflection.flected_form, "egi", "ьgi");
        replaceAll(inflection.flected_form, "egě", "ьgě");
        replaceAll(inflection.flected_form, "exi", "ьxi");
        replaceAll(inflection.flected_form, "exě", "ьxě");
        replaceAll(inflection.flected_form, "skj", "šč");

        return;
    }

    replaceAll(inflection.flected_form, "ę̌", "Z"); //prevent nasalised-jat from being replaced with Ǣ after palatals

    replaceAll(inflection.flected_form, "eksę", "ěšę");
    replaceAll(inflection.flected_form, "ekst", "ěst");
    replaceAll(inflection.flected_form, "egsę", "ěšę");
    replaceAll(inflection.flected_form, "egst", "ěst");
    replaceAll(inflection.flected_form, "ebs", "ěbs");
    replaceAll(inflection.flected_form, "eks", "ěx");
    replaceAll(inflection.flected_form, "ods", "as");
    replaceAll(inflection.flected_form, "ogs", "ax");
    replaceAll(inflection.flected_form, "egs", "ěx");
    replaceAll(inflection.flected_form, "eds", "ěs");
    replaceAll(inflection.flected_form, "ess", "ěs");
    replaceAll(inflection.flected_form, "eps", "ěs");
    replaceAll(inflection.flected_form, "ksę", "šę");
    replaceAll(inflection.flected_form, "gsę", "šę");
    replaceAll(inflection.flected_form, "gst", "st");
    replaceAll(inflection.flected_form, "kst", "st");
    replaceAll(inflection.flected_form, "skj", "šč");
    replaceAll(inflection.flected_form, "ks", "x");
    replaceAll(inflection.flected_form, "gs", "x");
    replaceAll(inflection.flected_form, "bt", "t");
    replaceAll(inflection.flected_form, "pt", "t");
    replaceAll(inflection.flected_form, "dt", "st");
    replaceAll(inflection.flected_form, "tt", "st");
    replaceAll(inflection.flected_form, "zt", "st");
    replaceAll(inflection.flected_form, "gě", "žǢ");
    replaceAll(inflection.flected_form, "xě", "šǢ");
    replaceAll(inflection.flected_form, "kě", "čǢ");
    replaceAll(inflection.flected_form, "ge", "že");
    replaceAll(inflection.flected_form, "xe", "še");
    replaceAll(inflection.flected_form, "ke", "če");
    replaceAll(inflection.flected_form, "žě", "žǢ");
    replaceAll(inflection.flected_form, "šě", "šǢ");
    replaceAll(inflection.flected_form, "čě", "čǢ");
    replaceAll(inflection.flected_form, "jě", "jǢ");
    replaceAll(inflection.flected_form, "vt", "t");
    replaceAll(inflection.flected_form, "zs", "s");
    replaceAll(inflection.flected_form, "ts", "s");
    replaceAll(inflection.flected_form, "ds", "s");
    replaceAll(inflection.flected_form, "bs", "s");
    replaceAll(inflection.flected_form, "ps", "s");
    replaceAll(inflection.flected_form, "pn", "n");

    replaceAll(inflection.flected_form, "Z", "ę̌");
}
void LcsFlecter::class1NasalClean(std::string& flecter_output) {
    replaceAll(flecter_output, "ьnstъ", "ętъ");
    replaceAll(flecter_output, "ьmstъ", "ętъ");
    replaceAll(flecter_output, "ьns", "ęs");
    replaceAll(flecter_output, "ьms", "ęs");
    replaceAll(flecter_output, "ьnx", "ęx");
    replaceAll(flecter_output, "ьmx", "ęx");
    replaceAll(flecter_output, "ьnš", "ęš");
    replaceAll(flecter_output, "ьmš", "ęš");
    replaceAll(flecter_output, "ьml", "ęl");
    replaceAll(flecter_output, "ьnl", "ęl");
    replaceAll(flecter_output, "ьm", "ę");
    replaceAll(flecter_output, "ьn", "ę");
}
void LcsFlecter::itiClean(std::string& flecter_output) {
    replaceAll(flecter_output, "njь", "ni");
    replaceAll(flecter_output, "zjь", "zi");
    replaceAll(flecter_output, "tjь", "ti");
    replaceAll(flecter_output, "bjь", "bi");
    replaceAll(flecter_output, "tš", "š"); //very often replaced with tъš
    replaceAll(flecter_output, "+š", "š");
    replaceAll(flecter_output, "nš", "š");
    replaceAll(flecter_output, "zš", "sš"); //not strictly speaking correct but better for searching
}
void LcsFlecter::class14AblautClean(std::string& flecter_output) {
    replaceAll(flecter_output, "ĺ̥", "el");
    replaceAll(flecter_output, "ŕ̥", "er");
}
void LcsFlecter::class15AblautClean(std::string& flecter_output) {
    replaceAll(flecter_output, "el", "ĺ̥");
    replaceAll(flecter_output, "er", "ŕ̥");
}
void LcsFlecter::class3Clean(std::string& flecter_output) {
    replaceAll(flecter_output, "ę̌", "Z"); //prevent nasalised-jat from being replaced with Ǣ after palatals

    replaceAll(flecter_output, "žě", "žǢ");
    replaceAll(flecter_output, "šě", "šǢ");
    replaceAll(flecter_output, "čě", "čǢ");
    replaceAll(flecter_output, "jě", "jǢ");

    replaceAll(flecter_output, "Z",  "ę̌");
}
void LcsFlecter::class5AblautClean(std::string& flecter_output) {
    replaceAll(flecter_output, "strъg", "strug");
    replaceAll(flecter_output, "stьl", "stel");
    replaceAll(flecter_output, "ĺьv", "lu");
    replaceAll(flecter_output, "zьd", "zid");
    replaceAll(flecter_output, "žьd", "žid");
    replaceAll(flecter_output, "pьs", "pis");
    replaceAll(flecter_output, "jьm", "jem");
    replaceAll(flecter_output, "nьm", "nem");
    replaceAll(flecter_output, "tьm", "tem");
    replaceAll(flecter_output, "bьm", "bem");
    replaceAll(flecter_output, "zьm", "zem");
    replaceAll(flecter_output, "gъn", "žen");
    replaceAll(flecter_output, "ъv", "ov");
    replaceAll(flecter_output, "ьr", "er");
    replaceAll(flecter_output, "ьj", "ěj");
}
void LcsFlecter::class11InfixClean(std::string& flecter_output) {
    replaceAll(flecter_output, "leg", "lęg");
    replaceAll(flecter_output, "sěd", "sęd");
}
void LcsFlecter::class12InfixClean(std::string& flecter_output) {
    replaceAll(flecter_output, "rět", "ręt");
}
void LcsFlecter::imperfSheta(std::string& flecter_output) {
    replaceAll(flecter_output, "šeta", "sta");
    replaceAll(flecter_output, "šete", "ste");
}

void LcsFlecter::firstVelarClean(std::string& flecter_output) {
    replaceAll(flecter_output, "ske", "šče");
    replaceAll(flecter_output, "xve", "šve");
    replaceAll(flecter_output, "ge", "že");
    replaceAll(flecter_output, "ke", "če");
    replaceAll(flecter_output, "xe", "še");
    replaceAll(flecter_output, "gi", "ži");
    replaceAll(flecter_output, "ki", "či");
    replaceAll(flecter_output, "xi", "ši");
    replaceAll(flecter_output, "gь", "žь");
    replaceAll(flecter_output, "kь", "čь");
    replaceAll(flecter_output, "xь", "šь");
}
void LcsFlecter::pv1LongE(std::string& flecter_output) {
    replaceAll(flecter_output, "gě", "žǢ");
    replaceAll(flecter_output, "kě", "čǢ");
    replaceAll(flecter_output, "xě", "šǢ");
}
void LcsFlecter::Dejotate(std::string& jotated_form) {
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
}


int main() {

    // std::string conj_type, stem, desinence_ix;
    // std::getline(std::cin, conj_type);
    // std::getline(std::cin, stem);
    // std::getline(std::cin, desinence_ix);
    
    LcsFlecter* noun_flecter = new LcsFlecter({"", "azъ", NOUN});
    LcsFlecter* verb_flecter = new LcsFlecter({"jьsk", "iskati", VERB});

    std::cout << verb_flecter->addEnding(13).flected_form<< "\n";
    std::cout << noun_flecter->addEnding(20).flected_form << "\n";

    for(const auto& inflections : verb_flecter->getFullParadigm()) {
        for(const auto& inflection : inflections) {
            std::cout << inflection.desinence_ix << " " << inflection.flected_form << "\n";
        }
        std::cout << "\n";
    }
    
    delete noun_flecter;
    delete verb_flecter;

    return 0;
}