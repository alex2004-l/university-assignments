## **_Tema 1 - PCLP 4 - RustyBox_**
---
###### _Tema realizată de Lache Alexandra Florentina Georgiana, 321 CD, Anul II, UNSTPB ACS 2023-2024_
---
<br>

### Descrierea cerinței
În cadrul acestei teme, a fost necesară implementarea unui utilitar numit `Rustybox`, un utilitar scris in Linux ce permite executarea unor comenzi de tip Linux bash. 

---

### Implementarea comenzilor

1. `pwd` => pentru a obține calea absolută a directorului curent în ierarhia de fișiere am folosit funcția current_dir() din modului std::env, iar înainte de a printa calea, am verifcat ca rezultatul să fie valid(std::env::current_dir() întoarce un Result<> cu path-ul ori o eroare);

2. `echo` => inițial, se verifică dacă al doilea argument primit din linia de comanda este `-n`, pentru a se ști dacă printarea celorlalte argumente va fi succedată sau nu de o nouă linie; după aceea, sunt afișate restul argumentelor în ordinea primită;

3. `cat` => pentru toate argumentele primite din linia de comandă, se verifică dacă fișierele pot fi deschise, iar ,în caz afirmativ, se copiază conținul acestora folosindu-se funcția read_to_string(), string-ul rezultat fiind afișat la ieșirea standard; în cazul în care vreun fișier nu poate fi deschis, se întoarce codul de eroare -20;

4. `mkdir` => pentru toate argumentele primite din linia de comandă se apelează funcția create_dir() din modulul standard fs, iar în caz de eroare la creare se returnează codul -30;

5. `mv` => se verifică dacă s-a primit numărul valid de parametri, iar în caz afirmativ se apelează std::fs::rename(); în caz de eroare, se returnează -40;

6. `ln` => se verifică dacă numărul de argumente este valid și dacă s-a primit parametrul -s/--symbolic; dacă nu s-a primit argumentul, se folosește funcția std::fs::hard_link pentru a crea un link între fișiere, iar dacă s-a primit se folosește funcția std::os::unix::fs::symlink pentru a crea link-ul simbolic; dacă nu se poate crea link-ul se returnează -50;

7. `rmdir` => pentru toate argumentele primite în linia de comandă se apelează funcția remove_dir() din modulul standard fs, iar în caz de eroare se returnează codul -60;

8. `rm` => se verifica flagurile primite ca argumente, pentru a sti ce fel de fisiere pot sau nu pot fi sterse; indiferent de flag, fisierele sunt sterse folosindu-se functia remove_file(), iar in cazul in care apare o eroare la eliminarea fisierului, se intoarce codul -70; pentru a elimina doar directoarele goale atunci cand se primeste argumentul -d, se foloseste functia remove_dir() care sterge directoarele goale, iar in cazul primirii parametrului -r, se foloseste functia remove_dir_all(), care sterge atat directorul de la calea primita ca argument, cat si toate intrarile acestuia; se verifica daca argumentele primite sunt fisiere ori directoare, folosind metodele is_dir() si is_file() aplicate variabilelor de tip path corespunzatoare;
Nota: functia aceasta ar putea fi imbunatatita prin adaugarea unei functii suplimentare care primeste flag-uri pentru -d si -r, dar timpul e in defavoarea mea :( ;

9. `ls` => pentru implementarea acestei functii, m-am folosit de doua functii auxiliare, folosite in functie de existenta sau absenta parametrului -r:
    * ls_path_directory() - in care se creeaza un iterator pentru directorul curent folosindu-se functia fs::read_dir(); pentru toate intrarile din iterator, se printeaza numele acestora(numele se obtine aplicand metoda file_name() intrarilor, urmata din .into_string().unwrap() pentru a formata drept String output-ul), iar in functie de valoarea parametrului all_flag, se afiseaza ori nu intrarile ascunse;
    * ls_path_recursive() - functia se aseamana ca implementare cu cea anterioara, dar pentru a respecta ordinea afisarii fisierelor, numele intrarilor sunt salvate in doi vectori, in functie de tipul acestora, urmand a fi printate dupa iterarea prin tot directorul; ulterior, se apeleaza recuriv functia pentru toate intrarile care sunt directoare din directorul curent folosindu-se caile catre acestea drept path;

10. `cp` => pentru implementarea acestei functii, m-am folosit de doua functii auxiliare, folosite in functie de tipul fisierului de copiat:
    * copy_file(), care in functie de tipul de intrare primit ca parametru destination, creeaza o noua intrare la acea cale ori copiaza intr-un fisier continutul fisierului source
    * copy_directory_rec() - functie pentru copierea recursiva a unui director, cand are loc si redenumirea acestuia; ca modalitate de parcurgere, se aseamana cu implementarea facuta la ls_path_recursive(); in cazul in care o intrare este un fisier, se apeleaza copy_file(), iar pentru un director, acesta se creeaza la noul path cu create_dir(), urmand sa se reapeleze recursiv functia copy_directory_rec();
Nota: nu am inteles chiar diferenta dintre cazul cu redenumire si cel fara la cp -r; mi-a fost mai usor sa-l implementez doar pe cel fara;

11. `touch` - daca se apeleaza functia fara parametru, se verifica existenta in sistemul de fisiere a caii respective; daca nu exista, se va crea fisierul(.create(true) la OpenOptions), iar daca deja exista, pentru a modifica data si ora de acces/modificare se creeaza o copie a fisierului cu acelasi nume si continut(se foloseste o copie intermediara cu nume diferit); atunci cand se primeste parametul -m, pentru a schimba timpul ultimei modificari, se foloseste functia write() pentru a scrie in acesta, dupa ca se reseteaza dimensiunea la cea de dinaintea scrierii, iar in cauzul -a, se citeste continutul fisierului, pentru a modifica data la care a avut loc ultimul acces;

12. `chmod` - in functie de tipul parametrului primit pentru chmod, exista doua variante: daca permisiunile se primesc sub forma numerica, argumentul de convesteste din string in u32 cu functia from_string_radix, urmand a fi setate noile permisiuni, iar daca acestea se primesc sub forma de string, pentru adaugarea/stergerea de permisiuni specifice, se creeaza o masca care reflecta la ce entitate se refera permisiunile(utilizator/grup/other/all) si o valoare reprezentand cumulul de permisiuni ce vor fi modificate, aceste fiind ulterior utilizate pentru a modifica permisiunile curente (pentru adaugare, se face | pe biti cu permisuniel curente, pentru stergere se face & pe biti cu complementul in baza 8).


