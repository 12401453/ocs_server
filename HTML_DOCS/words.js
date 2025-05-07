

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
let lemmas_json = Object.create(null);

fetch("lemmas_json.json")
.then(response => {
  return response.json()
})
.then(response => lemmas_json = response);

const randomLemma = () => {
  const idx = Math.floor(Math.random() * 3031);
  const lemma = lemmas_json[idx].slice(0, -1);
  generateInflection(lemma);
  const [lemma_id, stem, noun_verb, conj_type] = lemma;
  console.log(lemma_id, stem, noun_verb, conj_type);
};

let raw_lcs_paradigm = Object.create(null);
const generateInflection = ([lemma_id, stem, noun_verb, conj_type]) => {
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
    raw_lcs_paradigm = response;
    let lcs_paradigm = raw_lcs_paradigm;
    for(const variant_table of lcs_paradigm) {
      for(const idx in variant_table) {
        //need to add also nom. sg. PRAP variant in ǫћь- in table-2
        if(noun_verb == "1" && variant_table[idx] != "" && (idx == 38 || (idx > 39 && idx < 43))) {
            const jer = idx == 38 ? "ь" : "ъ";
            const raw_participle = variant_table[idx];
            variant_table[idx] = raw_participle.concat(jer);
        }
      }
    }
    
    writeTable(lcs_paradigm, conj_type, noun_verb, lemma_id);
    for(const obj of response) {
      //console.log(obj);
      for(const infl in obj) {
        //if(obj[infl] != "") console.log(convertToOCS(obj[infl]));
        //if(obj[infl] != "") console.log(convertToORV(obj[infl]));
      }
      //console.log("_________________________");
    }
  })
  .finally(() => {;});
};

const switchConv = () => {
  if(convertFunction == convertToOCS) convertFunction = convertToORV;
  else convertFunction = convertToOCS;
}

let convertFunction = convertToOCS;

const writeTable = (lcs_paradigm, conj_type, noun_verb, lemma_id) => {

  const writeCell = (idx) => {
    cell_html += "<div class='grid-child' title='"+lcs_paradigm[0][idx]+"'>"+convertFunction(lcs_paradigm[0][idx], inflection_class_map.get(conj_type), lemma_id);
    cell_html += "<div class='infl_variants'>";
    
    let variants_written = false;
    for(let i = 1; i < lcs_paradigm.length; i++) {
      const lcs_variant = lcs_paradigm[i][idx]; //this often returns undefineds and it really shouldn't
      //console.log(lcs_variant);
      let variant_type = i == 2 ? "variant" : "deviance";
      if(lcs_variant != "" && lcs_variant != undefined) {
        cell_html += "<span class='"+variant_type+"' title='" + lcs_variant + "'>" + convertFunction(lcs_variant, inflection_class_map.get(conj_type), lemma_id) + "</span>, ";
        variants_written = true;
      }
    }
    if(variants_written) cell_html = cell_html.slice(0, -2);

    cell_html += "</div>";
    cell_html += "</div>";
  }



  const grids_container = document.getElementById("grids-container");
  grids_container.innerHTML = "";

  // const verb_grid = document.getElementById("verb-grid");
  // const noun_grid = document.getElementById("noun-grid");
  // verb_grid.innerHTML = "";
  // noun_grid.innerHTML = "";
  // verb_grid.style.display = "none";
  // noun_grid.style.display = "none";
  const grid = document.createElement("div");
  grid.className = "infl-grid";
  if(noun_verb == "2") grid.classList.add("noun-grid");
  else grid.classList.add("verb-grid");

  let cell_html = "";

  if(conj_type.startsWith("adj") || conj_type.startsWith("pron")) {
    for(let k = 0; k < 3; k++) {
      const adj_grid = document.createElement("div");
      adj_grid.className = "infl-grid";
      adj_grid.classList.add("noun-grid");
      for(let i = k*21 + 1; i < (k+1)*21 + 1; i++) {
        writeCell(i);
      }
      const flect_cell = document.createRange().createContextualFragment(cell_html);
      adj_grid.append(flect_cell);
      grids_container.append(adj_grid);
      cell_html = "";
    }
    return;
  }

  for(const idx in lcs_paradigm[0]) {
    writeCell(idx); 
  }
  const flect_cell = document.createRange().createContextualFragment(cell_html);
  grid.append(flect_cell);
  grids_container.append(grid);
};

const lemma_searchbox = document.getElementById("lcs_lemma_searchbox");
const search_candidates = document.getElementById("search_candidates_box");

const removeSearchCandidates = () => {
  lemma_searchbox.style.borderBottomLeftRadius = "";
  lemma_searchbox.style.borderBottomRightRadius = "";
  search_candidates.innerHTML = "";
  search_candidates.style.display = "";
}

const filterLemmas = (event) => {


  console.log(event);
  
  const search_word = lemma_searchbox.value.trim();
  
  console.log(search_word);
  

  if(search_word == "") {
    // lemma_searchbox.style.borderBottomLeftRadius = "";
    // lemma_searchbox.style.borderBottomRightRadius = "";
    // search_candidates.innerHTML = "";
    // search_candidates.style.display = "";
    removeSearchCandidates();
    return;

  }
  //console.log(search_word);

  const filtered_lemmas = lemmas_json.filter(lemma_arr => lemma_arr[4].startsWith(search_word) || lemma_arr[1].startsWith(search_word));

  if(filtered_lemmas.length > 0) {
    lemma_searchbox.style.borderBottomLeftRadius = "0px";
    lemma_searchbox.style.borderBottomRightRadius = "0px";
    search_candidates.innerHTML = "";
    search_candidates.style.display = "flex";
  }
  else {
    // lemma_searchbox.style.borderBottomLeftRadius = "";
    // lemma_searchbox.style.borderBottomRightRadius = "";
    // search_candidates.innerHTML = "";
    // search_candidates.style.display = "";
    removeSearchCandidates();
    return;
  }

  let results_html = "";

  for(let i = 0; i < filtered_lemmas.length && i < 10; i++) {
    results_html += "<div class=\"search_candidate\" data-idx=\"" + filtered_lemmas[i][0] +"\">" + convertFunction(filtered_lemmas[i][4]) + "</div>";
  }
  const results_elem = document.createRange().createContextualFragment(results_html);
  search_candidates.append(results_elem);

  search_candidates.querySelectorAll(".search_candidate").forEach(elem => elem.addEventListener('click', showTable));
}

const showTable = (event) => {
  const lemma_id = Number(event.target.dataset.idx);
  console.log(lemma_id);
  const search_candidates = document.getElementById("search_candidates_box");
  lemma_searchbox.style.borderBottomLeftRadius = "";
  lemma_searchbox.style.borderBottomRightRadius = "";
  search_candidates.innerHTML = "";
  search_candidates.style.display = "";
  generateInflection(lemmas_json.find(arr => arr[0] == lemma_id));
}
const showTable2 = (lemma_id) => {
  console.log(lemma_id);
  const search_candidates = document.getElementById("search_candidates_box");
  lemma_searchbox.style.borderBottomLeftRadius = "";
  lemma_searchbox.style.borderBottomRightRadius = "";
  search_candidates.innerHTML = "";
  search_candidates.style.display = "";
  generateInflection(lemmas_json.find(arr => arr[0] == lemma_id));
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
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
