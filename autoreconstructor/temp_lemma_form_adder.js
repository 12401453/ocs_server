#!/usr/bin/node

const fs = require('node:fs');
const readline = require('readline');

const lemmas_map = new Map();

let csv_str = "torot_word|pos|morph_tag|deep_cleaned|lemma_id|sentence_no|pres_before|pres_after|text_id|subtitle_id|autotagged|lemma\n";

async function buildMaps() {  
  for await(const line of readline.createInterface({input: fs.createReadStream("chu_lemmas.csv")})) {
    const row = line.split(",");

    const lemma_id = row[0];
    const lemma = row[1];

    lemmas_map.set(lemma_id, lemma);
  }
  for await (const line of readline.createInterface({input: fs.createReadStream("chu_words_full_with_titles_untagged_autotags_added_with_assem.csv")})) {
    const row = line.split("|");

    const lemma_id = row[4];

    let lemma = "";
    if(lemmas_map.has(lemma_id)) lemma = lemmas_map.get(lemma_id);

    csv_str += line + "|" + lemma + "\n";


  }

  fs.writeFileSync("chu_words_full_with_titles_untagged_autotags_added_with_assem_NEW.csv", csv_str);
}

buildMaps();