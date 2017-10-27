
import collections
import re
import heapq


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
    :param d (dict[string, string]): mapping dict, all keys will be remplaced by it's value.
    :return: s with all instances remplaced with thoses of string
    :complexity:
        :math:`O(k n)` for k the length of dict, and n the length of the string"""

    pattern = re.compile('(' + '|'.join(dico.keys()) + ')')

    return pattern.sub(lambda x: dico[x.group()], chaine)

def huffman(ctr):
    """ Generate an huffman tree

    :param c (collections.Counter): Counter with all the elements
    :returns: list of couples. First element is the code, second is the element it is bound to.
    :complexity:
        :math:`O(n \\log n)` worst case, for n the number of differents elements.
    """

    # Heap est une foret d'arbres étiquetées par la somme des frequences des
    # feuilles des arbres, où les feuilles des arbres sont étiquetés par les
    # clés de c. Heap est un tas-min pour des raisons de performance.

    heap = []
    for key, freq in ctr.most_common():

        # Initialisation : On fabrique une foret de feuilles. On ne garde en
        # mémoire que les feuilles et le chemin (en binaire) pour acceder à la
        # feuille.

        heap.append((freq, [("", key)]))

    if len(heap) == 0:
        return []

    if len(heap) == 1:
        # Unique key.
        return ["0", heap[0][0][1]]

    heapq.heapify(heap)

    while len(heap) > 1:

        # Algorithme glouton : on prend les arbres les moins fréquents, et on
        # les mets à coté.

        freq_x, x = heapq.heappop(heap)
        freq_y, y = heapq.heappop(heap)

        freq_z = freq_x + freq_y
        z = [("0" + pos, key) for pos, key in x] + [("1" + pos, key) for pos, key in y]

        # Et on repose le résultat dans le tas.
        heapq.heappush(heap, (freq_z, z))

    # len(heap) = 1 ici.
    _, tree = heap[0]

    return tree

if __name__ == '__main__':
    pass
