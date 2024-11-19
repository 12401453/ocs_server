void Participise(Lemma &lemma_ref, std::string &participle_stem, short int strength, short int num, short int gender, short int gr_case, short int aspect, std::string cyr_id, bool sniff_on)
{
  int outer_map_no = 0;
  if (aspect == 39)
  {
    outer_map_no = 111;
  }
  int row_no = 21 * gender + 7 * num + gr_case - 28; // formula to get row number of all nominals
  if (row_no < 1)
  {
    participle_stem = "corrupted morphology tag";
    return;
  }

  if (aspect == 37 || aspect == 38)
  {
    outer_map_no = 101;
    if (row_no == 15 || row_no == 21)
    {
      if (Sniff(cyr_id, "шті", 4) == true)
      {
        if (sniff_on == true)
          outer_map_no++;
      }
    }
    if (row_no == 43 || row_no == 49)
    {
      if (Sniff(cyr_id, "ште", 4) == true)
      {
        if (sniff_on == true)
          outer_map_no++;
      }
    }
    if (row_no == 22)
    {
      if (Sniff(cyr_id, "шта", 3))
      {
        if (sniff_on == true)
          outer_map_no++;
      }
    }
  }
  if (aspect == 39)
  {
    if (row_no == 43 || row_no == 49)
    {
      if (Sniff(cyr_id, "ше", 3))
      {
        if (sniff_on == true)
          outer_map_no++;
      }
    }
    if (row_no == 15 || row_no == 21)
    {
      if (Sniff(cyr_id, "ші", 3))
      {
        if (sniff_on == true)
          outer_map_no++;
      }
    }
  }

  std::string article = Fetch_Article(row_no);

  participle_stem += Nom_[outer_map_no][row_no];

  if (strength == 1)
  {
    participle_stem += article;
  }

  return;
}