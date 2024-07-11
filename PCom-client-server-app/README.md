## Tema 2- PROTOCOALE DE COMUNICATII
---
#### Tema realizata de Lache Alexandra Florentina Georgiana, 321 CD
---

### ! In rezolvarea acestei teme am pornit de la scheletul pentru laboratorul 7.

`Server`

- in aplicatia server, se deschid inital doi socketi, unul pentru a primi mesajele de la clientii UDP si unul pentru a accepta cererile de conectare de la clientii TCP
- multiplexarea I/O se realizeaza cu ajutorul functiilor din biblioteca `poll.h`
- pentru a nu limita numarul de clienti, file descriptorii asociati socket-urilor care fac legatura cu clientii sunt salvati intr-un vector -> structura de date dinamica
- se foloseste la functia de listen INT_MAX, pentru a ne asigura ca exista suficient spatiu in coada de asteptare pentru conectarea la server si ca nu se pierd conexiune 
- se seteaza socketul TCP drept reusable pentru a putea reporni aplicatia fara probleme imediat dupa ce a fost inchisa (daca se vrea folosirea aceluiasi port pentru conexiunile TCP)
- informatiile referitoare la clientii activi ssunt retinute intr-un `unordered_map` sub forma de pereche (id, port la care este conectat), in timp ce informatiile referitoare la abonarile tuturor clientilor sunt retinute de asemenea tot intr-un `unordered_map`
- cand se primeste un mesaj catre server, se verifica daca acesta a fost primit de la stdin, de la un client UDP ori de la un client TCP
- de la stdin se verifica daca este stringul de iesire, caz in care se iese din bucla programului si se inchid toti file descriptorii
- mesajele primite de la clientii UDP sunt primite, decodificate in server si apoi trimise catre client
- de fiecare data cand un client incearca sa se conecteze, daca exista deja un client online cu acelasi id, conexiunea este inchisa (un client isi trimite id-ul imediat cum serverul accepta conexiunea)
- mesajele primite de la clienti sunt decodificate, iar topicurile sunt scose/adaugate din lista aferenta de abonari a fiecaruia in functie de comanda data de client
- daca pe socket-ul unui client se primeste EOF, inseamna ca acesta a inchis conexiunea, si se scoate de lista de utilizatori online; de asemenea, se elimina file descriptor-ul asociat acestuia 
- alegere de decodificare a mesajelor primite de la clientii UDP in aplicatia server a fost pentru a reduce efortul computational din partea clientilor, mai ales in cazul in care mai multi clienti sunt abonati la aceleasi topicuri; din moment ce decodificarea nu este o operatie atat de costisitoarea, aceasta ar fi putut fi realizata si in aplicatiile clienti, fara o pierdere mare de eficienta

`Client`
- in aplicatia client, se deschide un socket pentru conectarea la server (se dezactiveaza algoritmul lui Nagle pentru socketul creat)
- odata stabilita conexiunea cu serverul, clientul isi trimite id-ul
- daca este primita o comanda de la stdin, in functie de aceasta exista mai multe variante: comanda exit inchide clientul, comenzile subscribe/unsubscribe determina crearea si trimiterea de pachete catre server pentru abonarea, resprectiv dezabonarea de la diversele topicuri
- daca se primeste un mesaj de la server, acesta este afisat conform cerintei
- atunci cand serverul de inchide, pe socketul care realiza conexiunea cu acesta se va primi EOF, iar clientul se va inchide de asemenea

`Protocol nivel aplicatie peste TCP`
- din moment ce informatiile transmise prin socket-ul TCP se asemeana cu un stream de date, iar mesajele pot varia foarte mult in lungime (de la cative la 1500 bytes), am gasit de cuviinta ca pentru o buna functionare ar trebui limitata cantitea de informatii redundante trimise 
- asadar, atunci cand serverul incerca sa trimita un mesaj unui client, mai intai trimite un header asociat mesajului, unde se afla si lungimea payload-ului ce trebuie primit, urmand a se primi payload-ul
- astfel, nu este necesara trimiterea unui payload de dimensiune 1500 atat la mesajele de tip 'INT' cat si la mesajele 'STRING' de lungime maxima