# SAT Solver

Fisiere:
- **main.py**: citire input, initializare SAT Solver, apel DPLL, scriere output in fisier
- **SATSolver.py**: clasa `SATSolver`
- **Makefile**: regula de `run`

## Clasa SATSolver

### Atribute:
- `__input` - formula citita
- `__T` - lista ce retine valoarea de adevar a literalilor sau daca nu au primit inca o valoare
- `__sorted_literals` - lista cu toti literalii sortati descrescator dupa numarul de aparitii in toate clauzele

### Metode:

#### `__init__(self, num_literals, input)`
Constructorul clasei. Initializeaza numarul de literali si formula initiala.
- `num_literals` - numarul de literali
- `input` - formula sub forma unei liste de clauze

#### `get_literal_value(self, literal)`
Getter pentru valoarea de adevar a unui literal. Necesar pentru ca `T` este atribut privat.
- `literal` - literalul a carui valoare de adevar va fi returnata

#### `sort_literals_by_frequency(self)`
Determina si returneaza lista de literali sortati descrescator dupa frecventa aparitiei in clauze.

#### `clause_is_true(self, clause)`
Verifica daca o clauza este satisfacuta de asignarea curenta. Returneaza `True` daca clauza este satisfacuta si `False` in caz contrar.
- `clause` - clauza de verificat

#### `all_clauses_are_satisfied(self)`
Verifica daca toate clauzele sunt satisfacute de asignarea curenta. Returneaza `True` in caz afirmativ, `False` altfel.
- `clause` - clauza de verificat

#### `has_falsified_clause(self)`
Verifica daca exista vreo clauza falsificata in formula de asignarea curenta. Returneaza `True` daca exista o clauza falsificata si `False` in caz contrar.

#### `unit_propagation(self)`
Realizeaza propagarea unitara. Cauta clauze cu toti termenii falsi, mai putin unul, care este `UNASSIGNED`. Returneaza lista de literali adaugati in timpul propagarii unitare.

#### `dpll(self)`
Implementeaza algoritmul DPLL. Returneaza `True` daca formula este satisfiabila si `False` daca nu este.

## Cum am ajuns la algoritmul curent, pornind de la pseudocod

Initial `T` era o simpla lista la care dadeam `.append` literalilor ce aveau valoarea `True`, dar am observat ca picau teste pt ca `has_falsified_clause` returna `True` si cand aveam doar literali neasignati intr-o clauza. De aceea am cautat o solutie care nu retinea doar valoarea de adevar a literalilor, ci si daca acestia au fost asignati sau nu.

In plus, am gasit inca o modalitate de optimizare a codului. Inainte sa o aplic, cel mai "lung" test ca durata rula in peste 5 secunde, acum toate testele ruleaza in sub o secunda. Mi-am creat o lista cu literalii ordonati dupa frecventa, dupa numarul de aparitii in clauze, fie ca apareau negati sau nu. La backtracking am adaugat literali noi in `T` urmand ordinea din aceasta lista. Ideea din spate a fost ca literalii care apar de cele mai multe ori au cel mai mare impact asupra valorii de adevar a expresiei, deci am inceput cu ei asignarea. Asa se poate ca multe clauze sa fie satisfacute aproape imediat, de la primele apeluri recursive, si sa apara mai repede clauze unitare.

**UPDATE:** Am adaugat si pure literal elimination, dar nu pare sa imbunatateasca prea mult timpul, depinde de test.
**UPDATE NR.2:** In unele cazuri list comprehension poate fi mai rapid decat un `for` normal, asa ca am inlocuit cateva astfel de bucle.
**UPDATE NR.3:** La cautarea de clauze unitare, daca gasesc o clauza falsa inseamna ca atribuirea curenta nu e buna, nu mai apelez recursiv DPLL. Pentru ca verific in `unit_propagation` daca o clauza e falsa, am eliminat aceasta verificare din functia pentru DPLL.