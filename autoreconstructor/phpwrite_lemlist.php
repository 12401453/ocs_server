<?php

$myfile = fopen("/home/joe/Programs/csv/conjugate/lemlist_2.h", "w") or die("Unable to open file!");

$servername = "localhost";
$username = "root";
$password = "Y8d0k2e7v9";
$dbname = "transliterate";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}

function conj_type_Trunc($conj_type) {

  if ($conj_type == "byti")
        return 4;
  if ($conj_type == "pref_byti")
        return 4;
  if ($conj_type == "nebyti")
        return 6;
  if ($conj_type == "dati")
        return 4;
  if ($conj_type == "have")
          return 6;
if ($conj_type == "eat")
        return 5;
  if ($conj_type == "pref_eat")
          return 4;
  if ($conj_type == "wote")
          return 6;
  if ($conj_type == "will")
          return 6;
  if ($conj_type == "stati")
          return 5;
  if ($conj_type == "dovleti")
          return 8;
  if ($conj_type == "dedj")
          return 4;
  if ($conj_type == "sleep")
          return 6;

  if ($conj_type == "čuti")
        return 2;
  if ($conj_type == "17")
        return 3;
  if ($conj_type == "18")
        return 3;
  if ($conj_type == "19")
        return 3;
  if ($conj_type == "jьti")
        return 4;
  if ($conj_type == "13")
        return 4;

  if ($conj_type == "21")
        return 4;
  if ($conj_type == "22")
        return 3;
  if ($conj_type == "ьt")
       return 4;
  if ($conj_type == "ьz")
       return 4;
  if ($conj_type == "uti")
       return 3;
  if ($conj_type == "rti")
       return 4;
  if ($conj_type == "rěsti")
        return 5;

  if ($conj_type == "31")
        return 3;
  if ($conj_type == "viděti")
        return 3;
  if ($conj_type == "jaxati")
        return 4;
  if ($conj_type == "32")
        return 3;
  if ($conj_type == "40")
        return 3;
  if ($conj_type == "51")
          return 3;
  if ($conj_type == "51_abl")
       return 3;
  if ($conj_type == "52_abl")
        return 3;
    if ($conj_type == "iskati")
          return 3;
  if ($conj_type == "52")
        return 3;
  if ($conj_type == "53")
        return 3;
  if ($conj_type == "53_abl")
        return 3;
  if ($conj_type == "54")
        return 2;
  if ($conj_type == "61")
        return 5;
  if ($conj_type == "62")
        return 5;


  if ($conj_type == "adj_soft")
        return 1;
  if ($conj_type == "adj_hard")
        return 1;
  if ($conj_type == "adj_soft_ord")
        return 1;
  if ($conj_type == "adj_hard_ord")
        return 1;
  if ($conj_type == "adj_ij")
        return 1;
  if ($conj_type == "masc_o")
      return 1;
  if ($conj_type == "masc_u")
      return 1;
  if ($conj_type == "masc_i")
      return 1;
  if ($conj_type == "masc_jo")
      return 1;
  if ($conj_type == "masc_jo_foreign")
      return 1;
  if ($conj_type == "masc_ju")
      return 1;
  if ($conj_type == "masc_o_u")
      return 1;
  if ($conj_type == "masc_a")
      return 1;
  if ($conj_type == "masc_ja")
      return 1;
  if ($conj_type == "masc_ji")
      return 1;
  if ($conj_type == "masc_N")
      return 1;
  if ($conj_type == "masc_tel")
      return 5;
  if ($conj_type == "masc_ar")
      return 1;
  if ($conj_type == "masc_o_PV3")
      return 1;
  if ($conj_type == "fem_a")
      return 1;
  if ($conj_type == "fem_a_PV3")
      return 1;
  if ($conj_type == "masc_a_PV3")
      return 1;
  if ($conj_type == "fem_ja")
      return 1;
  if ($conj_type == "fem_ji")
      return 1;
  if ($conj_type == "fem_R")
      return 1;
  if ($conj_type == "fem_uu")
      return 1;
  if ($conj_type == "fem_i")
      return 1;
  if ($conj_type == "tri")
      return 1;
      if ($conj_type == "nt_o")
          return 1;
      if ($conj_type == "nt_S")
          return 1;
      if ($conj_type == "nt_o_S")
          return 1;
      if ($conj_type == "four")
          return 1;
      if ($conj_type == "nt_jo")
          return 1;
      if ($conj_type == "nt_N")
          return 1;
      if ($conj_type == "nt_NT")
          return 1;
      if ($conj_type == "nt_o_PV3")
          return 1;
      if ($conj_type == "kamene")
          return 3;
      if ($conj_type == "oko")
                return 1;

          if ($conj_type == "den")
                return 4;   //extend to include all the $conj_types
          if ($conj_type == "masc_anin")
                return 4;
          if ($conj_type == "pron_soft")
                return 1;
          if ($conj_type == "pron_hard")
                return 1;
          if ($conj_type == "kъto")
                return 4;
          if ($conj_type == "kъtože")
                return 6;
          if ($conj_type == "čьto")
                return 4;
          if ($conj_type == "čьtože")
                return 6;
          if ($conj_type == "kъjь")
                return 3;
          if ($conj_type == "kъjьže")
                return 5;
          if ($conj_type == "vьxь")
                return 1;
          if ($conj_type == "sь")
                return 2;
          if ($conj_type == "jь")
                return 2;
          if ($conj_type == "jьže")
                return 4;
          if ($conj_type == "pron_hard_ђe")
                return 4;
          if ($conj_type == "pron_hard_že")
                return 3;
          if ($conj_type == "kъžьdo")
                return 5;
          if ($conj_type == "long_adj_noun")
                return 1;
          if ($conj_type == "azъ")
                return 3;
          if ($conj_type == "ty")
                return 2;
          if ($conj_type == "sę")
                return 2;
          if ($conj_type == "adj_ьj")
                return 1;
          if ($conj_type == "dъva")
                return 1;
          if ($conj_type == "adj_soft_comp")
              return 1;
          if ($conj_type == "ten")
              return 1;

    else        return 0;


}

function utf8_substr_replace($original, $replacement, $position, $length)
{
    $startString = mb_substr($original, 0, $position, "UTF-8");
    $endString = mb_substr($original, $position + $length, mb_strlen($original), "UTF-8");

    $out = $startString . $replacement . $endString;

    return $out;
}

function str_Truncate($str1, $lop_off) {
$end_pos = mb_strlen($str1);
$cut_pos = $end_pos - $lop_off;
$str_cut = utf8_substr_replace($str1, "", $cut_pos, $lop_off);
return $str_cut;
      }

$sql = "SET NAMES UTF8";
$res = $conn->query($sql);

$sql = "SELECT * FROM lemlist";
$result = $conn->query($sql);

$intro_line = 'std::set<Lemma> lemma_list = {';
$_nl = "\n";
$open_brace = '{';
$close_brace = '}';
$quote = '"';
$close_line = '};';
$comma = ',';
$sing_quote = "'";



fwrite($myfile, $intro_line);
fwrite($myfile, $_nl);

if ($result->num_rows > 0) {

  while($row = $result->fetch_assoc()) {

    $lem_id = $row["lem_id"];
    $cs_lemma = $row["cs_lemma"];
    $morph_replace = $row["morph_replace"];
    $second_velar = $row["second_velar"];
    $poss_doublet = $row["poss_doublet"];
    $loan_place = $row["loan_place"];
    $long_adj = $row["long_adj"];
    $non_assim = $row["non_assim"];
    $eng_trans = $row["eng_trans"];
    $pre_jot = $row["pre_jot"];
    $etym_disc = $row["etym_disc"];
    $bad_etym = $row["bad_etym"];
    $loan_source = $row["loan_source"];
    $conj_type = $row["conj_type"];
    $root_1 = $row["root_1"];
    $root_2 = $row["root_2"];
    $noun_verb = $row["noun_verb"];


if ($noun_verb != 99) {
    fwrite($myfile, $open_brace);
    fwrite($myfile, $lem_id);
    fwrite($myfile, $comma);

    fwrite($myfile, $quote);

    if ($conj_type == 11 OR $conj_type == 12 OR $conj_type == 15 OR $conj_type == 16 OR $conj_type == "infix_11" OR $conj_type == "infix_12") {
    fwrite($myfile, $root_2);  }
    else if ($conj_type == 14) {
    fwrite($myfile, $root_1);  }
    else if ($pre_jot == "") {
    fwrite($myfile, str_Truncate($cs_lemma, conj_type_Trunc($conj_type))); }
    else {fwrite($myfile, str_Truncate($pre_jot, conj_type_Trunc($conj_type)));}
    fwrite($myfile, $quote);
    fwrite($myfile, $comma);

    fwrite($myfile, $quote);
    fwrite($myfile, $morph_replace);
    fwrite($myfile, $quote);
    fwrite($myfile, $comma);

    fwrite($myfile, $quote);
    fwrite($myfile, $second_velar);
    fwrite($myfile, $quote);
    fwrite($myfile, $comma);

    fwrite($myfile, $quote);
    fwrite($myfile, $poss_doublet);
    fwrite($myfile, $quote);
    fwrite($myfile, $comma);

    fwrite($myfile, $loan_place);
    fwrite($myfile, $comma);

    fwrite($myfile, $long_adj);
    fwrite($myfile, $comma);

    if ($non_assim == "W")  { fwrite($myfile, "true"); }
    else { fwrite($myfile,"false"); }
      fwrite($myfile, $comma);

    fwrite($myfile, $quote);
    fwrite($myfile, $eng_trans);
    fwrite($myfile, $quote);
    fwrite($myfile, $comma);

    fwrite($myfile, $quote);
  //  fwrite($myfile, $pre_jot);
    fwrite($myfile, $quote);
    fwrite($myfile, $comma);

    fwrite($myfile, $quote);
    fwrite($myfile, $etym_disc);
    fwrite($myfile, $quote);
    fwrite($myfile, $comma);

  if ($bad_etym == 1)  { fwrite($myfile, "true"); }
  else { fwrite($myfile,"false"); }
    fwrite($myfile, $comma);

    fwrite($myfile, $quote);
    fwrite($myfile, $loan_source);
    fwrite($myfile, $quote);
    fwrite($myfile, $comma);

    fwrite($myfile, $quote);
    fwrite($myfile, $conj_type);
    fwrite($myfile, $quote);
    fwrite($myfile, $comma);

    fwrite($myfile, $noun_verb);



    fwrite($myfile, $close_brace);
    fwrite($myfile, $comma);
    fwrite($myfile, $_nl);
     }
  }
} else {
  echo "0 results";
}
   fwrite($myfile, '};');


$conn->close();

?>
