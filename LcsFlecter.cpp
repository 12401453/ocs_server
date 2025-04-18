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