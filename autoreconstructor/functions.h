bool Sniff(std::string cyr_id, std::string substr, short int substr_length)
{
  substr_length = substr_length * 2;
  if ((substr_length + 3) >= cyr_id.length())
  {
    if (cyr_id.find(substr) == -1)
    {
      return false;
    }

    else
    {
      return true;
    }
  }
  else
  {
    int end_pos = cyr_id.length();
    int offset_pos = 0;
    if (end_pos % 2 == 1)
    {
      offset_pos = end_pos - substr_length - 3;
    }
    else
    {
      offset_pos = end_pos - substr_length - 2;
    }
    std::string sniffstack = cyr_id.replace(0, offset_pos, "");

    if (sniffstack.find(substr) == -1)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
}

std::string Fetch_Article(int row_no)
{
  int outer_map_no = 51;
  std::string article = Nom_[outer_map_no][row_no];
  return article;
}

void pisanie_clean(std::string &verb)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 1; letno++)
  {

    switch (letno)
    {

    case 0:
      str1.assign("pьs");
      str2.assign("pis");
      break;
    }

    do
    {
      x = verb.find(str1);
      if (x == -1)
      {
        break;
      }
      y = str1.length();
      verb.replace(x, y, str2);
    } while (x != -1);
  }

  return;
}
