Manolache Marius-Alexandru, 333CA

Dupa cum spuneam, in Main.java obtin 2 valori, marimea fisierului in bytes si
impartirea acesteia la numarul de thread-uri.

In OrderThread.java functia getOffset calculeaza "bucata" aferenta fiecarui thread
de nivel 1, astfel:

- plecam de la un prevOffset = numarul thread-ului inmultit cu cat are de citit

- daca rezultatul este 0, returnam acest prevOffset

- altfel, daca caracterul imediat la stanga (prevOffset - 1) este un '\n', inseamna
ca suntem la inceputul unei linii noi, deci din nou returnam acest prevOffset

- daca insa nimerim in mijlocul unei linii, vom considera ca acea linie a fost deja
prelucrata, si cautam la dreapta urmatorul '\n', pentru a gasi urmatoarea linie

In metoda run() se citeste pana cand este depasit acel toRead, deci practic si linia
abia inceputa.

Ex:

$ cat orders\_out.txt
o_lovfpw0,3,shipped
o_z1educ4,6,shipped
o_dhms1d5,7,shipped

P = 2;

Pentru primul thread de nivel 1, prevOffset va fi chiar 0 (deoarece este thread-ul 0)
acesta va citi primele 2 linii (fiind doua thread-uri, size / P va pica chiar in
mijloc). Abia dupa ce citeste si a doua linie va vedea ca a depasit limita de citit.

Al doilea thread de nivel 1 va nimeri in mijlocul celei de-a doua linii, o va considera
citita, si va trece mai departe la urmatoare linie (a treia), de unde va incepe sa 
citeasca si se va opri la finalul fisierului.

Practic am stabilit o conventie: daca un thread nimereste in mijlocul unei linii,
considera ca linia respectiva a fost citita si citeste incepand cu urmatoarea, pana
cand depaseste toRead.


