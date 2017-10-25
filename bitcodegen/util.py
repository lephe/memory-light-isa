import collections

class Queue(collections.deque):
    """ Fast Queue based on collections.deque"""
    def push(self, x):
        self.append_left()

    def is_empty(self):
        return len(self) == 0

class Stack(collections.deque):
    """ Fast Stack based on collections.deque"""
    def push(self, x):
        self.append(x)
    def is_empty(self):
        return len(self) == 0


def huffman(c):

    """take a collections.Counter, and generate an huffman-tree. O(n) = n log(n)
    worst case."""

    # Heap est une foret d'arbres étiquetées par la somme des frequences des
    # feuilles des arbres, où les feuilles des arbres sont étiquetés par les
    # clés de c. Heap est un tas-min pour des raisons de performance.

    heap = []
    for key, freq in c.most_common():

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
    tot, tree = heap[0]

    return tree


