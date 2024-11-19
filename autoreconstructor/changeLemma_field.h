#include "participise.h"
#include "verb_Flect.h"

void changeLemma_field(Lemma &lemma_ref, std::string morph_tag, std::string cyr_id)
{
  if (lemma_ref.noun_verb == 0)
    return;
  short int int_morph_tag[10]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  for (int i = 0; i < 10; i++)
  {

    std::string morph_tag_elem = morph_tag.substr(i, 1);
    if (morph_tag.length() == i)
    {
      break;
    }

    switch (i)
    {
    case 0:
      if (morph_tag_elem == "-")
      {
        int_morph_tag[0] = 0;
      }
      else if (morph_tag_elem == "1")
      {
        int_morph_tag[0] = 1;
      }
      else if (morph_tag_elem == "2")
      {
        int_morph_tag[0] = 2;
      }
      else if (morph_tag_elem == "3")
      {
        int_morph_tag[0] = 3;
      }
      else if (morph_tag_elem == "x")
      {
        int_morph_tag[0] = 4;
      }
      else
      {
        int_morph_tag[0] = 0;
      }

    case 1:
      if (morph_tag_elem == "-")
      {
        int_morph_tag[1] = 0;
      }
      else if (morph_tag_elem == "s")
      {
        int_morph_tag[1] = 1;
      }
      else if (morph_tag_elem == "d")
      {
        int_morph_tag[1] = 2;
      }
      else if (morph_tag_elem == "p")
      {
        int_morph_tag[1] = 3;
      }
      else if (morph_tag_elem == "x")
      {
        int_morph_tag[1] = 4;
      }
      else
      {
        int_morph_tag[1] = 0;
      }
    case 2:
      if (morph_tag_elem == "-")
      {
        int_morph_tag[2] = 0;
      }
      else if (morph_tag_elem == "p")
      {
        int_morph_tag[2] = 1;
      } // order has been changed to make converting to row_number in unordered_map easy by formula (6*[tense-1] + 3*[num -1] + pers)
      else if (morph_tag_elem == "a")
      {
        int_morph_tag[2] = 2;
      }
      else if (morph_tag_elem == "i")
      {
        int_morph_tag[2] = 3;
      }
      else if (morph_tag_elem == "s")
      {
        int_morph_tag[2] = 4;
      }
      else if (morph_tag_elem == "r")
      {
        int_morph_tag[2] = 5;
      }
      else if (morph_tag_elem == "u")
      {
        int_morph_tag[2] = 6;
      }
      else if (morph_tag_elem == "l")
      {
        int_morph_tag[2] = 7;
      }
      else if (morph_tag_elem == "f")
      {
        int_morph_tag[2] = 8;
      }
      else if (morph_tag_elem == "t")
      {
        int_morph_tag[2] = 9;
      }
      else if (morph_tag_elem == "x")
      {
        int_morph_tag[2] = 10;
      }
      else
      {
        int_morph_tag[2] = 0;
      }

    case 3:
      if (morph_tag_elem == "-")
      {
        int_morph_tag[3] = 0;
      }
      else if (morph_tag_elem == "i")
      {
        int_morph_tag[3] = 1;
      }
      else if (morph_tag_elem == "s")
      {
        int_morph_tag[3] = 2;
      }
      else if (morph_tag_elem == "m")
      {
        int_morph_tag[3] = 3;
      }
      else if (morph_tag_elem == "o")
      {
        int_morph_tag[3] = 4;
      }
      else if (morph_tag_elem == "n")
      {
        int_morph_tag[3] = 5;
      }
      else if (morph_tag_elem == "p")
      {
        int_morph_tag[3] = 6;
      }
      else if (morph_tag_elem == "d")
      {
        int_morph_tag[3] = 7;
      }
      else if (morph_tag_elem == "g")
      {
        int_morph_tag[3] = 8;
      }
      else if (morph_tag_elem == "u")
      {
        int_morph_tag[3] = 9;
      }
      else if (morph_tag_elem == "x")
      {
        int_morph_tag[3] = 10;
      }
      else if (morph_tag_elem == "y")
      {
        int_morph_tag[3] = 11;
      }
      else if (morph_tag_elem == "e")
      {
        int_morph_tag[3] = 12;
      }
      else if (morph_tag_elem == "f")
      {
        int_morph_tag[3] = 13;
      }
      else if (morph_tag_elem == "h")
      {
        int_morph_tag[3] = 14;
      }
      else if (morph_tag_elem == "t")
      {
        int_morph_tag[3] = 15;
      }
      else
      {
        int_morph_tag[3] = 0;
      }

    case 4:
      if (morph_tag_elem == "-")
      {
        int_morph_tag[4] = 0;
      }
      else if (morph_tag_elem == "a")
      {
        int_morph_tag[4] = 1;
      }
      else if (morph_tag_elem == "m")
      {
        int_morph_tag[4] = 2;
      }
      else if (morph_tag_elem == "p")
      {
        int_morph_tag[4] = 3;
      }
      else if (morph_tag_elem == "e")
      {
        int_morph_tag[4] = 4;
      }
      else if (morph_tag_elem == "x")
      {
        int_morph_tag[4] = 5;
      }
      else
      {
        int_morph_tag[4] = 0;
      }

    case 5:
      if (morph_tag_elem == "-")
      {
        int_morph_tag[5] = 0;
      }
      else if (morph_tag_elem == "m")
      {
        int_morph_tag[5] = 1;
      }
      else if (morph_tag_elem == "f")
      {
        int_morph_tag[5] = 2;
      }
      else if (morph_tag_elem == "n")
      {
        int_morph_tag[5] = 3;
      }
      else if (morph_tag_elem == "p")
      {
        int_morph_tag[5] = 1;
      }
      else if (morph_tag_elem == "o")
      {
        int_morph_tag[5] = 1;
      }
      else if (morph_tag_elem == "r")
      {
        int_morph_tag[5] = 2;
      }
      else if (morph_tag_elem == "q")
      {
        int_morph_tag[5] = 1;
      }
      else if (morph_tag_elem == "x")
      {
        int_morph_tag[5] = 8;
      }
      else
      {
        int_morph_tag[5] = 0;
      }

    case 6:
      if (morph_tag_elem == "-")
      {
        int_morph_tag[6] = 0;
      }
      else if (morph_tag_elem == "n")
      {
        int_morph_tag[6] = 1;
      }
      else if (morph_tag_elem == "a")
      {
        int_morph_tag[6] = 2;
      }
      else if (morph_tag_elem == "g")
      {
        int_morph_tag[6] = 3;
      }
      else if (morph_tag_elem == "c")
      {
        int_morph_tag[6] = 3;
      }
      else if (morph_tag_elem == "d")
      {
        int_morph_tag[6] = 4;
      }
      else if (morph_tag_elem == "l")
      {
        int_morph_tag[6] = 5;
      }
      else if (morph_tag_elem == "i")
      {
        int_morph_tag[6] = 6;
      }
      else if (morph_tag_elem == "v")
      {
        int_morph_tag[6] = 7;
      }
      else if (morph_tag_elem == "o")
      {
        int_morph_tag[6] = 9;
      }
      else if (morph_tag_elem == "e")
      {
        int_morph_tag[6] = 10;
      }
      else if (morph_tag_elem == "b")
      {
        int_morph_tag[6] = 11;
      }
      else if (morph_tag_elem == "x")
      {
        int_morph_tag[6] = 12;
      }
      else if (morph_tag_elem == "z")
      {
        int_morph_tag[6] = 13;
      }
      else
      {
        int_morph_tag[6] = 0;
      }

    case 7:
      if (morph_tag_elem == "-")
      {
        int_morph_tag[7] = 0;
      }
      else if (morph_tag_elem == "p")
      {
        int_morph_tag[7] = 1;
      }
      else if (morph_tag_elem == "c")
      {
        int_morph_tag[7] = 2;
      }
      else if (morph_tag_elem == "s")
      {
        int_morph_tag[7] = 3;
      }
      else if (morph_tag_elem == "x")
      {
        int_morph_tag[7] = 4;
      }
      else if (morph_tag_elem == "z")
      {
        int_morph_tag[7] = 5;
      }
      else
      {
        int_morph_tag[7] = 0;
      }

    case 8:
      if (morph_tag_elem == "-")
      {
        int_morph_tag[8] = 0;
      }
      else if (morph_tag_elem == "w")
      {
        int_morph_tag[8] = 1;
      }
      else if (morph_tag_elem == "s")
      {
        int_morph_tag[8] = 2;
      }
      else if (morph_tag_elem == "t")
      {
        int_morph_tag[8] = 2;
      }
      else
      {
        int_morph_tag[8] = 0;
      }

    case 9:
      if (morph_tag_elem == "-")
      {
        int_morph_tag[9] = 0;
      }
      else if (morph_tag_elem == "n")
      {
        int_morph_tag[9] = 1;
      }
      else if (morph_tag_elem == "i")
      {
        int_morph_tag[9] = 2;
      }
      else
      {
        int_morph_tag[9] = 0;
      }
    }
  }
  if (lemma_ref.noun_verb == 1)
    verb_Flect(lemma_ref, int_morph_tag, cyr_id);
  else if (lemma_ref.noun_verb == 2)
    noun_Flect(lemma_ref, int_morph_tag, cyr_id);

  else
    return;

  return;
}
