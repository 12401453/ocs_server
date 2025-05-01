for(const idx in paradigm[0]) {
    if(idx == 38 || (idx > 39 && idx < 43)) {
        const raw_participle = paradigm[0][idx];
        paradigm[0][idx] = raw_participle.concat("ъ");
    }
}

const grid = document.getElementById("verb-grid");

for(const idx in paradigm[0]) {
    const flect_cell = document.createRange().createContextualFragment("<div class='grid-child'>"+convertToOCS(paradigm[0][idx])+"</div>");
    grid.append(flect_cell);

}
