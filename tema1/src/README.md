Croicia Rares 333CC

# Tema 1 APD

## Ideea implementarii

- Initial am crezut ca trebuie paralelizate mai multe operatii, insa m-am gandit
ca ar fi mai inteligent intai sa fac diferentele de timp pe toate functiile si sa
vad cine e functia care consuma cel mai mult

- Concluzie: Functia `rescale_image` e cea care consuma cel mai mult (singura la care se vedea o diferenta drastica)
asa ca am paralelizat-o doar pe ea

## Detalii implementare

- Ea a trebuit paralelizata doar cand ar fi fost apelata, conditia fiind
daca rezolutia e mai mare decat 2048x2048, asa ca am facut check-ul acesta
si cand trebuia apelata functia, am paralelizat-o pe cate threaduri sunt
cerute la rulare. Am folosit impartirea cat mai egala a taskurilor pe threaduri,
astfel fiecare thread ia 1/P din taskuri, unde P e numarul de threaduri.

## Alte detalii

- Nu prea mai am, fiindca doar asta am facut.

