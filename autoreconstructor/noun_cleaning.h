void first_velar_clean(std::string &noun)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 10; letno++)
  {

    switch (letno)
    {
    case 0:
      str1.assign("ske");
      str2.assign("šče");
      break;
    case 1:
      str1.assign("ge");
      str2.assign("že");
      break;
    case 2:
      str1.assign("ke");
      str2.assign("če");
      break;
    case 3:
      str1.assign("xe");
      str2.assign("še");
      break;
    case 4:
      str1.assign("gi");
      str2.assign("ži");
      break;
    case 5:
      str1.assign("ki");
      str2.assign("či");
      break;
    case 6:
      str1.assign("xi");
      str2.assign("ši");
      break;
    case 7:
      str1.assign("gь");
      str2.assign("žь");
      break;
    case 8:
      str1.assign("kь");
      str2.assign("čь");
      break;
    case 9:
      str1.assign("xь");
      str2.assign("šь");
      break;
    }

    do
    {
      x = noun.find(str1);
      if (x == -1)
      {
        break;
      }
      y = str1.length();
      noun.replace(x, y, str2);
    } while (x != -1);
  }
  return;
}

void PV1_long_e(std::string &noun)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 3; letno++)
  {

    switch (letno)
    {

    case 0:
      str1.assign("gě");
      str2.assign("žǞ");
      break;
    case 1:
      str1.assign("kě");
      str2.assign("čǞ");
      break;
    case 2:
      str1.assign("xě");
      str2.assign("šǞ");
      break;
    }

    do
    {
      x = noun.find(str1);
      if (x == -1)
      {
        break;
      }
      y = str1.length();
      noun.replace(x, y, str2);
    } while (x != -1);
  }
  return;
}
