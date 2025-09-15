

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const app_state = {
  theme: 0,
  show_corpus_forms: false,
  displayed_lemma: [0, "", "", "", "", ""],
  corpus_paradigm: {},
  raw_lcs_paradigm: []
}

let lemmas_json = Object.create(null);

fetch("orv_lemmas_json.json")
.then(response => {
  return response.json()
})
.then(response => lemmas_json = response);

const randomLemma = () => {
  app_state.raw_lcs_paradigm = {};
  app_state.corpus_paradigm = {};
  const idx = Math.floor(Math.random() * lemmas_json.length);

  if(app_state.show_corpus_forms) getCorpusInflections(lemmas_json[idx][0], lemmas_json[idx][2]);
  else generateInflection(lemmas_json[idx]);
  
};


const generateInflection = ([lemma_id, stem, noun_verb, conj_type, lcs_lemma, pv2_3_exists]) => {
  console.log(lemma_id, stem, noun_verb, conj_type);
  let send_data = "stem="+stem+"&conj_type="+conj_type+"&noun_verb="+noun_verb;
  const myheaders = new Headers();
  myheaders.append('Content-Type', 'application/x-www-form-urlencoded');
  myheaders.append('Cache-Control', 'no-cache');
  const options = {method: "POST", headers: myheaders, cache: "no-store", body: send_data};
  
  fetch("generate_inflection.php", options)
  .then((response) => {
    //alert("first response");
    return response.json();
  })
  .then(response => {
    app_state.raw_lcs_paradigm = response;
    for(const variant_table of app_state.raw_lcs_paradigm) {
      for(const idx in variant_table) {
        //need to add also nom. sg. PRAP variant in ǫћь- in table-2
        if(noun_verb == "1" && variant_table[idx] != "" && (idx == 38 || (idx > 39 && idx < 43))) {
            const jer = idx == 38 ? "ь" : "ъ";
            const raw_participle = variant_table[idx];
            variant_table[idx] = raw_participle.concat(jer);
        }
      }
    }
    
    if(noun_verb ==  "1") writeVerbTable(pv2_3_exists, lemma_id);
    else if(noun_verb ==  "2") writeNounTable(pv2_3_exists, lemma_id);

    app_state.displayed_lemma = [lemma_id, stem, noun_verb, conj_type, lcs_lemma, pv2_3_exists];

    app_state.corpus_paradigm = {};
  })
  .finally(() => {
    lemma_searchbox.blur();
  });
};

const getCorpusInflections = (lemma_id=app_state.displayed_lemma[0], noun_verb=app_state.displayed_lemma[2]) => {
  let send_data = "lemma_id="+String(lemma_id)+"&noun_verb="+noun_verb;
  const myheaders = new Headers();
  myheaders.append('Content-Type', 'application/x-www-form-urlencoded');
  myheaders.append('Cache-Control', 'no-cache');
  const options = {method: "POST", headers: myheaders, cache: "no-store", body: send_data};

  fetch("get_corpus_inflections.php", options)
  .then(response => {
    return response.json();
  })
  .then(response => {
    app_state.corpus_paradigm = response;
    //console.log(app_state.corpus_paradigm);

    app_state.displayed_lemma = lemmas_json.find(arr => arr[0] == lemma_id);

    switch(Number(noun_verb)) {
      case 1:
        writeVerbTableCorpus();
        break;
      case 2:
        writeNounTableCorpus();
        break;
    }
  });
};

const switchConv = () => {
  if(convertFunction == convertToOCS) convertFunction = convertToORV;
  else convertFunction = convertToOCS;
}

let convertFunction = convertToORV;

const writeVerbTable = (pv2_3_exists, lemma_id) => {  
    const writeCell = (idx) => {
        table_html += "<td";
        if(idx > 27) {
          table_html += " class='last-col'";
        }
        table_html += ">";

        let lcs_form = "";
        if(app_state.raw_lcs_paradigm[0][idx] != undefined) {
          lcs_form = app_state.raw_lcs_paradigm[0][idx];
        }
        table_html += "<div class='grid-child' title='"+lcs_form+"'>"+convertFunction(lcs_form, pv2_3_exists, lemma_id);
        table_html += "<div class='infl_variants'>";
        
        let variants_written = false;
        for(let i = 1; i < app_state.raw_lcs_paradigm.length; i++) {
            const lcs_variant = app_state.raw_lcs_paradigm[i][idx]; //this often returns undefineds and it really shouldn't
            //console.log(lcs_variant);
            let variant_type = i == 2 ? "variant" : "deviance";
            if(lcs_variant != "" && lcs_variant != undefined) {
                table_html += "<span class='"+variant_type+"' title='" + lcs_variant + "'>" + convertFunction(lcs_variant, pv2_3_exists, lemma_id) + "</span> ";
                variants_written = true;
            }
        }
        if(variants_written) table_html = table_html.slice(0, -1);
        
        table_html += "</div>";
        table_html += "</div>";
        table_html += "</td>"
    }
    
    const grids_container = document.getElementById("grids-container");
    grids_container.innerHTML = "";

    // const finite_verb_table = document.createElement("table");
    // finite_verb_table.className = "infl-grid";
    // if(noun_verb == "2") finite_verb_table.classList.add("noun-grid");
    // else finite_verb_table.classList.add("verb-grid");

    let table_html = "<div class='infl_table_rounder'><table class='infl-grid'><tbody>";
    
    const verb_persons = ["1st sg.", "2nd sg.", "3rd sg.", "1st du.", "2nd du.", "3rd du.", "1st pl.", "2nd pl.", "3rd pl.",];
    const verb_tenses = ["Present", "Aorist", "Imperfect", "Imperative", "Participles"];
    const participle_forms = ["PRAP<sup>1</sup>", "PRAP<sup>2</sup>", "PAP", "L-Part.", "PPP", "PrPP", "Infinitive", "Supine"];
    const participle_titles = ["Present Active Participle (masc./nt. Nsg.)", "Present Active Participle", "Past Active Participle", "L-Participle", "Past Passive Participle", "Present Passive Participle", "Infinitive", "Supine"];
    
    table_html += "<tr class='verb_row'><th class='infl_titles' style='background-color: #040a16; border-top: 1px solid black; border-left: 1px solid black;'></th>";
    for(let i = 0; i < 4; i++) {
      table_html += "<th class='infl_titles top_headers'>"+verb_tenses[i]+"</th>";
    }
    table_html += "</tr>";

    for(let i = 1; i < 10; i++) {
      table_html += "<tr class='verb_row'><th class='infl_titles side_headers'>"+ verb_persons[i - 1]+"</th>";
      for(let j = i; j < 37; j+=9) {
        writeCell(j);
      }
      table_html += "</tr>";
    }
    table_html += "</tbody></table></div>";

    
    const finite_verb_table = document.createRange().createContextualFragment(table_html);

    table_html = "<div class='infl_table_rounder'><table class='infl-grid'><tbody>";
    table_html += "<tr class='verb_row'><th class='infl_titles top_headers' colspan='2'>Participles</th></tr>";
    for(let i = 0; i < participle_forms.length; i++) {
      table_html += "<tr class='verb_row'><th class='infl_titles' title='"+participle_titles[i]+"'>"+ participle_forms[i]+"</th>";
      writeCell(37+i);
      table_html += "</tr>";
    }
    table_html += "</tbody></table></div>";
    const participles_table = document.createRange().createContextualFragment(table_html);
    //console.log(participles_table);
   
    //finite_verb_table.innerHTML = table_html;
    grids_container.append(finite_verb_table);
    grids_container.append(participles_table);
};
const writeVerbTableCorpus = () => {
  const writeCell = (idx) => {
      table_html += "<td";
      if(idx > 27) {
        table_html += " class='last-col'";
      }
      table_html += ">";

      const form_set = new Set();

      let corpus_forms = new Array();
      if(app_state.corpus_paradigm[idx] != undefined) {
        corpus_forms = app_state.corpus_paradigm[idx];
      }
      for(const corpus_form of corpus_forms) {
        const clean_corpus_form = cleanWord(corpus_form, remove_punct_map).toLowerCase();
        if(form_set.has(clean_corpus_form)) {
          continue;
        }
        else {
          table_html += "<div class='corpus-grid-child'>"+clean_corpus_form+"</div>";
          form_set.add(clean_corpus_form);
        }
        
      }
      // table_html += "</div>";
      // table_html += "</div>";
      table_html += "</td>"
  }
  
  const grids_container = document.getElementById("grids-container");
  grids_container.innerHTML = "";

  // const finite_verb_table = document.createElement("table");
  // finite_verb_table.className = "infl-grid";
  // if(noun_verb == "2") finite_verb_table.classList.add("noun-grid");
  // else finite_verb_table.classList.add("verb-grid");

  let table_html = "<div class='infl_table_rounder'><table class='infl-grid'><tbody>";
  
  const verb_persons = ["1st sg.", "2nd sg.", "3rd sg.", "1st du.", "2nd du.", "3rd du.", "1st pl.", "2nd pl.", "3rd pl.",];
  const verb_tenses = ["Present", "Aorist", "Imperfect", "Imperative", "Participles"];
  const participle_forms = ["PRAP<sup>1</sup>", "PRAP<sup>2</sup>", "PAP", "L-Part.", "PPP", "PrPP", "Infinitive", "Supine"];
  const participle_titles = ["Present Active Participle (masc./nt. Nsg.)", "Present Active Participle", "Past Active Participle", "L-Participle", "Past Passive Participle", "Present Passive Participle", "Infinitive", "Supine"];
  
  table_html += "<tr class='verb_row'><th class='infl_titles' style='background-color: #040a16; border-top: 1px solid black; border-left: 1px solid black;'></th>";
  for(let i = 0; i < 4; i++) {
    table_html += "<th class='infl_titles top_headers'>"+verb_tenses[i]+"</th>";
  }
  table_html += "</tr>";

  for(let i = 1; i < 10; i++) {
    table_html += "<tr class='verb_row'><th class='infl_titles side_headers'>"+ verb_persons[i - 1]+"</th>";
    for(let j = i; j < 37; j+=9) {
      writeCell(j);
    }
    table_html += "</tr>";
  }
  table_html += "</tbody></table></div>";

  
  const finite_verb_table = document.createRange().createContextualFragment(table_html);

  table_html = "<div class='infl_table_rounder'><table class='infl-grid'><tbody>";
  table_html += "<tr class='verb_row'><th class='infl_titles top_headers' colspan='2'>Participles</th></tr>";
  for(let i = 0; i < participle_forms.length; i++) {
    table_html += "<tr class='verb_row'><th class='infl_titles' title='"+participle_titles[i]+"'>"+ participle_forms[i]+"</th>";
    writeCell(37+i);
    table_html += "</tr>";
  }
  table_html += "</tbody></table></div>";
  const participles_table = document.createRange().createContextualFragment(table_html);
  //console.log(participles_table);
 
  //finite_verb_table.innerHTML = table_html;
  grids_container.append(finite_verb_table);
  grids_container.append(participles_table);
};

const writeNounTable = (pv2_3_exists, lemma_id) => {  
  const writeCell = (idx, gender) => {
      table_html += "<td";
      if(idx > gender*21+14) {
        table_html += " class='last-col'";
      }
      table_html += ">";

      let lcs_form = "";
      if(app_state.raw_lcs_paradigm[0][idx] != undefined) {
        lcs_form = app_state.raw_lcs_paradigm[0][idx];
      }
      table_html += "<div class='grid-child' title='"+lcs_form+"'>"+convertFunction(lcs_form, pv2_3_exists, lemma_id);
      table_html += "<div class='infl_variants'>";
      
      let variants_written = false;
      for(let i = 1; i < app_state.raw_lcs_paradigm.length; i++) {
          const lcs_variant = app_state.raw_lcs_paradigm[i][idx]; //this often returns undefineds and it really shouldn't
          //console.log(lcs_variant);
          let variant_type = i == 2 ? "variant" : "deviance";
          if(lcs_variant != "" && lcs_variant != undefined) {
              table_html += "<span class='"+variant_type+"' title='" + lcs_variant + "'>" + convertFunction(lcs_variant, pv2_3_exists, lemma_id) + "</span> ";
              variants_written = true;
          }
      }
      if(variants_written) table_html = table_html.slice(0, -1);
      
      table_html += "</div>";
      table_html += "</div>";
      table_html += "</td>"
  }
  
  const grids_container = document.getElementById("grids-container");
  grids_container.innerHTML = "";  
  
  const noun_cases = ["Nom.", "Acc.", "Gen.", "Dat.", "Loc.", "Instr.", "Voc."];
  const noun_numbers = ["Sing.", "Dual", "Plural"];
  const noun_genders = ["Masc.", "Fem.", "Neuter"];
  
  let firstKey;
  for(firstKey in app_state.raw_lcs_paradigm[0]) {
    break;
  }
  let gender_coefficient = Math.floor(Number(firstKey) / 21); //0 for masc, 1 for fem, 2 for nt. (or equivalent)

  let table_html = "";
  const makeNomTableHTML = (gender, adjectival=false) => {
    table_html = "";
    table_html += "<div class='infl_table_rounder'><table class='infl-grid'><tbody>";
    table_html += "<tr><th class='infl_titles' style='background-color: #040a16; border-top: 1px solid black; border-left: 1px solid black;'>";
    if(adjectival) table_html += "<b>"+noun_genders[gender]+"</b>";
    table_html += "</th>";
    for(let i = 0; i < 3; i++) {
      table_html += "<th class='infl_titles top_headers'>"+noun_numbers[i]+"</th>";
    }
    table_html += "</tr>";

    for(let i = 1; i < 8; i++) {
      table_html += "<tr><th class='infl_titles side_headers'>"+ noun_cases[i - 1]+"</th>";
      for(let j = i + 21*gender; j < (gender + 1)*21 + 1; j+=7) {
        writeCell(j, gender);
      }
      table_html += "</tr>";
    }
    table_html += "</tbody></table></div>";
  };
  
  const tables_arr = new Array();

  if(gender_coefficient == 0 && Object.keys(app_state.raw_lcs_paradigm[0]).length > 21) {
    console.log("adjectival");
    makeNomTableHTML(0, true)
    tables_arr.push(document.createRange().createContextualFragment(table_html));
    makeNomTableHTML(1, true);
    tables_arr.push(document.createRange().createContextualFragment(table_html));
    makeNomTableHTML(2, true);
    tables_arr.push(document.createRange().createContextualFragment(table_html));
  }
  else {
    makeNomTableHTML(gender_coefficient, false)
    tables_arr.push(document.createRange().createContextualFragment(table_html));
  }

  for(const nom_table of tables_arr) {
    grids_container.append(nom_table);
  }
};
const writeNounTableCorpus = () => {  
  const writeCell = (idx, gender) => {
      table_html += "<td";
      if(idx > gender*21+14) {
        table_html += " class='last-col'";
      }
      table_html += ">";

      const form_set = new Set();

      let corpus_forms = new Array();
      if(app_state.corpus_paradigm[idx] != undefined) {
        corpus_forms = app_state.corpus_paradigm[idx];
      }
      for(const corpus_form of corpus_forms){
        const clean_corpus_form = cleanWord(corpus_form, remove_punct_map).toLowerCase();
        if(form_set.has(clean_corpus_form)) {
          continue;
        }
        else {
          table_html += "<div class='grid-child'>"+clean_corpus_form+"</div>";
          form_set.add(clean_corpus_form);
        }
      }
      table_html += "</td>"
  }
  
  const grids_container = document.getElementById("grids-container");
  grids_container.innerHTML = "";  
  
  const noun_cases = ["Nom.", "Acc.", "Gen.", "Dat.", "Loc.", "Instr.", "Voc."];
  const noun_numbers = ["Sing.", "Dual", "Plural"];
  const noun_genders = ["Masc.", "Fem.", "Neuter"];

  let table_html = "";
  const makeNomTableHTML = (gender) => {
    table_html = "";
    table_html += "<div class='infl_table_rounder'><table class='infl-grid'><tbody>";
    table_html += "<tr><th class='infl_titles' style='background-color: #040a16; border-top: 1px solid black; border-left: 1px solid black;'>";
    table_html += "<b>"+noun_genders[gender]+"</b>";
    table_html += "</th>";
    for(let i = 0; i < 3; i++) {
      table_html += "<th class='infl_titles top_headers'>"+noun_numbers[i]+"</th>";
    }
    table_html += "</tr>";

    for(let i = 1; i < 8; i++) {
      table_html += "<tr><th class='infl_titles side_headers'>"+ noun_cases[i - 1]+"</th>";
      for(let j = i + 21*gender; j < (gender + 1)*21 + 1; j+=7) {
        writeCell(j, gender);
      }
      table_html += "</tr>";
    }
    table_html += "</tbody></table></div>";
  };
  
  const tables_arr = new Array();

  //where annotators have been inconsistent in sticking to one gender of a noun like ближика (which should be masc. but has been tagged as fem. in Supr.), more than one table will show, which is a good indicator of sloppy gender-tagging practices
  let gender_bitmask = 0;
  for(const key in app_state.corpus_paradigm) {
    gender_bitmask = gender_bitmask | 1 << (Math.floor((Number(key) - 1) / 21));
        
  }

  for(let i = 0; i < 3; i++) {
    if(gender_bitmask & (1 << i)) {
      makeNomTableHTML(i);
      tables_arr.push(document.createRange().createContextualFragment(table_html));
    }
  }

  if(tables_arr.length == 0) {
    //handling for lemmas with no corpus occurrences (it can happen due to corrections etc.)
    for(let i = 0; i < 3; i++) {
      makeNomTableHTML(i);
      tables_arr.push(document.createRange().createContextualFragment(table_html));
    }
  }

/*  if(gender_coefficient == 0 && Object.keys(lcs_paradigm[0]).length > 21) {
    console.log("adjectival");
    makeNomTableHTML(0, true)
    tables_arr.push(document.createRange().createContextualFragment(table_html));
    makeNomTableHTML(1, true);
    tables_arr.push(document.createRange().createContextualFragment(table_html));
    makeNomTableHTML(2, true);
    tables_arr.push(document.createRange().createContextualFragment(table_html));
  }
  else {
    makeNomTableHTML(gender_coefficient, false)
    tables_arr.push(document.createRange().createContextualFragment(table_html));
  } */

  for(const nom_table of tables_arr) {
    grids_container.append(nom_table);
  }
};

const lemma_searchbox = document.getElementById("lcs_lemma_searchbox");
const search_candidates = document.getElementById("search_candidates_box");

search_candidates.addEventListener('mousedown', (event) => event.preventDefault()); //this is to prevent the textarea from losing focus and causing the search-candidates to disappear before the 'click' event fires, since I need the 'click' event to fire to trigger the paradigm-retrieval

const removeSearchCandidates = () => {
  lemma_searchbox.style.borderBottomLeftRadius = "";
  lemma_searchbox.style.borderBottomRightRadius = "";
  search_candidates.innerHTML = "";
  search_candidates.style.display = "";
}

const filterLemmas = (event) => {
 
  const search_word = lemma_searchbox.value.trim();

  if(search_word == "") {
    removeSearchCandidates();
    return;
  }

  const filtered_lemmas = lemmas_json.filter(lemma_arr => lemma_arr[4].startsWith(search_word) || lemma_arr[1].startsWith(search_word));

  if(filtered_lemmas.length > 0) {
    lemma_searchbox.style.borderBottomLeftRadius = "0px";
    lemma_searchbox.style.borderBottomRightRadius = "0px";
    search_candidates.innerHTML = "";
    search_candidates.style.display = "flex";
  }
  else {
    removeSearchCandidates();
    return;
  }

  let results_html = "";

  for(let i = 0; i < filtered_lemmas.length && i < 10; i++) {
    const pv2_3_exists = filtered_lemmas[i][5] == "" ? false : true;
    //console.log("search_candidate data: ", filtered_lemmas[i][4], inflection_class_map.get(filtered_lemmas[i][3]), filtered_lemmas[i][0]);
    results_html += "<div class=\"search_candidate\" data-idx=\"" + filtered_lemmas[i][0] +"\">" + convertFunction(filtered_lemmas[i][4].replaceAll("pn", "n"), pv2_3_exists, filtered_lemmas[i][0]) + "</div>";
  }
  const results_elem = document.createRange().createContextualFragment(results_html);
  search_candidates.append(results_elem);

  search_candidates.querySelectorAll(".search_candidate").forEach(elem => elem.addEventListener('click', (event) => {
    showTable2(Number(event.target.dataset.idx));
  }));
};

/*const showTable = (event) => {
  const lemma_id = Number(event.target.dataset.idx);
  console.log(lemma_id);
  const search_candidates = document.getElementById("search_candidates_box");
  lemma_searchbox.style.borderBottomLeftRadius = "";
  lemma_searchbox.style.borderBottomRightRadius = "";
  search_candidates.innerHTML = "";
  search_candidates.style.display = "";
  generateInflection(lemmas_json.find(arr => arr[0] == lemma_id));
} */
const showTable2 = (lemma_id) => {
  console.log(lemma_id);
  const search_candidates = document.getElementById("search_candidates_box");
  lemma_searchbox.style.borderBottomLeftRadius = "";
  lemma_searchbox.style.borderBottomRightRadius = "";
  search_candidates.innerHTML = "";
  search_candidates.style.display = "";
  if(app_state.show_corpus_forms) {
    app_state.raw_lcs_paradigm = {};
    getCorpusInflections(lemma_id, lemmas_json.find(arr => arr[0] == lemma_id)[2]);
  }
  else {
    generateInflection(lemmas_json.find(arr => arr[0] == lemma_id));
  }
}

const arrowkeySelect = (arrow_key) => {
  const incrementer = arrow_key == "ArrowDown" ? 1 : -1;
  const search_candidate_boxes = search_candidates.children;
  const candidates_length = search_candidate_boxes.length;
  let highlighted_box = false;
  for(let i = 0; i < candidates_length; i++) {
    if(search_candidate_boxes[i].classList.contains('keyboard_selected')) {
      search_candidate_boxes[i].classList.remove('keyboard_selected');
      console.log("kb_selected index: ", (i+incrementer+candidates_length)%candidates_length);
      search_candidate_boxes[(i+incrementer+candidates_length)%candidates_length].classList.add('keyboard_selected');
      highlighted_box = true;
      break;
    }
  }
  if(highlighted_box == false && candidates_length > 0) {
    search_candidate_boxes[0].classList.add('keyboard_selected');
    console.log("kb_selected index: ", 0);
  }
}

lemma_searchbox.addEventListener('input', filterLemmas);
lemma_searchbox.addEventListener('keydown', (event) => {
  if(event.key == 'Enter') {
    event.preventDefault();
    if(search_candidates.querySelector('.keyboard_selected')){
      showTable2(Number(search_candidates.querySelector('.keyboard_selected').dataset.idx));
    } 
    return;
  }
  else if(event.key == 'ArrowDown' || event.key == 'ArrowUp') {
    event.preventDefault();
    arrowkeySelect(event.key);
  }
}); //this is needed because 'input' events have no .key property (only inputType == "insertLineBreak"), and keydown events fire before the textarea's value updates
lemma_searchbox.addEventListener('focus', filterLemmas);
lemma_searchbox.addEventListener('blur', removeSearchCandidates);

const toggleInflTables = (event) => {

  event.currentTarget.parentElement.classList.toggle('active');
  app_state.show_corpus_forms = app_state.show_corpus_forms ? false : true;

  //if(Object.keys(app_state.raw_lcs_paradigm).length == 0 && Object.keys(app_state.corpus_paradigm).length == 0) {
  if(app_state.displayed_lemma[0] == 0){
    return;
  }

  if(app_state.show_corpus_forms) {
    if(Object.keys(app_state.corpus_paradigm).length == 0) {
      getCorpusInflections();
    }
    else {
      if(app_state.displayed_lemma[2] == '2') writeNounTableCorpus();
      else if(app_state.displayed_lemma[2] == '1') writeVerbTableCorpus();
    }
  }
  else {

    if(Object.keys(app_state.raw_lcs_paradigm).length == 0) {
      generateInflection(app_state.displayed_lemma);
      return;
    }

    const pv2_3_exists = app_state.displayed_lemma[5] == "" ? false : true;
    const lemma_id = app_state.displayed_lemma[0];

    if(app_state.displayed_lemma[2] == '2') writeNounTable(pv2_3_exists, lemma_id);
    else if(app_state.displayed_lemma[2] == '1') writeVerbTable(pv2_3_exists, lemma_id);
  }
  
}
document.getElementById("infl_table_switch").addEventListener('click', toggleInflTables);

const changeButtonSize = (event) => {
  if(event.type == 'mousedown' || event.type == "touchstart") event.currentTarget.style.transform = "scale(0.9, 0.9)";
  else if(event.type == 'mouseout') event.currentTarget.style.transform = "";
  else event.currentTarget.style.transform = "";
};
const button_events = ["touchstart", "touchend", "mousedown", "mouseup", "mouseout"];
for(const btn_evnt of button_events) {
  document.getElementById("random_lemma_btn").addEventListener(btn_evnt, changeButtonSize);
}
document.getElementById("random_lemma_btn").addEventListener('click', randomLemma);


//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
app_state.theme = document.getElementById("theme_switcher").src.endsWith("moon-stars.svg") ? 1 : 0;
const theme_urls = [["light_theme.css", "sun.svg", "Switch to dark theme"], ["dark_theme.css", "moon-stars.svg", "Switch to light theme"]];
const switchTheme = (event) => {
  app_state.theme = (app_state.theme + 1)%2;
  document.getElementById("colour_theme").href = theme_urls[app_state.theme][0];
  document.getElementById("theme_switcher").src = theme_urls[app_state.theme][1];
  document.getElementById("theme_switcher").title = theme_urls[app_state.theme][2];

  document.cookie = "theme=" + String(app_state.theme);
};
document.getElementById("theme_switcher").addEventListener('click', switchTheme);


const glag_map = new Array(
  ["а", "ⰰ"],
  ["б", "ⰱ"],
  ["ц", "ⱌ"],
  ["г", "ⰳ"],
  ["д", "ⰴ"],
  ["е", "ⰵ"],
  ["ж", "ⰶ"],
  ["ѕ", "ⰷ"],
  ["з", "ⰸ"],
  ["і", "ⰹ"],
  ["ꙇ", "ⰺ"],
  ["и", "ⰻ"],
  ["ꙉ", "ⰼ"],
  ["к", "ⰽ"],
  ["л", "ⰾ"],
  ["м", "ⰿ"],
  ["н", "ⱀ"],
  ["о", "ⱁ"],
  ["п", "ⱂ"],
  ["р", "ⱃ"],
  ["с", "ⱄ"],
  ["т", "ⱅ"],
  ["ѹ", "ⱆ"],
  ["ф", "ⱇ"],
  ["х", "ⱈ"],
  ["ч", "ⱍ"],
  ["ш", "ⱎ"],
  ["ъ", "ⱏ"],
  ["ь", "ⱐ"],
  ["ѣ", "ⱑ"],
  ["ю", "ⱓ"],
  ["ѫ", "ⱘ"],
  ["ѭ", "ⱙ"],
  ["в", "ⰲ"],
  ["щ", "ⱋ"],
  ["ѵ", "ⱛ"],
  ["ѡ", "ⱉ"],
  ["ѳ", "ⱚ"],
  ["ѧ", "ⱔ"],
  ["ѩ", "ⱗ"],
  ["ꙙ", "ⱕ"],
  ["А", "Ⰰ"],
  ["Б", "Ⰱ"],
  ["Ц", "Ⱌ"],
  ["Г", "Ⰳ"],
  ["Д", "Ⰴ"],
  ["Е", "Ⰵ"],
  ["Ж", "Ⰶ"],
  ["Ѕ", "Ⰷ"],
  ["З", "Ⰸ"],
  ["І", "Ⰹ"],
  ["Ꙇ", "Ⰺ"],
  ["И", "Ⰻ"],
  ["Ꙉ", "Ⰼ"],
  ["К", "Ⰽ"],
  ["Л", "Ⰾ"],
  ["М", "Ⰿ"],
  ["Н", "Ⱀ"],
  ["О", "Ⱁ"],
  ["П", "Ⱂ"],
  ["Р", "Ⱃ"],
  ["С", "Ⱄ"],
  ["Т", "Ⱅ"],
  ["Ѹ", "Ⱆ"],
  ["Ф", "Ⱇ"],
  ["Х", "Ⱈ"],
  ["Ч", "Ⱍ"],
  ["Ш", "Ⱎ"],
  ["Ъ", "Ⱏ"],
  ["Ь", "Ⱐ"],
  ["Ѣ", "Ⱑ"],
  ["Ю", "Ⱓ"],
  ["Ѫ", "Ⱘ"],
  ["Ѭ", "Ⱙ"],
  ["В", "Ⰲ"],
  ["Щ", "Ⱋ"],
  ["Ѵ", "Ⱛ"],
  ["Ѡ", "Ⱉ"],
  ["Ѳ", "Ⱚ"],
  ["Ѧ", "Ⱔ"],
  ["Ѩ", "Ⱗ"],
  ["Ꙙ", "Ⱕ"],
  ["й", "ⰻ"],
  ["ꙗ", "ⱑ"],
  ["Ꙗ", "Ⱑ"],
  ["ѥ", "ⰵ"],
  ["Ѥ", "Ⰵ"]
);
const cyr_glag_clean_map = new Array(
  ["о҄у", "ѹ҄"],
  ["о͑у", "ѹ҅"],
  ["ꙑ", "ъі"],
  ["Оу", "Ѹ"],
  ["ОУ", "Ѹ"],
  ["оу", "ѹ"],
  ["о̑у", "ѹ̂"],
  ["A", "А"],
  ["O", "О"],
  ["E", "Е"],
  ["C", "С"],
  ["a", "а"],
  ["o", "о"],
  ["e", "е"],
  ["c", "с"],
  ["ы", "ьі"],
  ["У", "Ѵ"],
  ["Ꙃ", "Ѕ"],
  ["Ћ", "Ꙉ"],
  ["у", "ѵ"],
  ["ꙃ", "ѕ"],
  ["ћ", "ꙉ"],
  ["Ⱕ", "Ꙙ"],
  ["Я", "Ꙗ"],
  ["ⱕ", "ꙙ"],
  ["я", "ꙗ"],
  ["Ҍ", "Ѣ"],
  ["ҍ", "ѣ"],
  ["Ї", "Ꙇ"],
  ["ї", "ꙇ"],
  ["X", "Х"],
  ["x", "х"]
);

const original_cyr_forms = new Array();
const cyrToGlag = (str) => {
  for(const pair of cyr_glag_clean_map) {
    str = str.replaceAll(pair[0], pair[1]);
  }
  for(const pair of glag_map) {
    str = str.replaceAll(pair[0], pair[1]);
  }
  return str;
};
