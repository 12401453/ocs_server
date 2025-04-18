#include <vector>
#include <array>
#include <set>
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <map>

#define NOUN true
#define VERB false

typedef std::map<int, std::string> inner_map;

class LcsFlecter {
    public:
        LcsFlecter(bool noun, std::string conj_type="") : m_active_endings(noun ? m_noun_endings : m_verb_endings), m_noun_verb(noun) {
            setConjType(conj_type);
        };

    std::string getEnding(int desinence_ix);
    std::string getEnding(std::string conj_type, int desinence_ix);
    void setConjType(std::string conj_type);

    std::string addEndings(std::string stem, int desinence_ix);
    std::vector<std::string> getFullParadigm(std::string stem, std::string conj_type);
    std::vector<std::string> getFullParadigm(std::string stem);
    

    private:
        static const std::unordered_map<int, inner_map> m_noun_endings;
        static const std::unordered_map<int, inner_map> m_verb_endings;
        static const std::unordered_map<std::string, int> m_conj_type_map;

        const std::unordered_map<int, inner_map>& m_active_endings;

        int m_outer_map_no;
        bool m_noun_verb;
        std::string m_conj_type; 

};