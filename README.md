# ASM 2017


## Python

by alain


### Utilisation

```python3 myasm.py filename```

Où filename est le fichier d'entrée. La sortie est toujours `a.out`. :^) Il
n'y a pas pour le moment d'une option -o finameout. Le fichier de sortie mets
des espace et des sauts de ligne pour pouvoir relire à la main le fichier.

### Description du code

Le code du prof étant en python2, et n'étant pas "beau", j'ai tout recodé.
Pour l'instant, je n'utilise pas de classes, mais je finirais par le faire
avec des classes. (pour mieux gérer les variables de compilation). Il est
organisé de la façon suivante :

 * Des fonctions *helper* comme binary_repr (qui fait la même chose que celle
de numpy, sauf qu'on a pas besoin d'importer   l'usine à gaz qu'est numpy pour
l'utiliser).

 * Les fonctions de transformation de variables en bitcode suivant les tables
de l'énoncé (qu'il faudrait recopier en `markdown`). Elles sont organisées
toujours de la même façon : expression régulière, puis parsing, puis
vérification des parametres en fonction de ce qui est marqué dans le poly.

 * Le parsing de ligne : on vire les commentaires, et on parse la ligne si
elle n'est pas vide. J'utilise les fonctions de base (`string.split()`) pour
le faire, et pas d'expression régulières. Peut etre que c'est mieux d'utiliser
des expressions régulières ici...

 * Le parsing global d'une grosse chaine de caractères avec des retours de
lignes. Toutes les fonctions ci-dessus peuvent retourner des erreurs de
parsing si un paramètre n'est pas bon. C'est ici qu'on les `except` pour
afficher la ligne où il y a eu l'erreur de syntaxe. C'est **beaucoup** plus
pratique je trouve ! Et c'est fait de façon pythonnique par rapport au code du
prof. (Je défini une nouvelle exception qui correspond à ce que je fais).

 * Une partie qui est plus de l'ordre du gadget, qui est la gestion de l'appel
en ligne de commande.

### Truc à faire

J'ai fait un petit truc juste pour que tu puisses avancer sur le simulateur ce
week-end. Je vais faire un code plus robuste pour la suite, avec des options
en plus.

Déjà, je vais faire en sorte faire une première passe de lexer de l'assembleur
pour tout transformer en tokens. Ensuite, je ferais une deuxième passe qui
sera là pour récuperer les espaces entre chaques labels et jumps. Puis
finalement, une troisième passe qui sera là pour mettre les bonnes longueurs
de jumps.

Je propose l'algorithme suivant pour les jumps : Je commence par considèrer
que toutes les addresses sont en 64 bits. Puis, pour chaque jump qui va plus
loin dans le code, je regarde si je peux réduire la longueur. Ensuite, je fais
la meme chose pour les jumps qui remontent, en partant cette fois de la fin.


L'algo ne donne sûrmement pas de résultat optimal, mais il a le don de finir,
et de ne pas etre exponentiel, mais juste polynomial.



## Autres considérations

Il n'y a pas de constantes signées, vu qu'on peut utiliser sub.... A priori.
De toute façon, elles ne sont pas définies dans pdf, mais il y a une fonction
dans le code python du prof. Bizarre. A voir avec le prof.


