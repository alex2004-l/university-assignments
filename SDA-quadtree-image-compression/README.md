## **Tema 2 - SDA - _Arbore cuaternar de compresie_- README**
---
###### _Lache Alexandra Florentina Georgiana, 311CD, Anul I, UPB ACS 2022-2023_

<br />

### **_Descrierea structurilor de date folosite_**
---

**QuadTree** - structura pentru arborele cuaternar. Are 3 câmpuri:
* type - indică tipul nodului, 0 pentru un nod interior și 1 pentru un nod frunză;
* pixel - reține valoarea RGB pentru un nod frunză;
* kids - vector de pointeri către copiii nodului curent; NULL pentru nodurile frunză.

**pixel** - structură de pixel; are 3 câmpuri unsigned char, r,g,b, pentru a reține canalele de culoare roșu, verde, albastru.

**PPMimg** - structură pentru o imagine PPM. Câmpurile acesteia sunt:
* file_type - tipul imaginii PPM (P6 pentru această temă);
* width & height - dimensiunile imaginii;
* max_range - valoarea maximă pentru canalele de culoare (255);
* matrix - matricea de pixeli asociată imaginii.

**QueueNode && Queue** - o coadă implementată cu ajutorul unei liste simplu înlănțuită. În noduri sunt reținute referințe către noduri din arborele cuaternar. Structura de coadă are 3 câmpuri:
* first & last - pointer către primul, respectiv ultimul,element din coadă;
* size - dimensiunea cozii;

---

### **_Descrierea implementării cerințelor temei_**

---

**Implementare arbore cuaternar de compresie** 

Dându-se o imagine în format PPM drept argument, datele acestei imaginii sunt reținuți într-o structură `PPMImg`. Arborele de compresie se obține cu ajutorul funcției `get_compression_tree()`, funcție care primește matricea pixelilor asociată imaginii, dimensiunea unei zone și coordonata de început.

Această funcție apelează la rândul ei funcție `get_mean_score()` pentru a determina scorul similarității canalelor de culoare pentru blocul respectiv. Dacă acest scor depășește pragul acceptat, în arbore este adăugat un nod intern ai cărui copii sunt apeluri recursive ale funcției pentru subdiviziuni ale matricii inițiale: primul copil - pătratul din stânga sus, al doilea copil - dreapta sus, al treilea copil - dreapta jos, al patrulea copil - stânga jos.

Celălalt caz posibil este cel în care scorul similarității nu depășește constrângerea impusă, caz în care în arbore este adăugat un nod frunză în care este reținut într-o structură `pixel` media canalelor de culoare RGB pentru zona primită (obținută cu ajutorul funcțiilor `get_red_score()`, `get_green_score()`, `get_blue_score()`).

**Cerința 1** 

Datele necesare pentru rezolvarea primei cerințe sunt obținute cu ajutorul funcțiilor `QTree_heigh()`, `QTree_min_path()` și `get_leaf_count()`. Numărul de niveluri alea arborelui coincide cu înălțimea acestuia + 1, iar numărul de blocuri din imagine pentru care scorul similarității pixelilor este mai mic sau egal decât factorul furnizat coincide cu numărul frunzelor, iar dimensiunea laturii celui mai mare pătrat este dimensiunea imaginii/2^(cea mai mica cale de la radacină la o frunză).

**Cerința 2** 

Pentru a obține formatul de compresie impus, este nevoie ca arborele cuaternar să fie parcurs pe niveluri, parcurgere care se poate realiza cu ajutorul unei cozi. În funcția `write_compressed_image()`, este inițializată o coadă în care la început este băgat nodul rădăcină. Cât timp această coadă are elemente, este scos primul element din coadă și:
* dacă acesta este un nod intern, în coadă sunt adăugați copiii nodului și este scris în fișier 0, corespunzător tipului nodului;
* dacă este un nod extern, în fișier este scris 1, corespunzător tipului nodului, și valoarea RGB reținută în câmpul pixel.

**Conversia formatului comprimat într-un arbore cuaternar**

Această conversie este realizată tot prin intermediul unei cozi. Se citește din fișier tipul primului nod (iar în cazul în care acesta este un nod extern, și citesc și valorile aferente canalelor de culoare) și în funcție de acesta este creat nodul, prin funcția `createQTreeIntNode()` ori `createQTreeExtNode()`.

Dacă primul nod citit, respectiv rădăcina arborelui, are tipul 1, înseamnă că imaginea comprimată este reprezntată de o culoare uniormă, caz în care termină prelucrarea fără adăugarea altor noduri în arbore. 

Dacă acest nod are tipul 0, adică este un nod intern, acesta este băgat într-o coadă. Are loc următoarea prelucrare până când coada rămâne fără elemente: se scoate primul element din coadă, pentru cei patru copii ai acestuia se citește din fișier tipul nodului, iar:
* dacă este 0, se creează un nod intern în arbore, care este adăugat în coadă;
* dacă este 1, se creează un nod extern aferent în arbore și se citește din fișier valoarea corespunzătoare pixelului (fără a se adăuga acest nod în coadă);

Observație: în coadă se vor adăuga doar noduri interne ale arborelui, deoarece pentru acestea trebuie să stabilim legăturile cu nodurile copil.

**Conversia arborelui cuaternar în imaginea decomprimată**

În funcția `decompress_img()`, are loc o parcurgere a arborelui de compresie în care:
* pentru nodurile interioare, functia este apelata recursiv pentru copii, iar zona din matrice aferentă este subdivizată ca mai înainte;
* pentru nodurile exterioare are loc completarea imaginii cu valoarea pixelului pentru zona primită ca argument.

---

// Punctaj checker local: 80p + 20p bonus
