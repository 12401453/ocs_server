/*void first_velar_clean(std::string &noun)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 11; letno++)
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
    case 10:
      str1.assign("xve"); //Supr. Vita of Kodratos has влъшве
      str2.assign("šve");
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
}*/
void first_velar_clean(std::string& flecter_output) {
  replaceAll(flecter_output, "ske", "šče");
  replaceAll(flecter_output, "xve", "šve");
  replaceAll(flecter_output, "ge", "že");
  replaceAll(flecter_output, "ke", "če");
  replaceAll(flecter_output, "xe", "še");
  replaceAll(flecter_output, "gi", "ži");
  replaceAll(flecter_output, "ki", "či");
  replaceAll(flecter_output, "xi", "ši");
  replaceAll(flecter_output, "gь", "žь");
  replaceAll(flecter_output, "kь", "čь");
  replaceAll(flecter_output, "xь", "šь");
}

void first_velar_vocative_clean(std::string& flecter_output) {
  replaceEnd(flecter_output, "ske", "šče");
  replaceEnd(flecter_output, "xve", "šve");
  replaceEnd(flecter_output, "ge", "že");
  replaceEnd(flecter_output, "ke", "če");
  replaceEnd(flecter_output, "xe", "še");
  replaceEnd(flecter_output, "gi", "ži");
  replaceEnd(flecter_output, "ki", "či");
  replaceEnd(flecter_output, "xi", "ši");
  replaceEnd(flecter_output, "gь", "žь");
  replaceEnd(flecter_output, "kь", "čь");
  replaceEnd(flecter_output, "xь", "šь");
}

/*void PV1_long_e(std::string &noun)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 3; letno++)
  {

    switch (letno)
    {

    case 0:
      str1.assign("gě");
      str2.assign("žǢ");
      break;
    case 1:
      str1.assign("kě");
      str2.assign("čǢ");
      break;
    case 2:
      str1.assign("xě");
      str2.assign("šǢ");
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
}*/
void PV1_long_e(std::string& flecter_output) {
  replaceAll(flecter_output, "gě", "žǢ");
  replaceAll(flecter_output, "kě", "čǢ");
  replaceAll(flecter_output, "xě", "šǢ");
}
