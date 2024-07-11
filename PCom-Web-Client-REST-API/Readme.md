# TEMA 4 - PROTOCOALE DE COMUNICATII - CLIENT WEB. COMUNICATIE CU REST API

---
### Tema realizata de Lache Alexandra Florentina Georgiana, 321CD, 2023-2024

---

IN CADRUL TEMEI CURENT AM PLECAT DE LA SCHELETUL PENTRU LABORATORUL 9.
Alte resurse folosite :
- https://www.codeguru.com/cplusplus/switch-on-strings-in-c/ -> am incercat sa fac o varianta mai eleganta, cu un switch, in locul clasicului if..else, if..else

Cereri HTTP catre server :
- in fisierul request.cpp exista functii specializate pentru cererile GET/POST/DELETE care construiesc mesajul ce urmeaza a fi trimis catre server pe baza parametrilor primiti ca argumente
- exista o functie specializata send_server_request(), care actioneaza ca un fel de "factory" pentru cererile mai sus mentionate
- in cadrul acestei functii, se deschide socketul care face conexiunea cu serverul, se construieste si se trimite cererea catre server, se primeste raspunsul si se inchide conexiunea
- am separat intr-o functie specializata acest procedeu, deoarece in urma tuturor comenzilor valide ar trebui sa se realizeze aceeeasi pasi pentru comunicarea cu serverul, ele diferind prin mesajele trimise

Comenzile:
- se verifica ca existe credentialele necesare anumitor actiuni(token pentru a accesa cartile din biblioteca); se verifica existenta cookies pentru a demonstra autentificarea
- nu se permit comenzile register/login daca un utilizator este deja logat
- in cazul in care anumite input-uri de la user sunt invalide, de cere repetarea introducerii input-ului pana se da unul valid(nume de utilizator si parola fara spatii, numar natural pentru paginile cartii)
- dupa ce se primeste raspunsul de la server, se incearca parsarea json-ului(daca exista) sau afisarea unui mesaj de succes daca s-a primit ok

Parser JSON:
- am folosit parser-ul recomandat pentru Cpp in documentatia temei
- am folosit constructorul pentru crearea mesajelor, functia parse pentru parsarea mesajelor de la server si value pentru a obtine valoarea de la un anumit camp dupa parsare, si dump pentru a obtine un string dintr-un json

