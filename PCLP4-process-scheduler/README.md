## __Readme - Tema 2 - Process Scheduler - PCLP4__

---

##### Temă realizată de Lache Alexandra Florentina Georgiana, 321 CD, Anul II, 2023 - 2024
---

### Funcțiile pentru Scheduler
---

Funcția next: 
- inițial, se verifică dacă în coada de ready se află procesul care a rulat anterior;
- în caz afirmativ, se verifică dacă acesta se poate replanifica, comparându-se timeslice-ul rămas pentru execuția acestuia cu cel minim;
- dacă este mai mare, procesul este planificat să ruleze pentru restul timeslice-ului, iar în caz contrar, trece la finalul cozii de ready și este planificat procesul din vârful cozii;
- în cazul în care nu se află la începutul listei cu procese în stare ready procesul rulat anterior, se alege pentru rulare elementul din vârful listei, dacă mai sunt elemente în coadă, sau se tratează cazurile de Done, Panic, Deadlock și Sleep.

---
Funcția stop:
- Syscall -> dacă procesul se oprește din cauza unui apel de sistem, în funcție de tipul acestuia, au loc diferite prelucrări asupra cozilor de procese;
- Fork -> se crează un nou proces și se adaugă la finalul cozii de Ready;
- Sleep -> se actualizează câmpul de sleep pentru procesul curent, după care acesta este trecut în coada de waiting;
- Wait -> se actualizează câmpul state pentru procesul curent, după care acesta este trecut în coada de waiting;
- Signal -> se verifică în coada de waiting dacă există procese care așteaptă semnalul cu codul primit, iar în caz afirmativ, aceste procese sunt trecete în starea de ready;
- Exit -> se elimină definitiv procesul din lista de procese;
- Expired -> dacă durata de executare a procesului a expirat, acesta este pus la finalul cozii cu procesele ready;
- în cadrul funcției de stop se actualizează timpul de execuție + syscall pentru procesul curent, respectiv timpul total pentru toate procesele;

---
Funcția list:
- returnează o listă cu toate procesele, formată din contopirea listelor de cu procese în starea ready și a listei cu procesele în starea ready;

---

### Implementare Round Robin
---

Structura de date folosita pentru planificator:

- ready_processes și waiting_processes = două vecdeque care rețin o listă cu procesele în starea de ready(procese care așteaptă să fie planificate), respectiv in starea de waiting(procese care sunt în sleep/ așteaptă un semnal extern); procesul care rulează pe procesor se află în vârful cozii de ready;

- timeslice, minimum_remining_timesclice, remaining_timeslice = numărul de unități de timp petru care este planificat inițial un proces, durata minimă necesară ca acesta să fie replanificat în urma unui stop, respectiv durată de execuție rămasă procesului care rulează la momentul curent;

- next_pid = câmp care reține următorul pid care se generează în urma unui apel fork;

- sleep, sleep_time = valori folosite în urma unei perioade în care procesorul s-a aflat în stare de sleep, pentru a verifica momentul la care acesta a fost și a actualiza timpul total al tuturor proceselor.

---

### Implementare Round Robin cu priorități
---

Față de implementarea planificatorului Round Robin simplu, la această implementare se adaugă două aspecte: 
- de fiecare dată când are loc un apel de stop, prioritatea procesului curent scade, dacă durata alocată rulării acestuia pe procesor a expirat, sau crește, dacă a realizat un syscall (nu poate depăși durata inițială);
- înainte de planificarea altui proces pentru rulare, are loc o ordonare a tuturor proceselor în funcție de prioritatea acestora;
---

### Implementare CFS
---
Față de implementarea planificatorului Round Robin simplu, la această implementare se adaugă trei aspecte: 
- la fiecare apel al funcție stop, se incrementează virtual runtime-ul procesului și se generează string-ul "vruntime=val";
- se recaluculeaza inainte de planificarea unui proces valoare timeslice-ului(numarul de procese a crescut/scazut de la un apel la altul);
- înainte de planificarea altui proces pentru rulare, are loc o ordonare a tuturor proceselor în funcție vruntime-ul acestora, iar in caz de egalitate, se iau procesele in ordinea crescatoare a pid-ului;
---

### Structura pentru retinerea informatiilor despre procese
---

Structura folosita pentru reținerea informațiilor despre procese are următoarele câmpuri:
- pid = id-ul procesului(diferit de 0);
- priority, actual_priority = prioritatea acestuia la un moment dat în execuția programului, respectiv prioritatea inițiala primită la fork(primul câmp variază doar în cazul planificatorului Round Robin cu priorități);
- total, syscall, execute = timpul total, timpul in care procesul a facut syscall-uri și timpul de executare pe procesor;
- sleep = timpul rămas procesului în starea de sleep(0 pentru procesele care nu sunt în starea de sleep);
- state = starea procesului;
- vruntime, extra = timpul virtual de rulare pe procesor, sub forma de string, respectiv sub formă de usize.

---

### Dificultăți de implementare și îmbunătățiri pe care as vrea să le aduc codului
---
- nu am stiut cum sa realizez o singura implementare pentru functiile auxiliare create pentru RoundRobin, PriorityQueueRoundRobin, CFS -> mult cod duplicat, care ar fi putut fi evitat;
- nu am implementat trait-ul partialOrd ori add -> m-am folosit de implementarea din biblioteca standard rust pentru tipul usize si am sortat vectorul folosind metoda sort_by; nu am inteles daca acest trait ar fi trebuit aplicat structurii de proces si cum s-ar fi realizat acest lucru(nu ar fi intrat in conflict cele doua implementari distincte necesare pentru Round Robin cu prioritati si CFS);
