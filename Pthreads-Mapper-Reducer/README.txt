Manolache Marius-Alexandru, 333CA

Implementarea e destul de straight forward, cateva lucruri merita mentionate:

1. Sincronizarea:
	- fiecare mapper trebuie sa verifice daca mai sunt fisiere de parsat, iar
operatia asta trebuie sa fie atomica (am folosit un mutex pentru asta, din considerente
de race condition), daca un fisier este luat in lucru de un mapper sau terminat, se
scoate din setul care tine numele fisierelor.
	- bariera de (M + R), la sfarsitul thread-urilor mapper si la inceputul celor
reducer, pentru a ma asigura ca se termina mai intai toate thread-urile mapper
inainte ca cele reducer sa isi inceapa treaba. Am facut asta pentru ca asa e
constrangerea in enunt, ar fi fost mai rapid si mai intuitiv sa existe o bariera
de (M + 1) asteptata la finalul mapperelor si in main inainte de crearea reducerelor.

2. Calculul de puteri:
	- am pus toate numerele din fisier intr-un set, si am ridicat toate numerele
de la 2 la maximul fisierului la toate puterile pana la R, iar apoi am verificat
daca exista in fisier rezultatul. Exista un edge case pt numarul "1", in cazul in
care exista in fisier, trebuie adaugat in lista corespunzatoare fiecarei puteri.

3. Structurile de argumente:
	- unele elemente sunt partajate intre mapere, altele intre reducere, altele
sunt comune atat pentru reducere cat pentru mapere (cum ar fi lista cu puterile
perfecte). Am folosit pointeri pentru resursele ce trebuiesc partajate.
	- reducerele vor prelua lista mai sus mentionata si din ea vor extrage doar
sub-array-ul destinat lor, apoi vor completa fisierele de output, scapand de duplicate.