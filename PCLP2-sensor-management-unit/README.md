## __Tema 1 - PCLP 2 - README__
---
###### _Lache Alexandra Florentina Georgiana, 311CD, Anul I, UPB ACS 2022-2023_

<br />

### __*Descrierea implementării cerințelor temei*__
---

Tema dată presupune implementarea unui program care alcătuiește un vector de senzori pentru mașini, acest vector având o restricție structurală bazată pe prioritatea celor doua tipuri de senzor, precum și implementarea a patru comenzi care se pot aplica fie elementelor vectorului, fie întregului vector: `print`, `analyze`, `clear`, `exit`.

#### __*Alcătuirea vectorului*__

---

Datele vectorului sunt citite dintr-un fișier binar primit ca argument, numărul de elemente din acesta fiind cunoscut.

Se disting două cazuri posibile pentru citirea datelor unui senzor: citirea informațiilor unui Tire Sensor și citirea informațiilor unui PMU Sensor. Pe lângă acestea, mai trebuie respectată și constrângerea legată de ordinea senzorilor în vector (PMU > Tire sensor). 

Funcțiile definite pentru construirea vectorului sunt: 
- `get_tire_sensor()` - funcție care citește din fișier datele unui Tire Sensor primit ca parametru (sub formă de pointer) și alocă dinamic memorie pentru câmpurile `sensor_data` și `operations_idxs`;
- `get_power_management_unit()` - echivalentul funcției de mai sus pentru senzorii de tip PMU;
- `get_list_of_sensors()` - funcție care primește ca argument un vector de tipul sensors*; pentru fiecare element, se citește tipul acestuia, și în funcție de el este apelată una din cele două funcții menționate mai sus. În plus, pentru a respecta ordinea impusă a elementelor, înainte de a se citi un element de tipul PMU, se realizează o permutare la dreapta cu o poziție a tuturor elementelor de tipul Tire Sensor (folosind funcția `copy()`);
- `copy()` - funcție care copiază la adresa primului senzor primit ca parametru informațiile celui de-al doilea senzor primit ca parametru (fără dealocarea memoriei vreunui senzor, deci pentru a nu avea leak-uri aceasta funcție trebuie folosită când mai există alte referințe la câmpurile primului senzor);

#### __*Printarea informațiilor din senzori*__

---

Citirea comenzilor de la tastatură se realizează în cadrul funcției `main()`. La citirea comenzii `print` se citește și argumentul corespunzător și dacă acesta este valid, se apelează una din următoarele funcții: 

- `print_tire_sensor()` - pentru senzorii de tip TIRE;
- `print_power_management_unit()` - pentru senzorii PMU;

Ambele funcții primesc ca parametru data unui senzor, îi fac cast la tipul senzorului respectiv și realizează afișarea informațiilor conform specificațiilor. 

#### __*Implementare analyze*__

---

La citirea comenzii `analyze` în funcția `main()`, asemenanator cu printarea, se citește și argumentul corespunzător și i se verifică corectitudinea. Dacă argumentul este valid se apelează funcția `analyze_senzor()`.

În cadrul acestei funcții se declară un vector operations, care prin apelarea funcției `get_operations()` ajunge să conțină pointeri la funcțiile operațiilor din `operations.c`.

Aceste referințe la funcții sunt folosite prin intermediul unui pointer la o funcție de tip void, `fun_ptr`, care primește ca argument un element de tip `void *` pentru realizarea tuturor operațiilor pe datele unui senzor. 

#### __*Implementare clear*__

---

La citirea comenzii `clear` se apelează funcția `clear_list_of_senzors()`. Această funcție verifică pentru toți senzorii din vector corectitudinea datelor (cu ajutorul macro-ului `flag_assert`), și la întâlnirea unui senzor eronat, dealocă memoria pentru acel senzor și realizează o permutare la stânga a tuturor elementelor din vector care urmau după senzorul eliminat.

Numărul de senzori este dat sub formă de pointer ca parametru acestei funcții, pentru a garanta modificarea acestuia după terminarea executării funcției si a permite dealocarea. 

#### __*Dealocarea memoriei*__

___

După întâlnirea comenzii `exit` în cadrul buclei while din `main()` are loc mai întâi dealocarea memoriei pentru câmpurile `sensor_data` și `operations_idxs` ale fiecărui senzor, urmând dealocarea memoriei pentru întreg vectorul. După aceea, are loc închiderea fișierului.

Vectorul operations este definit doar la nivelul funcției `analyze_sensor()`, unde are loc atât alocarea memoriei pentru el, cât și dealocarea acesteia, deci nu trebuie dealocat la final.

---
FIN.
