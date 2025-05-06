

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
let lemmas_json = Object.create(null);

fetch("lemmas_json.json")
.then(response => {
  return response.json()
})
.then(response => lemmas_json = response);

const randomLemma = () => {
  const idx = Math.floor(Math.random() * 3031);
  const lemma = lemmas_json[idx];
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
    for(const idx in lcs_paradigm[0]) {
      //need to add also nom. sg. PRAP variant in ǫћь- in table-2
      if(noun_verb == "1" && (idx == 38 || (idx > 39 && idx < 43))) {
          const jer = idx == 38 ? "ь" : "ъ";
          const raw_participle = lcs_paradigm[0][idx];
          lcs_paradigm[0][idx] = raw_participle.concat(jer);
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
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
