#include "LcsFlecter.h"

const std::unordered_map<int, inner_map> LcsFlecter::m_noun_classes = {
    #include "autoreconstructor/data/noun_inflections.txt"
};

const std::unordered_map<int, inner_map> LcsFlecter::m_verb_classes = {
    #include "autoreconstructor/data/verb_inflections.txt"
};

int main() {


    LcsFlecter* flecter = new LcsFlecter;

    std::cout << flecter->getEnding(101, 2) << "\n";
    delete flecter;

    return 0;
}