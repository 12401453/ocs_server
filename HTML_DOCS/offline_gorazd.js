

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const app_state = {
  theme: 0,
  show_corpus_forms: false,
  displayed_lemma: [0, "", "", "", "", "", ""],
  corpus_paradigm: {},
  raw_lcs_paradigm: [],
  corpus_form_attestations_shown: false,
  titles_info: []
}

document.getElementById("grids-container").addEventListener('click', (event) => {
  const td_cell = event.target.closest("td");
  if(app_state.show_corpus_forms && td_cell) {
    const td_cell_rect = td_cell.getBoundingClientRect();
    getCorpusInflectionSentences(Number(td_cell.dataset.infl_idx), td_cell_rect);
  }
});

let gorazd_json = Object.create(null);

fetch("lookup_json_snsp.json")
.then(response => {
  return response.json();
})
.then(response => {
  gorazd_json = response;
  document.getElementById("offline_gorazd_searchbox").disabled = false;
});

const randomEntry = () => {
  const idx = Math.floor(Math.random() * gorazd_json.length);

  
};

const getGorazdEntries = (gorazd_dict_type, dict_entry_ids) => {
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

    removeCorpusAttestationsBox();

    switch(Number(noun_verb)) {
      case 1:
        writeVerbTableCorpus();
        break;
      case 2:
        writeNounTableCorpus();
        break;
    }
    lemma_searchbox.value = app_state.displayed_lemma[6];
  });
};

const switchConv = () => {
  if(convertFunction == convertToOCS) convertFunction = convertToORV;
  else convertFunction = convertToOCS;
}

let convertFunction = convertToOCS;

const lemma_searchbox = document.getElementById("offline_gorazd_searchbox");
const search_candidates = document.getElementById("search_candidates_box");

search_candidates.addEventListener('mousedown', (event) => event.preventDefault()); //this is to prevent the textarea from losing focus and causing the search-candidates to disappear before the 'click' event fires, since I need the 'click' event to fire to trigger the paradigm-retrieval

const removeSearchCandidates = () => {
  lemma_searchbox.style.borderBottomLeftRadius = "";
  lemma_searchbox.style.borderBottomRightRadius = "";
  search_candidates.innerHTML = "";
  search_candidates.style.display = "";
}

const filterLemmas = (event) => {
 
  const search_word = lemma_searchbox.value.trim().replaceAll("Ǣ", "a").toLowerCase();
  if(search_word == "") {
    removeSearchCandidates();
    return;
  }

  let filter_predicate = x => (x[4].replaceAll("Ǣ", "a").startsWith(search_word) || x[6].startsWith(search_word));

  if(search_word.startsWith("*")) {
    filter_predicate = x => (x[4].replaceAll("Ǣ", "a").includes(search_word.slice(1)) || x[6].includes(search_word.slice(1)))
  }

  //const filtered_lemmas = lemmas_json.filter(lemma_arr => lemma_arr[4].startsWith(search_word) || lemma_arr[1].startsWith(search_word));
  const filtered_lemmas = gorazd_json.filter(filter_predicate);

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
    showTable(Number(event.target.dataset.idx));
  }));
};

const showTable = (lemma_id) => {
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
      showTable(Number(search_candidates.querySelector('.keyboard_selected').dataset.idx));
    }
    else if(search_candidates.children.length > 0) {
      showTable(Number(search_candidates.children[0].dataset.idx));
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
      removeCorpusAttestationsBox();
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
    const conj_type = app_state.displayed_lemma[3];

    removeCorpusAttestationsBox();

    if(app_state.displayed_lemma[2] == '2') writeNounTable(pv2_3_exists, lemma_id, conj_type);
    else if(app_state.displayed_lemma[2] == '1') writeVerbTable(pv2_3_exists, lemma_id, conj_type);
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
  document.getElementById("random_entry_btn").addEventListener(btn_evnt, changeButtonSize);
}
document.getElementById("random_entry_btn").addEventListener('click', randomEntry);

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
