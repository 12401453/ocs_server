void noun_Flect(Lemma &lemma_ref, short int int_morph_tag[10], std::string cyr_id)
{

  std::string conj_type = lemma_ref.conj_type;
  std::string stem = lemma_ref.lemma_form;

  int row_no = 0;
  int outer_map_no = conj_type_enum(conj_type);
  //   if(outer_map_no == 0) {return; }

  short int num{int_morph_tag[1]}, pers{int_morph_tag[0]}, gr_case{int_morph_tag[6]}, gender{int_morph_tag[5]}, strength{int_morph_tag[8]}, degree{int_morph_tag[7]};
  if (lemma_ref.lemma_id == 67633)
  {
    gender = 2;
  } // force fem. on a-stems which can be masc. or fem. or the bastard gets o-stem endings
  row_no = 21 * gender + 7 * num + gr_case - 28;

  // check for *jedьn-
  if (lemma_ref.lemma_id == 58827 || lemma_ref.lemma_id == 58944)
  {
    if (Sniff(cyr_id, "дін", 20) == false)
    {
      stem = lemma_ref.poss_doublet;
      lemma_ref.poss_doublet = lemma_ref.lemma_form + "ъ";
    }
  }

  // definitise ordinal numbers and "которъі" and "universe/population" which TOROT wrongly marks as strong or both
  if (conj_type == "adj_hard_ord" || conj_type == "adj_soft_ord" || lemma_ref.lemma_id == 64673 || lemma_ref.lemma_id == 67825)
  {
    strength = 1;
  }
  if (conj_type == "long_adj_noun")
  {
    strength = 1;
  }

  // mark long-adjectives as 2 if they're fucked and 1 if not
  if (strength == 1)
  {
    if (row_no == 6 || row_no == 11 || row_no == 13 || row_no == 18 || row_no == 19 || row_no == 32 || row_no == 34 || row_no == 39 || row_no == 40 || row_no == 41 || row_no == 48 || row_no == 53 || row_no == 55 || row_no == 60 || row_no == 61)
      lemma_ref.long_adj = 2;
    else
      lemma_ref.long_adj = 1;
  }

  if (conj_type == "masc_ji" || conj_type == "masc_a" || conj_type == "masc_a_PV3" || conj_type == "masc_ja")
  {
    row_no += 21;
  }

  if (row_no < 1)
  {
    lemma_ref.lemma_form = "corrupted morphology tag";
    return;
  }

  // оходити vs отъходити (strictly speaking the first is etymologically correct, but levelling to the preposition *otъ- is doubtless Common Slavonic also)
  if (lemma_ref.lemma_id == 64811 && Sniff(cyr_id, "от", 20))
  {
    stem = "otъxod";
  }

  // switch to adjectival declension in толикъ, селикъ, коликъ, еликъ
  if (lemma_ref.lemma_id == 69339 || lemma_ref.lemma_id == 69449 || lemma_ref.lemma_id == 68844 || lemma_ref.lemma_id == 83963 || lemma_ref.lemma_id == 66105)
  {
    if (row_no == 3 || row_no == 4 || row_no == 5 || row_no == 10 || row_no == 12 || row_no == 24 || row_no == 25 || row_no == 26 || row_no == 31 || row_no == 33 || row_no == 45 || row_no == 46 || row_no == 47 || row_no == 52 || row_no == 54)
    {
      if (Sniff(cyr_id, "о", 3) == false)
        outer_map_no = 121;
    }

    if (row_no == 6 || row_no == 11 || row_no == 13 || row_no == 17 || row_no == 18 || row_no == 20 || row_no == 32 || row_no == 34 || row_no == 38 || row_no == 39 || row_no == 40 || row_no == 41 || row_no == 48 || row_no == 53 || row_no == 55 || row_no == 59 || row_no == 60 || row_no == 62)
    {
      if (Sniff(cyr_id, "ѣ", 3) == false)
        outer_map_no = 121;
    }
  }

  // !!!!CHECK FOR the тѧжъкъ lemma when doing comparatives to give correct тѧжии form, and вꙑшь҆ша
  if (conj_type == "adj_hard" && degree == 2)
  {
    if (lemma_ref.lemma_id == 65093)
    {
      stem = "vyš";
      outer_map_no = 1311;
    }
    else if (lemma_ref.lemma_id == 67766)
    {
      stem = "tęž";
      outer_map_no = 1311;
    }
    else if (lemma_ref.lemma_id == 64043)
    {
      stem = "liš";
      outer_map_no = 1311;
    }
    else if (lemma_ref.lemma_id == 58888)
    {
      stem = "krěpĺ";
      outer_map_no = 1311;
    }

    else
    {
      stem += "ěj";
      PV1_long_e(stem);
      outer_map_no = 1311;
    }
  }

  // soft comparatives boĺьjь etc.
  if (outer_map_no == 1311 && Sniff(stem, "ěj", 2) == false && (row_no == 1 || row_no == 2 || row_no == 7))
  {
    strength = 1;
  } // only definitises the *jьš- comparatives
  if (outer_map_no == 1311 && (row_no == 15 || row_no == 21))
  {
    if (Sniff(cyr_id, "ші", 3))
      outer_map_no++;
  }
  if (outer_map_no == 1311 && (row_no == 43 || row_no == 44 || row_no == 49))
  {
    if (Sniff(cyr_id, "ше", 3) && stem != "liš")
      outer_map_no++;
  }
  if (outer_map_no == 1311 && (row_no == 22 || row_no == 28))
  {
    if (Sniff(cyr_id, "ша", 3) || Sniff(cyr_id, "шѣ", 3))
      outer_map_no++;
  }

  if (conj_type == "oko" && ((row_no < 49 && row_no > 44) || (row_no < 64 && row_no > 56)))
  {
    if (Sniff(cyr_id, "ес", 5) == false)
      outer_map_no++;
  }
  if (conj_type == "oko" && (row_no < 56 && row_no > 51))
  {
    if (Sniff(cyr_id, "ч", 6) == false && Sniff(cyr_id, "ш", 6) == false)
      outer_map_no++;
  }

  if (conj_type == "den" && (row_no == 3 || row_no == 5))
  {
    if (Sniff(cyr_id, "ні", 2))
      outer_map_no++;
  }
  if (conj_type == "den" && (row_no == 10 || row_no == 12))
  {
    if (Sniff(cyr_id, "і", 2) || Sniff(cyr_id, "ью", 2) || Sniff(cyr_id, "ю", 1) || Sniff(cyr_id, "ьѹ", 2))
      outer_map_no++;
  }
  if (conj_type == "den" && (row_no == 15 || row_no == 21))
  {
    if (Sniff(cyr_id, "і", 2) || Sniff(cyr_id, "ье", 2))
      outer_map_no++;
  }
  if (conj_type == "den" && (row_no == 17))
  {
    if (Sniff(cyr_id, "і", 2) || Sniff(cyr_id, "ьі", 2) || Sniff(cyr_id, "еі", 2))
      outer_map_no++;
  }
  if (conj_type == "den" && (row_no == 20))
  {
    if (Sniff(cyr_id, "м", 2))
      outer_map_no += 2;
  }
  // fem i-stems
  if (outer_map_no == 141 && row_no == 28)
  {
    if (Sniff(cyr_id, "ь", 1))
      outer_map_no++;
  }
  // masc i-stems
  if (outer_map_no == 141 && (row_no == 3))
  {
    if (Sniff(cyr_id, "а", 1) || Sniff(cyr_id, "ѣ", 1))
      outer_map_no++;
  }
  if (outer_map_no == 141 && (row_no == 4))
  {
    if (Sniff(cyr_id, "ѹ", 1) || Sniff(cyr_id, "ю", 1))
      outer_map_no++;
  }
  if (outer_map_no == 141 && (row_no == 7) && conj_type != "tri")
  {
    if (Sniff(cyr_id, "ь", 1))
      outer_map_no++;
  }

  // apparent nasal doublet in "thousand"
  if ((lemma_ref.lemma_id == 64577 || lemma_ref.lemma_id == 147004) && Sniff(cyr_id, "сѧ", 20))
  {
    stem = "tysętj";
  }
  // fem ja-stems with /ji/ in nominative singular
  if (outer_map_no == 151 && row_no == 22)
  {
    if (Sniff(cyr_id, "ѣ", 1) || Sniff(cyr_id, "ꙗ", 2) || Sniff(cyr_id, "а", 1))
      outer_map_no++;
  }

  // fem R-stems
  if (outer_map_no == 161 && (row_no == 24 || row_no == 26))
  {
    if (Sniff(cyr_id, "рі", 2))
      outer_map_no++;
  }
  if (outer_map_no == 161 && (row_no == 31 || row_no == 33))
  {
    if (Sniff(cyr_id, "і", 2) || Sniff(cyr_id, "ью", 2) || Sniff(cyr_id, "ю", 1) || Sniff(cyr_id, "ьѹ", 2))
      outer_map_no++;
  }
  if (outer_map_no == 161 && (row_no == 36 || row_no == 42))
  {
    if (Sniff(cyr_id, "ре", 2) == false)
      outer_map_no++;
  }

  // fem uu-stems
  if (outer_map_no == 171 && (row_no == 22 || row_no == 28))
  {
    if (Sniff(cyr_id, "в", 2))
      outer_map_no++;
  }
  if (outer_map_no == 171 && (row_no == 23))
  {
    if (Sniff(cyr_id, "в", 2) == false)
      outer_map_no++;
  }
  if (outer_map_no == 171 && (row_no == 24 || row_no == 26))
  {
    if (Sniff(cyr_id, "ві", 2))
      outer_map_no++;
  }
  if (outer_map_no == 171 && (row_no == 31 || row_no == 33))
  {
    if (Sniff(cyr_id, "і", 2) || Sniff(cyr_id, "ью", 2) || Sniff(cyr_id, "ю", 1) || Sniff(cyr_id, "ьѹ", 2))
      outer_map_no++;
  }
  if (outer_map_no == 171 && (row_no == 36 || row_no == 42))
  {
    if (Sniff(cyr_id, "ве", 2) == false)
      outer_map_no++;
  }
  if (outer_map_no == 171 && (row_no == 39 || row_no == 40 || row_no == 41))
  {
    if (Sniff(cyr_id, "ва", 3))
      outer_map_no++;
  }

  // masc anin-nouns
  if (outer_map_no == 181 && (row_no == 7))
  {
    if (Sniff(cyr_id, "е", 1) == false)
      outer_map_no++;
  }
  if (outer_map_no == 181 && (row_no == 16))
  {
    if (Sniff(cyr_id, "ъі", 2))
      outer_map_no++;
  }
  if (outer_map_no == 181 && (row_no == 18 || row_no == 19 || row_no == 20))
  {
    if (Sniff(cyr_id, "н", 4))
      outer_map_no++;
  }

  // foreign masc-jo stems which often just have o-stem endings
  if (outer_map_no == 201 && (row_no == 4))
  {
    if (Sniff(cyr_id, "ов", 3))
      outer_map_no++;
  }
  if (outer_map_no == 201 && (row_no == 4))
  {
    if (Sniff(cyr_id, "ев", 3))
      outer_map_no += 2;
  }
  if (outer_map_no == 201 && (row_no == 6 || row_no == 11 || row_no == 13 || row_no == 18))
  {
    if (Sniff(cyr_id, "ом", 3))
      outer_map_no++;
  }
  if (outer_map_no == 201 && row_no == 7)
  {
    if (Sniff(cyr_id, "е", 1))
      outer_map_no++;
  }

  // masc ju-stems (basically just mǫžь)
  if (outer_map_no == 211 && (row_no == 3 || row_no == 8 || row_no == 9 || row_no == 14))
  {
    if (Sniff(cyr_id, "а", 1) || Sniff(cyr_id, "ѣ", 1) || Sniff(cyr_id, "ꙗ", 2))
      outer_map_no++;
  }
  if (outer_map_no == 211 && (row_no == 4 || row_no == 10 || row_no == 12 || row_no == 15 || row_no == 17 || row_no == 21))
  {
    if (Sniff(cyr_id, "ев", 3) == false && Sniff(cyr_id, "ьв", 3) == false)
      outer_map_no++;
  }
  if (outer_map_no == 211 && row_no == 5)
  {
    if (Sniff(cyr_id, "і", 1))
      outer_map_no++;
  }
  if (outer_map_no == 211 && (row_no == 11 || row_no == 13))
  {
    if (Sniff(cyr_id, "ем", 3))
      outer_map_no++;
  }
  if (outer_map_no == 211 && (row_no == 19))
  {
    if (Sniff(cyr_id, "іх", 3))
      outer_map_no++;
  }
  if (outer_map_no == 211 && (row_no == 20))
  {
    if (Sniff(cyr_id, "ьм", 3) == false && Sniff(cyr_id, "ъм", 3) == false)
      outer_map_no++;
  }
  if (outer_map_no == 211 && (row_no == 6 || row_no == 18))
  {
    outer_map_no += 2;
  }

  // masc N-stems
  if (outer_map_no == 221 && (row_no == 1 || row_no == 7))
  {
    if (Sniff(cyr_id, "н", 2))
      outer_map_no++;
  }
  if (outer_map_no == 221 && (row_no == 3 || row_no == 5))
  {
    if (Sniff(cyr_id, "ні", 2))
      outer_map_no++;
  }
  if (outer_map_no == 221 && (row_no == 10 || row_no == 12))
  {
    if (Sniff(cyr_id, "і", 2) || Sniff(cyr_id, "ью", 2) || Sniff(cyr_id, "ю", 1) || Sniff(cyr_id, "ьѹ", 2))
      outer_map_no++;
  }
  if (outer_map_no == 221 && (row_no == 15 || row_no == 21))
  {
    if (Sniff(cyr_id, "і", 2) || Sniff(cyr_id, "ье", 2))
      outer_map_no++;
  }
  if (outer_map_no == 221 && (row_no == 17))
  {
    if (Sniff(cyr_id, "і", 2) || Sniff(cyr_id, "ьі", 2) || Sniff(cyr_id, "еі", 2))
      outer_map_no++;
  }
  if (outer_map_no == 221 && (row_no == 20))
  {
    if (Sniff(cyr_id, "нъі", 3))
      outer_map_no += 2;
  }

  //  masc -teĺь words
  if (outer_map_no == 231 && (row_no == 7))
  {
    if (Sniff(cyr_id, "ю", 1) == false && Sniff(cyr_id, "ѹ", 1) == false)
      outer_map_no++;
  }
  if (outer_map_no == 231 && (row_no == 15 || row_no == 21))
  {
    if (Sniff(cyr_id, "і", 1))
      outer_map_no++;
  }
  if (outer_map_no == 231 && row_no == 17)
  {
    if (Sniff(cyr_id, "ь", 1))
      outer_map_no++;
  }
  if (outer_map_no == 231 && row_no == 20)
  {
    if (Sniff(cyr_id, "ъі", 2) == false)
      outer_map_no++;
  }

  //  masc -aŕь words
  if (outer_map_no == 311 && row_no == 4)
  {
    if (Sniff(cyr_id, "еві", 3))
      outer_map_no++;
  }
  if (outer_map_no == 311 && (row_no == 7))
  {
    if (Sniff(cyr_id, "ю", 1) == false && Sniff(cyr_id, "ѹ", 1) == false)
      outer_map_no++;
  }
  if (outer_map_no == 311 && (row_no == 15 || row_no == 21))
  {
    if (Sniff(cyr_id, "е", 1))
      outer_map_no += 2;
  }

  //   masc u-stems
  if (outer_map_no == 241 && (row_no == 3 || row_no == 8 || row_no == 9 || row_no == 14))
  {
    if (Sniff(cyr_id, "а", 1))
      outer_map_no++;
  }
  if (outer_map_no == 241 && (row_no == 4 || row_no == 10 || row_no == 12 || row_no == 15 || row_no == 17 || row_no == 21))
  {
    if (Sniff(cyr_id, "в", 2) == false && Sniff(cyr_id, "в", 2) == false)
      outer_map_no++;
  }
  if (outer_map_no == 241 && (row_no == 5 || row_no == 7))
  {
    if (Sniff(cyr_id, "ѹ", 1) == false && Sniff(cyr_id, "у", 1) == false)
      outer_map_no++;
  }
  if (outer_map_no == 241 && (row_no == 11 || row_no == 13))
  {
    if (Sniff(cyr_id, "ом", 3))
      outer_map_no++;
  }
  if (outer_map_no == 241 && (row_no == 19))
  {
    if (Sniff(cyr_id, "ѣх", 3))
      outer_map_no++;
  }
  if (outer_map_no == 241 && (row_no == 20))
  {
    if (Sniff(cyr_id, "ъі", 2))
      outer_map_no++;
  }

  //   masc o-u-stems
  if (outer_map_no == 251 && (row_no == 3 || row_no == 8 || row_no == 9 || row_no == 14))
  {
    if (Sniff(cyr_id, "а", 1) == false)
      outer_map_no += 2;
  }
  if (outer_map_no == 251 && (row_no == 4 || row_no == 10 || row_no == 12 || row_no == 15 || row_no == 17 || row_no == 21))
  {
    if (Sniff(cyr_id, "в", 2) || Sniff(cyr_id, "в", 2))
      outer_map_no += 2;
  }
  if (outer_map_no == 251 && (row_no == 5 || row_no == 7))
  {
    if (Sniff(cyr_id, "ѹ", 1) || Sniff(cyr_id, "у", 1))
      outer_map_no += 2;
  }
  if (outer_map_no == 251 && (row_no == 11 || row_no == 13))
  {
    if (Sniff(cyr_id, "ом", 3) == false)
      outer_map_no += 2;
  }
  if (outer_map_no == 251 && (row_no == 19))
  {
    if (Sniff(cyr_id, "ѣх", 3) == false)
      outer_map_no += 2;
  }
  if (outer_map_no == 251 && (row_no == 20))
  {
    if (Sniff(cyr_id, "ъі", 2) == false)
      outer_map_no += 2;
  }

  // deviant u-stem desiences on masc o-stems (incomplete)
  if (conj_type == "masc_o" && (row_no == 4 || row_no == 15 || row_no == 17 || row_no == 21))
  {
    if (Sniff(cyr_id, "ов", 3))
      outer_map_no++;
  }
  if (conj_type == "masc_o" && (row_no == 20))
  {
    if (Sniff(cyr_id, "ъмі", 3))
      outer_map_no++;
  }

  // nt N-stems
  if (outer_map_no == 261 && (row_no == 45 || row_no == 47))
  {
    if (Sniff(cyr_id, "ні", 2))
      outer_map_no++;
  }
  if (outer_map_no == 261 && (row_no == 50 || row_no == 51 || row_no == 56))
  {
    if (Sniff(cyr_id, "нѣ", 2))
      outer_map_no++;
  }

  // nt NT-stems
  if (outer_map_no == 271 && (row_no == 45 || row_no == 47))
  {
    if (Sniff(cyr_id, "ті", 2))
      outer_map_no++;
  }
  if (outer_map_no == 271 && (row_no == 50 || row_no == 51 || row_no == 56))
  {
    if (Sniff(cyr_id, "тѣ", 2))
      outer_map_no++;
  }

  // nt S-stems
  if (outer_map_no == 281 && ((row_no > 44 && row_no < 49) || (row_no > 56 && row_no < 64)))
  {
    if (Sniff(cyr_id, "с", 4) == false)
      outer_map_no++;
  } // such a restricted thing to look for is necessary because of all the abbreviations of небо; it may under-detect if words with <с> in the root are present
  if (outer_map_no == 281 && (row_no == 50 || row_no == 51 || row_no == 56))
  {
    if (Sniff(cyr_id, "сѣ", 2))
      outer_map_no++;
  }

  // nt o_S-stems (this is just for дѣлеса in Suprasliensis)
  if (outer_map_no == 291 && ((row_no > 49 && row_no < 64) || (row_no > 44 && row_no < 49)))
  {
    if (Sniff(cyr_id, "с", 4))
      outer_map_no++;
  }

  // variants of the pronoun sь
  if (outer_map_no == 81 && (row_no == 1 || row_no == 7))
  {
    if (Sniff(cyr_id, "іі", 2) || Sniff(cyr_id, "ьі", 2) || Sniff(cyr_id, "еі", 2))
      outer_map_no += 2;
  }
  if (outer_map_no == 81 && (row_no == 15 || row_no == 21 || row_no == 29 || row_no == 30 || row_no == 35))
  {
    if (Sniff(cyr_id, "іі", 2) || Sniff(cyr_id, "ьі", 2) || Sniff(cyr_id, "еі", 2))
      outer_map_no += 2;
  }
  if (outer_map_no == 81 && (row_no == 36 || row_no == 37 || row_no == 42))
  {
    if (Sniff(cyr_id, "ѧ", 2) && Sniff(cyr_id, "ѩ", 2) == false)
      outer_map_no += 2;
  }

  // masc_i-stem endings of jo-stems
  if (conj_type == "masc_jo" && row_no == 15)
  {
    if (Sniff(cyr_id, "іе", 2) || Sniff(cyr_id, "ье", 2) || Sniff(cyr_id, "ъе", 2))
      outer_map_no++;
  }
  if (conj_type == "masc_jo" && row_no == 20)
  {
    if (Sniff(cyr_id, "мі", 2))
      outer_map_no++;
  }

  // o-, a-stem vocatives
  if (outer_map_no == 121 && row_no == 7 && strength == 2)
  {
    if (Sniff(cyr_id, "ъ", 1))
      outer_map_no++;
  }
  if (outer_map_no == 121 && row_no == 28 && strength == 2)
  {
    if (Sniff(cyr_id, "а", 1))
      outer_map_no++;
  }
  if (outer_map_no == 121 && row_no == 7 && strength == 1)
  {
    if (Sniff(cyr_id, "ъ", 2))
      outer_map_no++;
  }
  if (outer_map_no == 121 && row_no == 28 && strength == 1)
  {
    if (Sniff(cyr_id, "а", 2))
      outer_map_no++;
  }

  // jo-, ja-stem vocatives
  if (outer_map_no == 131 && row_no == 7 && strength == 2)
  {
    if (Sniff(cyr_id, "ь", 1) || Sniff(cyr_id, "ъ", 1))
      outer_map_no++;
  }
  if (outer_map_no == 131 && row_no == 28 && strength == 2)
  {
    if (Sniff(cyr_id, "ѣ", 1) || Sniff(cyr_id, "ꙗ", 2))
      outer_map_no++;
  }
  if (outer_map_no == 131 && row_no == 7 && strength == 1)
  {
    if (Sniff(cyr_id, "ь", 2) || Sniff(cyr_id, "ъ", 2) || Sniff(cyr_id, "і", 2))
      outer_map_no++;
  }
  if (outer_map_no == 131 && row_no == 28 && strength == 1)
  {
    if (Sniff(cyr_id, "ѣѣ", 2) || Sniff(cyr_id, "ꙗꙗ", 3) || Sniff(cyr_id, "ѣꙗ", 3) || Sniff(cyr_id, "ꙗꙗ", 3) || Sniff(cyr_id, "ꙗа", 3) || Sniff(cyr_id, "ѣа", 2) || Sniff(cyr_id, "аа", 2) || Sniff(cyr_id, "аѣ", 2) || Sniff(cyr_id, "аꙗ", 3))
      outer_map_no++;
  }

  // check for Gsg. čьso
  if (outer_map_no == 2051 && (row_no == 45 || row_no == 3 || row_no == 24))
  {
    if (Sniff(cyr_id, "ес", 7) == false)
      outer_map_no += 2;
  }
  // consider all accusative кого as morph replacements of къто
  if (outer_map_no == 2041 && (row_no == 44 || row_no == 2 || row_no == 23))
  {
    outer_map_no++;
  }

  // check for acc dual na levelled to acc pl. ny and acc pl. ny levelled to gen pl. nasъ
  if (outer_map_no == 11 && (row_no == 9 || row_no == 30 || row_no == 51))
  {
    if (Sniff(cyr_id, "нъі", 3))
      outer_map_no++;
  }
  if (outer_map_no == 11 && (row_no == 16 || row_no == 37 || row_no == 58))
  {
    if (Sniff(cyr_id, "ас", 3))
      outer_map_no++;
  }

  // check ty for similar + put tobě as the NSl. doublet
  if (outer_map_no == 21 && (row_no == 8 || row_no == 9 || row_no == 14 || row_no == 29 || row_no == 30 || row_no == 35 || row_no == 50 || row_no == 51 || row_no == 56))
  {
    if (Sniff(cyr_id, "въі", 3))
      outer_map_no++;
  }
  if (outer_map_no == 21 && (row_no == 16 || row_no == 37 || row_no == 58))
  {
    if (Sniff(cyr_id, "ас", 3))
      outer_map_no++;
  }
  if (outer_map_no == 21 && (row_no == 4 || row_no == 5 || row_no == 25 || row_no == 26 || row_no == 46 || row_no == 47))
  {
    outer_map_no += 2;
  }

  // put sobě as the NSl. doublet of sebě
  if (outer_map_no == 31 && (row_no == 4 || row_no == 5 || row_no == 11 || row_no == 12 || row_no == 18 || row_no == 19 || row_no == 25 || row_no == 26 || row_no == 32 || row_no == 33 || row_no == 39 || row_no == 40 || row_no == 46 || row_no == 47 || row_no == 53 || row_no == 54 || row_no == 60 || row_no == 61))
  {
    outer_map_no += 2;
  }

  // desętь
  if (outer_map_no == 91 && (row_no == 3 || row_no == 5 || row_no == 15 || row_no == 24 || row_no == 26 || row_no == 36 || row_no == 45 || row_no == 47 || row_no == 57))
  {
    if (Sniff(cyr_id, "ті", 2))
      outer_map_no++;
  }
  if (outer_map_no == 91 && (row_no == 17 || row_no == 38 || row_no == 59))
  {
    if (Sniff(cyr_id, "тіі", 3))
      outer_map_no++;
  }
  if (outer_map_no == 91 && (row_no == 20 || row_no == 41 || row_no == 62))
  {
    if (Sniff(cyr_id, "мі", 2))
      outer_map_no++;
  }

  /*        //check for C-stem Ipl. declension of четыре (commented out because Diels says it dun't appen)
    if(outer_map_no == 191 && (row_no == 20 || row_no == 41 || row_no == 62)) {if(Sniff(cyr_id, "ръі", 3)) outer_map_no+=2; }
*/

  std::string flected_word = "";
  std::string morph_replace_word = "";

  if (outer_map_no == 301)
  {
    flected_word = stem + Nom_[outer_map_no][row_no];
    if (row_no == 7)
    {
      first_velar_clean(flected_word);
    }
    lemma_ref.lemma_form = flected_word;
    if (row_no != 3 && row_no != 4 && row_no != 7 && row_no != 8 &&
        row_no != 9 && row_no != 10 && row_no != 12 && row_no != 14 &&
        row_no != 15 && row_no != 21 && row_no != 22 && row_no != 23 &&
        row_no != 31 && row_no != 32 && row_no != 33 && row_no != 34 &&
        row_no != 39 && row_no != 40 && row_no != 41 && row_no != 45 &&
        row_no != 46 && row_no != 52 && row_no != 54 && row_no != 57 &&
        row_no != 58 && row_no != 63)
    {
      lemma_ref.morph_replace = stem + Nom_[outer_map_no + 1][row_no];
    }
    else
      lemma_ref.morph_replace = "";
    return;
  }

  if (outer_map_no == 2031)
  {
    flected_word = stem + Nom_[outer_map_no][row_no];
    lemma_ref.lemma_form = flected_word;
    if (row_no == 1 || row_no == 2 || row_no == 3 || row_no == 4 || row_no == 5 || row_no == 7 || row_no == 16 || row_no == 24 || row_no == 25 || row_no == 26 || row_no == 27 || row_no == 36 || row_no == 37 || row_no == 42 || row_no == 43 || row_no == 44 || row_no == 45 || row_no == 46 || row_no == 47 || row_no == 49)
    {
      lemma_ref.morph_replace = stem + Nom_[outer_map_no + 1][row_no];
    }
    else
      lemma_ref.morph_replace = "";
    return;
  }

  if (conj_type == "adj_ij")
  {
    lemma_ref.lemma_form = stem + Nom_[131][row_no];
    return;
  }

  // add the post-prepositional н to the personal pronouns
  if (outer_map_no == 51)
  {
    if (conj_type == "jьže")
    {
      if (Sniff(cyr_id, "н", 20))
      {
        lemma_ref.lemma_form = "n" + stem + Nom_[outer_map_no][row_no] + "že";
      }
      else
      {
        lemma_ref.lemma_form = stem + Nom_[outer_map_no][row_no] + "že";
      }
      return;
    }
    else
    {
      if (Sniff(cyr_id, "н", 20))
      {
        lemma_ref.lemma_form = "n" + stem + Nom_[outer_map_no][row_no];
      }
      else
      {
        lemma_ref.lemma_form = stem + Nom_[outer_map_no][row_no];
      }
      return;
    }
  }

  if (conj_type == "čьtože" || conj_type == "kъtože" || conj_type == "kъjьže" || conj_type == "pron_hard_že")
  {
    lemma_ref.lemma_form = stem + Nom_[outer_map_no][row_no] + "že";
    if (outer_map_no % 10 == 2)
    {
      lemma_ref.morph_replace = stem + Nom_[outer_map_no - 1][row_no] + "že";
    }
    if (outer_map_no % 10 == 3)
    {
      lemma_ref.poss_doublet = stem + Nom_[outer_map_no - 2][row_no] + "že";
    }
    return;
  }

  if (conj_type == "pron_hard_ђe")
  {
    lemma_ref.lemma_form = stem + Nom_[outer_map_no][row_no] + "dje";
    if (outer_map_no % 10 == 2)
    {
      lemma_ref.morph_replace = stem + Nom_[outer_map_no - 1][row_no] + "dje";
    }
    if (outer_map_no % 10 == 3)
    {
      lemma_ref.poss_doublet = stem + Nom_[outer_map_no - 2][row_no] + "dje";
    }
    return;
  }
  if (conj_type == "kъžьdo")
  {
    lemma_ref.lemma_form = stem + Nom_[outer_map_no][row_no] + "žьdo";
    if (outer_map_no % 10 == 2)
    {
      lemma_ref.morph_replace = stem + Nom_[outer_map_no - 1][row_no] + "žьdo";
    }
    if (outer_map_no % 10 == 3)
    {
      lemma_ref.poss_doublet = stem + Nom_[outer_map_no - 2][row_no] + "žьdo";
    }
    return;
  }

  // checks for cheeky PV1 vocatives отьче etc.
  if (strength == 1)
  {
    flected_word = stem + Nom_[outer_map_no][row_no] + Fetch_Article(row_no);
  }
  else
    flected_word = stem + Nom_[outer_map_no][row_no];
  if (((conj_type == "masc_o" || conj_type == "masc_o_PV3" || conj_type == "adj_hard") && row_no == 7 && (Sniff(flected_word, "3", 4) == true || Sniff(flected_word, "g", 3) == true || Sniff(flected_word, "x", 4) == true)) || conj_type == "oko")
  {
    first_velar_clean(flected_word);
  }

  // find *-pis- stem in *-pьsanьje N_PV3_nouns
  if ((lemma_ref.lemma_id == 60004 || lemma_ref.lemma_id == 121668) && Sniff(cyr_id, "піс", 20))
  {
    lemma_ref.morph_replace = flected_word;
    pisanie_clean(stem);
    lemma_ref.lemma_form = stem + Nom_[outer_map_no][row_no];
    return;
  }

  lemma_ref.lemma_form = flected_word;

  // Dsg. ми, си, ти
  if (conj_type == "ty" && gr_case == 4 && Sniff(cyr_id, "ті", 20))
  {
    lemma_ref.lemma_form = "ti";
  }
  if (conj_type == "azъ" && gr_case == 4 && Sniff(cyr_id, "мі", 20))
  {
    lemma_ref.lemma_form = "mi";
  }
  if (conj_type == "sę" && gr_case == 4 && Sniff(cyr_id, "сі", 20))
  {
    lemma_ref.lemma_form = "si";
  }

  if (outer_map_no % 10 == 2)
  {

    if (strength == 1)
    {
      morph_replace_word = stem + Nom_[outer_map_no - 1][row_no] + Fetch_Article(row_no);
    }
    else
      morph_replace_word = stem + Nom_[outer_map_no - 1][row_no];
    if (((conj_type == "masc_o" || conj_type == "masc_o_PV3" || conj_type == "adj_hard") && row_no == 7 && (Sniff(flected_word, "k", 3) == true || Sniff(flected_word, "g", 3) == true || Sniff(flected_word, "x", 3) == true)) || conj_type == "oko")
    {
      first_velar_clean(morph_replace_word);
    }
    lemma_ref.morph_replace = morph_replace_word;
    return;
  }
  if (outer_map_no % 10 == 3)
  {
    lemma_ref.poss_doublet = stem + Nom_[outer_map_no - 2][row_no];
    return;
  }
}
