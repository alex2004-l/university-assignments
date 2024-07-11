## Maze Navigation Using Markov Chains (MAY 2023)
---
#### This project was implemented as part of the numerical methods coursework. The project involves navigating a robot through a labyrinth using Markov Chains and various optimization techniques. Optimization algorithms are used to determine the optimal path, specifically the Jacobi iterative method and heuristic greedy algorithms.
---

1) parse_labyrinth() 
- după citirea dimensiunilor matricii Labyrinth, am folosit opțiunea funcției fscanf din Octave pentru a citi o matrice de dimensiune [m,n] (Notă: aplicarea acestei metode a trebui să fie însoțită de transpunerea matricii pentru obținerea rezultatului corect).

2) get_adjacency_matrix() 
- am folosit funcția dec2bin pentru a obține o matrice cu codificările binare ale elementelelor matricei Labyrinth primită ca argument
- pentru fiecare element, am verificat dacă vreunul din biții 1-4 era setat corespunzător (aveau valoare 0) pentru a determina dacă există posibilitatea de deplasare în acea direcție (1 - Nord, 2 - Sud, 3 - Est, 4 - Vest)
- am creat doi vectori, row & column, în care rețineam linia, respectiv coloana elementelor care trebuie adăugate în matricea de adiacență 
- pentru fiecare din cele două direcții a trebuit să abordez două cazuri distincte:

 1) Nord: robotul putea avansa nord și se afla pe primul rând -> ajungea la poziția de WIN SAU robotul putea avansa nord și se afla pe un rând diferit de primul rând -> există un drum între poziția actuală și cea de deasupra
 2) Sud: robotul se putea deplasa sud și se afla pe ultimul rând -> ajungea la poziția de WIN SAU robotul se putea deplasa sud și se afla pe un rând diferit de ultimul rând -> există un drum între poziția actuală și cea de dedesubt
 3) Est: robotul se afla pe ultima coloană - ajungea la poziția LOSE sau robotul se afla pe o coloană diferiră de ultima - se putea deplasa la dreapta
 4) Vest: robotul se afla pe prima coloană - ajungea la poziția LOSE sau robotul se afla pe o coloană diferită de prima - se putea deplasa cu o poziție la stânga
 
 - din moment ce toate valorile din matricea de adiacență sunt 1, am inițializat la final vectorul value care conține doar valori de 1
 - am folosit funcția sparse pentru a crea matricea de adiacență Adj din vectorii row, column & value.

 Notă: nu știu dacă e împotriva normelor de Codying Style, dar scrierea if-urilor sub forma cond && (instr_1) || (instr_2) mi s-a părut mai adecvată pentru citirea condițiilor; sorry dacă nu respectă

 3. get_link_matrix()

 - am procedat foarte asemănător ca la funcția de mai sus, doar că în acest caz nu aveam o valoare constantă în matrice, așa că a trebuit să regândesc maniera în care obțin vectorul value
 - pentru fiecare poziție a labirintului, aflam numărul de direcții posibile în care poate merge roboțelul - count, iar în vectorul de valori adăugam count valori de 1/count
- matricea Link se obține tot cu funcția sparse din trei vectori;

4. get_Jacobi_parameters()

- matricea G - iniția matricea Link din care am eliminat ultimele 2 linii și respectiv coloane (notă: am observat că nu puteam elimina direct coloanele din matricea sparse - nu știu dacă e de la lipsa mea de skill cu acest limbaj  sau de la modul de implementare al matricilor rare în Octave - așa că a trebuit să transpun de două ori matricea G)
- vectorul c - din matricea Link am selectat doar coloana win, primele win-1 elemente (echivalent la m*n, unde m,n sunt dimensiunile matricei Labyrinth)

5. perform_iterative()

- am aproximat prim metoda Jacobi soluția, pornind de la soluția inițială vectorul nul, cu ajutorul matricei și vectorului de iterație, primiți ca parametri
- iterarea unei noi soluții se oprește când s-a atins numărul maxim de pași sau s-a ajuns la o soluție care convine din punct de vedere al toleranței
- dacă niciuna dintre condiții nu este îndeplinită, soluția aflată la pasul curent al iterației devine soluția anterioară (x0), și se reia procedeul de aflare al soluției.

6. heuristic_greedy()

- am implementat în concordanță cu pseudocodul propus această soluție 
- vectorul path are initial doar poziția de start, și, plecând de la această poziție, în vector este adăugată poziția adiacentă cu cea mai mare probabilitate de câștig
- dacă nu există vreo poziție adiacentă nevizitată (lucru indicat de variabila flag), se scoate ultimul element din vectorul path
- această derulare de acțiuni continuă până se ajunge la WIN sau vectorul path rămâne fără elemente

7. decode_path()

- a trebuit să analizez 2 cazuri: caz în care poziția din path corespunde unei poziții aflate pe ultima coloană a matricei inițiale - poziția din path se traduce drept câtul împărțirii la numărul de linii și numărul de coloane, respectiv elementele care nu corespund ultimei coloane - același cât + 1, plus restul împărțirii dintre număr și numărul de coloane (notă: eu am gândit cu numărul de coloane, dar punctajul pe checker l-am luat doar folosind numărul de linii, presupun că iar e lipsă de skill și gândire din partea mea aici)

---