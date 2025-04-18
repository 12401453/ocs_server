#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>

typedef std::unordered_map<int, std::string> inner_map;

class FlectWriter {
  public:
    FlectWriter() {
        
    };


    static const std::unordered_map<int, inner_map> m_noun_classes;
    static const std::unordered_map<int, inner_map> m_verb_classes; 
    static const inner_map m_noun_endings;
    static const inner_map m_verb_endings;

};

#include "nom_map.cpp"
#include "verb_map.cpp"

const std::unordered_map<int, inner_map> FlectWriter::m_noun_classes = {
    {101, N_PRAP_endings_c0},
    {102, N_PRAP_endings_c1},
    {111, N_PAP_endings_c0},
    {112, N_PAP_endings_c1},
    {51, N_ji_c0},

    {61, N_pron_hard_c0},
    {71, N_pron_soft_c0},
    {81, N_si_c0},
    {82, N_si_c1},
    {83, N_si_c2},

    {121, N_adj_hard_c0},
    {122, N_adj_hard_c1},

    {1011, N_den_c0},
    {1012, N_den_c1},
    {1013, N_den_c2},
    {1021, N_oko_c0},
    {1022, N_oko_c1},
    {131, N_adj_soft_c0},
    {132, N_adj_soft_c1},

    {141, N_all_i_stems_c0},
    {142, N_all_i_stems_c1},

    {151, N_fem_ji_c0},
    {152, N_fem_ji_c1},
    {161, N_fem_R_c0},
    {162, N_fem_R_c1},
    {171, N_fem_uu_c0},
    {172, N_fem_uu_c1},
    {181, N_masc_anin_c0},
    {182, N_masc_anin_c1},

    {191, N_four_c0},
    {192, N_four_c1},
    {193, N_four_c2},

    {201, N_masc_jo_foreign_c0},
    {202, N_masc_jo_foreign_c1},
    {203, N_masc_jo_foreign_c2},
    {211, N_masc_ju_c0},
    {212, N_masc_ju_c1},
    {213, N_masc_ju_c2},
    {221, N_masc_N_c0},
    {222, N_masc_N_c1},
    {223, N_masc_N_c2},
    {231, N_masc_tel_c0},
    {232, N_masc_tel_c1},
    {241, N_masc_u_c0},
    {242, N_masc_u_c1},
    {251, N_masc_o_u_c0},
    {252, N_masc_o_u_c1},
    {253, N_masc_o_u_c2},
    {261, N_nt_N_c0},
    {262, N_nt_N_c1},
    {271, N_nt_NT_c0},
    {272, N_nt_NT_c1},
    {281, N_nt_S_c0},
    {282, N_nt_S_c1},
    {291, N_nt_o_S_c0},
    {292, N_nt_o_S_c1},

    {301, N_PV3_nouns_c0},
    {302, N_PV3_nouns_c1},

    {2031, N_vixi_c0},
    {2032, N_vixi_c1},

    {2051, N_chto_c0},
    {2053, N_chto_c2},
    {2041, N_kto_c0},
    {2042, N_kto_c1},

    {11, N_az_c0},
    {12, N_az_c1},
    {21, N_ty_c0},
    {22, N_ty_c1},
    {23, N_ty_c2},
    {31, N_sebe_c0},
    {32, N_sebe_c1},
    {33, N_sebe_c2},

    {1311, N_adj_soft_comp_c0},
    {1312, N_adj_soft_comp_c1},

    {1411, N_v1_PRAP_c0},

    {91, N_ten_c0},
    {92, N_ten_c1},

    {311, N_masc_ar_c0},
    {312, N_masc_ar_c1},
    {313, N_masc_ar_c2},

    {2061, N_kyj_c0},
};

const std::unordered_map<int, inner_map> FlectWriter::m_verb_classes = {
    {111, v_11_c0},
    {112, v_11_c1},
    {113, v_11_c2},

    {211, v_21_c0},
    {212, v_21_c1},
    {2111, v_211_c0},
    {2112, v_211_c1},

    {401, v_40_c0},
    {402, v_40_c1},

    {511, v_51_c0},
    {512, v_51_c1},
    {521, v_52_c0},
    {522, v_52_c1},
    {531, v_53_c0},
    {532, v_53_c1},
    {541, v_54_c0},
    {542, v_54_c1},

    {121, v_12_c0},
    {122, v_12_c1},
    {81, v_8_c0},
    {82, v_8_c1},

    {131, v_13_c0},
    {132, v_13_c1},

    {161, v_16_c0},
    {162, v_16_c1},

    {171, v_17_c0},
    {172, v_17_c1},
    {181, v_18_c0},
    {182, v_18_c1},
    {191, v_19_c0},
    {192, v_19_c1},
    {1601, v_160_c0},
    {1602, v_160_c1},

    {221, v_22_c0},
    {222, v_22_c1},
    {311, v_31_c0},
    {312, v_31_c1},
    {321, v_32_c0},
    {322, v_32_c1},

    {611, v_61_c0},
    {612, v_61_c1},
    {621, v_62_c0},
    {622, v_62_c1},

    {241, v_24_c0},
    {242, v_24_c1},
    {141, v_14_c0},
    {142, v_14_c1},

    {251, v_25_c0},
    {252, v_25_c1},
    {261, v_26_c0},
    {262, v_26_c1},
    {263, v_26_c2},
    {271, v_27_c0},
    {272, v_27_c1},

    {21, v_2_c0},
    {22, v_2_c1},

    {31, v_3_c0},
    {32, v_3_c1},

    {41, v_4_c0},
    {42, v_4_c1},
    {51, v_5_c0},
    {52, v_5_c1},

    {61, v_6_c0},
    {62, v_6_c1},
    {63, v_6_c2},
    {71, v_7_c0},
    {72, v_7_c1},
    {91, v_9_c0},
    {92, v_9_c1},

    {11, v_1_c0},
    {12, v_1_c1},
    {101, v_10_c0},
    {102, v_10_c1},
    {1101, v_110_c0},
    {1102, v_110_c1},
    {1103, v_110_c2},

    {901, v_90_c0},
    {902, v_90_c1},
    {903, v_90_c2},
    {701, v_70_c0},
    {702, v_70_c1},
    {751, v_75_c0},
    {752, v_75_c1},

    {3111, v_311_c0},
    {3112, v_311_c1},

    {4001, v_400_c0},
    {4002, v_400_c1},
};

int main() {


    std::ofstream noun_inflections_file("data/noun_inflections.txt");
    std::ofstream verb_inflections_file("data/verb_inflections.txt");

    FlectWriter* flecter = new FlectWriter;
    
    //noun_inflections_file << "std::unordered_map<int, std::unordered_map<int, std::string>> noun_inflections {";
    for(const auto& outer_map: flecter->m_noun_classes) {
        noun_inflections_file << "\n  {" << outer_map.first << ",{";
        for(const auto& inner_map : outer_map.second) {
            noun_inflections_file << "\n    {" << inner_map.first << ",\"" << inner_map.second << "\"},";
        }
        noun_inflections_file << "\n    }";
        noun_inflections_file << "\n  },";
    }
    //noun_inflections_file << "\n};\n";

    //verb_inflections_file << "std::unordered_map<int, std::unordered_map<int, std::string>> verb_inflections {";
    for(const auto& outer_map: flecter->m_verb_classes) {
        verb_inflections_file << "\n  {" << outer_map.first << ",{";
        for(const auto& inner_map : outer_map.second) {
            verb_inflections_file << "\n    {" << inner_map.first << ",\"" << inner_map.second << "\"},";
        }
        verb_inflections_file << "\n    }";
        verb_inflections_file << "\n  },";
    }
    //verb_inflections_file << "\n};\n";



    delete flecter;
    noun_inflections_file.close();
    verb_inflections_file.close();

    return 0;
}