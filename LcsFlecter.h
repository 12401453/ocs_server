#include <vector>
#include <array>
#include <set>
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <map>
#include <cstring>

#define NOUN true
#define VERB false

typedef std::map<int, std::string> inner_map;

struct Inflection {
    int desinence_ix;
    std::string flected_form;
};

class LcsFlecter {
    public:
        LcsFlecter(bool noun, std::string conj_type="") : m_active_endings(noun ? m_noun_endings : m_verb_endings), m_noun_verb(noun) {
            setConjType(conj_type);
        };

    std::string getEnding(int desinence_ix);
    std::string getEnding(std::string conj_type, int desinence_ix);
    void setConjType(std::string conj_type);

    Inflection addEnding(std::string stem, int desinence_ix);
    std::vector<Inflection> getFullParadigm(std::string stem, std::string conj_type);
    std::vector<Inflection> getFullParadigm(std::string stem);

    std::string postProcess(Inflection &indexed_inflection);



    std::string class1Clean(std::string flecter_output, bool imperative);
    std::string class1NasalClean(std::string flecter_output);
    std::string itiClean(std::string flecter_output);
    std::string class14AblautClean(std::string flecter_output);
    std::string class15AblautClean(std::string flecter_output);
    std::string class3Clean(std::string flecter_output);
    std::string class5AblautClean(std::string flecter_output);
    std::string class11InfixClean(std::string flecter_output);
    std::string class12InfixClean(std::string flecter_output);
    std::string imperfSheta(std::string flecter_output);

    std::string firstVelarClean(std::string flecter_output);
    std::string pv1LongE(std::string flecter_output);

    std::string Dejotate(std::string jotated_form);
    
    bool c_strStartsWith(const char* str1, const char* str2);


    private:
        static const std::unordered_map<int, inner_map> m_noun_endings;
        static const std::unordered_map<int, inner_map> m_verb_endings;
        static const std::unordered_map<std::string, int> m_conj_type_map;

        const std::unordered_map<int, inner_map>& m_active_endings;

        int m_outer_map_no;
        bool m_noun_verb;
        std::string m_conj_type; 

};
