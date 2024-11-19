
void class_I_clean(std::string &verb, bool imperative)
{
  if (imperative == true)
  {

    std::string str1, str2;
    int x, y;
    for (int letno = 0; letno < 7; letno++)
    {

      switch (letno)
      {
      case 0:
        str1.assign("eki");
        str2.assign("ьki");
        break;
      case 1:
        str1.assign("ekě");
        str2.assign("ьkě");
        break;
      case 2:
        str1.assign("egi");
        str2.assign("ьgi");
        break;
      case 3:
        str1.assign("egě");
        str2.assign("ьgě");
        break;
      case 4:
        str1.assign("exi");
        str2.assign("ьxi");
        break;
      case 5:
        str1.assign("exě");
        str2.assign("ьxě");
        break;
      case 6:
        str1.assign("skj");
        str2.assign("šč");
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

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 41; letno++)
  {

    switch (letno)
    {
    case 0:
      str1.assign("eksę");
      str2.assign("ěšę");
      break;
    case 1:
      str1.assign("ekst");
      str2.assign("ěst");
      break;
    case 2:
      str1.assign("egsę");
      str2.assign("ěšę");
      break;
    case 3:
      str1.assign("egst");
      str2.assign("ěst");
      break;
    case 4:
      str1.assign("ebs");
      str2.assign("ěs");
      break;
    case 5:
      str1.assign("eks");
      str2.assign("ěx");
      break;
    case 6:
      str1.assign("ods");
      str2.assign("as");
      break;
    case 7:
      str1.assign("ogs");
      str2.assign("ax");
      break;
    case 8:
      str1.assign("egs");
      str2.assign("ěx");
      break;
    case 9:
      str1.assign("eds");
      str2.assign("ěs");
      break;
    case 10:
      str1.assign("ess");
      str2.assign("ěs");
      break;
    case 11:
      str1.assign("eps");
      str2.assign("ěs");
      break;
    case 12:
      str1.assign("ksę");
      str2.assign("šę");
      break;

    case 13:
      str1.assign("gsę");
      str2.assign("šę");
      break;
    case 14:
      str1.assign("gst");
      str2.assign("st");
      break;
    case 15:
      str1.assign("kst");
      str2.assign("st");
      break;
    case 16:
      str1.assign("skj");
      str2.assign("šč");
      break;
    case 17:
      str1.assign("ks");
      str2.assign("x");
      break;
    case 18:
      str1.assign("gs");
      str2.assign("x");
      break;
    case 19:
      str1.assign("bt");
      str2.assign("t");
      break;
    case 20:
      str1.assign("pt");
      str2.assign("t");
      break;
    case 21:
      str1.assign("dt");
      str2.assign("st");
      break;
    case 22:
      str1.assign("tt");
      str2.assign("st");
      break;
    case 23:
      str1.assign("zt");
      str2.assign("st");
      break;
    case 24:
      str1.assign("gě");
      str2.assign("žǞ");
      break;
    case 25:
      str1.assign("xě");
      str2.assign("šǞ");
      break;
    case 26:
      str1.assign("kě");
      str2.assign("čǞ");
      break;
    case 27:
      str1.assign("ge");
      str2.assign("že");
      break;
    case 28:
      str1.assign("xe");
      str2.assign("še");
      break;

    case 29:
      str1.assign("ke");
      str2.assign("če");
      break;
    case 30:
      str1.assign("žě");
      str2.assign("žǞ");
      break;
    case 31:
      str1.assign("šě");
      str2.assign("šǞ");
      break;

    case 32:
      str1.assign("čě");
      str2.assign("čǞ");
      break;
    case 33:
      str1.assign("jě");
      str2.assign("jǞ");
      break;
    case 34:
      str1.assign("vt");
      str2.assign("t");
      break;
    case 35:
      str1.assign("zs");
      str2.assign("s");
      break;
    case 36:
      str1.assign("ts");
      str2.assign("s");
      break;
    case 37:
      str1.assign("ds");
      str2.assign("s");
      break;
    case 38:
      str1.assign("bs");
      str2.assign("s");
      break;
    case 39:
      str1.assign("ps");
      str2.assign("s");
      break;
    case 40:
      str1.assign("pn");
      str2.assign("n");
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

void class_I_nasal_clean(std::string &verb)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 12; letno++)
  {

    switch (letno)
    {
    case 0:
      str1.assign("ьnstъ");
      str2.assign("ętъ");
      break;
    case 1:
      str1.assign("ьmstъ");
      str2.assign("ętъ");
      break;
    case 2:
      str1.assign("ьns");
      str2.assign("ęs");
      break;
    case 3:
      str1.assign("ьms");
      str2.assign("ęs");
      break;
    case 4:
      str1.assign("ьnx");
      str2.assign("ęx");
      break;
    case 5:
      str1.assign("ьmx");
      str2.assign("ęx");
      break;
    case 6:
      str1.assign("ьnš");
      str2.assign("ęš");
      break;
    case 7:
      str1.assign("ьmš");
      str2.assign("ęš");
      break;
    case 8:
      str1.assign("ьml");
      str2.assign("ęl");
      break;
    case 9:
      str1.assign("ьnl");
      str2.assign("ęl");
      break;
    case 10:
      str1.assign("ьm");
      str2.assign("ę");
      break;
    case 11:
      str1.assign("ьn");
      str2.assign("ę");
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

void iti_clean(std::string &verb)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 8; letno++)
  {

    switch (letno)
    {
    case 0:
      str1.assign("njь");
      str2.assign("ni");
      break;
    case 1:
      str1.assign("zjь");
      str2.assign("zi");
      break;
    case 2:
      str1.assign("tjь");
      str2.assign("ti");
      break;
    case 3:
      str1.assign("bjь");
      str2.assign("bi");
      break;
    case 4:
      str1.assign("tš");
      str2.assign("tъš");
      break;
    case 5:
      str1.assign("+š");
      str2.assign("š");
      break;
    case 6:
      str1.assign("nš");
      str2.assign("š");
      break;
    case 7:
      str1.assign("zš");
      str2.assign("sš");
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

void ablaut_clean_14(std::string &verb)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 2; letno++)
  {

    switch (letno)
    {
    case 0:
      str1.assign("ĺ̥");
      str2.assign("el");
      break;
    case 1:
      str1.assign("ŕ̥");
      str2.assign("er");
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

void ablaut_clean_15(std::string &verb)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 2; letno++)
  {

    switch (letno)
    {
    case 0:
      str1.assign("el");
      str2.assign("ĺ̥");
      break;
    case 1:
      str1.assign("er");
      str2.assign("ŕ̥");
      break;
      /*          case 2:
                    str1.assign("tjь");
                    str2.assign("ti");
                    break;
                case 3:
                   str1.assign("tš");
                   str2.assign("tъš");
                   break;
                case 4:
                   str1.assign("+š");
                   str2.assign("š");
                   break;
                case 5:
                   str1.assign("nš");
                   str2.assign("š");
                   break;                  */
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

void class_3_clean(std::string &verb)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 4; letno++)
  {

    switch (letno)
    {
    case 0:
      str1.assign("žě");
      str2.assign("žǞ");
      break;
    case 1:
      str1.assign("šě");
      str2.assign("šǞ");
      break;
    case 2:
      str1.assign("čě");
      str2.assign("čǞ");
      break;
    case 3:
      str1.assign("jě");
      str2.assign("jǞ");
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

void class_5_ablaut_clean(std::string &verb)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 15; letno++)
  {

    switch (letno)
    {

    case 0:
      str1.assign("strъg");
      str2.assign("strug");
      break;
    case 1:
      str1.assign("stьl");
      str2.assign("stel");
      break;
    case 2:
      str1.assign("ĺьv");
      str2.assign("ĺu");
      break;
    case 3:
      str1.assign("zьd");
      str2.assign("zid");
      break;
    case 4:
      str1.assign("žьd");
      str2.assign("žid");
      break;
    case 5:
      str1.assign("pьs");
      str2.assign("pis");
      break;
    case 6:
      str1.assign("jьm");
      str2.assign("jem");
      break;
    case 7:
      str1.assign("nьm");
      str2.assign("nem");
      break;

    case 8:
      str1.assign("tьm");
      str2.assign("tem");
      break;
    case 9:
      str1.assign("bьm");
      str2.assign("bem");
      break;
    case 10:
      str1.assign("zьm");
      str2.assign("zem");
      break;
    case 11:
      str1.assign("gъn");
      str2.assign("žen");
      break;
    case 12:
      str1.assign("ъv");
      str2.assign("ov");
      break;
    case 13:
      str1.assign("ьr");
      str2.assign("er");
      break;
    case 14:
      str1.assign("ьj");
      str2.assign("ěj");
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

void class_11_infix_clean(std::string &verb)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 2; letno++)
  {

    switch (letno)
    {

    case 0:
      str1.assign("leg");
      str2.assign("lęg");
      break;
    case 1:
      str1.assign("sěd");
      str2.assign("sęd");
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

void class_12_infix_clean(std::string &verb)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 1; letno++)
  {

    switch (letno)
    {

    case 0:
      str1.assign("rět");
      str2.assign("ręt");
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

void imperf_sheta(std::string &verb)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 2; letno++)
  {

    switch (letno)
    {

    case 0:
      str1.assign("šeta");
      str2.assign("sta");
      break;
    case 1:
      str1.assign("šete");
      str2.assign("ste");
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
