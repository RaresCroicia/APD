Croicia Rares 333CC
# Tema 2 APD

## Implementare

### MyDispatcher

- Clasa MyDispatcher implementeaza distribuirea task-urilor catre noduri.
- In Constructor se initializeaza numarul de noduri si numarul de task-uri date catre noduri prin Round Robin.
- Metoda `addTask` apeleaza una din cele 4 metode de adaugare, in functie de algoritmul dorit
- Metoda `addTaskRoundRobin` adauga task-urile in ordine, in functie de numarul de noduri folosind Round Robin
- Metoda `addTaskShortestQueue` adauga task-urile in ordine, in functie de numarul de task-uri din coada fiecarui nod
- Metoda `addTaskSITA` adauga task-urile in functie de tipul taskului
- Metoda `addTaskLWL` adauga task-urile in functie de cantitatea de munca ramasa fiecarui nod

### MyHost

- Clasa MyHost implementeaza un nod care executa task-uri.
- `tasks` este o coada prioritara in care se adauga task-urile, prioritate fiind
    prioritatea task-ului si in caz de egalitate, timpul de start al task-ului
- `runningTask` este task-ul care se executa in momentul de fata, null daca nu ruleaza nimic
- `isRunning` este true daca nodul este in viata, false daca nu (este afectat doar de `shutdown`)
- In timpul rularii nodului, acesta verifica cat timp este in viata daca executa in momentul
    de fata un task si daca nu, daca exista un task in coada prioritara, il executa
- Daca exista deja un task care ruleaza, verifica daca e preemtabil si daca da, il preemteaza
    cu task-ul din coada prioritara cu prioritate mai mare
- In cazul in care nu e nevoie sa se preempteze sau nu e preemptabil, il va rula efectiv
    prin Sleep si va decrementa cantitatea de munca ramasa
- Metoda `addTask` adauga un task in coada prioritara si o initializeaza daca nu e initializata
- Metoda `shutdown` opreste nodul, setand `isRunning` pe false si dand finish la task-urile care nu
au fost deja terminate
- Metoda `getWorkLeft` returneaza cantitatea de munca ramasa taskurilor din coada si celui care ruleaza
- Metoda `getQueueSize` returneaza numarul de task-uri din coada prioritara + 1 daca ruleaza un task
