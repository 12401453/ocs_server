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

std::string LcsFlecter::addEndings(std::string stem, int desinence_ix) {
    return stem + getEnding(desinence_ix);
}

std::vector<std::string> LcsFlecter::getFullParadigm(std::string stem, std::string conj_type) {
    setConjType(conj_type);
    std::vector<std::string> inflected_forms;
    inflected_forms.reserve(64);

    for(const auto& ending_pair : m_active_endings.at(m_outer_map_no)) {
        inflected_forms.emplace_back(ending_pair.second);
    }

    return inflected_forms;

}
std::vector<std::string> LcsFlecter::getFullParadigm(std::string stem) {
    std::vector<std::string> inflected_forms;
    inflected_forms.reserve(64);

    for(const auto& ending_pair : m_active_endings.at(m_outer_map_no)) {
        inflected_forms.emplace_back(ending_pair.second);
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


int main() {

    // std::string conj_type, stem, desinence_ix;
    // std::getline(std::cin, conj_type);
    // std::getline(std::cin, stem);
    // std::getline(std::cin, desinence_ix);
    
    LcsFlecter* noun_flecter = new LcsFlecter(NOUN, "den");
    LcsFlecter* verb_flecter = new LcsFlecter(VERB, "wote");

    std::cout << verb_flecter->addEndings("zapo", 7) << "\n";
    std::cout << noun_flecter->addEndings("", 20) << "\n";

    for(const auto& ending : noun_flecter->getFullParadigm("")) {
        std::cout << ending << "\n";
    }
    
    delete noun_flecter;
    delete verb_flecter;

    return 0;
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
std::string class5AblautClean(std::string flecter_output) {
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
std::string class11InfixClean(std::string flecter_output) {
    replaceAll(flecter_output, "leg", "lęg");
    replaceAll(flecter_output, "sěd", "sęd");

    return flecter_output;
}
std::string class12InfixClean(std::string flecter_output) {
    replaceAll(flecter_output, "rět", "ręt");

    return flecter_output;
}
std::string imperfSheta(std::string flecter_output) {
    replaceAll(flecter_output, "šeta", "sta");
    replaceAll(flecter_output, "šete", "ste");

    return flecter_output;
}

std::string firstVelarClean(std::string flecter_output) {
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
std::string pv1LongE(std::string flecter_output) {
    replaceAll(flecter_output, "gě", "žǢ");
    replaceAll(flecter_output, "kě", "čǢ");
    replaceAll(flecter_output, "xě", "šǢ");

    return flecter_output;
}
