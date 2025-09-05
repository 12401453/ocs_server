
//this sets the outer_map_no so decides which set of endings will be added
int conj_type_enum(std::string_view conj_type) {

  switch(runTimeHashString(conj_type)) {
      case compileTimeHashString("azъ"): return 11;
      case compileTimeHashString("ty"): return 21;
      case compileTimeHashString("sę"): return 31;
      case compileTimeHashString("adj_hard"): return 121;
      case compileTimeHashString("masc_o"): return 121;
      case compileTimeHashString("fem_a"): return 121;
      case compileTimeHashString("masc_a"): return 121;
      case compileTimeHashString("nt_o"): return 121;
      case compileTimeHashString("long_adj_noun"): return 121;
      case compileTimeHashString("adj_soft"): return 131;
      case compileTimeHashString("adj_soft_comp"): return 1311;
      case compileTimeHashString("adj_soft_ord"): return 131;
      case compileTimeHashString("adj_hard_ord"): return 121;
      case compileTimeHashString("masc_jo"): return 131;
      case compileTimeHashString("fem_ja"): return 131;
      case compileTimeHashString("nt_jo"): return 131;
      case compileTimeHashString("masc_ja"): return 131;
      case compileTimeHashString("fem_i"): return 141;
      case compileTimeHashString("masc_i"): return 141;
      case compileTimeHashString("tri"): return 141;
      case compileTimeHashString("fem_ji"): return 151;
      case compileTimeHashString("masc_ji"): return 151;
      case compileTimeHashString("fem_R"): return 161;
      case compileTimeHashString("fem_uu"): return 171;
      case compileTimeHashString("masc_anin"): return 181;
      case compileTimeHashString("four"): return 191;
      case compileTimeHashString("masc_jo_foreign"): return 201;
      case compileTimeHashString("masc_ju"): return 211;
      case compileTimeHashString("masc_N"): return 221;
      case compileTimeHashString("masc_tel"): return 231;
      case compileTimeHashString("masc_u"): return 241;
      case compileTimeHashString("masc_o_u"): return 251;
      case compileTimeHashString("masc_o_in"): return 121;
      case compileTimeHashString("nt_N"): return 261;
      case compileTimeHashString("nt_NT"): return 271;
      case compileTimeHashString("nt_S"): return 281;
      case compileTimeHashString("nt_o_S"): return 291;
      case compileTimeHashString("nt_o_PV3"): return 301;
      case compileTimeHashString("masc_o_PV3"): return 301;
      case compileTimeHashString("fem_a_PV3"): return 301;
      case compileTimeHashString("masc_a_PV3"): return 301;
      case compileTimeHashString("den"): return 1011;
      case compileTimeHashString("oko"): return 1021;
      case compileTimeHashString("vьxь"): return 2031;
      case compileTimeHashString("kъto"): return 2041;
      case compileTimeHashString("čьto"): return 2051;
      case compileTimeHashString("kъtože"): return 2041;
      case compileTimeHashString("čьtože"): return 2051;
      case compileTimeHashString("kъjь"): return 2061;
      case compileTimeHashString("kъjьže"): return 2061;
      case compileTimeHashString("jь"): return 51;
      case compileTimeHashString("jьže"): return 51;
      case compileTimeHashString("pron_hard_ђe"): return 61;
      case compileTimeHashString("pron_hard_že"): return 61;
      case compileTimeHashString("kъžьdo"): return 61;
      case compileTimeHashString("pron_hard"): return 61;
      case compileTimeHashString("pron_soft"): return 71;
      case compileTimeHashString("sь"): return 81;
      case compileTimeHashString("ten"): return 91;
      case compileTimeHashString("masc_ar"): return 311;
      case compileTimeHashString("1.1_PRAP"): return 1411;

      //VERBS
      case compileTimeHashString("byti"): return 11;
      case compileTimeHashString("pref_byti"): return 5001;
      case compileTimeHashString("nebyti"): return 101;
      case compileTimeHashString("have"): return 21;
      case compileTimeHashString("dati"): return 31;
      case compileTimeHashString("eat"): return 41;
      case compileTimeHashString("pref_eat"): return 51;
      case compileTimeHashString("wote"): return 61;
      case compileTimeHashString("will"): return 71;
      case compileTimeHashString("stati"): return 91;
      case compileTimeHashString("dovleti"): return 901;
      case compileTimeHashString("dedj"): return 701;
      case compileTimeHashString("sleep"): return 751;
      case compileTimeHashString("11"): return 111;
      case compileTimeHashString("12"): return 121;
      case compileTimeHashString("13"): return 131;
      case compileTimeHashString("14"): return 111;
      case compileTimeHashString("15"): return 111;
      case compileTimeHashString("infix_11"): return 111;
      case compileTimeHashString("rěsti"): return 271;
      case compileTimeHashString("16"): return 161;
      case compileTimeHashString("čuti"): return 1601;
      case compileTimeHashString("17"): return 171;
      case compileTimeHashString("18"): return 181;
      case compileTimeHashString("19"): return 191;
      case compileTimeHashString("ьt"): return 241;
      case compileTimeHashString("ьz"): return 141;
      case compileTimeHashString("uti"): return 251;
      case compileTimeHashString("rti"): return 261;
      case compileTimeHashString("jьti"): return 81;
      case compileTimeHashString("21"): return 211;
      case compileTimeHashString("22"): return 221;
      case compileTimeHashString("31"): return 311;
      case compileTimeHashString("viděti"): return 3111;
      case compileTimeHashString("jaxati"): return 4001;
      case compileTimeHashString("32"): return 321;
      case compileTimeHashString("40"): return 401;
      case compileTimeHashString("51"): return 511;
      case compileTimeHashString("51_abl"): return 511;
      case compileTimeHashString("52"): return 521;
      case compileTimeHashString("52_abl"): return 521;
      case compileTimeHashString("iskati"): return 521;
      case compileTimeHashString("53"): return 531;
      case compileTimeHashString("53_abl"): return 531;
      case compileTimeHashString("54"): return 541;
      case compileTimeHashString("61"): return 611;
      case compileTimeHashString("62"): return 621;

      default:                           return 0;
  }
/*
  if(conj_type == "azъ")
      return 11;
  if(conj_type == "ty")
      return 21;
  if(conj_type == "sę")
      return 31;

  if(conj_type == "adj_hard")
        return 121;
  if (conj_type == "masc_o")
        return 121;
  if (conj_type == "fem_a")
        return 121;
  if (conj_type == "masc_a")
        return 121;
  if (conj_type == "nt_o")
        return 121;
  if (conj_type == "long_adj_noun")
        return 121;
  if(conj_type == "adj_soft")
        return 131;
//if(conj_type == "adj_ij") dealt with separately in noun_Flect.h
 //     return 131;
  if(conj_type == "adj_soft_comp")
        return 1311;
  if(conj_type == "adj_soft_ord")
        return 131;
  if(conj_type == "adj_hard_ord")
        return 121;
  if(conj_type == "masc_jo")
        return 131;
  if(conj_type == "fem_ja")
        return 131;
  if(conj_type == "nt_jo")
        return 131;
  if(conj_type == "masc_ja")
        return 131;

  if(conj_type == "fem_i")
        return 141;
  if(conj_type == "masc_i")
        return 141;
  if(conj_type == "tri")
        return 141;

  if(conj_type == "fem_ji")
        return 151;
  if(conj_type == "masc_ji")
        return 151;
  if(conj_type == "fem_R")
        return 161;
  if(conj_type == "fem_uu")
        return 171;
  if(conj_type == "masc_anin")
        return 181;
  if(conj_type == "four")
        return 191;

  if(conj_type == "masc_jo_foreign")
        return 201;
  if(conj_type == "masc_ju")
        return 211;
  if(conj_type == "masc_N")
        return 221;
  if(conj_type == "masc_tel")
        return 231;
  if(conj_type == "masc_u")
        return 241;
  if(conj_type == "masc_o_u")
        return 251;
  if(conj_type == "masc_o_in")
        return 121;
  if(conj_type == "nt_N")
        return 261;
  if(conj_type == "nt_NT")
        return 271;
  if(conj_type == "nt_S")
        return 281;
  if(conj_type == "nt_o_S")
        return 291;
  if(conj_type == "nt_o_PV3")
        return 301;
  if(conj_type == "masc_o_PV3")
        return 301;
  if(conj_type == "fem_a_PV3")
        return 301;
  if(conj_type == "masc_a_PV3")
        return 301;



if (conj_type == "den")
      return 1011;

  if (conj_type == "oko")
        return 1021;




  if (conj_type == "vьxь")
        return 2031;

  if (conj_type == "kъto")
        return 2041;
  if (conj_type == "čьto")
        return 2051;
  if (conj_type == "kъtože")
        return 2041;
  if (conj_type == "čьtože")
        return 2051;

  if (conj_type == "kъjь")
        return 2061;
  if (conj_type == "kъjьže")
        return 2061;


  if (conj_type == "jь")
        return 51;
  if (conj_type == "jьže")
        return 51;

  if (conj_type == "pron_hard_ђe")
      return 61;
  if (conj_type == "pron_hard_že")
      return 61;
  if (conj_type == "kъžьdo")
      return 61;

  if (conj_type == "pron_hard")
        return 61;
  if (conj_type == "pron_soft")
        return 71;
  if (conj_type == "sь")
        return 81;
  if (conj_type == "ten")
        return 91;
  if (conj_type == "masc_ar")
        return 311;

  if (conj_type == "1.1_PRAP")
        return 1411;






//VERBS



  if (conj_type == "byti")
       return 11;
  if (conj_type == "pref_byti")
       return 11;
  if (conj_type == "nebyti")
       return 101;
  if (conj_type == "have")
       return 21;
  if (conj_type == "dati")
       return 31;
  if (conj_type == "eat")
       return 41;
  if (conj_type == "pref_eat")
       return 51;
 if (conj_type == "wote")
     return 61;
 if (conj_type == "will")
     return 71;
 if (conj_type == "stati")
     return 91;
 if (conj_type == "dovleti")
     return 901;
if (conj_type == "dedj")
    return 701;
if (conj_type == "sleep")
    return 751;

  if (conj_type == "11")
        return 111;
  if (conj_type == "12")
      return 121;
  if (conj_type == "13")
      return 131;
  if (conj_type == "14")
      return 111;
  if (conj_type == "15")
      return 111;

  if(conj_type == "infix_11")
        return 111;
  if(conj_type == "rěsti")
        return 271;
  if (conj_type == "16")
      return 161;
  if (conj_type == "čuti")
      return 1601;
  if (conj_type == "17")
      return 171;
  if (conj_type == "18")
      return 181;
  if (conj_type == "19")
      return 191;
  if (conj_type == "ьt")
        return 241;
  if (conj_type == "ьz")
        return 141;
  if (conj_type == "uti")
        return 251;
  if (conj_type == "rti")
        return 261;

  if (conj_type == "jьti")
      return 81;
  if (conj_type == "21")
        return 211;
  if (conj_type == "22")
        return 221;
  if (conj_type == "31")
        return 311;
  if (conj_type == "viděti")
        return 3111;
  if (conj_type == "jaxati")
        return 4001;
  if (conj_type == "32")
        return 321;
  if (conj_type == "40")
        return 401;
  if (conj_type == "51")
        return 511;
  if (conj_type == "51_abl")
        return 511;
  if (conj_type == "52")
        return 521;
  if (conj_type == "52_abl")
        return 521;
  if (conj_type == "iskati")
        return 521;

  if (conj_type == "53")
        return 531;
  if (conj_type == "53_abl")
        return 531;
  if (conj_type == "54")
        return 541;
  if (conj_type == "61")
        return 611;
  if (conj_type == "62")
        return 621;

  else
          return 0; */
}
