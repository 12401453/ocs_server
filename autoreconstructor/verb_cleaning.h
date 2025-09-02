
/*void class_I_clean(std::string &verb, bool imperative)
{
  if (imperative == true)
  {

    std::string str1, str2;
    int x, y;
    for (int letno = 0; letno < 8; letno++)
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
      case 7:
        str1.assign("pn"); //imperatives of top-nǫti verbs should still be cleaned
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

  std::string str1, str2;
  int x, y;

  //prevent nasalised-jat from being replaced with Ǣ after palatals
  str1.assign("ę̌");
  str2.assign("Z");
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

  for (int letno = -2; letno < 44; letno++)
  {

    switch (letno)
    {
    case -2:
      str1.assign("ogsę");
      str2.assign("ašę");
      break;
    case -1:
      str1.assign("oksę");
      str2.assign("ašę");
      break;
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
      str1.assign("vsę");
      str2.assign("šę");
      break;
    case 15:
      str1.assign("gst");
      str2.assign("st");
      break;
    case 16:
      str1.assign("kst");
      str2.assign("st");
      break;
    case 17:
      str1.assign("vst");
      str2.assign("st");
      break;
    case 18:
      str1.assign("skj");
      str2.assign("šč");
      break;
    case 19:
      str1.assign("ks");
      str2.assign("x");
      break;
    case 20:
      str1.assign("gs");
      str2.assign("x");
      break;
    case 21:
      str1.assign("bt");
      str2.assign("t");
      break;
    case 22:
      str1.assign("pt");
      str2.assign("t");
      break;
    case 23:
      str1.assign("dt");
      str2.assign("st");
      break;
    case 24:
      str1.assign("tt");
      str2.assign("st");
      break;
    case 25:
      str1.assign("zt");
      str2.assign("st");
      break;
    case 26:
      str1.assign("gě");
      str2.assign("žǢ");
      break;
    case 27:
      str1.assign("xě");
      str2.assign("šǢ");
      break;
    case 28:
      str1.assign("kě");
      str2.assign("čǢ");
      break;
    case 29:
      str1.assign("ge");
      str2.assign("že");
      break;
    case 30:
      str1.assign("xe");
      str2.assign("še");
      break;

    case 31:
      str1.assign("ke");
      str2.assign("če");
      break;
    case 32:
      str1.assign("žě");
      str2.assign("žǢ");
      break;
    case 33:
      str1.assign("šě");
      str2.assign("šǢ");
      break;

    case 34:
      str1.assign("čě");
      str2.assign("čǢ");
      break;
    case 35:
      str1.assign("jě");
      str2.assign("jǢ");
      break;
    case 36:
      str1.assign("vt");
      str2.assign("t");
      break;
    case 37:
      str1.assign("vs");
      str2.assign("x");
      break;
    case 38:
      str1.assign("zs");
      str2.assign("s");
      break;
    case 39:
      str1.assign("ts");
      str2.assign("s");
      break;
    case 40:
      str1.assign("ds");
      str2.assign("s");
      break;
    case 41:
      str1.assign("bs");
      str2.assign("s");
      break;
    case 42:
      str1.assign("ps");
      str2.assign("s");
      break;
    case 43:
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

  str1.assign("Z");
  str2.assign("ę̌");
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

  return;
} */

void class_I_clean(std::string& flected_form, bool imperative) {
  replaceAll(flected_form, "pn", "n"); //class 2.1 -nǫti verbs  

  if(imperative) {
      replaceAll(flected_form, "eki", "ьki");
      replaceAll(flected_form, "ekě", "ьkě");
      replaceAll(flected_form, "egi", "ьgi");
      replaceAll(flected_form, "egě", "ьgě");
      replaceAll(flected_form, "exi", "ьxi");
      replaceAll(flected_form, "exě", "ьxě");
      replaceAll(flected_form, "skj", "šč");

      //Old Russian has present-participle forms of решти with zero-grade рькуштий etc., but unsure whether this is Russian-specific (or caused by Bytovaja Orfografija etc.)
      return;
  }

  replaceAll(flected_form, "ę̌", "Z"); //prevent nasalised-jat from being replaced with Ǣ after palatals

  replaceAll(flected_form, "eksę", "ěšę");
  replaceAll(flected_form, "ekst", "ěst");
  replaceAll(flected_form, "egsę", "ěšę");
  replaceAll(flected_form, "ogsę", "ašę");
  replaceAll(flected_form, "oksę", "ašę");
  replaceAll(flected_form, "egst", "ěst");
  replaceAll(flected_form, "ebs", "ěbs");
  replaceAll(flected_form, "eks", "ěx");
  replaceAll(flected_form, "ods", "as");
  replaceAll(flected_form, "ogs", "ax");
  replaceAll(flected_form, "egs", "ěx");
  replaceAll(flected_form, "eds", "ěs");
  replaceAll(flected_form, "ezs", "ěs");
  replaceAll(flected_form, "ess", "ěs");
  replaceAll(flected_form, "ęss", "ęs"); //class 11 sъtręs-ti when it adds S-aorist endings; a check for S-aorists needs adding to the autoreconstructor because Mar. Matt 28 сътрѧсѧ is wrongly reconstructed as the root-aorist *sъtręsǫ
  replaceAll(flected_form, "eps", "ěs");
  replaceAll(flected_form, "ksę", "šę");
  replaceAll(flected_form, "gsę", "šę");
  replaceAll(flected_form, "vsę", "šę"); //I'm assuming that the /v/ in the stem of жити was a RUKI-sound
  replaceAll(flected_form, "gst", "st");
  replaceAll(flected_form, "kst", "st");
  replaceAll(flected_form, "vst", "st");  //I'm assuming that the /v/ in the stem of жити was a RUKI-sound
  replaceAll(flected_form, "skj", "šč");
  replaceAll(flected_form, "ks", "x");
  replaceAll(flected_form, "gs", "x");
  replaceAll(flected_form, "bt", "t");
  replaceAll(flected_form, "pt", "t");
  replaceAll(flected_form, "dt", "st");
  replaceAll(flected_form, "tt", "st");
  replaceAll(flected_form, "zt", "st");
  replaceAll(flected_form, "gě", "žǢ");
  replaceAll(flected_form, "xě", "šǢ");
  replaceAll(flected_form, "kě", "čǢ");
  replaceAll(flected_form, "ge", "že");
  replaceAll(flected_form, "xe", "še");
  replaceAll(flected_form, "ke", "če");
  replaceAll(flected_form, "žě", "žǢ");
  replaceAll(flected_form, "šě", "šǢ");
  replaceAll(flected_form, "čě", "čǢ");
  replaceAll(flected_form, "jě", "jǢ");
  replaceAll(flected_form, "vt", "t");
  replaceAll(flected_form, "vs", "x"); //I'm assuming that the /v/ in the stem of жити was a RUKI-sound
  replaceAll(flected_form, "zs", "s");
  replaceAll(flected_form, "ts", "s");
  replaceAll(flected_form, "ds", "s");
  replaceAll(flected_form, "bs", "s");
  replaceAll(flected_form, "ps", "s");
  //pn -> n moved before the imperative-check
  replaceAll(flected_form, "sč", "šč");
  replaceAll(flected_form, "vl", "l"); //only needed for past. part. of žiti and possibly will mess up other things

  replaceAll(flected_form, "Z", "ę̌");
}

/*void class_I_nasal_clean(std::string &verb)
{

  std::string str1, str2;
  int x, y;
  for (int letno = 0; letno < 12; letno++)
  {
    //need to add versions with ъm- to account for *dǫti (already added to LcsFlecter.cpp)
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
}*/
void class_I_nasal_clean(std::string& flecter_output) {
  replaceAll(flecter_output, "ьnstъ", "ętъ");
  replaceAll(flecter_output, "ьmstъ", "ętъ");
  replaceAll(flecter_output, "ъmstъ", "ǫtъ");
  replaceAll(flecter_output, "ьns", "ęs");
  replaceAll(flecter_output, "ьms", "ęs");
  replaceAll(flecter_output, "ъms", "ǫs");
  replaceAll(flecter_output, "ьnx", "ęx");
  replaceAll(flecter_output, "ьmx", "ęx");
  replaceAll(flecter_output, "ъmx", "ǫx");
  replaceAll(flecter_output, "ьnš", "ęš");
  replaceAll(flecter_output, "ьmš", "ęš");
  replaceAll(flecter_output, "ъmš", "ǫš");
  replaceAll(flecter_output, "ьml", "ęl");
  replaceAll(flecter_output, "ьnl", "ęl");
  replaceAll(flecter_output, "ъml", "ǫl");
  replaceAll(flecter_output, "ьmt", "ęt");
  replaceAll(flecter_output, "ьnt", "ęt");
  replaceAll(flecter_output, "ъmt", "ǫt");
  replaceEnd(flecter_output, "ьm", "ę");
  replaceEnd(flecter_output, "ьn", "ę");
  replaceEnd(flecter_output, "ъm", "ǫ");
}

/*void iti_clean(std::string &verb)
{

  std::string str1, str2;
  int x, y;
  for (int letno = -1; letno < 8; letno++)
  {

    switch (letno)
    {
    case -1:
      str1.assign("ǫtj");
      str2.assign("ǫћ");
      break;
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
}*/
void iti_clean(std::string& flecter_output) {
  replaceAll(flecter_output, "ǫtj", "ǫћ");
  replaceAll(flecter_output, "njь", "ni");
  replaceAll(flecter_output, "zjь", "zi");
  replaceAll(flecter_output, "tjь", "ti");
  replaceAll(flecter_output, "bjь", "bi");
  replaceAll(flecter_output, "tš", "š"); //very often replaced with tъš
  replaceAll(flecter_output, "+š", "š");
  replaceAll(flecter_output, "nš", "š");
  replaceAll(flecter_output, "zš", "sš"); //not strictly speaking correct but better for searching
}

/*void ablaut_clean_14(std::string &verb)
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
}*/
void ablaut_clean_14(std::string& flecter_output) {
  replaceAll(flecter_output, "ĺ̥", "el");
  replaceAll(flecter_output, "ŕ̥", "er");
}

/*void ablaut_clean_15(std::string &verb)
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
}*/
void ablaut_clean_15(std::string& flecter_output) {
  replaceAll(flecter_output, "el", "ĺ̥");
  replaceAll(flecter_output, "er", "ŕ̥");
}

/*void class_3_clean(std::string &verb)
{

  std::string str1, str2;
  int x, y;

  str1.assign("ę̌");
  str2.assign("Z");
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



  for (int letno = 0; letno < 4; letno++)
  {

    switch (letno)
    {
    case 0:
      str1.assign("žě");
      str2.assign("žǢ");
      break;
    case 1:
      str1.assign("šě");
      str2.assign("šǢ");
      break;
    case 2:
      str1.assign("čě");
      str2.assign("čǢ");
      break;
    case 3:
      str1.assign("jě");
      str2.assign("jǢ");
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

  str1.assign("Z");
  str2.assign("ę̌");
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

  return;
}*/
void class_3_clean(std::string& flecter_output) {
  replaceAll(flecter_output, "ę̌", "Z"); //prevent nasalised-jat from being replaced with Ǣ after palatals

  replaceAll(flecter_output, "žě", "žǢ");
  replaceAll(flecter_output, "šě", "šǢ");
  replaceAll(flecter_output, "čě", "čǢ");
  replaceAll(flecter_output, "jě", "jǢ");

  replaceAll(flecter_output, "Z",  "ę̌");
}

/*void class_5_ablaut_clean(std::string &verb)
{

  std::string str1, str2;
  int x, y;
  for (int letno = -1; letno < 15; letno++)
  {

    switch (letno)
    {
    case -1:
      str1.assign("pĺ̥z");
      str2.assign("pelz");
      break;
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
}*/
void class_5_ablaut_clean(std::string& flecter_output) {
  replaceAll(flecter_output, "strъg", "strug");
  replaceAll(flecter_output, "pĺ̥z", "pelz");
  replaceAll(flecter_output, "stьl", "stel");
  replaceAll(flecter_output, "ĺьv", "ĺu");
  replaceAll(flecter_output, "zьd", "zid");
  replaceAll(flecter_output, "žьd", "žid");
  replaceAll(flecter_output, "pьs", "pis");
  replaceAll(flecter_output, "jьm", "jem");
  replaceAll(flecter_output, "nьm", "nem");
  replaceAll(flecter_output, "tьm", "tem");
  replaceAll(flecter_output, "bьm", "bem");
  replaceAll(flecter_output, "zьm", "zem");
  replaceAll(flecter_output, "gъn", "žen");
  replaceAll(flecter_output, "ъv", "ov");
  replaceAll(flecter_output, "ьr", "er");
  replaceAll(flecter_output, "ьj", "ěj");
}

/*void class_11_infix_clean(std::string &verb)
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
}*/
void class_11_infix_clean(std::string& flecter_output) {
  replaceAll(flecter_output, "leg", "lęg");
  replaceAll(flecter_output, "sěd", "sęd");
}

/*void class_12_infix_clean(std::string &verb)
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
}*/
void class_12_infix_clean(std::string& flecter_output) {
  replaceAll(flecter_output, "rět", "ręt");
}

/*void imperf_sheta(std::string &verb)
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
}*/
void imperf_sheta(std::string& flecter_output) {
  replaceAll(flecter_output, "šeta", "sta");
  replaceAll(flecter_output, "šete", "ste");
}
