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
  replaceAll(flected_form, "ogst", "ast");
  replaceAll(flected_form, "ebs", "ěbs");
  replaceAll(flected_form, "eks", "ěx");
  replaceAll(flected_form, "ods", "as");
  replaceAll(flected_form, "ogs", "ax");
  replaceAll(flected_form, "egs", "ěx");
  replaceAll(flected_form, "eds", "ěs");
  replaceAll(flected_form, "ets", "ěs");
  replaceAll(flected_form, "ezs", "ěs");
  replaceAll(flected_form, "ess", "ěs");
  replaceAll(flected_form, "ęss", "ęs"); //class 11 sъtręs-ti when it adds S-aorist endings; a check for S-aorists needs adding to the autoreconstructor because Mar. Matt 28 сътрѧсѧ is wrongly reconstructed as the root-aorist *sъtręsǫ
  replaceAll(flected_form, "eps", "ěs");
  replaceAll(flected_form, "ksę", "šę");
  replaceAll(flected_form, "gsę", "šę");
  replaceAll(flected_form, "vsę", "šę"); //I'm assuming that the /v/ in the stem of жити was a RUKI-sound
  replaceAll(flected_form, "gst", "st");
  replaceAll(flected_form, "kst", "st");
  replaceAll(flected_form, "vst", "st");
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
  replaceAll(flecter_output, "ьnv", "ęv");
  replaceAll(flecter_output, "ьmv", "ęv");
  replaceEnd(flecter_output, "ьm", "ę");
  replaceEnd(flecter_output, "ьn", "ę");
  replaceEnd(flecter_output, "ъm", "ǫ");
}

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

void ablaut_clean_14(std::string& flecter_output) {
  replaceAll(flecter_output, "ĺ̥", "el");
  replaceAll(flecter_output, "ŕ̥", "er");
}

void ablaut_clean_15(std::string& flecter_output) {
  replaceAll(flecter_output, "el", "ĺ̥");
  replaceAll(flecter_output, "er", "ŕ̥");
}

void class_3_clean(std::string& flecter_output) {
  replaceAll(flecter_output, "ę̌", "Z"); //prevent nasalised-jat from being replaced with Ǣ after palatals

  replaceAll(flecter_output, "žě", "žǢ");
  replaceAll(flecter_output, "šě", "šǢ");
  replaceAll(flecter_output, "čě", "čǢ");
  replaceAll(flecter_output, "jě", "jǢ");

  replaceAll(flecter_output, "Z",  "ę̌");
}

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

void class_11_infix_clean(std::string& flecter_output) {
  replaceAll(flecter_output, "leg", "lęg");
  replaceAll(flecter_output, "sěd", "sęd");
}

void class_12_infix_clean(std::string& flecter_output) {
  replaceAll(flecter_output, "rět", "ręt");
}

void imperf_sheta(std::string& flecter_output) {
  replaceAll(flecter_output, "šeta", "sta");
  replaceAll(flecter_output, "šete", "ste");
}
