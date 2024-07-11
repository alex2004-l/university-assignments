## **Tema 1 - SDA - _Banda Magică_- README**
---
###### _Lache Alexandra Florentina Georgiana, 311CD, Anul I, UPB ACS 2022-2023_

<br />

### **_Descrierea structurilor de date folosite_**
---

**Banda magică**: Conform restricțiilor temei, banda magică este implementată ca o listă dublu înlănțuită cu santinelă. Structura definiă pentru reprezentarea listei, `magicStrip`, conține 3 câmpuri: 

*  `first` - pointer către primul nod al benzii, adică santinela;
*  `last` - pointer către ultimul nod al benzii ;
*  `actual` - pointer care desemnează poziția actuală a degetului lui David. 

Structura definită pentru reprezentarea nodurilor benzii, `magicStripNode`, conține de asemenea 3 câmpuri:

* `prev` și `next` - referințe la nodul anterior, respectiv următor;
*  `character` - câmpul unde se memorează simbolul de la nodul respectiv.

<br />

**Stivele UNDO/REDO:** În acord cu condițiile temei, operațiile de UNDO și REDO sunt realizate prin intermediul a două stive distincte, definite ca două liste simplu înlănțuite. În implementarea temei, structura care reprezintă stiva, `Stack`, are 2 câmpuri:

*  `head` - pointer către nodul din vârful stivei;
*  `size` - întreg în care se reține dimensiunea stivei.

Pentru a reprezenta nodurile stivei se definește o structură denumită `StackNode` care are 2 câmpuri:

*   `node` - un pointer către un nod tip bandă magică, unde urmează a fi memorate pozițiile anterioare degetului;
*   `next` - un pointer care indică către următorul element din stivă.

<br />

**Coada de comenzi:** Respectând dispozițiile temei, coada în care sunt memorate comenzile de tipul UPDATE este realizată ca o listă simplu înlănțuită. Structura folosită, `Queue`, cuprinde 3 câmpuri:

*   `front` - pointer către primul nod din coadă;
*   `rear` - pointer către ultimul nod din coadă;
*   `size` - întreg folosit pentru a reține dimensiunea cozii.

Nodurile sunt implementate sub denumirea de `QueueNode`, structura lor cuprinzând 3 câmpuri:

*   `command` - șir de caractere unde este copiată comanda citită din fișier;
*   `argument` - caracter care reține argumentul pentru operațiile care îl necesită; implementarea temei din main.c duce ca în acest câmp să se rețină caracterul ' ' pentru comenzile care nu au nevoie de argument;
*   `next` - referință către următorul nod din coadă.

<br />

---

### **_Descrierea implementării cerințelor temei_**

---

**Implementare operații de tip UPDATE**

Fiecare operație de tip UPDATE este implementată printr-o funcție specifică, aceste funcții găsindu-se în fișierul `magicStrip.c` și fiind predominant independende una de cealaltă. Singura excepție este că funcțiile corespunzătoare operațiilor `MOVE_RIGHT`, `MOVE_RIGHT_CHAR` și `INSERT_RIGHT` apelează funcția `addNodeRear()` pentru a prelungi banda în partea dreaptă când este necesar, conform enunțului temei.

*   Operația `MOVE_LEFT` - implementată în funcția `moveLeft()` - verifică dacă nu s-a ajuns la capătul din stanga al benzii și în caz afirmativ mută poziția degetului;
*   Operația `MOVE_RIGHT` - funcția `moveRight()` - mută degetul cu o poziție la dreapta și o prelungește dacă este nevoie;
*   Operația `MOVE_LEFT_CHAR` - funcția `moveLeftChar()` - începe căutarea caracterului în partea stângă a benzii _de la poziția curentă_, parcuregerea benzii realizându-se prin variabila `iter`; găsirea simbolului căutat este indicată de variabila `charFound`; dacă nu s-a găsit caracterul se scrie în fișierul de ieșire mesajul de eroare;
*   Operația `MOVE_RIGHT_CHAR` - funcția `moveRightChar()` - are o implementare asemănătoare cu funcția de căutare în partea stângă, singurele diferențe fiind sensul de parcurgere a benzii și situația în care caracterul nu este găsit;
*   Operația `WRITE` - funcția `write()` - suprascrie caracterul de la poziția actuală a degetului;
*   Operația `INSERT_LEFT` - funcția `isertLeft()` - în implementarea funcției am avut în vedere cele două situații specificate în cerință: cea în care nodul actual este primul nod din bandă, caz în care se scrie în fișierul de output mesajul de eroare și se încetează execuția funcției, și cea în care nodul actual este diferit de primul nod, caz în care se poate realiza inserarea, se creează un nou nod și se refac legăturile;
*   Operația `INSERT_RIGHT` - funcția `insertRight()` - asemănătoare ca implementare cu funcția de inserare în partea stângă, diferența fiind că inserarea la dreapta este posibilă în orice situație; pentru inserarea la finalul benzii se folosește de funcția `addNodeRear()` și se schimbă poziția degetului pentru a indica către ultimul nod, iar pentru celelalte cazuri se creează un nod nou și se refac legăturile.

<br />

---

**_Implementare operații de tip QUERY_**

Cele două operații de tip QUERY sunt, de asemenea, fiecare implementată printr-o funcție specifică, cele două funcții aflandu-se în fișierul `magicStrip.c`. 
*   Operația `SHOW_CURRENT` - funcția `showCurrent()` - asigură scrierea în fișier a caracterului aflat la poziția actuală a degetului;
*   Operația `SHOW` - funcția `show()` - în implementarea căreia este parcursă banda de la stânga la dreapta cu ajutorul variabilei `iter`, pentru fiecare nod scriindu-se în fișier caracterul aferent; de asemenea, este respectată indicarea caracterului curent cu ajutorul simbolului `|`.

<br />

---

**_Implementare operații UNDO/REDO_**

Funcțiile necesare pentru inițializarea, modificarea și distrugerea celor două stive se află în fișierul `UndoRedo.c`. Sunt definite două funcții pentru prelucarea stivelor :

* `push()` - funcție care primește ca parametru un pointer la un nod al benzii și îl adaugă în vârful stivei;
* `pop()` - funcție care extrage vârful stivei primite ca parametru, dealocă memoria folosită pentru nodul stivei și returnează pointerul de tip nod al benzii care era memorat acolo.

În primă fază, înainte de executarea unei operații `MOVE_LEFT` ori `MOVE_RIGHT` poziția actuală a degetului este pusă în vârful stivei `UNDO`.
La întâlnirea primei operații `UNDO` are loc adăugarea poziției actuale a degetului în stiva `REDO` și revenirea poziției anterioare a degetului (aflate în vârful stivei `UNDO`). Procedeul invers are loc la întâlnirea unei operații `REDO`: adăugarea în stiva `UNDO` a poziției actuale și revenirea la poziția din vârful stivei `REDO`.
Restricțiile problemei prevăd realizarea de operații `UNDO`/`REDO` doar pentru operațiile realizate cu succes. De aceea are loc o verificare a reușitei operației de mutare cu o poziție la stânga a degetului prin compararea cu poziția actuală (dacă nu s-a realizat această operație, cele două poziții corespund).

<br />

---

**_Implementare coadă operații/operația EXECUTE_**

Funcțiile necesare pentru inițializarea, modificarea și distrugerea cozii se află în fișierul `CommandQueue.c`. Sunt definite două funcții pentru prelucarea cozii :

*   `enqueue()` - funcție care adaugă în coadă un nod nou, primind ca parametri operația citită din fișier și argumentul aferent, pentru operațiile care au argument;
*   `dequeue()` - funcție care elimină primul element din coadă și îl returnează.

Operațiile de tip UPDATE sunt introduse în coadă prin intermediul funcției `enqueue()`, iar la întâlnirea operației `EXECUTE` se folosește funcția `dequeue()` pentru a scoate primul element din coadă. În funcție de comanda extrasă și argumentul acesteia, se apelează una din funcțiile definite mai sus (pentru operațiile UPDATE) și are loc actualizarea stivei `UNDO` pentru operațiile `MOVE_LEFT` și `MOVE_RIGHT`. Din moment ce funcția `dequeue()` returnează adresa nodului, fără a dealoca memoria, această memorie trebuie dealocată după folosirea informațiilor relevante.
<br />

---

**_Aspecte generale & Miscellaneous_**

* Pentru fiecare structură de date necesară au fost definite funcții pentru inițializare/distrugere.
* Nu au fost introduse alte funcționalități pe lângă cele cerute. 
* Am încercat să fac o verificare cât mai amănunțită când vine vorba de alocarea dinamică a memoriei și de existența/deschiderea cu succes a fișierelor. 
* Folosirea de macros în funcția `execute()` a fost o preferință personală (știu că puteam folosi un switch()). 

<br />

---

### **_Punctajul pe checker-ul local_**
<br />

Pe computer-ul personal, tema primește punctajul 100.0/100. aferent testelor și verificării existenței README-ului, iar în urma verificării cu Valgrind pentru leak-uri de memorie se obțin 20.0/20.0.

---
