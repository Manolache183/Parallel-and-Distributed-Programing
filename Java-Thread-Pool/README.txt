Manolache Marius-Alexandru, 333CA

Entry point-ul este Team2.java, unde se gaseste si metoda main:
- am creat un ExecutorService de marime P pentru thread-urile de level 2, acesta
va fi populat de thread-urile de level 1, fiecare task reprezentand practic un produs

- am pornit P thread-uri de level 1 si am calculat 2 valori necesare pentru
bonus: size (marimea fisierului in bytes, data de sistemul de operare) offsetul
de unde si pana unde ar trebui sa citeasca (cu formulele din lab 1).

OrderThread.java:
- am facut impartirea thread-urilor de level 1 conform cu bonusul, o sa specific
cum in README_BONUS, calculand un offset

- avand offset-ul, am inceput citirea linie cu linie pana cand endPoint-ul este
depasit

- pentru fiecare linie (care reprezinta de fapt o comanda) am adaugat in
ExecutorService produsele sale aferente, asignand fiecarui Task un numar de produse
peste care thread-ul de nivel 2 ce se va ocupa de acel Task trebuie sa sara (pentru
a ajunge la produsul sau)

- cu ajutorul unui semafor (initializat negativ) am asteptat terminarea produselor
comenzii, ca apoi si comanda sa fie shipped, ajutandu-ma de un semafor de size 1
(practic un mutex) ca sa nu am race condition pe output.

ProductThread.java:

- pur si simplu am cautat produsul cu numarul dat de thread-ul parinte, linie
cu linie, iar la final am incrementat semaforul thread-ului de nivel 1 responsabil
cu comanda.



