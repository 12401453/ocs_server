void verb_Flect(Lemma &lemma_ref, short int int_morph_tag[10], std::string cyr_id)
{

  std::string conj_type = lemma_ref.conj_type;
  std::string stem = lemma_ref.lemma_form;

  int col_no = 0; // get rid
  int row_no = 0;
  int outer_map_no = conj_type_enum(conj_type);

  // помѣнѫти vs помѧнѫти doublets
  if (lemma_ref.lemma_id == 68352)
  {
    if (Sniff(cyr_id, "мѣн", 20))
      stem = "poměn";
  }
  if (lemma_ref.lemma_id == 59280)
  {
    if (Sniff(cyr_id, "мѣн", 20))
      stem = "vъspoměn";
  }

  short int num{int_morph_tag[1]}, pers{int_morph_tag[0]}, tense{int_morph_tag[2]}, mood{int_morph_tag[3]};
  if (mood != 2 && tense != 8)
  {
    if (mood != 6)
    {
      if (mood == 1)
      {
        row_no = (9 * (tense - 1) + 3 * (num - 1) + pers);
      } // number of the unordered_map for normal finite verb forms (present, imperfect, or aorist)
      else if (mood == 3)
      {
        row_no = (9 * 3 + 3 * (num - 1) + pers);
      } // number of the unordered_map for imperatives (which are all present tense)
      else if (mood == 5)
      {
        row_no = 43;
      }
      else if (mood == 9)
      {
        row_no = 44;
      }
      else
      {
        lemma_ref.lemma_form = "corrupted morphology tag";
        return;
      }

      bool pisati = false;

      if ((conj_type == "iskati") && (row_no < 10 || mood == 3) && (Sniff(cyr_id, "скѫ", 6) || Sniff(cyr_id, "метѫ", 7) || Sniff(cyr_id, "мете", 7)))
      {
        outer_map_no -= 10;
      }
      if ((conj_type == "51_abl" || conj_type == "52_abl" || conj_type == "53_abl") && (row_no < 10 || mood == 3) && Sniff(cyr_id, "жьд", 20) == false && Sniff(cyr_id, "жъд", 20) == false && Sniff(cyr_id, "жʼд", 20) == false)
      {
        class_5_ablaut_clean(stem);
      }
      // check for pisati etc.
      if ((conj_type == "52_abl" || conj_type == "51_abl") && row_no > 9 && mood != 3 && (Sniff(cyr_id, "піс", 20) || Sniff(cyr_id, "жід", 20)))
      {
        pisati = true;
      }

      if ((conj_type == "infix_11") && (row_no < 10 || mood == 3))
      {
        class_11_infix_clean(stem);
      }
      if ((conj_type == "have") && (row_no == 9))
      {
        if (Sniff(cyr_id, "ѣ", 4))
          outer_map_no++;
      }
      if ((conj_type == "dati" || conj_type == "eat" || conj_type == "pref_eat" || conj_type == "byti" || conj_type == "pref_byti") && (row_no == 11 || row_no == 12))
      {
        if (Sniff(cyr_id, "ст", 3) || Sniff(cyr_id, "ⷭ҇", 20))
          outer_map_no++;
      } // add byti
      if ((conj_type == "eat" || conj_type == "pref_eat") && (row_no == 10 || row_no == 13 || row_no == 16))
      {
        if (Sniff(cyr_id, "х", 4))
          outer_map_no++;
      }
      if ((row_no == 18) && (conj_type == "eat" || conj_type == "pref_eat"))
      {
        if (Sniff(cyr_id, "ш", 2))
          outer_map_no++;
      }
      if ((row_no == 1) && (conj_type == "wote"))
      {
        if (Sniff(cyr_id, "дѣ", 2))
          outer_map_no += 2;
      }
      if (conj_type == "byti")
      {
        if (row_no == 19 || row_no == 22 || row_no == 25)
        {
          if (Sniff(cyr_id, "ѣх", 5))
            outer_map_no = outer_map_no * 100 + 1;
        }
        if (row_no == 20 || row_no == 21 || row_no == 23 || row_no == 24 || row_no == 26)
        {
          if (Sniff(cyr_id, "ш", 4) == false && Sniff(cyr_id, "ѣа", 6) == false)
            outer_map_no = outer_map_no * 100 + 1;
        }
        if (row_no == 27)
        {
          if (Sniff(cyr_id, "ш", 2))
            outer_map_no = outer_map_no * 100 + 1;
        }
      }
      if (conj_type == "pref_byti" && row_no < 10)
      {
        outer_map_no = outer_map_no * 100 + 1;
      }
      if (conj_type == "dovleti" && row_no == 9)
      {
        if (Sniff(cyr_id, "ѧт", 3))
          outer_map_no = 903;
      }
      if (conj_type == "dovleti" && row_no < 10)
      {
        if (Sniff(cyr_id, "ѣ", 5))
          outer_map_no = 902;
      }

      if ((conj_type == "21" || conj_type == "11" || conj_type == "jьti" || conj_type == "14" || conj_type == "15" || conj_type == "rěsti") && (row_no == 10 || row_no == 13 || row_no == 16)) {
        if (Sniff(cyr_id, "ох", 5)) outer_map_no++;
      }
      if ((conj_type == "21" || conj_type == "11" || conj_type == "jьti" || conj_type == "14" || conj_type == "15" || conj_type == "rěsti") && (row_no == 14 || row_no == 15 || row_no == 17)) {
        if (Sniff(cyr_id, "ост", 4)) outer_map_no++;
      }
      if ((conj_type == "21" || conj_type == "11" || conj_type == "jьti" || conj_type == "14" || conj_type == "15" || conj_type == "rěsti") && row_no == 18) {
        if (Sniff(cyr_id, "ош", 3)) outer_map_no++;
      }
      if ((conj_type != "11" && conj_type != "jьti" && conj_type != "14" && conj_type != "15" && conj_type != "rěsti" && conj_type != "infix_11" && conj_type != "jaxati") && (row_no == 6 || row_no == 15 || row_no == 24))
      {
        if (Sniff(cyr_id, "та", 2))
          outer_map_no++;
      } // sniffs for 3rd dual replaced by 2nd dual ending (all tenses)
      if ((row_no > 31 && row_no < 36) && (outer_map_no == 521 || conj_type == "53" || conj_type == "54" || conj_type == "16" || conj_type == "17" || conj_type == "18" || conj_type == "rěsti" || conj_type == "19"))
      {
        if (Sniff(cyr_id, "ѣ", 3) || Sniff(cyr_id, "ат", 3) || Sniff(cyr_id, "ам", 3))
        {
          outer_map_no++;
        }
      } // leaves out 1st dual because only one class 4 attestation exists in TOROT and it is сътворивѣ
      if (conj_type == "rěsti" && (row_no > 18 && row_no < 28))
      {
        if (Sniff(cyr_id, "ѧшт", 20))
          outer_map_no++;
      }

      // class 21 *-nǫ- aorists
      if (outer_map_no == 211 && (row_no < 19 && row_no > 9))
      {
        if (Sniff(cyr_id, "нѫ", 6) || Sniff(cyr_id, "нѭ", 6) || Sniff(cyr_id, "нѹ", 6))
          outer_map_no = outer_map_no * 10 + 2;
      }

      if ((conj_type == "11" || conj_type == "14" || conj_type == "15") && outer_map_no % 10 == 1)
      {
        if (row_no == 10 || row_no == 13 || row_no == 16)
        {
          if (Sniff(cyr_id, "ѣх", 5))
            outer_map_no += 2;
        } // these two if statements exist entirely due to решти
        if (row_no == 18)
        {
          if (Sniff(cyr_id, "ѣш", 5))
            outer_map_no += 2;
        }
        if (outer_map_no % 10 == 1)
        {
          if (row_no == 10 || row_no == 18)
          {
            if (Sniff(cyr_id, "ѣс", 3) || Sniff(cyr_id, "ас", 3))
              outer_map_no += 2;
          }
          if (row_no == 14 || row_no == 15 || row_no == 17)
          {
            if (Sniff(cyr_id, "ѣст", 4) || Sniff(cyr_id, "аст", 4))
              outer_map_no += 2;
          }
          if (row_no == 13 || row_no == 16)
          {
            if (Sniff(cyr_id, "ѣсо", 5) || Sniff(cyr_id, "асо", 5))
              outer_map_no += 2;
          }
        }
      }

      if ((conj_type == "12" || conj_type == "16") && outer_map_no % 10 == 1)
      {
        if (row_no == 10)
        {
          if (Sniff(cyr_id, "х", 2))
            outer_map_no += 1;
        }
        if (row_no == 11 || row_no == 12)
        {
          if (Sniff(cyr_id, "т", 2))
            outer_map_no += 1;
        }
        if (row_no == 13 || row_no == 16)
        {
          if (Sniff(cyr_id, "х", 4))
            outer_map_no += 1;
        }
        if (row_no == 18)
        {
          if (Sniff(cyr_id, "ш", 2))
            outer_map_no += 1;
        }
      }

      if (conj_type == "ьt" /* || conj_type == "ьz" */)
      {
        if (row_no == 18)
        {
          if (Sniff(cyr_id, "ш", 2))
            outer_map_no += 1;
        }
        if (row_no > 9 && row_no < 18)
        {
          if (Sniff(cyr_id, "то", 6))
            outer_map_no += 1;
        }
      }

      if (conj_type == "rti")
      {
        if (row_no == 11 || row_no == 12)
        {
          if (Sniff(cyr_id, "ръ", 2) || Sniff(cyr_id, "рь", 2))
            outer_map_no += 2;
        }
        if (row_no == 18)
        {
          if (Sniff(cyr_id, "ръш", 4) || Sniff(cyr_id, "рьш", 4) || Sniff(cyr_id, "рш", 3))
            outer_map_no += 2;
        }
      }

      // check for 3rd sg. -тъ aorists on the пити and пѣти verbs
      if ((conj_type == "17" || conj_type == "19") && (row_no == 11 || row_no == 12))
      {
        if (Sniff(cyr_id, "т", 2))
          outer_map_no += 1;
      }
      // check for съпѣаше-style imperfects of пѣти verbs, which I maintain are deviances from after the aorist/infinite-stem stopped being identical to the present stem
      if (outer_map_no == 191 && (row_no > 18 && row_no < 28))
      {
        if (Sniff(cyr_id, "пѣ", 20))
          outer_map_no++;
      }

      // checks for -tъ extension of 2nd and 3rd -er-stem aorists like *umertъ for *umer
      if (conj_type == "13" && outer_map_no % 10 == 1)
      {

        if (row_no == 11 || row_no == 12)
        {
          if (Sniff(cyr_id, "ѣт", 3))
            outer_map_no += 1;
        }
      }

      // jǞxati checks
      if (outer_map_no == 4001)
      {
        if (row_no == 6 && Sniff(cyr_id, "та", 2))
        {
          outer_map_no++;
        }
        if ((row_no == 10 || row_no == 13 || row_no == 16) && Sniff(cyr_id, "ох", 5))
        {
          outer_map_no++;
        }
        if ((row_no == 14 || row_no == 15 || row_no == 17) && Sniff(cyr_id, "ос", 4))
        {
          outer_map_no++;
        }
        if (row_no == 18 && Sniff(cyr_id, "ош", 3))
        {
          outer_map_no++;
        }
        if (row_no == 24 && Sniff(cyr_id, "та", 2))
        {
          outer_map_no++;
        }
        if ((row_no == 29 || row_no == 30 || row_no == 35) && Sniff(cyr_id, "ха", 5))
        {
          outer_map_no++;
        }
      }

      /*      std::cout << "row_no: " << row_no << std::endl;
            std::cout << "outer_map_no: " << outer_map_no << std::endl;      */

      std::string ending = verb_[outer_map_no][row_no];
      std::string flected_word = stem + ending;

      if (conj_type == "11" || conj_type == "infix_11" || conj_type == "infix_12" || conj_type == "14" || conj_type == "15" || conj_type == "21" || conj_type == "31" || conj_type == "iskati")
      {
        bool check_imper = (mood == 3);
        class_I_clean(flected_word, check_imper);
        if (conj_type == "14" && (row_no == 43 || row_no == 44))
        {
          ablaut_clean_14(flected_word);
        } // change the infinitive and supine to full-grade ablaut. the imperfectives are left with zero-grade, which may be wrong but none are attested in the TOROT texts anyway
        if (conj_type == "14" && outer_map_no % 10 == 3 && (row_no == 10 || row_no == 13 || row_no == 14 || row_no == 15 || row_no == 16 || row_no == 17 || row_no == 18))
        {
          ablaut_clean_14(flected_word);
        }

        // check for 2,3rd dual and 3pl. imperfects with aorist ending
        if (outer_map_no != 1101 && (row_no == 23 || row_no == 24 || row_no == 26) && (Sniff(cyr_id, "ста", 3) || Sniff(cyr_id, "сте", 3)))
        {
          lemma_ref.morph_replace = flected_word;
          imperf_sheta(flected_word);
          lemma_ref.lemma_form = flected_word;
        }
        else
        {
          lemma_ref.lemma_form = flected_word;
        }
        if (outer_map_no % 10 == 2)
        {
          std::string morph_replace_word = stem + verb_[outer_map_no - 1][row_no];
          class_I_clean(morph_replace_word, check_imper); // class 14 morph_replace words don't need to be cleaned, as the infinitive and supine never occur in non-etymological form, so when row_no ==43 or 44, outer_map_no is never +1

          if ((conj_type == "21" || conj_type == "31" || conj_type == "32" || conj_type == "iskati") == false)
          {
            std::string doublet_word = stem + verb_[outer_map_no + 1][row_no];
            class_I_clean(doublet_word, check_imper);
            if ((conj_type == "14") && (row_no == 10 || row_no == 13 || row_no == 14 || row_no == 15 || row_no == 16 || row_no == 17 || row_no == 18))
            {
              ablaut_clean_14(doublet_word);
            } // S-aorists of class 14 have full-grade ablaut
            lemma_ref.poss_doublet = doublet_word;
          }
          lemma_ref.morph_replace = morph_replace_word;

          return;
        }
        if (outer_map_no % 10 == 3)
        {
          std::string doublet_word = stem + verb_[outer_map_no - 2][row_no];
          class_I_clean(doublet_word, check_imper);
          lemma_ref.poss_doublet = doublet_word;
          return;
        }
        return;
      }

      if (conj_type == "12" || conj_type == "16")
      {

        if (tense == 2 || /* row_no == 40 || row_no == 41 || */ row_no == 43 || row_no == 44)
        {
          class_I_nasal_clean(flected_word);
        }
        if (outer_map_no % 10 == 2)
        {
          std::string morph_replace_word = stem + verb_[outer_map_no - 1][row_no];
          if (tense == 2 || /* row_no == 40 || row_no == 41 || */ row_no == 43 || row_no == 44)
          {
            class_I_nasal_clean(morph_replace_word);
          }
          lemma_ref.lemma_form = flected_word;
          // check for 2,3rd dual and 3pl. imperfects with aorist ending
          if (outer_map_no != 1101 && (row_no == 23 || row_no == 24 || row_no == 26) && (Sniff(cyr_id, "ста", 3) || Sniff(cyr_id, "сте", 3)))
          {
            imperf_sheta(flected_word);
            lemma_ref.lemma_form = flected_word;
          }

          lemma_ref.morph_replace = morph_replace_word;
          return;
        }
        else
        {
          if (outer_map_no != 1101 && (row_no == 23 || row_no == 24 || row_no == 26) && (Sniff(cyr_id, "ста", 3) || Sniff(cyr_id, "сте", 3)))
          {
            lemma_ref.morph_replace = flected_word;
            imperf_sheta(flected_word);
            lemma_ref.lemma_form = flected_word;
            return;
          }
          else
          {
            lemma_ref.lemma_form = flected_word;
            return;
          }
        }
      }

      if (conj_type == "jьti")
      {

        iti_clean(flected_word);
        if (outer_map_no % 10 == 2)
        {
          std::string morph_replace_word = stem + verb_[outer_map_no - 1][row_no];
          iti_clean(morph_replace_word);
          // check for 2,3rd dual and 3pl. imperfects with aorist ending
          if (outer_map_no != 1101 && (row_no == 23 || row_no == 24 || row_no == 26) && (Sniff(cyr_id, "ста", 3) || Sniff(cyr_id, "сте", 3)))
          {
            imperf_sheta(flected_word);
            lemma_ref.lemma_form = flected_word;
          }
          else
          {
            lemma_ref.lemma_form = flected_word;
          }
          lemma_ref.morph_replace = morph_replace_word;
          return;
        }

        else
        {
          if (outer_map_no != 1101 && (row_no == 23 || row_no == 24 || row_no == 26) && (Sniff(cyr_id, "ста", 3) || Sniff(cyr_id, "сте", 3)))
          {
            lemma_ref.morph_replace = flected_word;
            imperf_sheta(flected_word);
            lemma_ref.lemma_form = flected_word;
            return;
          }
          else
          {
            lemma_ref.lemma_form = flected_word;
            return;
          }
        }
      }

      // ******** this shit will not find deviant "ста" imperfect desinences
      // finds *žьdǫ etc. where we expect the present-stem *žid-
      if (conj_type == "51_abl" && (row_no < 10 || mood == 3) && (flected_word.find("žьd") != -1))
      {
        lemma_ref.lemma_form = flected_word;
        class_5_ablaut_clean(stem);
        lemma_ref.morph_replace = stem + ending;
      }
      // checks whether the non-present stems of *pьsati and *žьdati have been levelled to *pis, *žid
      if (pisati == true)
      {
        lemma_ref.morph_replace = flected_word;
        class_5_ablaut_clean(stem);
        lemma_ref.lemma_form = stem + ending;
        return;
      }

      if (outer_map_no != 1101 && (row_no == 23 || row_no == 24 || row_no == 26) && (Sniff(cyr_id, "ста", 3) || Sniff(cyr_id, "сте", 3)))
      {
        lemma_ref.morph_replace = flected_word;
        imperf_sheta(flected_word);
        lemma_ref.lemma_form = flected_word;
      }
      else
      {
        lemma_ref.lemma_form = flected_word;
      }
      if (outer_map_no % 10 == 2)
      {
        lemma_ref.morph_replace = stem + verb_[outer_map_no - 1][row_no];
        return;
      }
      if (outer_map_no % 10 == 3)
      {
        lemma_ref.poss_doublet = stem + verb_[outer_map_no - 2][row_no];
        return;
      }

    } // participles
    else if (mood == 6)
    {
      if (conj_type == "15" && (Sniff(cyr_id, "ѣ", 20) == false))
      {
        ablaut_clean_15(stem);
      } // Diels p.247-8 claims that only the PAPs PPPs and l-participles of this class have Reduktionsstufe, but enough counterexamples are given to make it impossible to tell which was Common Slavonic, so I accept both
      bool pisano = false;

      short int gender{int_morph_tag[5]}, gr_case{int_morph_tag[6]}, voice{int_morph_tag[4]}, strength{int_morph_tag[8]};

      int nom_map_row = 21 * gender + 7 * num + gr_case - 28;
      // mark long-form fucked adjectivals (here participles) as number 4 to exclude from search
      if (strength == 1)
      {
        if (nom_map_row == 6 || nom_map_row == 11 || nom_map_row == 13 || nom_map_row == 18 || nom_map_row == 19 || nom_map_row == 32 || nom_map_row == 34 || nom_map_row == 39 || nom_map_row == 40 || nom_map_row == 41 || nom_map_row == 48 || nom_map_row == 53 || nom_map_row == 55 || nom_map_row == 60 || nom_map_row == 61)
          lemma_ref.long_adj = 4;
        else
          lemma_ref.long_adj = 3;
      }
      // working out the participle row numbers in this way is retarded; the only thing that needs to be done here is distinguishing between 37 and 38 (because this is determined by nominal grammar); it really should all be moved to before the finite/participle split at the top of the function
      if (tense == 1)
      {
        if (voice == 1 && (nom_map_row == 1 || nom_map_row == 7 || nom_map_row == 43 || nom_map_row == 49)) // if NVsg nt or masc PRAP
        {
          row_no = 37;
        } // if active voice present participle non NVsg nt or masc
        else if (voice == 1)
        {
          row_no = 38;
        }
        else
        {
          row_no = 42;
        } // if passive voice present participle
      }
      else if (tense == 6)
      {
        if (voice == 1)
        {
          row_no = 39;
          if (conj_type == "40")
          {
            if (cyr_id.find("ів") != -1 || cyr_id.find("ꙇв") != -1 || cyr_id.find("ів") != -1)
            {
              outer_map_no++;
            }
          }
        }

        else
        {
          row_no = 41;
        }
      }
      else if (tense == 4)
      {
        row_no = 40;
      }
      else
      {
        lemma_ref.lemma_form = "corrupted morphology tag";
        return;
      }
      // check for present-stem in the past-participles of pьsati and žьdati
      if ((row_no == 39 || row_no == 40 || row_no == 41) && (conj_type == "51_abl" || conj_type == "52_abl") && (Sniff(cyr_id, "піс", 20) || Sniff(cyr_id, "жід", 20)))
      {
        pisano = true;
      }

      if ((conj_type == "infix_11") && (row_no == 37 || row_no == 38 || row_no == 42))
      {
        class_11_infix_clean(stem);
      }

      if ((conj_type == "have") && (row_no == 37 || row_no == 38))
      {
        if (Sniff(cyr_id, "ѣ", 20))
          outer_map_no++;
      }
      // jaxati PAP in *jaxav- rather than *jav-
      if (outer_map_no == 4001 && row_no == 39)
      {
        if (Sniff(cyr_id, "хав", 20))
          outer_map_no++;
      }

      // class 2.1 *nǫvъš- PAPs
      if (outer_map_no == 211 && (Sniff(cyr_id, "нѫв", 10) || Sniff(cyr_id, "нѹв", 10)))
      {
        outer_map_no++;
      }

      if ((conj_type == "iskati") && (row_no == 37 || row_no == 38 || row_no == 42) && (Sniff(cyr_id, "скѫ", 20) || Sniff(cyr_id, "ско", 20) || Sniff(cyr_id, "метѫ", 20) || Sniff(cyr_id, "мето", 20) || Sniff(cyr_id, "метѧ", 20) || Sniff(cyr_id, "метꙙ", 20)))
      {
        outer_map_no = outer_map_no - 10;
      }

      if ((conj_type == "51_abl" || conj_type == "52_abl" || conj_type == "53_abl") && (row_no == 42 || row_no == 37 || row_no == 38))
      {
        class_5_ablaut_clean(stem);
      }

      if (row_no == 37 && (nom_map_row == 43 || nom_map_row == 49))
      {
        if (Sniff(cyr_id, "шт", 4) == true)
        {
          outer_map_no++;
        }
      }
      if (conj_type == "17" && row_no == 41)
      {
        if (Sniff(cyr_id, "іт", 6) == true)
        {
          outer_map_no++;
        }
      }

      std::string ending = verb_[outer_map_no][row_no];

      /*     std::cout << "row_no: " << row_no << std::endl;
           std::cout << "outer_map_no: " << outer_map_no << std::endl;        */

      if (row_no == 37 || row_no == 38 || row_no == 39)
      {
        std::string participle_stem = stem + ending;

        if (conj_type == "31" || conj_type == "32")
        {
          class_3_clean(participle_stem);
        }

        if (outer_map_no % 10 == 1)
        {

          Participise(lemma_ref, participle_stem, strength, num, gender, gr_case, row_no, cyr_id, true);
          if (conj_type == "jьti")
          {
            iti_clean(participle_stem);
          }
          if (conj_type == "11" || conj_type == "14" || conj_type == "15" || conj_type == "iskati" || conj_type == "21")
          {
            class_I_clean(participle_stem, false);
          }
          lemma_ref.lemma_form = participle_stem;
        }
        else
        {

          Participise(lemma_ref, participle_stem, strength, num, gender, gr_case, row_no, cyr_id, true);
          if (conj_type == "jьti")
          {
            iti_clean(participle_stem);
          }
          if (conj_type == "11" || conj_type == "14" || conj_type == "15" || conj_type == "iskati" || conj_type == "21")
          {
            class_I_clean(participle_stem, false);
          }
          lemma_ref.lemma_form = participle_stem;
          ending = verb_[outer_map_no - 1][row_no];
          participle_stem = stem + ending;
          Participise(lemma_ref, participle_stem, strength, num, gender, gr_case, row_no, cyr_id, false);
          if (conj_type == "jьti")
          {
            iti_clean(participle_stem);
          }
          if (conj_type == "11" || conj_type == "14" || conj_type == "15" || conj_type == "iskati" || conj_type == "21")
          {
            class_I_clean(participle_stem, false);
          }
          lemma_ref.morph_replace = participle_stem;
        }
        return;
      }
      if (row_no == 40 || row_no == 41 || row_no == 42)
      {

        if (strength == 1)
        {
          std::string finished_participle = stem + ending + Nom_[121][nom_map_row] + Fetch_Article(nom_map_row);
          if (conj_type == "31" || conj_type == "32")
          {
            class_3_clean(finished_participle);
          }
          if (conj_type == "jьti")
          {
            iti_clean(finished_participle);
          }
          if (conj_type == "11" || conj_type == "14" || conj_type == "15" || conj_type == "21" || conj_type == "iskati")
          {
            class_I_clean(finished_participle, false);
          }
          if (conj_type == "12" || conj_type == "16")
          {
            if (row_no == 40 || row_no == 41)
              class_I_nasal_clean(finished_participle);
          }
          if (pisano == true)
          {
            lemma_ref.morph_replace = finished_participle;
            class_5_ablaut_clean(finished_participle);
          }
          lemma_ref.lemma_form = finished_participle;
        }

        else
        {

          std::string finished_participle = stem + ending + Nom_[121][nom_map_row];
          if (conj_type == "31" || conj_type == "32")
          {
            class_3_clean(finished_participle);
          }
          if (conj_type == "jьti")
          {
            iti_clean(finished_participle);
          }
          if (conj_type == "11" || conj_type == "14" || conj_type == "15" || conj_type == "21" || conj_type == "iskati")
          {
            class_I_clean(finished_participle, false);
          }
          if (conj_type == "12" || conj_type == "16")
          {
            if (row_no == 40 || row_no == 41)
              class_I_nasal_clean(finished_participle);
          }
          if (pisano == true)
          {
            lemma_ref.morph_replace = finished_participle;
            class_5_ablaut_clean(finished_participle);
          }
          lemma_ref.lemma_form = finished_participle;
        }

        return;
      }
      else
        lemma_ref.lemma_form = "corrupted morphology tag";
      return;
    }
    else
      lemma_ref.lemma_form = "corrupted morphology tag";
    return;
  }

  else
  {

    outer_map_no = outer_map_no * 100 + 1;
    if (tense == 8)
    {
      row_no = 3 * num + pers - 3;
    }
    else
    {
      row_no = 3 * num + pers + 6;
    }

    if (row_no == 18)
    {
      if (Sniff(cyr_id, "ъіш", 4))
        outer_map_no++;
      else if (Sniff(cyr_id, "біш", 4) || Sniff(cyr_id, "біш", 4) || Sniff(cyr_id, "бꙇш", 4))
        outer_map_no += 2;
    }
    if (row_no == 14 || row_no == 15 || row_no == 17)
    {
      if (Sniff(cyr_id, "ъіс", 5))
        outer_map_no++;
      else if (Sniff(cyr_id, "біс", 5) || Sniff(cyr_id, "біс", 5) || Sniff(cyr_id, "бꙇс", 5))
        outer_map_no += 2;
    }
    if (row_no == 10 || row_no == 13 || row_no == 16)
    {
      if (Sniff(cyr_id, "ъіх", 6))
        outer_map_no++;
      else if (Sniff(cyr_id, "біх", 6) || Sniff(cyr_id, "біх", 6) || Sniff(cyr_id, "бꙇх", 6))
        outer_map_no += 2;
    }
    if (row_no == 11 || row_no == 12)
    {
      if (Sniff(cyr_id, "бъі", 3))
        outer_map_no++;
    }

    if (row_no == 6 || row_no == 24)
    {
      if (Sniff(cyr_id, "та", 2))
        outer_map_no++;
    }

    std::string ending = verb_[outer_map_no][row_no];
    std::string flected_word = stem + ending;

    lemma_ref.lemma_form = flected_word;
    if (outer_map_no % 10 == 2)
    {
      lemma_ref.morph_replace = stem + verb_[outer_map_no - 1][row_no];
      lemma_ref.poss_doublet = stem + verb_[outer_map_no + 1][row_no];
    }
    if (outer_map_no % 10 == 3)
    {
      lemma_ref.morph_replace = stem + verb_[outer_map_no - 2][row_no];
    }
    return;
  }
}