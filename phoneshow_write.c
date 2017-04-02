/*
phoneshow_write

Copyright (C) 2015-2017, Thomas Gollmer, th_goso@freenet.de
Dieses Programm ist freie Software. Sie können es unter den Bedingungen der GNU General Public License,
wie von der Free Software Foundation veröffentlicht, weitergeben und/oder modifizieren,
entweder gemäß Version 3 der Lizenz oder (nach Ihrer Option) jeder späteren Version.

Die Veröffentlichung dieses Programms erfolgt in der Hoffnung, daß es Ihnen von Nutzen sein wird,
aber OHNE IRGENDEINE GARANTIE, sogar ohne die implizite Garantie der
MARKTREIFE oder der VERWENDBARKEIT FÜR EINEN BESTIMMTEN ZWECK.
Details finden Sie in der GNU General Public License. Sie sollten ein Exemplar der GNU General Public License
zusammen mit diesem Programm erhalten haben. Falls nicht, siehe <http://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <string.h>
#include "phonetics.h"
#include "buffer_sizes.h"
#include "phoneshow_str.h"
#include "phoneshow_types.h"
#include "phoneshow_read.h"
#include "phoneshow_comp.h"
#include "phoneshow_write.h"


// Private Funktionen Prototypen
static void write_verbose (const nameslist_t *list, const phops_t *phops, const char *word, const size_t line_no);
static void verbose_word (const nameslist_t *list, const phops_t *phops, const char *word);
static void show_footer (const nameslist_t *list, const phops_t *phops, const size_t foundcount);


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Öffentliche Funktionen zur Ausgabe liefern:
// PHS_MATCH                    mind. einem Fund
// PHS_NO_MATCH                 wenn kein Fund
// PHS_ERR_REC_LINE_OVERSIZE    Wort mit Überlänge von stdin empfangen
// PHS_ERR_REC_WORD_OVERSIZE    Zeile mit Überlänge von stdin empfangen
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus a
// gesammten Text anzeigen
int write_out_a (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops)
{
  int     line_stat;
  int     word_stat;
  char    word[BUFFER_SIZE_WORD];
  name_t  *name;
  size_t  found_cnt=0;
  size_t  line_cnt=0;

  while (1) {
    line_stat = read_line();
    line_cnt++;
    if (line_stat == PHS_ERR_REC_LINE_OVERSIZE) return PHS_ERR_REC_LINE_OVERSIZE;
    if (line_stat == PHS_REC_COMPLEETE) break;
    read_word_init();
    if (frmops->n == true) printf("%zd ", line_cnt);
    while (1) {
      word_stat = read_word(word);
      if (word_stat == PHS_ERR_REC_WORD_OVERSIZE) return PHS_ERR_REC_WORD_OVERSIZE;
      if (word_stat == PHS_LINE_END_RACHED) break;
      if (word_stat == PHS_IS_SPECIAL_CHAR) printf("%s", word);
      else if (word_stat == PHS_IS_WORD) {
        name = compare_word_nameslist(list, phops, word);
        if (name != NULL) {
          found_cnt++;
          printf("%s%s\033[m", name->color_string, word);
        }
        else printf("%s", word);
      }
    }
    printf("\n");
    // ggf. mehr Anzeigen (Alle Zeilen)
    if (frmops->v == true) write_verbose(list, phops, NULL, line_cnt);
  }

  if (frmops->x == true) show_footer(list, phops, found_cnt);
  if (found_cnt > 0) return PHS_MATCH;
  else return PHS_NO_MATCH;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus z
// Nur Fundzeilen anzeigen
int write_out_z (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops)
{
  int     line_stat;
  int     word_stat;
  char    word[BUFFER_SIZE_WORD];
  name_t  *name;
  size_t  found_cnt=0;
  size_t  line_cnt=0;
  bool    show_line;

  while (1) {
    // Zeile lesen
    line_stat = read_line();
    line_cnt++;
    if (line_stat == PHS_ERR_REC_LINE_OVERSIZE) return PHS_ERR_REC_LINE_OVERSIZE;
    if (line_stat == PHS_REC_COMPLEETE) break;
    read_word_init();
    show_line = false;
    // Alle Worte in Zeile prüfen ob passendes Wort in Zeile, noch keine Ausgabe
    while (1) {
      word_stat = read_word(word);
      if (word_stat == PHS_ERR_REC_WORD_OVERSIZE) return PHS_ERR_REC_WORD_OVERSIZE;
      if (word_stat == PHS_LINE_END_RACHED) break;
      if (word_stat == PHS_IS_WORD) {
        name = compare_word_nameslist(list, phops, word);
        if (name != NULL) {
          show_line = true;
          break;
        }
      }
    }
    // Wenn Fund in Zeile... das ganze Spiel von vorn, aber alles brav ausgeben
    if (show_line == true) {
      read_word_init();
      if (frmops->n == true) printf("%zd ", line_cnt);

      while (1) {
        word_stat = read_word(word);
        if (word_stat == PHS_ERR_REC_WORD_OVERSIZE) return PHS_ERR_REC_WORD_OVERSIZE;
        if (word_stat == PHS_LINE_END_RACHED) break;
        if (word_stat == PHS_IS_SPECIAL_CHAR) printf("%s", word);
        else if (word_stat == PHS_IS_WORD) {
          name = compare_word_nameslist(list, phops, word);
          if (name != NULL) {
            found_cnt++;
            printf("%s%s\033[m", name->color_string, word);
          }
          else printf("%s", word);
        }
      }
      printf("\n");
      // Normale Ausgabe abgeschlossen ggf. mehr Anzeigen (Fundzeilen)
      if (frmops->v == true) write_verbose(list, phops, NULL, line_cnt);
    }
  }

  if (frmops->x == true) show_footer(list, phops, found_cnt);
  if (found_cnt > 0) return PHS_MATCH;
  else return PHS_NO_MATCH;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus w
// nur Fundworte anzeigen
int write_out_w (const nameslist_t *list, const phops_t *phops, const frmops_t *frmops)
{
  int     line_stat;
  int     word_stat;
  char    word[BUFFER_SIZE_WORD];
  name_t  *name;
  size_t  found_cnt=0;
  size_t  line_cnt=0;

  while (1) {
    line_stat = read_line();
    line_cnt++;
    if (line_stat == PHS_ERR_REC_LINE_OVERSIZE) return PHS_ERR_REC_LINE_OVERSIZE;
    if (line_stat == PHS_REC_COMPLEETE) break;
    read_word_init();
    while (1) {
      word_stat = read_word(word);
      if (word_stat == PHS_ERR_REC_WORD_OVERSIZE) return PHS_ERR_REC_WORD_OVERSIZE;
      if (word_stat == PHS_LINE_END_RACHED) break;
      if (word_stat == PHS_IS_WORD) {
        name = compare_word_nameslist(list, phops, word);
        if (name != NULL) {
          found_cnt++;
          if (frmops->n == true) printf("%zd ",line_cnt);
          printf("%s%s\033[m\n", name->color_string, word);
          // ggf. mehr Anzeigen für einzelnes wort
          if (frmops->v == true) write_verbose(list, phops, word, line_cnt);
        }
      }
    }
  }

  if (frmops->x == true) show_footer(list, phops, found_cnt);
  if (found_cnt > 0) return PHS_MATCH;
  else return PHS_NO_MATCH;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Ausgabemodus c
// Textzeilen Original, in Wörter zerlegt und allen Codevarianten ausgeben
int write_out_c (void)
{
  int   line_stat;
  int   word_stat;
  char  word[BUFFER_SIZE_WORD];
  char  code[BUFFER_SIZE_WORD];

  while (1) {
    line_stat = read_line();
    if (line_stat == PHS_ERR_REC_LINE_OVERSIZE) return PHS_ERR_REC_LINE_OVERSIZE;
    if (line_stat == PHS_REC_COMPLEETE) break;
    // 1. Durchgang Zeile Original ausgeben
    // Einmalig WORD_OVERSIZE prüfen, ab 2. Durchgang nicht mehr nötig
    printf("Zeile: ");
    read_word_init();
    while (1) {
      word_stat = read_word(word);
      if (word_stat == PHS_ERR_REC_WORD_OVERSIZE) return PHS_ERR_REC_WORD_OVERSIZE;
      if (word_stat == PHS_LINE_END_RACHED) break;
      else printf("%s", word);
    }
    printf("\n");
    // 2. Durchgang Wörter der Zeile ausgeben
    printf("Worte: ");
    read_word_init();
    while (1) {
      word_stat = read_word(word);
      if (word_stat == PHS_LINE_END_RACHED) break;
      if (word_stat == PHS_IS_WORD) printf("%s ", word);
    }
    printf("\n");
    // 3. Durchgang CodesK zu Wörtern der Zeile ausgeben
    printf("CodeK: ");
    read_word_init();
    while (1) {
      word_stat = read_word(word);
      if (word_stat == PHS_LINE_END_RACHED) break;
      if (word_stat == PHS_IS_WORD) {
        phoneconvert_cologne(word, code);
        printf("%s ", code);
      }
    }
    printf("\n");
    // 4. Durchgang CodesP zu Wörtern der Zeile ausgeben
    printf("CodeP: ");
    read_word_init();
    while (1) {
      word_stat = read_word(word);
      if (word_stat == PHS_LINE_END_RACHED) break;
      if (word_stat == PHS_IS_WORD) {
        phoneconvert_phonem(word, code);
        printf("%s ", code);
      }
    }
    printf("\n");
    // 5. Durchgang CodesS zu Wörtern der Zeile ausgeben
    printf("CodeS: ");
    read_word_init();
    while (1) {
      word_stat = read_word(word);
      if (word_stat == PHS_LINE_END_RACHED) break;
      if (word_stat == PHS_IS_WORD) {
        phoneconvert_soundex(word, code);
        printf("%s ", code);
      }
    }
    printf("\n");
    // 6. Durchgang CodesE zu Wörtern der Zeile ausgeben
    printf("CodeE: ");
    read_word_init();
    while (1) {
      word_stat = read_word(word);
      if (word_stat == PHS_LINE_END_RACHED) break;
      if (word_stat == PHS_IS_WORD) {
        phoneconvert_exsoundex(word, code);
        printf("%s ", code);
      }
    }
    printf("\n");
  }
  // War ja nichts zu suchen
  return PHS_NO_MATCH;
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Private Funktion
// Zeigt die Legende zur Suche an
// Wird von Ausgaberoutinen aufgerufen
// Übergeben werden muß Anzahl der Funde
static void show_footer (const nameslist_t *list, const phops_t *phops, const size_t foundcount)
{
  int  cnt;
  bool show_minus=false;


  printf("---------------LEGENDE---------------\n");

  //Suchnamen gibt's immer, gleich prüfen ob's auch "Minusnamen" in Liste gibt
  printf("Suche: ");
  for (cnt = 0; cnt < list->number_of_names; cnt++) {
    if (list->items[cnt].is_minusname == false) {
      printf("%s%s\033[m ", list->items[cnt].color_string, list->items[cnt].name_norm);
    }
    else show_minus=true;
  }

  //Verbotene Namen, falls vorhanden
  if (show_minus == true) {
    printf("\nnicht: ");
    for (cnt = 0; cnt < list->number_of_names; cnt++) {
      if (list->items[cnt].is_minusname == true) printf("%s ", list->items[cnt].name_norm);
    }
  }

  //Optionen
  if (phops->k == false && phops->p == false && phops->s == false && phops->e == false && phops->l == 0) {
    printf ("\nSuche: Einfache Textsuche");
  }
  else if (phops->k == false && phops->p == false && phops->s == false && phops->e == false && phops->l != 0) {
    printf ("\nTyp:   Textsuche mit Levenshtein-Distanz von nicht mehr als %i", phops->l);
  }
  else {
    printf("\nTyp:   Phonetische Suche ");
    if (phops->k == true) printf("(Kölner Phonetik) ");
    if (phops->p == true) printf("(Phonem) ");
    if (phops->s == true) printf("(Soundex) ");
    if (phops->e == true) printf("(Extended Soundex) ");
    if (phops->l != 0) printf("\n       mit Levenshtein-Distanz von nicht mehr als %i im phonetischen Code", phops->l);
  }

  //Funde
  printf("\nFunde: %zd\n", foundcount);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Verbose Ausgabe
// Übergabe in *word: NULL-Pointer ... Zusatzinfos für ganze Zeile ausgeben
//                    Pointer auf Wort ... Zusatzinfos für Wort ausgeben
// wird von den Ausgaberoutinen write_mode_ angesprungen
// NACHDEM eine neue Zeile gelesen/ausgegeben wurde
static void write_verbose (const nameslist_t *list, const phops_t *phops, const char *word, const size_t line_no)
{
  int   stat;
  char  tmp[BUFFER_SIZE_WORD];

  printf("  Zeile=%zd\n", line_no);
  if (word != NULL) verbose_word(list, phops, word);
  else {
    read_word_init();
    while (1) {
      stat = read_word(tmp);
      // PHS_ERR_REC_WORD_OVERSIZE könnte geliefert werden...
      // Zeile wurde aber schon mal in write_mode_ gelesen, hätte da schon Fehler gegeben
      if (stat == PHS_LINE_END_RACHED) break;
      if (stat == PHS_IS_WORD) verbose_word(list, phops, tmp);
    }
  }
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Unterfunktion von write_verbose
// Verbose Ausgabe Wort
// wird von write_verbose angesprungen
static void verbose_word (const nameslist_t *list, const phops_t *phops, const char *word)
{
  char code_k[BUFFER_SIZE_WORD];
  char code_p[BUFFER_SIZE_WORD];
  char code_s[BUFFER_SIZE_WORD];
  char code_e[BUFFER_SIZE_WORD];
  char word_lower[BUFFER_SIZE_WORD];
  int  lev_k, lev_p, lev_s, lev_e, lev;
  int  cnt, last_idx;

  // Einfache Textsuche
  if ((phops->k == false && phops->p == false && phops->s == false && phops->e == false) && (phops->l == 0)) {
    printf("    Wort=%s\n", word);
    for (cnt = 0; cnt < list->number_of_names; cnt++) {
      if (list->items[cnt].is_minusname == true) continue;
      strcpy(word_lower, word);
      to_lower(word_lower);
      if (strcmp(word_lower, list->items[cnt].name_lower) == 0) {
        printf("      %sSuche=%s\033[m\n", list->items[cnt].color_string, list->items[cnt].name_norm);
      }
      else printf("      Suche=%s\n", list->items[cnt].name_norm);
    }
    return;
  }

  // Einfache Textsuche incl. Lev
  if ((phops->k == false && phops->p == false && phops->s == false && phops->e == false) && (phops->l > 0)) {
  printf("    Wort=%s\n", word);
    for (cnt = 0; cnt < list->number_of_names; cnt++) {
      if (list->items[cnt].is_minusname == true) continue;
      printf("      Suche=%s\n", list->items[cnt].name_norm);
      strcpy(word_lower, word);
      to_lower(word_lower);
      lev=levenshtein(word_lower, list->items[cnt].name_lower);
      if (lev <= phops->l) printf("        %sLev=%i\033[m\n", list->items[cnt].color_string, lev);
      else printf("        Lev=%i\n", lev);
    }
    return;
  }

  // Phonetische Suche
  // letzten "Plusnamen" aus Liste bestimmen, sonst gibt's keine schöne Grafik
  // Falls der letzte Name ein "Minusname" ist
  last_idx=(list->number_of_names)-1;
  while (1) {
    if (list->items[last_idx].is_minusname == false) break;
    last_idx--;
  }

  printf("    Wort=%s\n", word);
  phoneconvert_cologne(word, code_k);
  phoneconvert_phonem(word, code_p);
  phoneconvert_soundex(word, code_s);
  phoneconvert_exsoundex(word, code_e);
  printf("    │ ");
  // Codes zu Wort in einer Zeile ausgeben
  if (phops->k == true) printf("CodeK=%s ", code_k);
  if (phops->p == true) printf("CodeP=%s ", code_p);
  if (phops->s == true) printf("CodeS=%s ", code_s);
  if (phops->e == true) printf("CodeE=%s ", code_e);
  printf("\n");

    for (cnt = 0; cnt <= last_idx; cnt++) {
    if (list->items[cnt].is_minusname == false) {
      // einen Suchnamen ausgeben, Verbindungslinien zu Wort anständig zeichnen
      if (cnt < last_idx) printf("    ├─Suche=%s\n", list->items[cnt].name_norm);
      else printf("    └─Suche=%s\n", list->items[cnt].name_norm);
      if (cnt < last_idx) printf("    │ ");
      else printf("      ");
      // Codes zu Suchnamen in eine Zeile
      if (phops->k == true) {
        if (strcmp(list->items[cnt].code_k, code_k) == 0) {
          printf("%sCodeK=%s\033[m ", list->items[cnt].color_string, list->items[cnt].code_k);
        }
        else printf("CodeK=%s ", list->items[cnt].code_k);
      }
      if (phops->p == true) {
        if (strcmp(list->items[cnt].code_p, code_p) == 0) {
          printf("%sCodeP=%s\033[m ", list->items[cnt].color_string, list->items[cnt].code_p);
        }
        else printf("CodeP=%s ", list->items[cnt].code_p);
      }
      if (phops->s == true) {
        if (strcmp(list->items[cnt].code_s, code_s) == 0) {
          printf("%sCodeS=%s\033[m ", list->items[cnt].color_string, list->items[cnt].code_s);
        }
        else printf("CodeS=%s ", list->items[cnt].code_s);
      }
      if (phops->e == true) {
        if (strcmp(list->items[cnt].code_e, code_e) == 0) {
          printf("%sCodeE=%s\033[m ", list->items[cnt].color_string, list->items[cnt].code_e);
        }
        else printf("CodeE=%s ", list->items[cnt].code_e);
      }
      printf("\n");

      // Hier mur weiter wenn phon. Suche incl. Lev-Dist
      if (phops->l > 0) {
        lev_k=levenshtein(list->items[cnt].code_k, code_k);
        lev_p=levenshtein(list->items[cnt].code_p, code_p);
        lev_s=levenshtein(list->items[cnt].code_s, code_s);
        lev_e=levenshtein(list->items[cnt].code_e, code_e);
        if (cnt < last_idx) printf("    │   ");
        else printf("        ");
        // Lev-Distanzen in einer Zeile ausgeben
        if (phops->k == true) {
          if (lev_k <= phops->l) printf("%sLevK=%i\033[m ", list->items[cnt].color_string, lev_k);
          else printf("LevK=%i ", lev_k);
        }
        if (phops->p == true) {
          if (lev_p <= phops->l) printf("%sLevP=%i\033[m ", list->items[cnt].color_string, lev_p);
          else printf("LevP=%i ", lev_p);
          }
        if (phops->s == true) {
          if (lev_s <= phops->l) printf("%sLevS=%i\033[m ", list->items[cnt].color_string, lev_s);
          else printf("LevS=%i ", lev_s);
        }
        if (phops->e == true) {
          if (lev_e <= phops->l) printf("%sLevE=%i\033[m ", list->items[cnt].color_string, lev_e);
          else printf("LevE=%i ", lev_e);
        }
        printf("\n");
      }
    }
  }
}