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

Inflection LcsFlecter::addEnding(std::string stem, int desinence_ix) {
    return {desinence_ix, stem.append(getEnding(desinence_ix))};
}

std::string LcsFlecter::postProcess(Inflection &indexed_inflection) {
    if(m_noun_verb == NOUN) {
        short int gender_third = 0;
        if(c_strStartsWith(m_conj_type.c_str(), "masc")) {
            gender_third = 1;
        }
        else if(c_strStartsWith(m_conj_type.c_str(), "nt")) {
            gender_third = 3;
        }
        else if(c_strStartsWith(m_conj_type.c_str(), "fem")) {
            gender_third = 2;
        }

        if(m_conj_type == "masc_ji" || m_conj_type == "masc_a" || m_conj_type == "masc_a_PV3" || m_conj_type == "masc_ja") {
            gender_third = 2;
        }

    }
    else {
        
    }
};

std::vector<Inflection> LcsFlecter::getFullParadigm(std::string stem, std::string conj_type) {
    setConjType(conj_type);
    std::vector<Inflection> inflected_forms;
    inflected_forms.reserve(64);

    for(const auto& ending_pair : m_active_endings.at(m_outer_map_no)) {
        Inflection infl = {ending_pair.first, stem + ending_pair.second};
        inflected_forms.emplace_back(infl);
    }

    return inflected_forms;
}
std::vector<Inflection> LcsFlecter::getFullParadigm(std::string stem) {
    const auto& desinences = m_active_endings.at(m_outer_map_no);
    auto desinences_iter = desinences.begin();
    auto desinences_iter_end = desinences.end();
    
    short int gender_third = 0;
    if(m_noun_verb == NOUN) {
        if(c_strStartsWith(m_conj_type.c_str(), "masc")) {
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
        
        if(m_conj_type == "masc_ji" || m_conj_type == "masc_a" || m_conj_type == "masc_a_PV3" || m_conj_type == "masc_ja") {
            gender_third = 2;
        }

    }
    std::vector<Inflection> inflected_forms;
    inflected_forms.reserve(64);

    
    //for(const auto& ending_pair : m_active_endings.at(m_outer_map_no)) {
    for(;desinences_iter != desinences_iter_end; ++desinences_iter) {
        Inflection infl = {desinences_iter->first, stem + desinences_iter->second};
        //inflected_forms.emplace_back(infl);
        inflected_forms.emplace_back(desinences_iter->first, stem + desinences_iter->second);
    }

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

std::string LcsFlecter::class1Clean(std::string flecter_output, bool imperative) {
    if(imperative) {
        replaceAll(flecter_output, "eki", "ьki");
        replaceAll(flecter_output, "ekě", "ьkě");
        replaceAll(flecter_output, "egi", "ьgi");
        replaceAll(flecter_output, "egě", "ьgě");
        replaceAll(flecter_output, "exi", "ьxi");
        replaceAll(flecter_output, "exě", "ьxě");
        replaceAll(flecter_output, "skj", "šč");

        return flecter_output;
    }

    replaceAll(flecter_output, "ę̌", "Z"); //prevent nasalised-jat from being replaced with Ǣ after palatals

    replaceAll(flecter_output, "eksę", "ěšę");
    replaceAll(flecter_output, "ekst", "ěst");
    replaceAll(flecter_output, "egsę", "ěšę");
    replaceAll(flecter_output, "egst", "ěst");
    replaceAll(flecter_output, "ebs", "ěbs");
    replaceAll(flecter_output, "eks", "ěx");
    replaceAll(flecter_output, "ods", "as");
    replaceAll(flecter_output, "ogs", "ax");
    replaceAll(flecter_output, "egs", "ěx");
    replaceAll(flecter_output, "eds", "ěs");
    replaceAll(flecter_output, "ess", "ěs");
    replaceAll(flecter_output, "eps", "ěs");
    replaceAll(flecter_output, "ksę", "šę");
    replaceAll(flecter_output, "gsę", "šę");
    replaceAll(flecter_output, "gst", "st");
    replaceAll(flecter_output, "kst", "st");
    replaceAll(flecter_output, "skj", "šč");
    replaceAll(flecter_output, "ks", "x");
    replaceAll(flecter_output, "gs", "x");
    replaceAll(flecter_output, "bt", "t");
    replaceAll(flecter_output, "pt", "t");
    replaceAll(flecter_output, "dt", "st");
    replaceAll(flecter_output, "tt", "st");
    replaceAll(flecter_output, "zt", "st");
    replaceAll(flecter_output, "gě", "žǢ");
    replaceAll(flecter_output, "xě", "šǢ");
    replaceAll(flecter_output, "kě", "čǢ");
    replaceAll(flecter_output, "ge", "že");
    replaceAll(flecter_output, "xe", "še");
    replaceAll(flecter_output, "ke", "če");
    replaceAll(flecter_output, "žě", "žǢ");
    replaceAll(flecter_output, "šě", "šǢ");
    replaceAll(flecter_output, "čě", "čǢ");
    replaceAll(flecter_output, "jě", "jǢ");
    replaceAll(flecter_output, "vt", "t");
    replaceAll(flecter_output, "zs", "s");
    replaceAll(flecter_output, "ts", "s");
    replaceAll(flecter_output, "ds", "s");
    replaceAll(flecter_output, "bs", "s");
    replaceAll(flecter_output, "ps", "s");
    replaceAll(flecter_output, "pn", "n");

    replaceAll(flecter_output, "Z", "ę̌");
    
    return flecter_output;
}
std::string LcsFlecter::class1NasalClean(std::string flecter_output) {
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

    return flecter_output;
}
std::string LcsFlecter::itiClean(std::string flecter_output) {
    replaceAll(flecter_output, "njь", "ni");
    replaceAll(flecter_output, "zjь", "zi");
    replaceAll(flecter_output, "tjь", "ti");
    replaceAll(flecter_output, "bjь", "bi");
    replaceAll(flecter_output, "tš", "š"); //very often replaced with tъš
    replaceAll(flecter_output, "+š", "š");
    replaceAll(flecter_output, "nš", "š");
    replaceAll(flecter_output, "zš", "sš"); //not strictly speaking correct but better for searching

    return flecter_output;
}
std::string LcsFlecter::class14AblautClean(std::string flecter_output) {
    replaceAll(flecter_output, "ĺ̥", "el");
    replaceAll(flecter_output, "ŕ̥", "er");

    return flecter_output;
}
std::string LcsFlecter::class15AblautClean(std::string flecter_output) {
    replaceAll(flecter_output, "el", "ĺ̥");
    replaceAll(flecter_output, "er", "ŕ̥");

    return flecter_output;
}
std::string LcsFlecter::class3Clean(std::string flecter_output) {
    replaceAll(flecter_output, "ę̌", "Z"); //prevent nasalised-jat from being replaced with Ǣ after palatals

    replaceAll(flecter_output, "žě", "žǢ");
    replaceAll(flecter_output, "šě", "šǢ");
    replaceAll(flecter_output, "čě", "čǢ");
    replaceAll(flecter_output, "jě", "jǢ");

    replaceAll(flecter_output, "Z",  "ę̌");
    return flecter_output;
}
std::string LcsFlecter::class5AblautClean(std::string flecter_output) {
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

    return flecter_output;
}
std::string LcsFlecter::class11InfixClean(std::string flecter_output) {
    replaceAll(flecter_output, "leg", "lęg");
    replaceAll(flecter_output, "sěd", "sęd");

    return flecter_output;
}
std::string LcsFlecter::class12InfixClean(std::string flecter_output) {
    replaceAll(flecter_output, "rět", "ręt");

    return flecter_output;
}
std::string LcsFlecter::imperfSheta(std::string flecter_output) {
    replaceAll(flecter_output, "šeta", "sta");
    replaceAll(flecter_output, "šete", "ste");

    return flecter_output;
}

std::string LcsFlecter::firstVelarClean(std::string flecter_output) {
    replaceAll(flecter_output, "ske", "šče");
    replaceAll(flecter_output, "ge", "že");
    replaceAll(flecter_output, "ke", "če");
    replaceAll(flecter_output, "xe", "še");
    replaceAll(flecter_output, "gi", "ži");
    replaceAll(flecter_output, "ki", "či");
    replaceAll(flecter_output, "xi", "ši");
    replaceAll(flecter_output, "gь", "žь");
    replaceAll(flecter_output, "kь", "čь");
    replaceAll(flecter_output, "xь", "šь");

    return flecter_output;

}
std::string LcsFlecter::pv1LongE(std::string flecter_output) {
    replaceAll(flecter_output, "gě", "žǢ");
    replaceAll(flecter_output, "kě", "čǢ");
    replaceAll(flecter_output, "xě", "šǢ");

    return flecter_output;
}
std::string LcsFlecter::Dejotate(std::string jotated_form) {
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

    return jotated_form;
}


int main() {

    // std::string conj_type, stem, desinence_ix;
    // std::getline(std::cin, conj_type);
    // std::getline(std::cin, stem);
    // std::getline(std::cin, desinence_ix);
    
    LcsFlecter* noun_flecter = new LcsFlecter(NOUN, "den");
    LcsFlecter* verb_flecter = new LcsFlecter(VERB, "11");

    verb_flecter->setConjType("14");

    std::cout << verb_flecter->addEnding("rek", 13).flected_form<< "\n";
    std::cout << noun_flecter->addEnding("", 20).flected_form << "\n";

    for(const auto& ending : verb_flecter->getFullParadigm("vъvŕ̥g")) {
        std::cout << ending.desinence_ix << " " << ending.flected_form << "\n";
    }
    
    delete noun_flecter;
    delete verb_flecter;

    return 0;
}