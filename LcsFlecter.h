#include <vector>
#include <array>
#include <set>
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>

typedef std::unordered_map<int, std::string> inner_map;

class LcsFlecter {
    public:
        LcsFlecter() {
            
        };

    std::string getEnding(int outer_no, int inner_no) {
        // return "fuck off\n";
        return m_noun_classes.at(outer_no).at(inner_no);
    }
    

    private:
        static const std::unordered_map<int, inner_map> m_noun_classes;
        static const std::unordered_map<int, inner_map> m_verb_classes; 

};