### Tema 2 Proiectarea Algoritmilor
---
##### Tema realizata de Lache Alexandra Florentina Georgiana, 321CD
---

Task 1 - Numarare
- Se "suprapun" cele doua grafuri, adica se construieste un singur graf ce contine doar muchiile comune ale acestora.
- Se aplica algoritmul de sortare topologica (se poate aplica acest algoritm deoarece graful este orientat aciclic) pentru a stabili dependintele intre nodurile grafului -> am aplicat sortarea de tip BFS(algoritmul lui Kahn), o sortare de tip DFS nefiind fezabila din cauza numarului mare de noduri;
- Folosind sortarea topologica, se calculeaza numarul de drumuri elementare de la nodul 1 la nodul n astfel : pentru un nod oarecare i, numarul drumurilor elementare de la nodul i pana la nodul n este egala cu suma tuturor drumurilor elementare de la un nod oarecare j la n, cu proprietatea ca exista o muchie orientata de la i la j
- Astfel, aplicand aceasta recurenta in sens invers, se obtine numarul total de drumuri elementare de la 1 la n
- O alta solutie posibila la care m-am gandit implica o combinatie de parcurgere DFS cu programare dinamica pentru generarea tuturor drumurilor posibile de la 1 la n si memorarea apelurilor recursive duplicat
- Complexitatea spatiala : O(n + m) pentru constructia listei de adiacenta, O(n) auxiliar pentru coada folosita in sortare topologica si vectorul in care se retine ordinea rezultata in urma sortarii topologice, O(n) pentru vectorul de in_degree
- Complexitate temporala : O(n + m) pentru sortarea topologica, O(n + m) pentru calcularea numarului de drumuri elementare

Task 2 - Trenuri
- Am folosit un hash_map pentru a asocia fiecarui oras un anumit id
- Se aplica acelasi rationament ca la primul task, dar acum pentru fiecare nod i se retine numarul maxim de noduri care exista pentru o cale de la i la stop
- Complexitatea spatiala : O(n + m) pentru constructia listei de adiacenta, O(n) auxiliar pentru coada folosita in sortare topologica si vectorul in care se retine ordinea rezultata in urma sortarii topologice, O(n) pentru hashmap, O(n) pentru vectorul de in_degree
- Complexitate temporala : O(n + m) pentru sortarea topologica, O(n + m) pentru calcularea numarului de drumuri elementare

- Implementare topological sort adaptata de la https://www.geeksforgeeks.org/topological-sorting/

Task 3 - Drumuri
- Se calculeaza folosind Dijkstra drumurile de cost minim pornind din nodurile x si y pentru graful dat, respectiv pornind din z pentru graful inversat;
- Pentru fiecare nod i din graf, se calculeaza suma minima a costurilor muchiilor ca suma distantei de la x la i (calculata cu primul Dijkstra), distantei de la y la i (calculata cu al doilea Dijkstra) si a distantei de i la z (calculata cu al treilea Dijkstra)
- Dupa ce se calculeaza aceasta suma pentru toate nodurile din graf, se alege suma minima
- Complexitatea spatiala :  O(n + m) pentru constructia listelor de adiacenta, O(n) pentru coada de prioritati, vectorul de distante, vectorul de visited, O(1) pentru suma minima
- Complexitatea temporala : O(m * logn) pentru Dijkstra, O(n) pentru calculearea sumei minime
- Initial, am plecat de la implementarea de Dijkstra din solutia oficiala de laborator, dar am fost nevoita sa inlocuiesc set cu priority_queue pentru o performanta mai buna(operatiile pe priority_queue de scoatere de elemente se realizeaza in O(1))
- Nota : Initial, ideea aplicata a fost sa folosesc doar doua aplicari ale algoritmului Dijkstra, pornind din x si pornind din y, sa aflu drumurile pana la z si sa incerc sa imbunatatesc suma minima prin combinarea lor; aceasta idee era eronata, deoarece presupunea ca in drumul final se gaseste fie drumul minim de la x la z, fie drumul minim de la y la z

Task 4 - Scandal
- Se observa ca problema se reduce de fapt la 2-SAT, la satisfacerea unei expresii logice booleene.
- Se construieste un grafic, unde cele 4 reguli se translateaza astfel
    - regula 0 : cel putin x sau y sunt invitati => daca x nu e invitat, atunci y trebuie neaparat invitat, iar daca y nu este invitat, atunci x trebuie invitat
    - regula 1 : daca x nu participa la petrecere, atunci nici y nu participa => !x implica !y, sau expresia negata y implica x
    - regula 2 : daca y nu participa la petrecere, atunci nici x nu participa => !y implica !x, sau expresia negata x implica y
    - regula 3 : cel putin unul dintre x si y nu este invitat => daca x e invitat, y trebuie sa nu fie invitat, respectiv daca y este invitat, atunci x nu este invitat
- Folosim algoritmul lui Kosajaru pentru a determina componentele tari conexe din graf. Prin aplicarea algoritmului, acestea vor fi si sortate topologic. Importanta faptului ca sunt sortate topologic consta in faptul ca se faciliteaza prelucrarea datelor.
- Se poate intampla in graful rezultat ca de la nodul x sa fie accesibil nodul !x, sau viceversa. In acest caz trebuie aleasa valoarea de adevar asociata astfel incat sa nu se produca o contradictie. 
- Daca componentat conexa din care face parte x apare in sortarea topologica inaintea componentei din care face parte !x (component[x] < component[!x]), atunci lui x ii este atribuita valoarea logica false. In cazul invers (component[x] > component[!x]), lui x ii este atribuita valoarea true, deci in cadrul problemei noastre, invitatul x face parte din lista finala de invitati. Nu se va ajunge la cazul comp[x] == comp[!x] conform cerintei problemei (exista o combinatie de valori de adevar care duc la satisfacere expresiei booleene).Demonstratia relatiei de mai sus am gasit-o la https://cp-algorithms.com/graph/2SAT.html;
- Algoritmul Kosajaru este preluat si adaptat de la https://www.topcoder.com/thrive/articles/kosarajus-algorithm-for-strongly-connected-components;
- Complexitate temporala : Kosajaru - O(n + m), construire vector solution - O(n)
- Complexitate spatiala : O(m + n) pentru listele de adiacenta, O(n) apeluri recursive DFS, O(n) - vectori folositi