
function tt_type() {
  if(tooltips_shown) {
    let lemma_tooltips = document.querySelectorAll('.lemma_tt, .mw_tt');
    lemma_tooltips.forEach(lemma_tooltip => {
      lemma_tooltip.remove();
    });
    tooltips_shown = false;
  }
  else {
    document.getElementById("tt_toggle").disabled = true;
    lemmaTooltipMW();
    tooltips_shown = true; //redundant but done incase the AJAX request in lemmaTooltips() fails
  }
}

let lang_id = 0;
let dict = Object.create(null);
function setLangId() {
  const textSelect = document.getElementById('textselect');
  const selected_index = textSelect.selectedIndex;

  lang_id = Number(textSelect.options[selected_index].dataset.lang_id);
  console.log(lang_id);
  if(dict.bool_displayed == true) dict.remove();
  dict = new Dictionary();
  dict.display();
}

//these should be put into the app_state object
let page_toknos_arr = [];
let current_pageno = 1;
let dt_end = 0;

const updateFont = (textselect_value) => {
  if(textselect_value == 1 || textselect_value == 6 || textselect_value == 7 || textselect_value == 8) {
    document.documentElement.style.setProperty("--chunk-font-family", "var(--cyr-font-family)");
    document.documentElement.style.setProperty("--chunk-font-size", "var(--cyr-font-size)");
    document.documentElement.style.setProperty("--chunk-letter-spacing", "var(--cyr-letter-spacing)");
  }
  else {
    document.documentElement.style.setProperty("--chunk-font-family", "var(--glag-font-family)");
    document.documentElement.style.setProperty("--chunk-font-size", "var(--glag-font-size)");
    document.documentElement.style.setProperty("--chunk-letter-spacing", "var(--glag-letter-spacing)");
  }
};

updateFont(document.getElementById("textselect").value);

const app_state = {
  annotation_mode: "none",
  search_box_shown: false,
  search_scope: 1,
  search_box_minimised: false,
  regex_search: false,
  fuzzy_search: false,
  search_type: 1,
  dict_box_shown: false,
  dict_box_minimised: true,
  dict_html_entries: {1: "", 2: ""},
  titles_info: []
}

function selectText() {

  showLoadingButton();

  let textselect_value = document.getElementById('textselect').value;
  let post_data = "textselect="+textselect_value;
  console.log(post_data);
  const httpRequest = (method, url) => {

      const para1 = document.getElementById('p1');

      const xhttp = new XMLHttpRequest();
      xhttp.open(method, url, true);
      xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
      xhttp.responseType = 'json';

      xhttp.onreadystatechange = () => {
    
        if(xhttp.readyState == 4) {
          para1.innerHTML = xhttp.response["html"];
          page_toknos_arr = xhttp.response["pagenos"];
          const subtitles_json = xhttp.response["subtitles_json"];

          updateFont(textselect_value);

          const subtitle_select = document.getElementById("subtitle_select");
          subtitle_select.innerHTML = "";
          for(const subtitle_id of Object.keys(subtitles_json)) {
            const option = document.createElement("option");
            option.value = subtitle_id;
            option.dataset.tokno_start = subtitles_json[subtitle_id][0];
            option.dataset.tokno_end = subtitles_json[subtitle_id][1];
            option.textContent = subtitles_json[subtitle_id][2];
            subtitle_select.append(option);
          }
          subtitle_select.dispatchEvent(new Event('cookie_selection'));

          current_pageno = 1;

          if(page_toknos_arr.length > 1) {
            document.getElementById("pagenos").addEventListener('click', selectText_splitup);
            document.getElementById("pagenos").addEventListener('keydown', selectText_splitup);
          }
          
          if(tooltips_shown) {
          }
          document.getElementById("textselect").blur();
          //loadingbutton.remove();

          removeLoadingButton();

          applyTooltips();
        }
     
      }

      xhttp.send(post_data);
  }

  httpRequest("POST", "retrieve_text.php");
}
const selectSubtitle = (event) => {
  // if(displayWordEditor.edit_mode) {
  //   displayWordEditor.stopEditing();
  // }
  if(display_word != null) delAnnotate();
  //setLangId();  //not async safe, needs to change

  // let loadingbutton = document.createElement('div');
  // loadingbutton.innerHTML = "Loading...";
  // loadingbutton.id = 'loadingbutton';
  // document.getElementById('spoofspan').after(loadingbutton);

  showLoadingButton();

  const select_element = event.target;
  const tokno_start = select_element.options[select_element.selectedIndex].dataset.tokno_start;
  const tokno_end = select_element.options[select_element.selectedIndex].dataset.tokno_end;
  const selected_subtitle_id = event.target.value;
  let post_data = "toknostart="+tokno_start+"&toknoend="+tokno_end;
  console.log(post_data);
  const httpRequest = (method, url) => {

     
      const para1 = document.getElementById('p1');

      const xhttp = new XMLHttpRequest();
      xhttp.open(method, url, true);
      xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
      xhttp.responseType = 'json';

      xhttp.onreadystatechange = () => {
      
        if(xhttp.readyState == 4) {
          para1.innerHTML = xhttp.response["html"];
          page_toknos_arr = xhttp.response["pagenos"];

          //updateFont(document.getElementById("textselect").value);

          document.cookie = "subtitle_id="+selected_subtitle_id+";max-age=157680000";
          current_pageno = 1;
          document.cookie = "current_pageno="+current_pageno+";max-age=157680000";

          if(page_toknos_arr.length > 1) {
            document.getElementById("pagenos").addEventListener('click', selectText_splitup);
            document.getElementById("pagenos").addEventListener('keydown', selectText_splitup);
          }
         
          //document.getElementById("textbody").addEventListener('click', showAnnotate);

          document.querySelectorAll('.multiword').forEach(mw => {
            mw.addEventListener('click', showMultiwordAnnotate);
            mw.addEventListener('mouseover', underlineMultiwords);
            mw.addEventListener('mouseout', removeUnderlineMultiwords);
          });
          
          if(tooltips_shown) {
            //lemmaTooltipMW();
          }
          document.getElementById("subtitle_select").blur();
          //loadingbutton.remove();
          removeLoadingButton();

          applyTooltips();
          

        }
     
      }

      xhttp.send(post_data);
  }

  httpRequest("POST", "retrieve_subtitle.php");
}

const proiel_pos_map = [
  [3, "adjective"],
  [4, "adverb"],
  [3, "article"],
  [1, "cardinal number"],
  [1, "common noun"],
  [6, "conjunction"],
  [3, "demonstrative pronoun"],
  [1, "foreign word"],
  [3, "indefinite pronoun"],
  [7, "infinitive marker"],
  [7, "interjection"],
  [8, "interrogative adverb"],
  [8, "interrogative pronoun"],
  [3, "ordinal number"],
  [1, "personal pronoun"],
  [1, "personal reflexive pronoun"],
  [3, "possessive pronoun"],
  [3, "possessive reflexive pronoun"],
  [5, "preposition"],
  [1, "proper noun"],
  [9, "quantifier"],
  [1, "reciprocal pronoun"],
  [4, "relative adverb"],
  [1, "relative pronoun"],
  [6, "subjunction"],
  [2, "verb"],
  [10, "unassigned"]
];

const morph_tag_map = [
  {1: "1st pers.", 2: "2nd pers.", 3:"3rd pers.", x:"??? pers."},
  
  {s:"sing.", d:"dual", p:"plural", x:"??? num."},
  
  {p:"present", i:"imperfect", r:"perfect", s:"resultative", a:"aorist", u:"past", l:"pluperfect", f:"future", t:"future perfect", x:"??? tense"},
  
  {i:"indicative", s:"subjunctive", m:"imperative", o:"optative", n:"infinitive", p:"participle", d:"gerund", g:"gerundive", u:"supine", x:"??? mood", y:"unspec. finiteness", e:"indic. or subj.", f:"indic. or imper.", h:"subj. or imper.", t:"finite"},

  {a:"active", m:"middle", p:"passive", e:"middle or passive", x:"??? voice"},
  
  {m:"masc.", f:"fem.", n:"neut.", p:"masc./fem.", o:"masc./neut.", r:"fem./neut.", q:"masc./fem./neut.", x:"??? gender"},

  {n:"nominative", a:"accusative", o:"oblique", g:"genitive", c:"gen./dat.", e:"acc./dat.", d:"dative", b:"ablative", i:"instrumental", l:"locative", v:"vocative", x:"??? case", z:"no case"},

  {p:"positive", c:"comparative", s:"superlative", x:"??? degree", z:"no degree"},

  {w:"weak", s:"strong", t:"weak/strong"},

  {n:"non-inflecting", i:"inflecting"}  
];
const convertMorphTag = (morph_tag) => {
  if(morph_tag[9] == "n") {
    return "non-inflecting";
  }

  let morph_string = "";
  for(let i = 0; i < 9; i++) {
    const tag_char = morph_tag[i];
    if(tag_char == "-") {
      continue;
    }
    morph_string += morph_tag_map[i][tag_char] + " ";
  }
  return morph_string.trim();
};

const load_spinner = document.createElement("div");
load_spinner.id = "load_spinner";

const showLoadingButton = () => {
  // let loadingbutton = document.createElement('div');
  // loadingbutton.innerHTML = "Loading...";
  // loadingbutton.id = 'loadingbutton';
  // document.getElementById('spoofspan').after(loadingbutton);
  
  document.getElementById("textbody").style.opacity = "50%";
  document.body.append(load_spinner);
};
const removeLoadingButton = () => {
  //document.getElementById("loadingbutton").remove();
  document.getElementById("textbody").style.opacity = "";
  document.getElementById("load_spinner").remove();
};

const selectText_splitup = (event) => {
  let new_pageno = 1;
  const pageno_box = document.getElementById("pageno_box");
  if(event.type == "keydown" && event.target.id == "pageno_box") {
    if(event.key == "Enter") {
      event.preventDefault();
      new_pageno = Math.floor(Number(event.target.value));
      if(Number.isNaN(new_pageno)) {
        pageno_box.value = current_pageno;
        return;
      }
      if(new_pageno < 1) new_pageno = 1;
      else if(new_pageno > page_toknos_arr.length) new_pageno = page_toknos_arr.length;
      pageno_box.value = new_pageno;
      pageno_box.blur();
    }
    else return;
  }
  else if(event.type == "click") {
    if(event.target.id == "pageno_leftarrow" && current_pageno > 1) {
      new_pageno = current_pageno - 1;
      pageno_box.value = new_pageno;   
    }
    else if(event.target.id == "pageno_rightarrow" && current_pageno < page_toknos_arr.length) {
      new_pageno = current_pageno + 1;
      pageno_box.value = new_pageno;   
    }
    else return;
  }
  if(new_pageno == current_pageno) return;

  // if(displayWordEditor.edit_mode) {
  //   displayWordEditor.stopEditing();
  // }
  if(display_word != null) delAnnotate();

  // let loadingbutton = document.createElement('div');
  // loadingbutton.innerHTML = "Loading...";
  // loadingbutton.id = 'loadingbutton';
  // document.getElementById('spoofspan').after(loadingbutton);
  showLoadingButton();

  const tokno_start = page_toknos_arr[new_pageno - 1][0];
  const tokno_end = page_toknos_arr[new_pageno - 1][1];

  let post_data = "toknostart="+tokno_start+"&toknoend="+tokno_end;
  console.log(post_data);
  const httpRequest = (method, url) => {
      const textbody = document.getElementById('textbody');

      const xhttp = new XMLHttpRequest();
      xhttp.open(method, url, true);
      xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
      xhttp.responseType = 'text';
      xhttp.onreadystatechange = () => {
      
        if(xhttp.readyState == 4) {

          textbody.innerHTML = xhttp.responseText;
          
          //document.getElementById("textbody").addEventListener('click', showAnnotate);

          document.querySelectorAll('.multiword').forEach(mw => {
            //mw.onclick = showMultiwordAnnotate;
            mw.addEventListener('click', showMultiwordAnnotate);
            mw.addEventListener('mouseover', underlineMultiwords);
            mw.addEventListener('mouseout', removeUnderlineMultiwords);
          });
   
          if(tooltips_shown) {
            //lemmaTooltipMW();
          }
          //loadingbutton.remove();
          removeLoadingButton();
          if(new_pageno > current_pageno) {
            document.getElementById("textselect").scrollIntoView();
          }
          current_pageno = new_pageno;
          document.cookie = "current_pageno="+current_pageno+";max-age=157680000";

          applyTooltips();
        }
     
      }

      xhttp.send(post_data);

}

  httpRequest("POST", "retrieve_text_pageno.php");
};

/*
const highlightPageno = function (event) {
  event.target.classList.add("current_pageno");
}; */


function progressBar(word_count) {

  let loading_bar_percentage = document.getElementById("percent");

  const httpRequest = (method, url) => {

    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.responseType = 'text';
    //xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');

    xhttp.onload = () => {

      if(xhttp.readyState == 4) {

        let words_progress = Number(xhttp.responseText);
        let percent = ((words_progress*100)/word_count).toFixed(2);
        loading_bar_percentage.innerHTML = `${percent}%`;
        //console.log("progress_bar.php triggered");
      }

    }
    xhttp.send();
  }
  httpRequest("GET", "progress_bar.php");

  setTimeout(`progressBar(${word_count})`, 100); //setTimeout() executes the function in its first argument after the number of milliseconds in its second argument, so this progressBar function gets called every 0.1 seconds forever until the page reloads when the text is fully loaded
 
} 



function loadText() {

  let newtext_raw = document.getElementById('newtext').value.replaceAll('\u00AD', '').replaceAll("’", "\'").trim();
  let text_title_raw = document.getElementById('text_title').value.replaceAll('\u00AD', '').replaceAll("’", "\'").trim(); //U+00AD is a soft-hyphen, invisible little bastard
  
  let langselect = document.getElementById('langselect').value;
  
  if(langselect == '2') {
    newtext_raw = newtext_raw.replaceAll('\u0259', '\u04D9').replaceAll('\u018F', '\u04D8'); //replace Latin schwa-letter with Cyrillic ә (they look identical)
    text_title_raw = text_title_raw.replaceAll('\u0259', '\u04D9').replaceAll('\u018F', '\u04D8');
  }

  if(text_title_raw == '' && newtext_raw == '') { return; }
  if(text_title_raw == '') { alert("Do not leave Text Title blank"); return; }
  if(newtext_raw == '') {alert("You cannot submit a blank text"); return; }

  let newtext = encodeURIComponent(newtext_raw);
 
  
  let text_title = encodeURIComponent(text_title_raw);
  

  
/*
  let loadingbutton = document.createElement('div');
  loadingbutton.innerHTML = 'Processing text: <span id="percent">0.00%</span>';
  loadingbutton.id = 'loadingbutton';
  let loading_bar_percentage = document.getElementById("percent");
  document.getElementById('spoofspan').after(loadingbutton); */

  let loadingbutton = document.createElement('div');
  loadingbutton.innerHTML = "Loading...";
  loadingbutton.id = 'loadingbutton';
  document.getElementById('spoofspan').after(loadingbutton);

    
  const httpRequest = (method, url) => {

   let send_data = "new_text="+newtext+"&text_title="+text_title+"&langselect="+langselect;

   const xhttp = new XMLHttpRequest();
   xhttp.open(method, url, true);
  // xhttp.responseType = 'json';
  xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');

  xhttp.onload = () => {
    console.log("sent");
  // console.log(xhttp.responseText);
  
    if(xhttp.readyState == 4)  {
      loadingbutton.innerHTML = "&nbsp;&nbsp;Done&nbsp;&nbsp;"; //not really needed
      //location.reload(); /* window.open("update_db.php"); */
      window.location = "/texts";
    }
  }
   xhttp.send(send_data);
 }

 if(Number(langselect) == 10) httpRequest("POST", "add_text_OE.php")
 else httpRequest("POST", "update_db.php");
 
}



function clearTable() {

  if(!confirm("Are you absolutely sure you want to clear the entire database and delete all your texts and saved words?")) {
    return;
  }
  
  let loadingbutton = document.createElement('div');
  loadingbutton.innerHTML = "Loading...";
  loadingbutton.id = 'loadingbutton';
  document.getElementById('spoofspan').after(loadingbutton);

  const httpRequest = (method, url) => {
 
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
   // xhttp.responseType = 'json';
   // xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    
    xhttp.onload = () => {
      console.log("sent");
     // console.log(xhttp.responseText);
     if(xhttp.readyState == 4) {
      loadingbutton.remove(); 
      location.reload();
    }
  }
    xhttp.send();
  }
 
  httpRequest("POST", "clear_table.php");
}

function deleteText() {
  
  let title_select = document.getElementById('textselect');
  let textselect_value = title_select.value;
  if(textselect_value == 0) { return;}
  let text_title = title_select.options[title_select.selectedIndex].textContent;

  if(!confirm(`Are you sure you want to delete "${text_title}"?`)) {
    title_select.selectedIndex = 0;
    title_select.dispatchEvent(new Event('input'));
    return;
  }
  
  let loadingbutton = document.createElement('div');
  loadingbutton.innerHTML = "Loading...";
  loadingbutton.id = 'loadingbutton';
  document.getElementById('spoofspan').after(loadingbutton);

  
  let post_data = "textselect="+textselect_value;

  const httpRequest = (method, url) => {
 
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
   // xhttp.responseType = 'json';
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    
    xhttp.onload = () => {
      console.log(post_data);
     // console.log(xhttp.responseText);
     if(xhttp.readyState == 4) {
      loadingbutton.remove(); 
      location.reload();
    }
  }
    xhttp.send(post_data);
  }
 
  httpRequest("POST", "delete_text.php");

}

function showDeletion() {
  let spoofspan_display = document.getElementById("spoofspan").style.display;
  if(spoofspan_display == "inline") {
    document.getElementById("spoofspan").style.display = "none";
    document.getElementById("deletion").style.visibility = "visible";
  }
  else if(spoofspan_display == "none"){
    document.getElementById("spoofspan").style.display = "inline";
    document.getElementById("deletion").style.visibility = "hidden";
  }

}

const noun_pos = '<span id="pos_tag_noun" class="pos_tag" onclick="selectPoS()">noun</span>';
const verb_pos = '<span id="pos_tag_verb" class="pos_tag" onclick="selectPoS()">verb</span>';
const adj_pos = '<span id="pos_tag_adj" class="pos_tag" onclick="selectPoS()" title="adjective">adject.</span>';
const adverb_pos = '<span id="pos_tag_adverb" class="pos_tag" onclick="selectPoS()">adverb</span>';
const prep_pos = '<span id="pos_tag_prep" class="pos_tag" title="preposition" onclick="selectPoS()">prep.</span>';
const conj_pos = '<span id="pos_tag_conj" class="pos_tag" title="conjunction" onclick="selectPoS()">conj.</span>';
const part_pos = '<span id="pos_tag_part" class="pos_tag" title="particle/interjection" onclick="selectPoS()">part.</span>';
const ques_pos = '<span id="pos_tag_ques" class="pos_tag" title="interrogative" onclick="selectPoS()">ques.</span>';

const noun_pos_tt = '<span id="pos_tag_noun_tt" class="pos_tag_tt" title="noun"></span>';
const verb_pos_tt = '<span id="pos_tag_verb_tt" class="pos_tag_tt" title="verb"></span>';
const adj_pos_tt = '<span id="pos_tag_adj_tt" class="pos_tag_tt" title="adjective"></span>';
const adverb_pos_tt = '<span id="pos_tag_adverb_tt" class="pos_tag_tt" title="adverb"></span>';
const prep_pos_tt = '<span id="pos_tag_prep_tt" class="pos_tag_tt" title="preposition"></span>';
const conj_pos_tt = '<span id="pos_tag_conj_tt" class="pos_tag_tt" title="conjunction"></span>';
const part_pos_tt = '<span id="pos_tag_part_tt" class="pos_tag_tt" title="particle/interjection"></span>';
const ques_pos_tt = '<span id="pos_tag_ques_tt" class="pos_tag_tt" title="interrogative"></span>';
const quant_pos_tt_elem = '<span id="pos_tag_quant_tt" class="pos_tag_tt" title="quantifier"></span>';
const unassigned_pos_tt_elem = '<span id="pos_tag_unassigned_tt" class="pos_tag_tt" title="unassigned"></span>';

const tt_pos_arr = {1: noun_pos_tt, 2: verb_pos_tt, 3: adj_pos_tt, 4: adverb_pos_tt, 5: prep_pos_tt, 6: conj_pos_tt, 7: part_pos_tt, 8: ques_pos_tt, 9: quant_pos_tt_elem, 10: unassigned_pos_tt_elem};


const noun_pos_tt_elem = document.createRange().createContextualFragment('<span id="pos_tag_noun_tt" class="pos_tag_tt" title="noun"></span>');
const verb_pos_tt_elem = document.createRange().createContextualFragment('<span id="pos_tag_verb_tt" class="pos_tag_tt" title="verb"></span>');
const adj_pos_tt_elem = document.createRange().createContextualFragment('<span id="pos_tag_adj_tt" class="pos_tag_tt" title="adjective"></span>');
const adverb_pos_tt_elem = document.createRange().createContextualFragment('<span id="pos_tag_adverb_tt" class="pos_tag_tt" title="adverb"></span>');
const prep_pos_tt_elem = document.createRange().createContextualFragment('<span id="pos_tag_prep_tt" class="pos_tag_tt" title="preposition"></span>');
const conj_pos_tt_elem = document.createRange().createContextualFragment('<span id="pos_tag_conj_tt" class="pos_tag_tt" title="conjunction"></span>');
const part_pos_tt_elem = document.createRange().createContextualFragment('<span id="pos_tag_part_tt" class="pos_tag_tt" title="particle/interjection"></span>');
const ques_pos_tt_elem = document.createRange().createContextualFragment('<span id="pos_tag_ques_tt" class="pos_tag_tt" title="interrogative"></span>');
// const quant_pos_tt_elem = document.createRange().createContextualFragment('<span id="pos_tag_quant_tt" class="pos_tag_tt" title="quantifier"></span>');
// const unassigned_pos_tt_elem = document.createRange().createContextualFragment('<span id="pos_tag_unassigned_tt" class="pos_tag_tt" title="unassigned"></span>');
// const tt_pos_elements = [noun_pos_tt_elem, verb_pos_tt_elem, adj_pos_tt_elem, adverb_pos_tt_elem, prep_pos_tt_elem, conj_pos_tt_elem, part_pos_tt_elem, ques_pos_tt_elem, quant_pos_tt_elem, unassigned_pos_tt_elem];

const smallscreen_annot_box_html = '<div id="annot_box"><div id="annot_topbar"><div id="close_button"><svg id="red_cross" xmlns="http://www.w3.org/2000/svg" enable-background="new 0 0 24 24" height="24" viewBox="0 0 24 24" width="24" focusable="false"><path fill-rule="evenodd" clip-rule="evenodd" d="M5.29289 5.29289C5.68342 4.90237 6.31658 4.90237 6.70711 5.29289L12 10.5858L17.2929 5.29289C17.6834 4.90237 18.3166 4.90237 18.7071 5.29289C19.0976 5.68342 19.0976 6.31658 18.7071 6.70711L13.4142 12L18.7071 17.2929C19.0976 17.6834 19.0976 18.3166 18.7071 18.7071C18.3166 19.0976 17.6834 19.0976 17.2929 18.7071L12 13.4142L6.70711 18.7071C6.31658 19.0976 5.68342 19.0976 5.29289 18.7071C4.90237 18.3166 4.90237 17.6834 5.29289 17.2929L10.5858 12L5.29289 6.70711C4.90237 6.31658 4.90237 5.68342 5.29289 5.29289Z"/></svg></div><span id="edit_button" title="Edit current text-word">Edit</span><span id="pos_tag_box"><span id="pos_tag_adj" class="pos_tag" title="adjective">adject.</span></span></div><div id="lemma_tag_row"><textarea id="lemma_tag" spellcheck="false">uigennemtrængelig</textarea></div><div id="annot"><div id="right_column"><div id="left_column"><span id="lemma_box" class="box">Lemma</span><span id="multiword_box" class="box">Multiword</span><span id="context_box" class="box" title="not yet implemented">Context translation</span><span id="morph_box" class="box" title="not yet implemented">Morphology</span><span id="accent_box" class="box" title="not yet implemented">Accentology</span></div><div id="right_body"><textarea id="lemma_textarea" autocomplete="off">impenetrable</textarea><div id="meaning_no_box"><div id="meaning_leftarrow" class="nav_arrow">&lt;</div><div id="meaning_no">Meaning <span id="number">1</span></div><div id="meaning_rightarrow" class="nav_arrow">&gt;</div></div></div><div id="right_footer"><div id="save_button">Save</div><div id="delete_lemma_button">Delete</div></div></div></div></div>';

const fullscreen_annot_box_html = '<div id="annot_box"><div id="annot_topbar" ondblclick="makeDraggable()"><span id="close_button" onclick="delAnnotate()">Close</span><span id="edit_button" title="Edit current text-word">Edit</span></div><div id="annot"><div id="left_column"><span id="lemma_box" class="box">Lemma translation</span><span id="multiword_box" class="box">Multiword</span><span id="context_box" class="box" title="not yet implemented">Context translation</span><span id="morph_box" class="box" title="not yet implemented">Morphology</span><span id="accent_box" class="box" title="not yet implemented">Accentology</span></div><div id="right_column"><div id="right_header"><textarea id="lemma_tag" spellcheck="false"></textarea></div><div id="right_body"><textarea id="lemma_textarea" autocomplete="off"></textarea></div><div id="right_footer"><span id="pos_tag_box"></span><div id="meaning_no_box"><div id="meaning_leftarrow" class="nav_arrow"><</div><div id="meaning_no">Meaning <span id="number"></span></div><div id="meaning_rightarrow" class="nav_arrow">></div></div><div id="save_and_delete_box"><div id="save_button">Save</div><div id="delete_lemma_button">Delete</div></div></div></div></div></div>';

function choosePoS(pos_number) {
  let pos_html = noun_pos;
  switch(pos_number) {
    case 1:
      pos_html = noun_pos;
      break;
    case 2:
      pos_html = verb_pos;
      break;
    case 3:
      pos_html = adj_pos;
      break;
    case 4:
      pos_html = adverb_pos;
      break;  
    case 5:
      pos_html = prep_pos;
      break;  
    case 6:
      pos_html = conj_pos;
      break;  
    case 7:
      pos_html = part_pos;
      break;
    case 8:
      pos_html = ques_pos;
      break;  
  }
  return pos_html;
}

const deadFunc = function () {
  let pos_selects = document.querySelectorAll('.pos_tag_select');
  pos_selects.forEach(pos_select => {
    pos_select.remove();
  });
  this.onclick = selectPoS;
};

const changePoS = function () {
  let pullInFunc = () => {};
  switch(annotation_mode){
    case 1:
      pullInFunc = pullInLemma;
      break;
    case 2:
      //pullInFunc = (boolean) => {}; 
      pullInFunc = pullInMultiwordByForm;
      break;
    default:
      pullInFunc = pullInLemma;
  }
  switch (this.id) {
    case "noun_pos":
      document.getElementById('pos_tag_box').innerHTML = noun_pos;
      pos = 1;
      pullInFunc(false);
      break;
    case "verb_pos":
      document.getElementById('pos_tag_box').innerHTML = verb_pos;
      pos = 2;
      pullInFunc(false);
      break;
    case "adj_pos":
      document.getElementById('pos_tag_box').innerHTML = adj_pos;
      pos = 3;
      pullInFunc(false);
      break;  
    case "adverb_pos":
      document.getElementById('pos_tag_box').innerHTML = adverb_pos;
      pos = 4;
      pullInFunc(false);
      break;
    case "prep_pos":
      document.getElementById('pos_tag_box').innerHTML = prep_pos;
      pos = 5;
      pullInFunc(false);
      break;
    case "conj_pos":
      document.getElementById('pos_tag_box').innerHTML = conj_pos;
      pos = 6;
      pullInFunc(false);
      break;  
    case "part_pos":
      document.getElementById('pos_tag_box').innerHTML = part_pos;
      pos = 7;
      pullInFunc(false);
      break;
    case "ques_pos":
      document.getElementById('pos_tag_box').innerHTML = ques_pos;
      pos = 8;
      pullInFunc(false);
      break;
  }
  pos_initial = pos;
};

const selectPoS = function () {
  let pos_tag_current_id = document.querySelector('.pos_tag').id;

  let pos_tag_select_current = "noun_pos";

  switch (pos_tag_current_id) {
    case "pos_tag_noun":
      pos_tag_select_current = "noun_pos";
      break;
    case "pos_tag_verb":
      pos_tag_select_current = "verb_pos";
      break;
    case "pos_tag_adj":
      pos_tag_select_current = "adj_pos";
      break;  
    case "pos_tag_adverb":
      pos_tag_select_current = "adverb_pos";
      break;
    case "pos_tag_prep":
      pos_tag_select_current = "prep_pos";
      break;
    case "pos_tag_conj":
      pos_tag_select_current = "conj_pos";
      break;  
    case "pos_tag_part":
      pos_tag_select_current = "part_pos";
      break;
    case "pos_tag_ques":
      pos_tag_select_current = "ques_pos";
      break;
  }

  let frag = document.createRange().createContextualFragment('<span id="noun_pos" class="pos_tag_select">noun</span><span id="verb_pos" class="pos_tag_select">verb</span><span id="adj_pos" class="pos_tag_select" title="adjective">adject.</span><span id="adverb_pos" class="pos_tag_select">adverb</span><span id="prep_pos" class="pos_tag_select" title="preposition">prep.</span><span id="conj_pos" class="pos_tag_select" title="conjunction">conj.</span><span id="part_pos" class="pos_tag_select" title="particle/interjection">part.</span><span id="ques_pos" class="pos_tag_select" title="interrogative">ques.</span>');

  document.getElementById(pos_tag_current_id).after(frag);
  document.getElementById(pos_tag_current_id).onclick = deadFunc;

  document.getElementById('noun_pos').onclick = changePoS;
  document.getElementById('verb_pos').onclick = changePoS;
  document.getElementById('adj_pos').onclick = changePoS;
  document.getElementById('adverb_pos').onclick = changePoS;
  document.getElementById('prep_pos').onclick = changePoS;
  document.getElementById('conj_pos').onclick = changePoS;
  document.getElementById('part_pos').onclick = changePoS;
  document.getElementById('ques_pos').onclick = changePoS;

  document.getElementById('pos_tag_box').removeChild(document.getElementById(pos_tag_select_current));

}; 

const pullInLemma = function (can_skip = true) {
  
  let lemma_form = document.getElementById('lemma_tag').value.trim();
  if (lemma_form == lemma_form_tag_initial && can_skip) {
    return;
  }
/*  if(lang_id == 5 && pos == 1) {
    lemma_form = lemma_form[0].toUpperCase().concat(lemma_form.slice(1));
  } */
  lemma_form_tag_initial = lemma_form;//TESTING

  document.getElementById('save_button').onclick = "";
  const httpRequest = (method, url) => {
    let send_data = "lemma_form=" + encodeURIComponent(lemma_form) + "&lemma_meaning_no=" + lemma_meaning_no + "&pos=";
    if(pos == pos_initial) {
      send_data += "0&lang_id=" + lang_id;
    }
    else {
      send_data += pos + "&lang_id=" + lang_id;
    }

    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.responseType = 'json';
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');

    xhttp.onload = () => {
      if (xhttp.readyState == 4) {
        let json_response = xhttp.response;
        console.log(json_response);
        lemma_id = Number(json_response.lemma_id);
        meanings = Object.create(null);
        if(lemma_id != 0) {
          
          let new_lemma_textarea_content = json_response.lemma_textarea_content;
          if(new_lemma_textarea_content != "") {
            meanings[lemma_meaning_no] = new_lemma_textarea_content;
          }
          lemma_textarea_content_initial = new_lemma_textarea_content;
          document.getElementById("lemma_textarea").value = new_lemma_textarea_content;

          pos = Number(json_response.pos);
          //German gets case-folded when the lemma-forms are looked up, so an originally lower-case noun-form that succeeds in pulling in a capitalised lemma's definition needs to be capitalised as well 
          if(lang_id == 5 && pos == 1) {
	          lemma_form = lemma_form[0].toUpperCase().concat(lemma_form.slice(1));
            document.getElementById('lemma_tag').value = lemma_form;
	        }
          pos_initial = pos;
          document.getElementById('pos_tag_box').innerHTML = choosePoS(pos);
        }
        document.getElementById('save_button').onclick = lemmaRecord;
      }
    }
    xhttp.send(send_data);
  }
  httpRequest("POST", "pull_lemma.php");
};

function switchMeaningAJAX() {
  const httpRequest = (method, url) => {
    let send_data = "lemma_id=" + lemma_id + "&lemma_meaning_no=" + lemma_meaning_no;

    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.responseType = 'text';
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');

    xhttp.onload = () => {
      if (xhttp.readyState == 4) {
        //let json_response = xhttp.response;
        let response_meaning = xhttp.response.trim();
        //console.log(response_meaning);
        //console.log(json_response);
        if(response_meaning != "") {
          meanings[lemma_meaning_no] = response_meaning;
        }
        document.getElementById("lemma_textarea").value = meanings[lemma_meaning_no] == undefined ? "" : meanings[lemma_meaning_no];
      }
    }
    xhttp.send(send_data);
  }
  httpRequest("POST", "retrieve_meanings.php");
}

const switchMeaning = function (event) {
  if(event.target.matches('.nav_arrow_deactiv')) return;
  let grey_arrows = document.querySelectorAll('.nav_arrow_deactiv');
  grey_arrows.forEach(grey_arrow => {
    grey_arrow.classList.add("nav_arrow");
    grey_arrow.classList.remove("nav_arrow_deactiv");
  });

  let bool_uparrow = event.target.id == "meaning_rightarrow" ? true : false;
  if (bool_uparrow && lemma_meaning_no < 10) {
    if(document.getElementById("lemma_textarea").value.trim() != "" || meanings[lemma_meaning_no] != undefined) {
      meanings[lemma_meaning_no] = document.getElementById("lemma_textarea").value;
    }
    lemma_meaning_no++;
  }
  else if (bool_uparrow == false && lemma_meaning_no > 1) {
    if(document.getElementById("lemma_textarea").value.trim() != "" || meanings[lemma_meaning_no] != undefined) {
      meanings[lemma_meaning_no] = document.getElementById("lemma_textarea").value;
    }
    lemma_meaning_no--;
  }
  document.getElementById("number").innerHTML = lemma_meaning_no;
  
  if (lemma_meaning_no == 10) {
    document.getElementById("meaning_rightarrow").classList.add("nav_arrow_deactiv");
    document.getElementById("meaning_rightarrow").classList.remove("nav_arrow");
  }
  if (lemma_meaning_no == 1) {
    document.getElementById("meaning_leftarrow").classList.add("nav_arrow_deactiv");
    document.getElementById("meaning_leftarrow").classList.remove("nav_arrow");
  }
  if (lemma_id != 0 && meanings[lemma_meaning_no] === undefined) {
    switchMeaningAJAX();
  }

  document.getElementById("lemma_textarea").value = meanings[lemma_meaning_no] == undefined ? "" : meanings[lemma_meaning_no];
  document.getElementById("lemma_textarea").focus();
  
};

const disRegard = function () {
  const httpRequest = (method, url) => {


    let send_data = "tokno_current=" + tokno_current + "&word_engine_id=" + word_engine_id;

    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.responseType = 'json';
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');

    xhttp.onload = () => {
      console.log("sent");
      // console.log(xhttp.responseText);
      if (xhttp.readyState == 4 && xhttp.status == 204) {
        let word_element = document.querySelector("[data-tokno='"+tokno_current+"']");
        word_element.outerHTML = word_element.firstChild.textContent;

        delAnnotate();
      }
    }
    xhttp.send(send_data);
  }

  httpRequest("POST", "disregard_word.php");
};

const lemmaRecord = function () {
  if(document.getElementById("lemma_textarea").value.trim() != "" || meanings[lemma_meaning_no] != undefined) {
    meanings[lemma_meaning_no] = document.getElementById("lemma_textarea").value.trim();
  }

  let skip = true;
  for(let lemma_meaning_no in meanings) {
    if(meanings[lemma_meaning_no].trim() != "") {
      skip = false;
    }
  }
  if(skip) return;
  
  let clicked_lemma_meaning_no = lemma_meaning_no;
  let meanings_length = Object.keys(meanings).length;
  
  let lemma_form_trimmed = document.getElementById('lemma_tag').value.trim();
  //auto-capitalise German nouns (will mess up words like 'iPhone' and 'eTicket', but those are unlikely to need translations)
  if(lang_id == 5 && pos == 1) {
    lemma_form_trimmed = lemma_form_trimmed[0].toUpperCase().concat(lemma_form_trimmed.slice(1));
  }
  const lemma_form = encodeURIComponent(lemma_form_trimmed);
  
  let count = 1;

  for (let lemma_meaning_no in meanings) {
    let lemma_meaning = meanings[lemma_meaning_no];
    const httpRequest = (method, url) => {
      lemma_meaning = encodeURIComponent(lemma_meaning);

      let send_data = "word_engine_id=" + word_engine_id + "&lemma_form=" + lemma_form + "&lemma_meaning=" + lemma_meaning + "&lemma_meaning_no=" + lemma_meaning_no + "&lang_id=" + lang_id + "&tokno_current=" + tokno_current + "&pos=" + pos +"&clicked_lemma_meaning_no=" + clicked_lemma_meaning_no;

      const xhttp = new XMLHttpRequest();
      xhttp.open(method, url, true);

      xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');

      xhttp.onload = () => {
        console.log("sent");
        // console.log(xhttp.responseText);
        if (xhttp.readyState == 4) {
          console.log("Lemma updated");      
        
          document.querySelector('[data-tokno="'+tokno_current+'"]').classList.add("lemma_set");
          let dataselectorstring = '[data-word_engine_id="' + word_engine_id + '"]';
          let current_words = document.querySelectorAll(dataselectorstring);
          current_words.forEach(current_word => {
            current_word.classList.add("lemma_set_unexplicit");
          });
          console.log("meanings_lengths: ", meanings_length); //remove
          console.log("count: ", count); //remove
          if(tooltips_shown == true && count == meanings_length) {
            lemmaRecordTooltipUpdate(current_words); //this being repeated for every meaning{} sometimes causes an issue with double tooltips; the lemma_record.php scripts are fired off without waiting for the previous one to return. possibly should get length of the meanings{} array and only run this on the final iteration (DONE)
          }
          count++;
        }
      }
      xhttp.send(send_data);
    }

    httpRequest("POST", "lemma_record.php");
  }
  delAnnotate();
};

const lemmaDelete = function () {
  const httpRequest = (method, url) => {

    let send_data = "lemma_id="+lemma_id+"&word_engine_id="+word_engine_id+"&tokno_current="+tokno_current;

    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    xhttp.responseType = 'text';

    xhttp.onload = () => {
      if(xhttp.readyState == 4) {
        let lemma_still_set = xhttp.responseText.trim() == "0" ? false : true;

        console.log('Lemma deleted');
       /* document.getElementById('annot_box').remove();
        display_word.classList.add("tooltip");
        display_word.classList.remove("tooltip_selected"); */
        if(lemma_still_set == false) {
          let dataselectorstring = '[data-word_engine_id="' + word_engine_id + '"]';
          let current_words = document.querySelectorAll(dataselectorstring);
          current_words.forEach(current_word => {
            current_word.classList.remove("lemma_set");
            current_word.classList.remove("lemma_set_unexplicit");
          });     
        }
        else {
          document.querySelector('[data-tokno="'+tokno_current+'"]').classList.remove("lemma_set");
        }
        delAnnotate();

        if(tooltips_shown) {
          lemmaTooltipMW();
        }
      }
    }
    xhttp.send(send_data);
  };
  httpRequest("POST", "lemma_delete.php");
  //meanings = {};
};


const setLemmaTagSize = function () {
  let lemma_tag = document.getElementById('lemma_tag');
  let hidden_lemma_tag = document.getElementById('hidden_lemma_tag');

  hidden_lemma_tag.textContent = lemma_tag.value;

  let new_width = hidden_lemma_tag.offsetWidth+"px";
  let new_height = hidden_lemma_tag.offsetHeight+"px";
  lemma_tag.style.width = new_width;
  lemma_tag.style.height = new_height;
};

const lemmaTooltipMW = function () {
  let lemma_tooltips = document.querySelectorAll('.lemma_tt, .mw_tt');
  lemma_tooltips.forEach(lemma_tooltip => {
    lemma_tooltip.remove();
  });

  const lemma_set_words = Array.from(document.getElementsByClassName('lemma_set_unexplicit'));
  const mw_set_words = document.querySelectorAll(".multiword");
  
  let lemma_set_toknos = new Array();
  let lemma_set_word_eng_ids = new Array();

  //let mw_toknos = new Array();
  
  let mw_first_toknos = new Array();
  let mw_first_word_eng_ids = new Array();
  
  lemma_set_words.forEach(lemma_set_word => {
    let lemma_set_tokno = lemma_set_word.dataset.tokno;
    let lemma_set_word_eng_id = lemma_set_word.dataset.word_engine_id;
    lemma_set_toknos.push(lemma_set_tokno);
    lemma_set_word_eng_ids.push(lemma_set_word_eng_id);
  
  });

  const mw_count_set = new Set();
  mw_set_words.forEach(mw_word => {
    const mw_count = Number(mw_word.dataset.multiword);
    if(mw_count_set.has(mw_count) == false) {
        mw_count_set.add(mw_count);
        mw_first_toknos.push(mw_word.dataset.tokno);
        mw_first_word_eng_ids.push(mw_word.dataset.word_engine_id); 
    }
    //mw_toknos.push(mw_word.dataset.tokno);
  });
  
  let mw_first_words = [];
  mw_first_toknos.forEach(tokno => {
    mw_first_words.push(document.querySelector("[data-tokno='"+tokno+"']"));
  });

  const httpRequest = (method, url) => {

    // let send_data = toknos_POST_data;
    let send_data = "toknos=" + lemma_set_toknos + "&word_eng_ids=" + lemma_set_word_eng_ids + "&mw_first_toknos=" + mw_first_toknos;
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    xhttp.responseType = 'json';

    xhttp.onload = () => {
      if(xhttp.readyState == 4) {
        tooltips_shown = true;
        const json_lemma_transes = xhttp.response[0];
        const json_mw_transes = xhttp.response[1];
       // console.log(json_lemma_transes);
        if(json_lemma_transes == null) {
         document.getElementById("tt_toggle").disabled = false;
         return;
        }
        let i = 0;
        //const lemma_set_only_words = lemma_set_words.filter(x => mw_first_toknos.includes(x.dataset.tokno) == false); 
        lemma_set_words.forEach(lemma_set_word => {
          if(mw_first_toknos.includes(lemma_set_toknos[i]) == false) {
            const json_pos = Number(json_lemma_transes[i].pos);

            let lemma_tt_box = '<span class="lemma_tt" onclick="event.stopPropagation()"><span id="tt_top"><div class="lemma_tag_tt">'+json_lemma_transes[i].lemma_form+'</div><span id="pos_tag_box_tt">'+tt_pos_arr[json_pos]+'</span></span><span id="tt_mid"><div id="tt_meaning">'+json_lemma_transes[i].lemma_trans+'</div></span><span id="tt_bottom"></span></span>';
  
            lemma_set_word.innerHTML = lemma_set_word.innerHTML + lemma_tt_box;
          }
          i++;
        });
        i = 0;
        mw_first_words.forEach(mw_set_word => {
          const json_mw_pos = Number(json_mw_transes[i].mw_pos);
          
          let mw_tt_box = '<span class="mw_tt" data-mw_active="1" onclick="event.stopPropagation()">'
          //if(lemma_set_toknos.includes(mw_first_toknos[i]))mw_tt_box += '<span class="appendages append_inact append_act" id="append1">M</span>    <span class="appendages append_inact" id="append2">L</span>';
          mw_tt_box += '<span id="tt_top"><div class="lemma_tag_tt">'+json_mw_transes[i].mw_lemma_form+'</div><span id="pos_tag_box_tt">'+tt_pos_arr[json_mw_pos]+'</span></span><span id="tt_mid"><div id="tt_meaning">'+json_mw_transes[i].mw_lemma_trans+'</div></span><span id="tt_bottom"></span></span>'
            
          mw_set_word.innerHTML = mw_set_word.innerHTML + mw_tt_box;
          i++;
            
        });
        
        
        document.getElementById("tt_toggle").disabled = false;
        setTimeout(ttPosition, 200);
      }

    }
    xhttp.send(send_data);
  };
  httpRequest("POST", "lemma_tooltip_mw.php");

};

//need to write an equivalent for multiword_recording to update mw_tooltips
const lemmaRecordTooltipUpdate = function (current_words) {
  current_words.forEach(current_word => {
    let current_lemma_tt = current_word.querySelector('.lemma_tt');
    if(current_lemma_tt != null) {
      current_lemma_tt.remove();
    }
  });

  let current_toknos = new Array();
  let current_word_eng_ids = new Array();
  current_words.forEach(current_word => {
    let current_tokno = current_word.dataset.tokno;
    let current_word_eng_id = current_word.dataset.word_engine_id;
    current_toknos.push(current_tokno);
    current_word_eng_ids.push(current_word_eng_id);
  });

  const httpRequest = (method, url) => {

    // let send_data = toknos_POST_data;
    let send_data = "toknos=" + current_toknos + "&word_eng_ids=" + current_word_eng_ids;
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    xhttp.responseType = 'json';

    xhttp.onload = () => {
      if (xhttp.readyState == 4) {
        tooltips_shown = true;
        const json_lemma_transes = xhttp.response;
       // console.log(json_lemma_transes);
        if (json_lemma_transes == null) {
          return;
        }
        let i = 0;
        current_words.forEach(current_word => {
          const json_pos = Number(json_lemma_transes[i].pos);

          let lemma_tt_box = '<span class="lemma_tt" onclick="event.stopPropagation()"><span id="tt_top"><div class="lemma_tag_tt">' + json_lemma_transes[i].lemma_form + '</div><span id="pos_tag_box_tt">' + tt_pos_arr[json_pos] + '</span></span><span id="tt_mid"><div id="tt_meaning">' + json_lemma_transes[i].lemma_trans + '</div></span><span id="tt_bottom"></span></span>';

          current_word.innerHTML = current_word.innerHTML + lemma_tt_box;
          i++;
        });
      }

    }
    xhttp.send(send_data);
  };
  httpRequest("POST", "lemma_tooltip.php");
};

const singleTooltipUpdate = function (single_word) {
  
  if(!single_word.classList.contains('lemma_set_unexplicit')) return;

  let current_lemma_tt = single_word.querySelector('.lemma_tt');
  if(current_lemma_tt != null) {
    current_lemma_tt.remove();
  }

  let single_tokno = single_word.dataset.tokno;
  let single_word_eng_id = single_word.dataset.word_engine_id;

  const httpRequest = (method, url) => {

    // let send_data = toknos_POST_data;
    let send_data = "toknos=" + single_tokno + "&word_eng_ids=" + single_word_eng_id;
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    xhttp.responseType = 'json';

    xhttp.onload = () => {
      if (xhttp.readyState == 4) {
        tooltips_shown = true;
        const json_lemma_transes = xhttp.response;
       // console.log(json_lemma_transes);
        if (json_lemma_transes == null) {
          return;
        }
        let i = 0;
        const json_pos = Number(json_lemma_transes[i].pos);
        let lemma_tt_box = '<span class="lemma_tt" onclick="event.stopPropagation()"><span id="tt_top"><div class="lemma_tag_tt">' + json_lemma_transes[i].lemma_form + '</div><span id="pos_tag_box_tt">' + tt_pos_arr[json_pos] + '</span></span><span id="tt_mid"><div id="tt_meaning">' + json_lemma_transes[i].lemma_trans + '</div></span><span id="tt_bottom"></span></span>';
        single_word.innerHTML = single_word.innerHTML + lemma_tt_box;
      }

    }
    xhttp.send(send_data);
  };
  httpRequest("POST", "lemma_tooltip.php");
};


let pos = 1;
let lemma_form_tag_initial = "";
let lemma_textarea_content_initial = "";
let lemma_meaning_no = 1;
let lemma_id = 0;
let meanings = Object.create(null);

let multiword_meanings = Object.create(null);
let multiword_indices = Object.create(null);
let multiword_id = 0;
let multiword_meaning_no = 1;
//let multiword_tag_initial = "";

let tooltips_shown = false;
let pos_initial = 1;

let display_word = null;
let tokno_current = 0;
let word_engine_id = 0;

let annotation_mode = 0;
///////////////////////////////

function showAnnotate(event) {
  if(window.innerWidth < 769) return;

  if(event.target.classList.contains("tooltip") == false || event.target.classList.contains("multiword") || event.target.classList.contains("tooltip_selected") || event.target.classList.contains("mw_selectable") || displayWordEditor.edit_mode) return;

  if(display_word != null) delAnnotate();
  display_word = event.target;
  //display_word.onclick = "";
  display_word.classList.add("tooltip_selected");
  display_word.classList.remove("tooltip");
  tokno_current = display_word.dataset.tokno;
  word_engine_id = display_word.dataset.word_engine_id;
  
  fetchLemmaData(false);
};


const fetchLemmaData = function (box_present = true) {
  delAnnotate(false);
  annotation_mode = 1;
  meanings = Object.create(null);
  const httpRequest = (method, url) => {
    let send_data = "word_engine_id="+word_engine_id+"&tokno_current="+tokno_current+"&lang_id="+lang_id;
 
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.responseType = 'json';
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    xhttp.onload = () => {
      console.log("sent");
      //setTimeout(() => {
      if(xhttp.readyState == 4)  {
       
        if(!box_present) boxFunction(annotation_mode);
        let json_response = xhttp.response;
        pos = Number(json_response.pos);
        pos_initial = pos;
        lemma_id = Number(json_response.lemma_id);
        lemma_meaning_no = Number(json_response.lemma_meaning_no);
        let lemma_textarea_content = json_response.lemma_textarea_content;
        lemma_textarea_content_initial = lemma_textarea_content;  
        
        show_delete_button = false;
        if(lemma_meaning_no != 0) {
          meanings[lemma_meaning_no] = lemma_textarea_content;
          show_delete_button = true;
        }
        else {
          lemma_meaning_no = 1;
        }

        let lemma_tag_content = json_response.lemma_tag_content;
        //capitalise German nouns which aren't annotated but whose casefolded forms match existing noun lemmas
        if(lang_id == 5 && lemma_id != 0 && pos == 1 && show_delete_button == false) {
          lemma_tag_content = lemma_tag_content[0].toUpperCase().concat(lemma_tag_content.slice(1));
        }

        lemma_form_tag_initial = lemma_tag_content;
        
        //let lemma_textarea_content_html = json_response.lemma_textarea_content_html;
        
        document.getElementById('pos_tag_box').innerHTML = choosePoS(pos);
        document.getElementById("number").innerHTML = lemma_meaning_no;     
        document.getElementById('lemma_tag').value = lemma_tag_content;
        setLemmaTagSize();
        document.getElementById('lemma_textarea').value = lemma_textarea_content; //might be able to get rid of _html versions on back and frontend doing it this way

        document.getElementById('lemma_tag').focus();

        reactivateArrows(lemma_meaning_no, 10);

        if(lemma_id == 0 || show_delete_button == false) {
          document.getElementById('delete_lemma_button').style.display = "none";
        }
        else {
          document.getElementById('delete_lemma_button').style.display = "";
        }
        
        document.getElementById('delete_lemma_button').onclick = lemmaDelete;  
        document.getElementById('edit_button').onclick = displayWordEditor.startEditing;
        toggleSave(true, lemmaRecord);
        //document.getElementById('save_button').onclick = lemmaRecord;
        document.getElementById('meaning_leftarrow').onclick = switchMeaning;
        document.getElementById('meaning_rightarrow').onclick = switchMeaning;
        document.getElementById('lemma_tag').onblur = pullInLemma;

        document.getElementById("lemma_tag").addEventListener("keydown", dict.lookUpMouseSelection); //DELETE
      } //}, 1300);
    }
    xhttp.send(send_data);
  }
  httpRequest("POST", "retrieve_engword.php");
};

const htmlSpecialChars = (unescaped) => {
  let escaped = "";
  for(const c of unescaped) {
    switch(c) {
      case '<':
          escaped += "&lt;";
          break;
      case '>':
          escaped += "&gt;";
          break;
      case '"':
          escaped += "&quot;";
          break;
      case '\'':
          escaped += "&#039;";
          break;
      case '&':
          escaped += "&amp;";
          break;
      default:
        escaped += c;
    }
  }
  return escaped;
};

const recordMultiword = function () {
  if(document.getElementById("lemma_textarea").value.trim() != "" || multiword_meanings[multiword_meaning_no] != undefined) {
    multiword_meanings[multiword_meaning_no] = document.getElementById("lemma_textarea").value.trim();
  }
  
  //let clicked_meaning_no = multiword_meaning_no;
  let mw_meanings_length = Object.keys(multiword_meanings).length;
  let prev_mw_count = display_word.dataset.multiword;
  let count = 1;
 
  let mw_meaning = multiword_meanings[multiword_meaning_no];
  if(mw_meaning == undefined) {
    mw_meaning = "";
  }
  const tt_mw_lemma_form = htmlSpecialChars(document.getElementById('lemma_tag').value.trim());
  const tt_mw_meaning = htmlSpecialChars(mw_meaning);

  const httpRequest = (method, url) => {

    let multiword_lemma_form = encodeURIComponent(document.getElementById('lemma_tag').value.trim());
    mw_meaning = encodeURIComponent(mw_meaning);
    let word_eng_ids = Object.values(multiword_indices).toString();
    let toknos = Object.keys(multiword_indices).toString();

    let send_data = "word_eng_ids="+word_eng_ids +"&toknos="+toknos+ "&multiword_lemma_form=" + multiword_lemma_form + "&multiword_lemma_meaning=" + mw_meaning + "&multiword_meaning_no=" + multiword_meaning_no + "&pos="+pos + "&lang_id="+lang_id + "&anchor_tokno=" + tokno_current;

    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);

    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    xhttp.responseType = 'text';
    xhttp.onload = () => {  
      console.log("sent");
      // console.log(xhttp.responseText);
      if (xhttp.readyState == 4) {
        console.log("Multiword updated");
        let response_numbers = xhttp.response.trim().split(",");
        console.log(response_numbers);
        let new_mw_count = response_numbers[0];
        let new_mw_id = response_numbers[1];

        for (let mw_meaning_no in multiword_meanings) {
          if(mw_meaning_no == multiword_meaning_no) continue;
          let eng_trans = multiword_meanings[mw_meaning_no];
          updateMultiwordTranslations(new_mw_id, mw_meaning_no, eng_trans);
          console.log(mw_meaning_no, ": ", eng_trans);
        }

        let dataselectorstring = '[data-multiword="' + prev_mw_count + '"]';
        const prev_multiwords = document.querySelectorAll(dataselectorstring);
        prev_multiwords.forEach(prev_mw => {
          prev_mw.removeAttribute('data-multiword');
          prev_mw.classList.remove("multiword");
        });

        document.querySelectorAll('.mw_current_select').forEach(mwc => {  
          mwc.classList.add("multiword");
          mwc.setAttribute("data-multiword", new_mw_count);
        });
        console.log("mw_meanings_lengths: ", mw_meanings_length); //remove
        console.log("count: ", count); //remove

        const new_multiwords = document.querySelectorAll(`[data-multiword="${new_mw_count}"`);
        new_multiwords.forEach(multiword => {
          multiword.addEventListener('click', showMultiwordAnnotate);
          multiword.addEventListener('mouseover', underlineMultiwords);
          multiword.addEventListener('mouseout', removeUnderlineMultiwords);
        });
        if(tooltips_shown == true) {
          if(prev_multiwords[0] != undefined) prev_multiwords[0].querySelector('.mw_tt').remove();
          
          let mw_tt_box = '<span class="mw_tt" data-mw_active="1" onclick="event.stopPropagation()">'
          mw_tt_box += '<span id="tt_top"><div class="lemma_tag_tt">'+tt_mw_lemma_form+'</div><span id="pos_tag_box_tt">'+tt_pos_arr[pos]+'</span></span><span id="tt_mid"><div id="tt_meaning">'+tt_mw_meaning+'</div></span><span id="tt_bottom"></span></span>';

          new_multiwords[0].innerHTML = new_multiwords[0].innerHTML + mw_tt_box;
        }
        delAnnotate();
      }
    }
    xhttp.send(send_data);
  };

  httpRequest("POST", "record_multiword.php");
};

const updateMultiwordTranslations = (new_mw_id, mw_meaning_no, eng_trans) => {
  const httpRequest = (method, url) => {
    let send_data = "multiword_id="+new_mw_id + "&multiword_meaning_no="+mw_meaning_no + "&mw_meaning="+encodeURIComponent(eng_trans);
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');

    xhttp.onload = () => {
      if (xhttp.readyState == 4) {
        console.log(`updated mw_trans${mw_meaning_no}`);
      }
    }
    xhttp.send(send_data);
  }
  httpRequest("POST", "update_MW_translations.php");
};

const deleteMultiword = function () {
  const httpRequest = (method, url) => {
    let prev_mw_count = display_word.dataset.multiword;

    let send_data = "multiword_id="+multiword_id+"&word_eng_ids="+Object.values(multiword_indices).toString()+"&toknos="+Object.keys(multiword_indices).toString()+"&lang_id="+lang_id;
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');

    xhttp.onload = () => {
      if(xhttp.readyState == 4) {

        delAnnotate();

        let dataselectorstring = '[data-multiword="' + prev_mw_count + '"]';
        const prev_multiwords = document.querySelectorAll(dataselectorstring);
        prev_multiwords.forEach(prev_mw => {
          prev_mw.removeAttribute('data-multiword');
          prev_mw.classList.remove("multiword");
          //prev_mw.onclick = showAnnotate;
          prev_mw.removeEventListener('click', showMultiwordAnnotate);
        });
        if(tooltips_shown && prev_multiwords[0] != undefined) {
          prev_multiwords[0].querySelector('.mw_tt').remove();
          singleTooltipUpdate(prev_multiwords[0]);
        }

      }
    }
    xhttp.send(send_data);
  };
  httpRequest("POST", "delete_multiword.php");
};

const switchMultiwordMeanings = function(event) {
  if(event.target.matches('.nav_arrow_deactiv')) return;
  let grey_arrows = document.querySelectorAll('.nav_arrow_deactiv');
  grey_arrows.forEach(grey_arrow => {
    grey_arrow.classList.add("nav_arrow");
    grey_arrow.classList.remove("nav_arrow_deactiv");
  });

  let bool_uparrow = event.target.id == "meaning_rightarrow" ? true : false;
  if (bool_uparrow && multiword_meaning_no < 5) {
    if(document.getElementById("lemma_textarea").value.trim() != "" || multiword_meanings[multiword_meaning_no] != undefined) {
      multiword_meanings[multiword_meaning_no] = document.getElementById("lemma_textarea").value;
    }
    multiword_meaning_no++;
  }
  else if (bool_uparrow == false && multiword_meaning_no > 1) {
    if(document.getElementById("lemma_textarea").value.trim() != "" || multiword_meanings[multiword_meaning_no] != undefined) {
      multiword_meanings[multiword_meaning_no] = document.getElementById("lemma_textarea").value;
    }
    multiword_meaning_no--;
  }
  document.getElementById("number").innerHTML = multiword_meaning_no;
  
  if (multiword_meaning_no == 5) {
    document.getElementById("meaning_rightarrow").classList.add("nav_arrow_deactiv");
    document.getElementById("meaning_rightarrow").classList.remove("nav_arrow");
  }
  if (multiword_meaning_no == 1) {
    document.getElementById("meaning_leftarrow").classList.add("nav_arrow_deactiv");
    document.getElementById("meaning_leftarrow").classList.remove("nav_arrow");
  }
  if (multiword_id != 0 && multiword_meanings[multiword_meaning_no] === undefined) {
    switchMultiwordMeaningAJAX();
  }

  document.getElementById("lemma_textarea").value = multiword_meanings[multiword_meaning_no] == undefined ? "" : multiword_meanings[multiword_meaning_no];
  document.getElementById("lemma_textarea").focus();
};

const switchMultiwordMeaningAJAX = function() {
  const httpRequest = (method, url) => {
    let send_data = "multiword_id=" + multiword_id + "&multiword_meaning_no=" + multiword_meaning_no;

    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.responseType = 'text';
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');

    xhttp.onload = () => {
      if (xhttp.readyState == 4) {
        //let json_response = xhttp.response;
        let response_meaning = xhttp.response.trim();
        //console.log(response_meaning);
        //console.log(json_response);
        if(response_meaning != "") {
          multiword_meanings[multiword_meaning_no] = response_meaning;
        }
        document.getElementById("lemma_textarea").value = multiword_meanings[multiword_meaning_no] == undefined ? "" : multiword_meanings[multiword_meaning_no];
      }
    }
    xhttp.send(send_data);
  }
  httpRequest("POST", "retrieve_MW_meanings.php");
};

const pullInMultiword = function(can_skip = true, word_eng_ids) {
 
  const httpRequest = (method, url) => {
    let send_data = "word_eng_ids="+word_eng_ids+"&lang_id="+lang_id;
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.responseType = 'json';
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    xhttp.onload = () => {
      if(xhttp.readyState == 4) {
        let json_response = xhttp.response;

        multiword_id = Number(json_response.multiword_id);
        
        if(multiword_id == 0) {
          //Add in call to pullInMultiwordByForm here to ensure that it runs once to check that you havent selected the lemma-form of a multiword which has only previously been recorded in an inflected form
          pullInMultiwordByForm();
          //lemma_form_tag_initial gets set by the above function
          return;
        }
        let multiword_tag_content = json_response.multiword_tag_content;
        let multiword_textarea_content = json_response.multiword_textarea_content;
        pos = Number(json_response.pos);
        pos_initial = pos;
        multiword_meaning_no = 1;
        
        multiword_meanings = Object.create(null);
        multiword_meanings[multiword_meaning_no] = multiword_textarea_content;

        document.getElementById('pos_tag_box').innerHTML = choosePoS(pos);
        document.getElementById("number").innerHTML = multiword_meaning_no;
        reactivateArrows(multiword_meaning_no, 5);
        document.getElementById('lemma_tag').value = multiword_tag_content;
        lemma_form_tag_initial = multiword_tag_content;
        setLemmaTagSize();
        document.getElementById('lemma_textarea').value = multiword_textarea_content;

      }
    }
    xhttp.send(send_data);
  }

  httpRequest("POST", "pull_multiword.php");

};

const toggleSave = (on, recordFunc) => {
  if(on == false) {
    document.getElementById("save_button").classList.add("save_button_deactiv");
    document.getElementById("save_button").onclick = "";
    console.log("toggleSave off");
  }
  else {
    document.getElementById("save_button").classList.remove("save_button_deactiv");
    document.getElementById("save_button").onclick = recordFunc;
    console.log("toggleSave on");
  }
};

const selectMultiword = (event) => {
  let mw_candidate = event.target;
  let mw_tokno = mw_candidate.dataset.tokno;
  let no_of_mwc = Object.keys(multiword_indices).length;

  if(mw_candidate.matches('.mw_current_select')) {
    mw_candidate.classList.remove("mw_current_select");
    let mw_candidate_text = "";
    document.querySelectorAll('.mw_current_select').forEach(mwc => {
      mw_candidate_text = mw_candidate_text + mwc.firstChild.textContent.trim() + " ";
    });
    document.getElementById("lemma_tag").value = mw_candidate_text.trim();
    setLemmaTagSize();
    document.getElementById("lemma_tag").focus();
    if(no_of_mwc == 2) toggleSave(false, recordMultiword);    
    delete multiword_indices[mw_tokno];

    let word_eng_ids = Object.values(multiword_indices);
    pullInMultiword(false, word_eng_ids);
  }
  else if(no_of_mwc < 10) {
    mw_candidate.classList.add("mw_current_select");
    let mw_candidate_text = "";
    document.querySelectorAll('.mw_current_select').forEach(mwc => {
      mw_candidate_text = mw_candidate_text + mwc.firstChild.textContent.trim() + " ";
    });
    document.getElementById("lemma_tag").value = mw_candidate_text.trim();
    setLemmaTagSize();
    document.getElementById("lemma_tag").focus();
    if(no_of_mwc == 1) toggleSave(true, recordMultiword);
    multiword_indices[mw_tokno] = mw_candidate.dataset.word_engine_id;

    let word_eng_ids = Object.values(multiword_indices);
    pullInMultiword(false, word_eng_ids);
  }
};

const showMultiwordAnnotate = (event) => {
  if(window.innerWidth < 769) return;
  if(event.target.classList.contains("tooltip_selected") || event.target.classList.contains("mw_selectable") || displayWordEditor.edit_mode) return;
  if(display_word != null) delAnnotate();
  display_word = event.target;
  //display_word.onclick = "";
  display_word.classList.add("tooltip_selected");
  display_word.classList.remove("tooltip");
  tokno_current = display_word.dataset.tokno;
  word_engine_id = display_word.dataset.word_engine_id;
  fetchMultiwordData(false);
};

const boxFunction = function (annotation_mode = 1) {
  displayAnnotBox();
  switch(annotation_mode) {
    case(1):
      document.getElementById("lemma_box").classList.add("current_box");
      break;
    case(2):
      document.getElementById("multiword_box").classList.add("current_box");
      break;
    case(3):
      document.getElementById("context_box").classList.add("current_box");
      break;
    case(4):
      document.getElementById("morph_box").classList.add("current_box");
      break;
    case(5):
      document.getElementById("accent_box").classList.add("current_box");
      break;
  }   
  let current_box = document.querySelector('.current_box');
  let left_column = document.getElementById('left_column');

  function selectBox(box) {
    if(current_box) {
      current_box.classList.remove("current_box");
    }
    current_box = box;
    current_box.classList.add("current_box");
    if(current_box.id == "lemma_box") {
      document.getElementById("right_body").style.visibility = "visible";
      document.getElementById("right_footer").style.visibility = "visible";
      document.getElementById('lemma_textarea').focus();
      fetchLemmaData();
    }
    else if(current_box.id == "multiword_box") {
      document.getElementById("right_body").style.visibility = "visible";
      document.getElementById("right_footer").style.visibility = "visible";
      document.getElementById('lemma_textarea').focus();
      fetchMultiwordData();
    }
    else {
      document.getElementById("right_body").style.visibility = "hidden";
      document.getElementById("right_footer").style.visibility = "hidden";
    }
  }
  left_column.onclick = function (event) {
    let target = event.target;
    if (target.className != 'box') return;
    selectBox(target);
  };
  document.getElementById('lemma_tag').oninput = setLemmaTagSize;
  setLemmaTagSize();
};

const fetchMultiwordData = function (box_present = true) {
  if(document.getElementById('annot_box') != null) delAnnotate(false);
  annotation_mode = 2;
  meanings = Object.create(null);
  multiword_meanings = Object.create(null);
  multiword_indices = Object.create(null);

  let current_mw_number = display_word.dataset.multiword;
 
  const httpRequest = (method, url) => {

    let send_data = "word_engine_id="+word_engine_id+"&tokno_current="+tokno_current+"&lang_id="+lang_id;
 
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.responseType = 'json';
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    xhttp.onload = () => {
      console.log("sent");
      if(xhttp.readyState == 4)  {
        if(!box_present) boxFunction(annotation_mode);

        let json_response = xhttp.response;
        let multiword_tag_content = json_response.multiword_tag_content;
        if(multiword_tag_content == "") multiword_tag_content = display_word.firstChild.textContent.trim();
        lemma_form_tag_initial = multiword_tag_content;
        let multiword_textarea_content = json_response.multiword_textarea_content;

        //multiword_tag_initial = multiword_tag_content;
        
        multiword_meaning_no = Number(json_response.multiword_meaning_no);
        multiword_id = Number(json_response.multiword_id);
        pos = Number(json_response.pos);
        pos_initial = pos;
        let adjacent_toknos = json_response.adjacent_toknos;
        //console.log(adjacent_toknos);

        if(current_mw_number != undefined) {
          document.querySelectorAll('[data-multiword="'+current_mw_number+'"]').forEach(current_mw => {
            multiword_indices[current_mw.dataset.tokno] = current_mw.dataset.word_engine_id;
            current_mw.classList.add("mw_current_select");
            current_mw.style.borderBottom = "";
            current_mw.removeEventListener('click', showMultiwordAnnotate);
            current_mw.removeEventListener('mouseover', underlineMultiwords);
            current_mw.removeEventListener('mouseout', removeUnderlineMultiwords);
          });
          toggleSave(true, recordMultiword);
        }
        else {
          display_word.classList.add("mw_current_select");
          multiword_indices[display_word.dataset.tokno] = display_word.dataset.word_engine_id;
          toggleSave(false, recordMultiword);
        }

        for(let adjacent_tokno of adjacent_toknos) {
          let wrd = document.querySelector('[data-tokno="'+adjacent_tokno+'"]');
          //the adjacent_toknos could include words from the next page which will make the querySelector return null
          if(wrd != null && (wrd.dataset.multiword == undefined || wrd.dataset.multiword == current_mw_number)) {
            wrd.classList.add("mw_selectable");
            //wrd.onclick = selectMultiword;
            wrd.addEventListener('click', selectMultiword);
          }
        }

        document.getElementById('pos_tag_box').innerHTML = choosePoS(pos);
        if(multiword_meaning_no != 0) {
          multiword_meanings[multiword_meaning_no] = multiword_textarea_content;
        }
        else {
          multiword_meaning_no = 1;
        }
        document.getElementById("number").innerHTML = multiword_meaning_no;     
        document.getElementById('lemma_tag').value = multiword_tag_content;
        setLemmaTagSize();
        document.getElementById('lemma_textarea').value = multiword_textarea_content;

        document.getElementById('lemma_tag').focus();

        reactivateArrows(multiword_meaning_no, 5);

        if(multiword_id == 0) {
          document.getElementById('delete_lemma_button').style.display = "none";
        }
        else {
          document.getElementById('delete_lemma_button').style.display = "";
        }

        document.getElementById('delete_lemma_button').onclick = deleteMultiword;

        //document.getElementById('save_button').onclick = recordMultiword;
        document.getElementById('meaning_leftarrow').onclick = switchMultiwordMeanings;
        document.getElementById('meaning_rightarrow').onclick = switchMultiwordMeanings;
        document.getElementById('lemma_tag').onblur = "";
        document.getElementById('lemma_tag').onblur = pullInMultiwordByForm;

        document.getElementById("lemma_tag").addEventListener("keydown", dict.lookUpMouseSelection);
      }
    }
    xhttp.send(send_data);
  }
  
  httpRequest("POST", "retrieve_multiword.php");
   
};

const reactivateArrows = (meaning_no, max_meaning_no) => {
  let leftarrow = document.getElementById("meaning_leftarrow");
  let rightarrow = document.getElementById("meaning_rightarrow");
  leftarrow.classList.add("nav_arrow");
  rightarrow.classList.add("nav_arrow");
  leftarrow.classList.remove("nav_arrow_deactiv");
  rightarrow.classList.remove("nav_arrow_deactiv");
  if(meaning_no == 1) {
    document.getElementById("meaning_leftarrow").classList.add("nav_arrow_deactiv");
    document.getElementById("meaning_leftarrow").classList.remove("nav_arrow");
  }
  else if (meaning_no == max_meaning_no) {
    document.getElementById("meaning_rightarrow").classList.add("nav_arrow_deactiv");
    document.getElementById("meaning_rightarrow").classList.remove("nav_arrow");
  }
};

const displayAnnotBox = function () {
  const annot_box = document.createRange().createContextualFragment(/*'<div id="annot_box"><div id="annot_topbar" ondblclick="makeDraggable()"><span id="close_button" onclick="delAnnotate()">Close</span><span id="edit_button" title="Edit current text-word">Edit</span></div><div id="annot"><div id="left_column"><span id="lemma_box" class="box">Lemma translation</span><span id="multiword_box" class="box">Multiword</span><span id="context_box" class="box" title="not yet implemented">Context translation</span><span id="morph_box" class="box" title="not yet implemented">Morphology</span><span id="accent_box" class="box" title="not yet implemented">Accentology</span></div><div id="right_column"><div id="right_header"><textarea id="lemma_tag" spellcheck="false"></textarea></div><div id="right_body"><textarea id="lemma_textarea" autocomplete="off"></textarea></div><div id="right_footer"><span id="pos_tag_box"></span><div id="meaning_no_box"><div id="meaning_leftarrow" class="nav_arrow"><</div><div id="meaning_no">Meaning <span id="number"></span></div><div id="meaning_rightarrow" class="nav_arrow">></div></div><div id="save_and_delete_box"><div id="save_button">Save</div><div id="delete_lemma_button">Delete</div></div></div></div></div></div>'*/fullscreen_annot_box_html);

  if(lang_id == 10) {
    annot_box.getElementById("lemma_tag").addEventListener('beforeinput', oldEnglishInput);
  }
  if(display_word.dataset.multiword !== undefined) {
    annot_box.getElementById("edit_button").style.visibility = "hidden";
  }
  document.getElementById('spoofspan').after(annot_box);
};

const delAnnotate = function (total = true) {
  display_word.classList.remove("mw_current_select");
  if(total){
    let annot_box = document.getElementById('annot_box');
    display_word.classList.add("tooltip");
    display_word.classList.remove("tooltip_selected");
    //display_word.onclick = showAnnotate;
    display_word = null;
    //tokno_current and word_engine_id are not set to zero by delAnnotate() because certain AJAX requests which require these values can run after delAnnotate() (e.g. lemmaRecordTooltipUpdate, which runs after the lemmaRecord AJAX request resolves), or code to add/remove event-listeners from elements with data-attributes of these numbers is only run after AJAX requests return, whereas delAnnotate() is often run immediately to prevent the poor user-experience of having the annotation-box hang around for the brief period during which the network-request completes (though on localhost on a modern computer with an SSD this is always sub 30ms)
    annot_box.remove();
  }

  lemma_id = 0;
  lemma_meaning_no = 1;
  multiword_id = 0;
  multiword_meaning_no = 1;

  meanings = Object.create(null);
  multiword_meanings = Object.create(null);
  multiword_indices = Object.create(null);

  document.querySelectorAll('.mw_selectable').forEach(mws => {
    mws.classList.remove("mw_selectable", "mw_current_select");
    mws.removeEventListener('click', selectMultiword);
  });
  document.querySelectorAll('.multiword').forEach(mw => {
    //mw.onclick = showMultiwordAnnotate;
    mw.addEventListener('click', showMultiwordAnnotate);
    mw.addEventListener('mouseover', underlineMultiwords);
    mw.addEventListener('mouseout', removeUnderlineMultiwords);
  });
  annotation_mode = 0;
  pos = 1;
  pos_initial = 1;

};

//this is stolen
const makeDraggable = function () {
  dragAnnotBox(document.getElementById("gorazd_outline"));

  function dragAnnotBox(elmnt) {
    var pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0;
    if (document.getElementById("gorazd_topbar")) {
      /* if present, the header is where you move the DIV from:*/
      document.getElementById("gorazd_topbar").onmousedown = dragMouseDown;
    } else {
      /* otherwise, move the DIV from anywhere inside the DIV:*/
      elmnt.onmousedown = dragMouseDown;
    }

    function dragMouseDown(e) {
      e = e || window.event;
      e.preventDefault();
      // get the mouse cursor position at startup:
      pos3 = e.clientX;
      pos4 = e.clientY;
      document.onmouseup = closeDragElement;
      // call a function whenever the cursor moves:
      document.onmousemove = elementDrag;
    }

    function elementDrag(e) {
      e = e || window.event;
      e.preventDefault();
      // calculate the new cursor position:
      pos1 = pos3 - e.clientX;
      pos2 = pos4 - e.clientY;
      pos3 = e.clientX;
      pos4 = e.clientY;
      // set the element's new position:
      elmnt.style.top = (elmnt.offsetTop - pos2) + "px";
      elmnt.style.left = (elmnt.offsetLeft - pos1) + "px";
    }

    function closeDragElement() {
      /* stop moving when mouse button is released:*/
      document.onmouseup = null;
      document.onmousemove = null;
    }
  }

  document.getElementById('gorazd_topbar').style.cursor = 'move';

};

let diff_unexplicit_annot = true;
const differentiateAnnotations = function () {
  if(diff_unexplicit_annot) {
    document.getElementById("lemma_set_style").href = "lemma_set.css";
    diff_unexplicit_annot = false;
  }
  else {
    document.getElementById("lemma_set_style").href = "lemma_set_unexplicit.css";
    diff_unexplicit_annot = true;
  }

};

// const keyboard_tt = (event) => {
//   if(display_word != null || displayWordEditor.edit_mode) return;
//   if(event.key == "T") {
//     if(document.getElementById("tt_toggle").checked == false) document.getElementById("tt_toggle").checked = true;
//     else document.getElementById("tt_toggle").checked = false;
//     tt_type();
//     //console.log("shit and T down");
//   }

// };
// window.addEventListener("keydown", event1 => {
//   if(event1.key == "Shift") {
//     window.addEventListener("keydown", keyboard_tt);
//     //console.log("shift down");
//   }
// });
// window.addEventListener("keyup", event1 => {
//   if(event1.key == "Shift") {
//     window.removeEventListener("keydown", keyboard_tt);
//     //console.log("shift up");
//   }
// });

const underlineMultiwords = function (event) {
  (document.querySelectorAll('[data-multiword="'+event.target.dataset.multiword+'"]').forEach(multiword =>  {multiword.style.borderBottom = "2px solid rgb(0, 255, 186)";})); 
};

const removeUnderlineMultiwords = function (event) {
  (document.querySelectorAll('[data-multiword="'+event.target.dataset.multiword+'"]').forEach(multiword =>  {multiword.style.borderBottom = "2px dotted rgb(0, 255, 186, 0.5)";})); 
};

document.querySelectorAll('.multiword').forEach(multiword => {
  //multiword.onclick = showMultiwordAnnotate;
  multiword.addEventListener('click', showMultiwordAnnotate);
  multiword.addEventListener('mouseover', underlineMultiwords);
  multiword.addEventListener('mouseout', removeUnderlineMultiwords);
});

//if(document.getElementById("textbody") !== null) document.getElementById("textbody").addEventListener('click', showAnnotate);

const ttPosition = function () {
  const viewport_width = window.visualViewport.width;
  if(viewport_width > 768) return;

  const tooltips = document.body.querySelectorAll(".lemma_tt, .mw_tt");
  
  tooltips.forEach(tooltip => {

    tooltip.style.transform = "none";
    tooltip.style.border = "none";
    
    let rectRight = tooltip.getBoundingClientRect().right;
    let right_overflow = rectRight - viewport_width;
    let rectLeft = tooltip.getBoundingClientRect().left;
    let left_overflow = -1*rectLeft;


    //console.log("right overflow: "+right_overflow);
    if(right_overflow > 0) {
    tooltip.style.transform = `translateX(${-1*right_overflow - 1}px)`;
    //tooltip.style.border = "1px solid red";
    }
    else if(left_overflow > 0) {
      tooltip.style.transform = `translateX(${left_overflow + 1}px)`;
    }

      /* console.log(tooltip);
      console.log(rectRight);
      console.log(right_overflow);
      console.log(viewport_width); */

  });

};

window.addEventListener("resize", ttPosition);


const pullInMultiwordByForm = (can_skip = true) => {
  const candidate_mw_lemma_form = document.getElementById('lemma_tag').value.trim();
  const mw_length = Object.keys(multiword_indices).length;

  if(candidate_mw_lemma_form == lemma_form_tag_initial && can_skip) return;

  let send_data = "mw_length="+mw_length+"&mw_lemma_form="+encodeURIComponent(candidate_mw_lemma_form)+"&lang_id="+lang_id;
  if(pos == pos_initial) {
    send_data += "&pos=0";
  }
  else {
    send_data += "&pos=" + pos;
  }
 
  const xhttp = new XMLHttpRequest();
  xhttp.open("POST", "pull_mw_by_form.php", true);
  xhttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  xhttp.responseType = 'json';
  
  xhttp.onreadystatechange = () => {
    if (xhttp.readyState == 4) {
      lemma_form_tag_initial = candidate_mw_lemma_form;

      const returned_multiword_id = xhttp.response.multiword_id;

      
      if(returned_multiword_id == 0) {
        return;
      }
      multiword_id = returned_multiword_id;
      console.log(xhttp.response);
      pos = xhttp.response.pos; //I've omitted the quotes round the numbers in the JSON-response so I reckon it should come through as an int type already
      pos_initial = pos;
      const multiword_textarea_content = xhttp.response.multiword_textarea_content;
      
      multiword_meaning_no = 1;      
      multiword_meanings = Object.create(null);
      multiword_meanings[multiword_meaning_no] = multiword_textarea_content;

      document.getElementById('pos_tag_box').innerHTML = choosePoS(pos);
      document.getElementById("number").innerHTML = multiword_meaning_no;
      reactivateArrows(multiword_meaning_no, 5);
      document.getElementById('lemma_textarea').value = multiword_textarea_content;

    }
  }
  xhttp.send(send_data);  
};

const removePoSMenu = () => {
  document.querySelectorAll('.pos_tag_select').forEach(pos_select => {
    pos_select.remove();
  });        
};
const setPoSEventListenersClosedMenu = () => {
  const pos_tag_box = document.getElementById('pos_tag_box');
  pos_tag_box.removeEventListener('click', changePoS);
  pos_tag_box.removeEventListener('touchstart', changePoS); //remove both events to be on the safeside incase someone has changed their screen-width
  pos_tag_box.addEventListener('click', selectPoS);
  pos_tag_box.addEventListener('touchstart', selectPoS);
};
const setPoSEventListenersOpenedMenu = (event_type) => {
  const pos_tag_box = document.getElementById('pos_tag_box');
  pos_tag_box.removeEventListener('click', selectPoS);
  pos_tag_box.removeEventListener('touchstart', selectPoS);
  pos_tag_box.addEventListener(event_type, changePoS); //we need to prevent changePoS from firing due to the click-event which happens after you release the touch-event
};











const oldEnglishInput = (event) => {

	const key_pressed = event.data;
	const textarea = event.target;
	const selection_start = textarea.selectionStart;
	const selection_end = textarea.selectionEnd;

	const digraph = (base_letter, replacement_upper, replacement_lower, input_element) => {
		event.preventDefault();
		const replacement = (base_letter == base_letter.toUpperCase()) ? replacement_upper : replacement_lower;
		input_element.value = input_element.value.slice(0, selection_start - 1) + replacement + textarea.value.slice(selection_end);
		textarea.selectionStart = selection_start; textarea.selectionEnd = selection_start;
	};

	if(key_pressed == 'w' || key_pressed == 'W' || key_pressed == 'g' || key_pressed == 'G') {
		event.preventDefault();
		let new_letter = '';

		switch(key_pressed) {
			case 'w':
				new_letter = 'ƿ';
				break;
			case 'W':
				new_letter = 'Ƿ';
				break;
			case 'g':
				new_letter = 'ᵹ';
				break;
			case 'G':
				new_letter = 'Ᵹ';
				break;
		}
		const insertPos = selection_start + 1;
		textarea.value = textarea.value.slice(0, selection_start) + new_letter + textarea.value.slice(selection_end);
		textarea.selectionStart = insertPos; textarea.selectionEnd = insertPos;
	}
  else if(key_pressed == ":") {
    let long_vowel = '';
		const last_letter = textarea.value.slice(selection_start - 1, selection_start);
		const upper_case = (last_letter == last_letter.toUpperCase());
		switch(last_letter.toLowerCase()) {
			case 'a':
				long_vowel = 'ā';
				break;
			case 'e':
				long_vowel = 'ē';
				break;
			case 'i':
				long_vowel = 'ī';
				break;
			case 'æ':
				long_vowel = 'ǣ';
				break;
			case 'u':
				long_vowel = 'ū';
				break;
			case 'o':
				long_vowel = 'ō';
				break;
			case 'y':
				long_vowel = 'ȳ';
				break;
			default:
				;		
		}
		if(long_vowel != '') {
			event.preventDefault();
			if(upper_case) long_vowel = long_vowel.toUpperCase();
			textarea.value = textarea.value.slice(0, selection_start - 1) + long_vowel + textarea.value.slice(selection_end);
			textarea.selectionStart = selection_start; textarea.selectionEnd = selection_start;
		}
  }

	else {
		const last_letter = textarea.value.slice(selection_start - 1, selection_start);
		if(key_pressed == 'e' && last_letter.toLowerCase() == 'a') {
			digraph(last_letter, 'Æ', 'æ', textarea);
		}
		else if(key_pressed == 'h' && last_letter.toLowerCase() == 't') {
			digraph(last_letter, 'Þ', 'þ', textarea);
		}
		else if(key_pressed == 'h' && last_letter.toLowerCase() == 'd') {
			digraph(last_letter, 'Ð', 'ð', textarea);
		}
		else if(key_pressed == 'j' && last_letter.toLowerCase() == 'c') {
			digraph(last_letter, 'Ċ', 'ċ', textarea);
		}
		else if(key_pressed == 'j' && last_letter.toLowerCase() == 'g') {
			digraph(last_letter, 'Ġ', 'ġ', textarea);
		}
	}
  setLemmaTagSize();
};


const glag_map = new Map([
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
  ["й", "ⰻ"]
]);
const cyr_glag_clean_map = new Map([
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
]);

const original_cyr_forms = new Array();
const cyrToGlag = (str) => {
  const cyr_map_iter = cyr_glag_clean_map.entries();
  for(const cyr_entry of cyr_map_iter) {
    str = str.replaceAll(cyr_entry[0], cyr_entry[1]);
  }
  const glag_map_iter = glag_map.entries();
  for(const glag_entry of glag_map_iter) {
    str = str.replaceAll(glag_entry[0], glag_entry[1]);
  }
  return str;
};

const convertToGlag = () => {
  original_cyr_forms.length = 0;
  document.querySelectorAll(".tooltip").forEach(tt => {
    const cyr_form = tt.firstChild.textContent
    original_cyr_forms.push(cyr_form);
    tt.firstChild.textContent = cyrToGlag(cyr_form);
  });
  ttPosition();
};
const convertToCyr = () => {
  document.querySelectorAll(".tooltip").forEach((tt, i) => tt.firstChild.textContent = original_cyr_forms[i]);
  ttPosition();
};


/*const lcsPageSearch = (query) => {
  if(query == "") return;
  query = query.replaceAll("ŕ̥", "W").replaceAll("r̥", "X").replaceAll("ĺ̥", "Y").replaceAll("l̥", "Z")
  resetLcsPageSearch();
  //const regex = new RegExp(`/${query}/u`);
  document.querySelectorAll("[data-lcs_recon]").forEach(reconstructed_word => {
    //if(regex.test(reconstructed_word.dataset.lcs_recon))
    if(reconstructed_word.dataset.lcs_recon.replaceAll("ŕ̥", "W").replaceAll("r̥", "X").replaceAll("ĺ̥", "Y").replaceAll("l̥", "Z").replaceAll("Q", "ъ").replaceAll("ę̌", "ę").replaceAll("y̨", "y").includes(query)) {
      reconstructed_word.classList.add("pulsate");
    }
  })
}; */
const resetLcsPageSearch = () => {
  document.querySelectorAll(".pulsate").forEach(elem => elem.classList.remove("pulsate"));
};

const btn_load_spinner = document.createElement("div");
btn_load_spinner.id = "btn_load_spinner";

const lemmaTooltip = function (show_load_spinner) {

  const torot_btn = document.getElementById("torot_mode");

  removeTooltips();
  

  document.querySelectorAll(".lemma_tt").forEach(lemma_tt => lemma_tt.remove());
  
  const page_lemma_ids_set = new Set();
  const lemma_set_words = new Array();
  for (const word of document.getElementsByClassName("tooltip")){
    const word_lemma_id = word.dataset.lemma_id;
    if(word_lemma_id != "0") {
      page_lemma_ids_set.add(word_lemma_id);
      lemma_set_words.push(word);
    }
  }
  const page_lemma_ids_arr = Array.from(page_lemma_ids_set);
  if(page_lemma_ids_arr.length == 0) {
    document.getElementById("lcs_mode").classList.remove("active");
    torot_btn.classList.add("active");
    app_state.annotation_mode = "torot";
    return;
  }

  if(show_load_spinner) torot_btn.append(btn_load_spinner);
  
  const page_lemma_ids_str = page_lemma_ids_arr.join(",");
  
  const httpRequest = (method, url) => {

    // let send_data = toknos_POST_data;
    let send_data = "lemma_ids=" + page_lemma_ids_str;
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    xhttp.responseType = 'json';

    xhttp.onload = () => {
      if(xhttp.readyState == 4) {
        tooltips_shown = true;
        const pos_array = xhttp.response[0];
        const lemma_ocs_array = xhttp.response[1];        
        // console.log(pos_array);
        // console.log(lemma_ocs_array);

        page_lemma_ids_arr.forEach((lemma_id, i) => {
          let tt_box_string = '<span class="lemma_tt" onclick="event.stopPropagation()"><span id="tt_top"><div id="lemma_tag_tt">';
          tt_box_string += lemma_ocs_array[i] + '</div><span id="pos_tag_box_tt">';
          tt_box_string += tt_pos_arr[proiel_pos_map[pos_array[i] - 1][0]] + '</span></span><span id="tt_mid"><div id="tt_meaning">';
          
          document.querySelectorAll(`[data-lemma_id="${lemma_id}"]`).forEach(lemma_word => {
            const finished_string = tt_box_string + convertMorphTag(lemma_word.dataset.morph_tag) + '</div></span><span id="tt_bottom"></span></span>'; 
            const tt_fragment = document.createRange().createContextualFragment(finished_string);
            tt_fragment.getElementById("pos_tag_box_tt").firstChild.title = proiel_pos_map[pos_array[i] - 1][1];
            lemma_word.append(tt_fragment);
          });
          
          i++;
        });

        document.getElementById("lcs_mode").classList.remove("active");
        if(show_load_spinner) torot_btn.removeChild(btn_load_spinner);
        torot_btn.classList.add("active");
	// document.getElementById("tt_toggle").disabled = false;
        setTimeout(ttPosition, 200);
        app_state.annotation_mode = "torot";
        
      }

    }
    xhttp.send(send_data);
  };
  httpRequest("POST", "lemma_tooltip.php");

};

const lcsTooltip = function () {

  removeTooltips();
  
  document.documentElement.style.setProperty("--no-lcs-opacity", "0.5");
  
  const lcs_words = document.querySelectorAll("[data-lcs_recon]");

  lcs_words.forEach(lcs_word => {
    
    const lcs_recon = lcs_word.dataset.lcs_recon;
    const pv2_3_exists = lcs_word.dataset.pv3 == "1" ? true : false;
    const lemma_id = Number(lcs_word.dataset.lemma_id);
    let tt_box_string = '<span class="lemma_tt" onclick="event.stopPropagation()"><span id="tt_top"><div id="lemma_tag_tt">';
    tt_box_string += convertToOCS(lcs_recon, pv2_3_exists, lemma_id) + '</div><span id="pos_tag_box_tt">';
    tt_box_string += '<span id="pos_tag_unassigned_tt" class="pos_tag_tt" title="unassigned"></span>' + '</span></span><span id="tt_mid"><div id="tt_meaning">';
    tt_box_string +=  lcs_recon.replaceAll("Q", "ъ") + '</div></span><span id="tt_bottom"></span></span>';
    
    lcs_word.append(document.createRange().createContextualFragment(tt_box_string));
  });
  document.getElementById("lcs_mode").classList.add("active");
  setTimeout(ttPosition, 200);
  app_state.annotation_mode = "lcs";
        
};

const removeTooltips = () => {
  document.querySelectorAll(".lemma_tt").forEach(lemma_tt => lemma_tt.remove());
  document.documentElement.style.setProperty("--no-lcs-opacity", "1.0");
  document.documentElement.style.setProperty("--no-lemma_id-opacity", "0.5");
  app_state.annotation_mode = "none";
};

const applyTooltips = () => {
  switch(app_state.annotation_mode) {
    case "torot":
      lemmaTooltip(false);
      break;
    case "lcs":
      lcsTooltip();
      break;
    case "greek":
      greekTooltips();
      break;
    default:
  

  }
};

document.getElementById("annotation_mode_box").addEventListener('click', (event) => {
  const annotation_button = event.target;
  if(annotation_button.classList.contains("annotation_mode")) {
    //removeTooltips();
    console.log(annotation_button.id.startsWith(app_state.annotation_mode));
    Array.from(event.currentTarget.children).forEach(child => child.classList.remove('active'));
    if(annotation_button.id.startsWith(app_state.annotation_mode)) {
      removeTooltips();
    }
    else {
      annotation_button.classList.add("active");
      switch(annotation_button.id) {
        case "torot_mode":
          lemmaTooltip(true);
          break;
        case "lcs_mode":
          lcsTooltip();
          break;
        case "greek_mode":
          greekTooltips(true);
          break;
        default:
          return;
      }
    }
  }
});

const ocs_search_letters = [
  "ѣ",
  "ѧ",
  "ѫ",
  "ѩ",
  "ѭ",
  "ꙗ",
  "ѥ",
  "ѕ",
  "ꙙ",
  "ѡ",
  "ѱ",
  "ѳ",
  "ѵ",
  "ѻ",
  "ѿ",
  "ꙋ",
];
const lcs_search_letters = [
  "š",
  "ž",
  "č",
  "ћ",
  "ђ",
  "Ǣ",
  "ě",
  "ǫ",
  "ę",
  "ĺ",
  "ń",
  "ŕ",
  "l̥",
  "r̥",
  "ĺ̥",
  "ŕ̥",
  "ǯ",
  "y̨",
  "ę̌",
  "ǵ",
  "ḱ",
  "x́",
  "ü",
  "ъ",
  "ь"
];

const switchSearchType = (event) => {
  if(event.target.classList.contains("active") || event.target.classList.contains("search_type") == false) return;
  let search_letters = [];
  document.querySelector(".search_type.active").classList.remove("active");
  event.target.classList.add("active");
  if(event.target.id == "lcs_search") {
    search_letters = lcs_search_letters;
    document.documentElement.style.setProperty("--search-letter-font", "IBM_PLEX_SANS");
    document.documentElement.style.setProperty("--search-letter-size", "13px");
    document.getElementById("fuzzy_box").style.display = "none";
    document.getElementById("regex_box").style.display = "";
    app_state.search_type = 1;
  }
  else if(event.target.id == "ocs_search") {
    search_letters = ocs_search_letters;
    document.documentElement.style.setProperty("--search-letter-font", "Bukyvede");
    document.documentElement.style.setProperty("--search-letter-size", "15px");
    document.getElementById("fuzzy_box").style.display = "";
    document.getElementById("regex_box").style.display = "none";
    app_state.search_type = 2;
  }

  document.getElementById("search_letter_button_box").innerHTML = "";
  let search_letter_html = "";
  for(const letter of search_letters) {
    search_letter_html += "<div class=\"letter_button\">" + letter + "</div>";
  }
  document.getElementById("search_letter_button_box").innerHTML = search_letter_html;
  document.getElementById("search_letter_button_box").scrollTop = 0;
};

const insertLetter = (event) => {
  if(event.target.className == "letter_button") {
    const searchbox = event.target.parentElement.parentElement.querySelector("textarea");
    const selection_start = searchbox.selectionStart;
    const selection_end = searchbox.selectionEnd;
    const new_letter = event.target.textContent;
    const new_search_string = searchbox.value.substr(0, selection_start) + new_letter + searchbox.value.substr(selection_end);
    searchbox.value = new_search_string;
    searchbox.setSelectionRange(selection_start + new_letter.length, selection_start + new_letter.length);
    searchbox.focus();
    
  }
};

const switchSearchScope = (event) => {
  if(event.target.classList.contains("active") || event.target.classList.contains("scope_heading") == false) return;

  event.currentTarget.querySelectorAll(".scope_heading").forEach(heading => heading.classList.remove("active"));
  event.target.classList.add("active");
  app_state.search_scope = Number(event.target.dataset.scope);
  lcsSearch(document.getElementById("dict_searchbox").value.trim(), app_state.regex_search);
}
document.getElementById("search_scope").addEventListener('click', switchSearchScope);


document.getElementById("search_type_options").addEventListener('click', switchSearchType);

document.getElementById("search_letter_button_box").addEventListener('wheel', (event) => {
  event.preventDefault();
  event.currentTarget.scrollTop += (event.deltaY > 0 ? 5 : -5);
});

document.querySelectorAll(".letter_button_box").forEach(letter_button_box => letter_button_box.addEventListener('click', insertLetter));

const lookupForm = () => {
  const search_term = document.getElementById("dict_searchbox").value.trim();
  //console.log(search_term);
  if(search_term == "") resetLcsPageSearch();
  else lcsPageSearch(search_term);
  document.getElementById("dict_searchbox").select();
}

document.getElementById("dict_searchbox").addEventListener('keydown', event => {
  if(event.key == "Enter") {
    event.preventDefault();
    lcsSearch(document.getElementById("dict_searchbox").value.trim(), app_state.regex_search);
  }
});
document.getElementById("corpus_search_button").addEventListener('click', event => {
  lcsSearch(document.getElementById("dict_searchbox").value.trim(), app_state.regex_search);
});

const minimiseSearchbox = () => {
  const dict_body = document.getElementById("dict_body");
  if(dict_body.style.display == "flex") {
    dict_body.style.display = "none";
    app_state.search_box_minimised = true;
  }
  else {
    dict_body.style.display = "flex";
    app_state.search_box_minimised = false;
  }
};
document.getElementById("dict_minimise").addEventListener('click', minimiseSearchbox);

const toggleSearchBox = () => {
  if(app_state.search_box_shown == true) {
    document.getElementById("dict_hidden_box").style.display = "";
    document.getElementById("dict_outline").style.display = "none";
    document.getElementById("dict_searchbox").blur();
    resetLcsPageSearch();
    app_state.search_box_shown = false;
  }
  else {
    document.getElementById("dict_hidden_box").style.display = "none";
    document.getElementById("dict_outline").style.display = "";
    document.getElementById("dict_searchbox").focus();
    app_state.search_box_shown = true;
  }
}

document.getElementById("dict_hidden_box").addEventListener('click', toggleSearchBox);
document.getElementById("dict_close").addEventListener('click', toggleSearchBox);

const corpus_search_load_spinner = document.createElement("div");
const gorazd_search_load_spinner = document.createElement("div");
corpus_search_load_spinner.className = "search_load_spinner";
gorazd_search_load_spinner.className = "search_load_spinner";
corpus_search_load_spinner.id = "corpus_search_load_spinner";
gorazd_search_load_spinner.id = "gorazd_search_load_spinner";

const showSearchLoadSpinner = (minimised) => {
  document.getElementById("dict_searchbox").disabled = true;
  if(minimised) {
    document.getElementById("corpus_search_button").append(corpus_search_load_spinner);
  }
  else {
    document.getElementById("dict_body").style.opacity = "50%";
    document.getElementById("dict_body").append(corpus_search_load_spinner);
  }
}
const removeSearchLoadSpinner = () => {
  document.getElementById("dict_body").style.opacity = "";
  document.getElementById("corpus_search_load_spinner").remove();
  document.getElementById("dict_searchbox").disabled = false;
}

const lcsSearch = (query, regex=false) => {
  let url = "lcs_trigram_search.php";

  if(app_state.search_type == 1){
    if(regex) url = "lcs_regex_search.php";
  }
  else if(app_state.search_type == 2) {
    url = "ocs_search.php";
    if(app_state.fuzzy_search) url = "ocs_fuzzy_search.php";
  }
  
  resetLcsPageSearch();
  if(query.trim() == "") {
    document.getElementById("search_results").innerHTML = '<div class="dict_row"><div class="dict_cell empty_results">Search results will appear here...</div></div>';
    return;
  }

  showSearchLoadSpinner(app_state.search_box_minimised);
  
  let tokno_start = 0;
  let tokno_end = 0;

  const textSelect = document.getElementById("textselect");
  const subtitleSelect = document.getElementById("subtitle_select");

  switch(app_state.search_scope) {
    case 1:
      tokno_start = page_toknos_arr[current_pageno - 1][0];
      tokno_end = page_toknos_arr[current_pageno - 1][1];
      break;
    case 2:
      tokno_start = textSelect.options[textSelect.selectedIndex].dataset.tokno_start;
      tokno_end = textSelect.options[textSelect.selectedIndex].dataset.tokno_end;
      break;
    case 3:
      tokno_start = 0;
      tokno_end = textSelect.options[textSelect.options.length - 1].dataset.tokno_end;
      break;
  }
  let send_data = "lcs_query=" + encodeURIComponent(query.trim()) +"&tokno_start=" + tokno_start + "&tokno_end=" + tokno_end;

  console.log(send_data);

  const httpRequest = (method, url) => {
    
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.responseType = 'json';

    xhttp.onload = () => {

      if(xhttp.readyState == 4) {

        const search_results = document.getElementById("search_results");
        document.getElementById("dict_body").style.display = "flex";
        app_state.search_box_minimised = false;
        search_results.innerHTML = "";
        //for regex-search only
        if(xhttp.response.error) {
          const error_msg = xhttp.response.error;
          search_results.appendChild(document.createRange().createContextualFragment('<div class="dict_row"><div class="dict_cell no_results">'+error_msg+'</div></div>'));
          removeSearchLoadSpinner();
          return;
        }

        const lcs_results = xhttp.response[0];
        const text_results = xhttp.response[1];
        const tokno_results = xhttp.response[2];

        app_state.titles_info = xhttp.response[3];

        const results_count = xhttp.response[0].length;

        if(results_count == 0) {
          search_results.appendChild(document.createRange().createContextualFragment('<div class="dict_row"><div class="dict_cell no_results">Nothing found</div></div>'));
          removeSearchLoadSpinner();
          return;
        }

        let search_results_html = "";
        for(let i = 0; i < results_count; i++) {            
          search_results_html += '<div class="dict_row"><div class="dict_cell left">'+lcs_results[i].replace("Q", "ъ")+'</div><div class="dict_cell middle ' + getTextResultFont(tokno_results[i]) + '">'+text_results[i]+'</div><div class="dict_cell right search_link" data-search_tokno=\"' + tokno_results[i] + '\">'+getShortTextLocation(tokno_results[i])+'</div></div>';
        }
        search_results.appendChild(document.createRange().createContextualFragment(search_results_html));

        if(app_state.search_scope == 1) {
          for(const result_tokno of tokno_results) {
            document.querySelector("[data-tokno=\""+result_tokno+"\"]").classList.add("pulsate");
          }
        }
        removeSearchLoadSpinner();
      }
    }
    xhttp.send(send_data);
  }
  httpRequest("POST", url);

};

const getShortTextLocation = (tokno) => {
  return app_state.titles_info.find(title => title[1] <= tokno && title[2] >= tokno)[0];
}
const getTextResultFont = (tokno) => {
  //this should be replaced with a simple "script" field in the texts table in the DB which would return glag or cyr, not bullshit switching on title-strings
  const title = app_state.titles_info.find(title => title[1] <= tokno && title[2] >= tokno)[0];

  if(title == "Codex Suprasliensis" || title == "Vita Constantini" || title == "Vita Methodii" || title == "Treatise on the letters") {
    return "cyr";
  }
  else return "glag";
}

const retrieveTextFromSearch = (tokno) => {
  showLoadingButton();

  let post_data = "tokno=" + tokno;
  const httpRequest = (method, url) => {

    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    xhttp.responseType = 'json';

    xhttp.onreadystatechange = () => {

      if (xhttp.readyState == 4) {

        const subtitle_select = document.getElementById("subtitle_select");
        subtitle_select.innerHTML = xhttp.response.subtitles_html;

        current_pageno = Number(xhttp.response.result_pageno);
        page_toknos_arr = xhttp.response.page_toknos;
        const result_text_id = Number(xhttp.response.text_id);
        const selected_subtitle_id = xhttp.response.subtitle_id;

        const textselect = document.getElementById("textselect");
        Array.from(textselect.options).forEach(option => { if (option.value == result_text_id) option.selected = true; });

        updateFont(textselect.value);
        textselect.dispatchEvent(new Event('cookie_selection'));
        subtitle_select.dispatchEvent(new Event('cookie_selection'));

        document.getElementById("p1").innerHTML = xhttp.response.html;


        if (page_toknos_arr.length > 1) {
          document.getElementById("pagenos").addEventListener('click', selectText_splitup);
          document.getElementById("pagenos").addEventListener('keydown', selectText_splitup);
        }

        removeLoadingButton();
        
        console.log(tokno);
        const result_word = document.querySelector("[data-tokno=\""+tokno+"\"]");
        result_word.classList.add("pulsate");
        result_word.scrollIntoView();

        document.cookie = "subtitle_id="+selected_subtitle_id+";max-age=157680000";
        document.cookie = "text_id="+result_text_id+";max-age=157680000";
        document.cookie = "current_pageno="+current_pageno+";max-age=157680000";

        minimiseSearchbox();
        applyTooltips();
      }

    }

    xhttp.send(post_data);
  }

  httpRequest("POST", "retrieve_text_search.php");
}

document.getElementById("search_results"),addEventListener('click', (event) => {
  if(event.target.classList.contains("search_link")) {
    retrieveTextFromSearch(event.target.dataset.search_tokno);
  }
});

const lcsRegexSearch = (query) => {
  
  resetLcsPageSearch();
  if(query.trim() == "") {
    document.getElementById("search_results").innerHTML = '<div class="dict_row"><div class="dict_cell empty_results">Search results will appear here...</div></div>';
    return;
  }

  showSearchLoadSpinner(app_state.search_box_minimised);
  
  let tokno_start = 0;
  let tokno_end = 0;

  const textSelect = document.getElementById("textselect");
  const subtitleSelect = document.getElementById("subtitle_select");

  switch(app_state.search_scope) {
    case 1:
      tokno_start = page_toknos_arr[current_pageno - 1][0];
      tokno_end = page_toknos_arr[current_pageno - 1][1];
      break;
    case 2:
      tokno_start = textSelect.options[textSelect.selectedIndex].dataset.tokno_start;
      tokno_end = textSelect.options[textSelect.selectedIndex].dataset.tokno_end;
      break;
    case 3:
      tokno_start = 0;
      tokno_end = textSelect.options[textSelect.options.length - 1].dataset.tokno_end;
      break;
  }
  let send_data = "lcs_query=" + encodeURIComponent(query.trim()) +"&tokno_start=" + tokno_start + "&tokno_end=" + tokno_end;

  console.log(send_data);

  const httpRequest = (method, url) => {
    
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.responseType = 'json';

    xhttp.onload = () => {

      if(xhttp.readyState == 4) {
        
        const search_results = document.getElementById("search_results");

        document.getElementById("dict_body").style.display = "flex";
        app_state.search_box_minimised = false;
        search_results.innerHTML = "";
        
        if(xhttp.response.length == 0) {
          search_results.appendChild(document.createRange().createContextualFragment('<div class="dict_row"><div class="dict_cell no_results">That regex was malformed. Repent!</div></div>'));
          removeSearchLoadSpinner();
          return;
        }

        const lcs_results = xhttp.response[0];
        const text_results = xhttp.response[1];
        const tokno_results = xhttp.response[2];

        const results_count = xhttp.response[0].length;

        if(results_count > 10000) {
          search_results.appendChild(document.createRange().createContextualFragment('<div class="dict_row"><div class="dict_cell no_results">Too many results, please narrow search</div></div>'));
          removeSearchLoadSpinner();
          return;
        }
        else if(results_count == 0) {
          search_results.appendChild(document.createRange().createContextualFragment('<div class="dict_row"><div class="dict_cell no_results">Nothing found</div></div>'));
          removeSearchLoadSpinner();
          return;
        }

        let search_results_html = "";
        for(let i = 0; i < results_count; i++) {            
          search_results_html += '<div class="dict_row"><div class="dict_cell left">'+lcs_results[i].replace("Q", "ъ")+'</div><div class="dict_cell middle">'+text_results[i]+'</div><div class="dict_cell right search_link">'+getShortTextLocation(tokno_results[i])+'</div></div>';  
        }
        search_results.appendChild(document.createRange().createContextualFragment(search_results_html));

        if(app_state.search_scope == 1) {
          for(const result_tokno of tokno_results) {
            document.querySelector("[data-tokno=\""+result_tokno+"\"]").classList.add("pulsate");
          }
        }
        removeSearchLoadSpinner();
      }
    }
    xhttp.send(send_data);
  }
  httpRequest("POST", "lcs_regex_search.php");

};

const greekTooltips = function (show_load_spinner) {

  const greek_btn = document.getElementById("greek_mode");
  removeTooltips();
  
  let tokno_start = page_toknos_arr[current_pageno - 1][0];
  let tokno_end = page_toknos_arr[current_pageno - 1][1];

  if(show_load_spinner) greek_btn.append(btn_load_spinner);
  
  const httpRequest = (method, url) => {

    // let send_data = toknos_POST_data;
    let send_data = "tokno_start=" + tokno_start + "&tokno_end=" + tokno_end;
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    xhttp.responseType = 'json';

    xhttp.onload = () => {
      if(xhttp.readyState == 4) {
        tooltips_shown = true;
        // const pos_array = xhttp.response[0];
        // const greek_forms_array = xhttp.response[1];
        // const greek_morph_tags_array = xhttp.response[2];
        const greek_data = xhttp.response;

        const alligned_toknos = Object.keys(xhttp.response);

        alligned_toknos.forEach(alligned_tokno => {
          const ocs_word = document.querySelector(`[data-tokno="${alligned_tokno}"]`);
          const greek_data_row = greek_data[alligned_tokno];
          
          let tt_box_string = '<span class="lemma_tt" onclick="event.stopPropagation()"><span id="tt_top"><div id="lemma_tag_tt" style="font-family: Bukyvede;font-weight: bold;">';
          tt_box_string += greek_data_row[0] + '</div><span id="pos_tag_box_tt">';
          tt_box_string += tt_pos_arr[proiel_pos_map[greek_data_row[3] - 1][0]] + '</span></span><span id="tt_mid"><div id="tt_meaning">';
          
  
          const finished_string = tt_box_string + "<span class=\"greek_form\" style=\"font-family: Bukyvede;font-weight: bold;\">" + greek_data_row[1] + "<br></span>" + convertMorphTag(greek_data_row[2]) + '</div></span><span id="tt_bottom"></span></span>'; 
          const tt_fragment = document.createRange().createContextualFragment(finished_string);
          tt_fragment.getElementById("pos_tag_box_tt").firstChild.title = proiel_pos_map[greek_data_row[3] - 1][1];
          ocs_word.append(tt_fragment);

          document.documentElement.style.setProperty("--no-lemma_id-opacity", "1.0");
    

        });

        document.getElementById("lcs_mode").classList.remove("active");
        if(alligned_toknos.length > 0) greek_btn.classList.add("active");
        if(show_load_spinner) greek_btn.removeChild(btn_load_spinner);
        setTimeout(ttPosition, 200);
        app_state.annotation_mode = "greek";
        
      }

    }
    xhttp.send(send_data);
  };
  httpRequest("POST", "greek_tooltips.php");

};

const toggleRegexSearch = (event) => {
  const checkbox = document.getElementById('regex_checkbox');
  
  if(app_state.regex_search === false) {
    app_state.regex_search = true;
    checkbox.checked = true;
    document.getElementById("regex_box_label").style.opacity = "1.0";
  }
  else {
    app_state.regex_search = false;
    checkbox.checked = false;
    document.getElementById("regex_box_label").style.opacity = "";
  }
}
document.getElementById("regex_box").addEventListener('click', toggleRegexSearch);

const toggleFuzzySearch = (event) => {
  const checkbox = document.getElementById('fuzzy_checkbox');
  
  if(app_state.fuzzy_search === false) {
    app_state.fuzzy_search = true;
    checkbox.checked = true;
    document.getElementById("fuzzy_box_label").style.opacity = "1.0";
  }
  else {
    app_state.fuzzy_search = false;
    checkbox.checked = false;
    document.getElementById("fuzzy_box_label").style.opacity = "";
  }
}
document.getElementById("fuzzy_box").addEventListener('click', toggleFuzzySearch);

const stealFromGorazdTOROT = (event) => {
  //console.log("stealFromGorazdTOROT");
  if(window.innerWidth < 769) return;
  const lemma_id = event.target.dataset.lemma_id;
  if(lemma_id == undefined) {
    return;
  }
  lookupGorazd(lemma_id, "");
};

const lookupGorazd = (lemma_id, query) => {
  if(app_state.dict_box_shown == false) {
    if(app_state.dict_box_minimised == false) minimiseGorazd();
    toggleGorazd();
  }
  showGorazdSearchLoadSpinner(app_state.dict_box_minimised);
  app_state.dict_html_entries = {1: "", 2: ""};
  
  //the bastards at Gorazd use the non-break-space character (U+00A0, or UTF8-hex 0xC2 0xA0), rather than normal space (UTF8-hex 0x20) for queries containing a space, e.g. reflexives
  query = query.replaceAll(" ", "\u00A0");
  //query = query.replaceAll("%20", "%C2%A0");

  let send_data = "lemma_id=" + lemma_id + "&query=" + encodeURIComponent(query);
  const httpRequest = (method, url) => {
    const xhttp = new XMLHttpRequest();
    xhttp.open(method, url, true);
    xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    xhttp.responseType = 'json';
    xhttp.onreadystatechange = () => { 
        if (xhttp.readyState == 4) {
          let error_msg = "";

          console.log("curl complete");
          if(xhttp.response.curl_return_text == "Request timeout") {
            console.log("Dictionary request took more than ten seconds so it was aborted");
            error_msg = "Dictionary request took more than ten seconds so it was aborted";
              
          }
          else if(xhttp.response.curl_return_text == "curl failure") {
            console.log("Dictionary request has failed");
            error_msg = "Dictionary request has failed";        
          }
          else {
            const json_response = xhttp.response.curl_return_text;
            document.getElementById("gorazd_searchbox").value = xhttp.response.query_form;
            if(json_response.response.found > 0) {
      
              json_response.response.result.pageData.forEach(dict_entry => {
                // console.log(dict_entry.recordData.EnTrl);

                const dict_no = dict_entry.recordData.Dictionary;
                const dict_html = dict_entry.recordData.SourceD;

                app_state.dict_html_entries[dict_no] = app_state.dict_html_entries[dict_no].concat(dict_html);
              });
            }
            else console.log("No results, repent");               
          }
          displayDictBox(error_msg);
        }
    }; 
    xhttp.send(send_data);
  };
  httpRequest("POST", "gorazd_lookup.php");
};

const displayDictBox = (error_msg) => {
  document.querySelectorAll(".dict_type").forEach(dict_type_button => {
    dict_type_button.classList.remove("active");
    dict_type_button.classList.remove("available");
    dict_type_button.removeEventListener('click', selectGorazdDictionary);
  });

  const snsp_button = document.getElementById("SNSP");
  const sjs_button = document.getElementById("SJS");

  const snsp_entry_exists = app_state.dict_html_entries[2] != "";
  const sjs_entry_exists = app_state.dict_html_entries[1] != "";

  let gorazd_result_content = "<div id=\"gorazd_no_results\">Nothing found. Repent!</div>";
  if(error_msg == "") {

    if(sjs_entry_exists) {
      sjs_button.classList.add("available");
      sjs_button.addEventListener('click', selectGorazdDictionary);
    }
    if(snsp_entry_exists) {
      snsp_button.classList.add("available");
      snsp_button.addEventListener('click', selectGorazdDictionary);
    }

    

    if(snsp_entry_exists) {
      gorazd_result_content = app_state.dict_html_entries[2];
      snsp_button.classList.add("active");
    }
    else if(sjs_entry_exists) {
      gorazd_result_content = app_state.dict_html_entries[1];
      sjs_button.classList.add("active");
    }
  }
  else gorazd_result_content = "<div id=\"gorazd_error\">" + error_msg + "</div>";

  updateGorazdBody(gorazd_result_content);
  fullyOpenGorazd();
  removeGorazdSearchLoadSpinner();
};

const updateGorazdBody = (html_str) => {
  const gorazd_viewboxcontent = document.getElementById('viewboxcontent');
  gorazd_viewboxcontent.innerHTML = "";
  gorazd_viewboxcontent.append(document.createRange().createContextualFragment(html_str));
  gorazd_viewboxcontent.scrollTop = 0;

  document.querySelectorAll("span[aip-type=\"odkaz\"]").forEach(dict_link => {
    dict_link.addEventListener('click', () => {
      let query = dict_link.querySelector("a").textContent;
      if(query.slice(-1) == "-") query = query.slice(0, -1);
      lookupGorazd(0, query);
    });
  });
};

const fullyOpenGorazd = () => {
  if(app_state.dict_box_shown == false) {
    document.getElementById("gorazd_hidden_box").style.display = "none";
    gorazd_outline.style.display = "";
    app_state.dict_box_shown = true;
  }
  if(app_state.dict_box_minimised == true) {
    document.getElementById("gorazd_box").style.display = "flex";
    app_state.dict_box_minimised = false;
  }
};

const selectGorazdDictionary = (event) => {
  if(event.target.classList.contains("available") && !event.target.classList.contains("active")) {
    event.target.parentElement.querySelector(".active").classList.remove("active");
    const new_html = event.target.id == "SJS" ? app_state.dict_html_entries[1] : app_state.dict_html_entries[2];
    updateGorazdBody(new_html);
    event.target.classList.add("active");
    fullyOpenGorazd();
  }
};

const ajaxContextSearch = () => {
  //this function is included as an onclick= attribute in the HTML returned in gorazd's JSON response, so I have to implement it to prevent errors when those words get clicked on. If I want to add link-functionality I can implement it myself with my own eventListeners, but I can't get rid of their bastard onclick= attributes
  return;
}
const minimiseGorazd = () => {
  const gorazd_body = document.getElementById("gorazd_box");
  if(gorazd_body.style.display == "flex") {
    gorazd_body.style.display = "none";
    app_state.dict_box_minimised = true;
  }
  else {
    gorazd_body.style.display = "flex";
    app_state.dict_box_minimised = false;
  }
};
document.getElementById("gorazd_minimise").addEventListener('click', minimiseGorazd);

const toggleGorazd = () => {
  if(app_state.dict_box_shown == true) {
    document.getElementById("gorazd_hidden_box").style.display = "";
    document.getElementById("gorazd_outline").style.display = "none";
    document.getElementById("gorazd_searchbox").blur();
    app_state.dict_box_shown = false;
  }
  else {
    document.getElementById("gorazd_hidden_box").style.display = "none";
    document.getElementById("gorazd_outline").style.display = "";
    document.getElementById("gorazd_searchbox").focus();
    app_state.dict_box_shown = true;
  }
}
document.getElementById("gorazd_hidden_box").addEventListener('click', toggleGorazd);
document.getElementById("gorazd_close").addEventListener('click', toggleGorazd);

document.getElementById("gorazd_searchbox").addEventListener('keydown', event => {
  if(event.key == "Enter") {
    event.preventDefault();
    let gorazd_query = document.getElementById("gorazd_searchbox").value.trim().toLowerCase().replaceAll("ы", "ꙑ").replaceAll("шт", "щ");
    //stealFromGorazd(gorazd_query);
    if(gorazd_query != "") lookupGorazd(0, gorazd_query);
  }
});
document.getElementById("gorazd_search_button").addEventListener('click', event => {
  let gorazd_query = document.getElementById("gorazd_searchbox").value.trim().toLowerCase().replaceAll("ы", "ꙑ").replaceAll("шт", "щ");
  if(gorazd_query != "") lookupGorazd(0, gorazd_query);
});
const showGorazdSearchLoadSpinner = (minimised) => {
  document.getElementById("gorazd_searchbox").disabled = true;
  document.getElementById("p1").removeEventListener('click', stealFromGorazdTOROT);
  if(minimised) {
    document.getElementById("gorazd_search_button").append(gorazd_search_load_spinner);
  }
  else {
    document.getElementById("gorazd_box").style.opacity = "50%";
    document.getElementById("gorazd_box").append(gorazd_search_load_spinner);
  }
};
const removeGorazdSearchLoadSpinner = () => {
  document.getElementById("gorazd_box").style.opacity = "";
  document.getElementById("gorazd_search_load_spinner").remove();
  document.getElementById("gorazd_searchbox").disabled = false;
  document.getElementById("p1").addEventListener('click', stealFromGorazdTOROT);
};

document.getElementById("p1").addEventListener('click', stealFromGorazdTOROT);
