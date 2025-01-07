# SAT Solver

In `assignemnts` salvez valoarea de adevar a literalilor sau UNASSIGNED daca inca nu au primit o valoare.

Am implementat `pure_literal_elimination`. Pure literal elimination inseamna ca asignez de la inceput o valoare de adevar literalilor care apar doar cu + sau doar cu - in clauze. Cei care apar negati primesc `FALSE`, ceilalti primesc `TRUE`. La final elimin din vectorul de clauze clauzele care devin adevarate in urma operatiilor adevarate.

O alta optimizare pe care am implementat-o este eliminirea clauzelor adevarate dupa ce fac propagarea unitara. Totusi, daca atribuirea nu este buna, trebuie sa pot sa le adaug inpoi inainte sa trec la urmatoarea iteratie. Ca sa pot face asta am creat o stiva. Inainte sa fac propagare unitara sau sa adaug o noua valoare de adevar unui literal, salvez starea curenta in stiva. Daca ceva nu a mers bine, inainte sa trec la urmatoarea iteratie dau pop ca sa restaurez starea clauzelor si a literalilor. Nu pot folosi simple variabile globale si imi trebuie o stiva din cauza recursivitatii (ce as salva in var globale normale poate sa nu fie ultima stare salvata inainte sa apelez `unit_propagation`, ci ceva ramas de la un alt apel recursiv). Stiva "merge" in acelasi timp cu stiva de recursivitate si imi asigura ca dau pop la starea corecta.

In plus, am gasit inca o modalitate de optimizare a codului. Mi-am creat o lista cu literalii ordonati dupa frecventa, dupa numarul de aparitii in clauze, fie ca apareau negati sau nu. La backtracking am adaugat literali noi in `assignments` urmand ordinea din aceasta lista. Ideea din spate a fost ca literalii care apar de cele mai multe ori au cel mai mare impact asupra valorii de adevar a expresiei, deci am inceput cu ei asignarea. Asa se poate ca multe clauze sa fie satisfacute aproape imediat, de la primele apeluri recursive, si sa apara mai repede clauze unitare.

De asemenea, la cautarea de clauze unitare, daca gasesc o clauza falsa inseamna ca atribuirea curenta nu e buna, nu mai apelez recursiv DPLL. Pentru ca verific in `unit_propagation` daca o clauza e falsa, am eliminat aceasta verificare din functia pentru DPLL.

Ordinea in care atribui TRUE sau FALSE unui nou literal adaugat conteaza, tin cont cu ce semn apare mai des in clauze (+ sau -) si in functie de asta atribui intai TRUE sau intai FALSE (asa sunt sanse mai mari sa atribui valoare corecta din prima).
