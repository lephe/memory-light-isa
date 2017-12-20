#!/usr/bin/env python3
""" Some utilities for the lexer/parser """
import collections
import re
import heapq


def inv_dict_list(dictionnary):
    """ Take a dictionnary[string, lists[string]], and return as
    dictionnary[string, string] reversed."""
    inv_d = dict()

    for key1, value in dictionnary.items():
        for key2 in value:
            inv_d[key2] = key1

    return inv_d


class Queue(collections.deque):
    """ Fast Queue based on collections.deque"""

    def push(self, value):
        """ Push value into the stack """
        self.appendleft(value)

    def is_empty(self):
        """ return True if empty """
        return len(self) == 0


class Stack(collections.deque):
    """ Fast Stack based on collections.deque"""

    def push(self, value):
        """ Push value to the stack """
        self.append(value)

    def is_empty(self):
        """ Return True if empty """
        return len(self) == 0


def add_global_enum(enum):
    """ Add all enum's value to globals"""
    for name, member in enum.__members__.items():
        globals()[name] = member


def del_global_enum(enum):
    """ Remove all enum's value globals"""
    for name, _ in enum.__members__.items():
        del globals()[name]


def sub(chaine, dico):
    """ Replace all occurences in a string

    :param s (string): string to be parsed
    :param d (dict[string, string]): mapping dict, all keys will be remplaced
                                     by it's value.
    :return: s with all instances remplaced with thoses of string
    :complexity:
        :math:`O(k n)` for k the length of dict, and n the length of the
                       string"""

    pattern = re.compile('(' + '|'.join(dico.keys()) + ')')

    return pattern.sub(lambda x: dico[x.group()], chaine)


def huffman(ctr):
    """ Generate an huffman tree

    :param c (collections.Counter): Counter with all the elements
    :returns: list of couples. First element is the code, second is the
              element it is bound to.
    :complexity:
        :math:`O(n \\log n)` worst case, for n the number of differents
                             elements.
    """

    # Heap est une foret d'arbres étiquetées par la somme des frequences des
    # feuilles des arbres, où les feuilles des arbres sont étiquetés par les
    # clés de c. Heap est un tas-min pour des raisons de performance.

    forest = []
    if type(ctr) is collections.Counter:
        for key, freq in ctr.most_common():

            # Initialisation : On fabrique une foret de feuilles. On ne garde
            # en mémoire que les feuilles et le chemin (en binaire) pour
            # acceder à la feuille.

            forest.append((freq, [("", key)]))

    else:
        for key, freq in ctr.items():
            forest.append((freq, [("", key)]))


    if len(forest) is 0:
        return []

    if len(forest) is 1:
        # Unique key.
        return ["0", forest[0][0][1]]

    heapq.heapify(forest)

    while len(forest) > 1:

        # Algorithme glouton : on prend les arbres les moins fréquents, et on
        # les mets à coté.

        freq_x, left_tree = heapq.heappop(forest)
        freq_y, right_tree = heapq.heappop(forest)

        freq_z = freq_x + freq_y
        tree = [("0" + pos, key) for pos, key in left_tree] + \
               [("1" + pos, key) for pos, key in right_tree]

        # Et on repose le résultat dans le tas.
        heapq.heappush(forest, (freq_z, tree))

    # len(forest) = 1 ici.
    _, tree = forest[0]

    return tree


if __name__ == '__main__':
    pass
