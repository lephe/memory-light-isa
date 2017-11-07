from .util import Queue
from .errors import BackEndError
from .enums import NB_BIT_REG, ValueType


class BackEnd(object):
    write_mode = "w+"

    def __init__(self, huffman_tree, line_gene):
        self.line_gene = line_gene
        self.out_queue = Queue()
        self.huffman_tree = huffman_tree

    def to_file(self, filename):
        with open(filename, self.write_mode) as f:
            for packet in self.packets():
                if "b" not in self.write_mode:
                    f.write(packet + "\n")
                else:
                    f.write(packet)

    def to_output(self):
        for packet in self.packets():
            print(packet)

    def packets(self):
        for line in self.line_gene:
            self.handle_line(line)

            while not self.out_queue.is_empty():
                yield self.out_queue.pop()

        res = self.post_packets()

        if res is not None:
            yield res

    def post_packets(self):
        pass

    def handle_line(self, line):
        raise NotImplementedError()


class MemonicBackEnd(BackEnd):
    write_mode = "w+"

    def handle_line(self, line):
        funcname, typed_args, linenumber = line

        funcname = funcname + " "*(7-len(funcname))

        realise_line = [funcname] + [("r" if x.typ is ValueType.REGISTER
                                      else "") + str(x.raw_value)
                                     for x in typed_args]

        self.out_queue.push(" ".join(realise_line))


class CleartextBitcodeBackEnd(BackEnd):
    write_mode = "w+"

    ctr = {
        "pc": "00",
        "sp": "01",
        "a0": "10",
        "a1": "11"}

    direction = {
        "left": "0",
        "right": "1"}

    conditions = {
        "eq": "000",
        "neq": "001",
        "sgt": "010",
        "slt": "011",
        "gt": "100",
        "ge": "101",
        "lt": "110",
        "le": "111"}

    def handle_line(self, line, space=None):
        if space is None:
            if b in self.write_mode:
                space = False
            else:
                space = True

        funcname, typed_args, linenumber = line

        realise_line = [self.huffman_tree[funcname]]

        for typ, raw_value in typed_args:
            method_name = f"bin_{typ.name.lower()}"
            if hasattr(self, method_name):
                method = getattr(self, method_name)
            else:
                raise BackEndError("Method not found")

            realise_line.append(method(raw_value))

        self.out_queue.push((" " if space else "").join(realise_line))

    def binary_repr(self, n, k, signed=False):
        """Given n an int, it return it's binary representation on k bits"""

        if signed:
            if n not in range(-2**(k-1), 2**(k-1)):
                raise BackEndError("Number not in range")

            n = (2**k + n) % 2**k

        unfilled = bin(n)[2:]

        if len(unfilled) > k:
            raise BackEndError("Too long binary")

        return "0" * (k-len(unfilled)) + unfilled

    def bin_register(self, val):
        return self.binary_repr(val, NB_BIT_REG)

    def bin_uconstant(self, val):
        if val in range(2**1):
            # Range in NOT a list in python3
            return "0" + self.binary_repr(val, 1)
        elif val in range(2**8):
            return "10" + self.binary_repr(val, 8)
        elif val in range(2**32):
            return "110" + self.binary_repr(val, 32)
        elif val in range(2**64):
            return "111" + self.binary_repr(val, 64)
        else:
            raise BackEndError("invalid constant : Not in range")

    def bin_sconstant(self, val):

        if val in range(2**1):
            # Range in NOT a list in python3
            return "0" + self.binary_repr(val, 1)
        elif val in range(-2**7, 2**7):
            return "10" + self.binary_repr(val, 8, signed=True)
        elif val in range(-2**31, 2**32):
            return "110" + self.binary_repr(val, 32, signed=True)
        elif val in range(-2**63, 2**63):
            return "111" + self.binary_repr(val, 64, signed=True)
        else:
            raise BackEndError("invalid constant : Not in range")

    def bin_direction(self, val):
        return self.direction[val]

    def bin_shiftval(self, val):
        if val == 1:
            return self.binary_repr(val, 1)
        elif val in range(2**6):
            return "0" + self.binary_repr(val, 6)
        else:
            raise BackEndError("invalid shiftval : Not in range")

    def bin_memcounter(self, val):
        return self.ctr[val]

    def bin_size(self, val):
        if val == 1:
            return "00"
        elif val == 4:
            return "01"
        elif val == 8:
            return "100"
        elif val == 16:
            return "101"
        elif val == 32:
            return "110"
        elif val == 64:
            return "111"
        else:
            raise BackEndError("invalid size : not in range")

    def bin_raddress(self, val):
        if val in range(-2**7, 2**7):
            return "0" + self.binary_repr(val, 8, signed=True)
        elif val in range(-2**15, 2**15):
            return "10" + self.binary_repr(val, 16, signed=True)
        elif val in range(-2**31, 2**31):
            return "110" + self.binary_repr(val, 32, signed=True)
        elif val in range(-2**63, 2**63):
            return "111" + self.binary_repr(val, 64, signed=True)
        else:
            raise BackEndError("invalid unsigned address : not in range")

    def bin_aaddress(self, val):
        if val in range(2**8):
            return "0" + self.binary_repr(val, 8)
        elif val in range(2**16):
            return "10" + self.binary_repr(val, 16)
        elif val in range(2**32):
            return "110" + self.binary_repr(val, 32)
        elif val in range(2**64):
            return "111" + self.binary_repr(val, 64)
        else:
            raise BackEndError("invalid unsigned address : not in range")

    def bin_condition(self, val):
        return self.conditions[val]

    def bin_label(self, val):
        raise BackEndError("Label unsported in this mode (faut pas déconner)")


class BinaryBitcodeBackEnd(CleartextBitcodeBackEnd):
    write_mode = "wb+"

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.binary = ""

    def handle_line(self, line):
        funcname, typed_args, linenumber = line

        super().handle_line(line)

        while not self.out_queue.is_empty():
            self.binary += "".join(self.out_queue.pop().split())

        q, r = divmod(len(self.binary), 8)

        bitline = int(self.binary[:q*8], 2).to_bytes(q, byteorder='big')
        self.binary = self.binary[q*8:]

        self.out_queue.push(bitline)

    def post_packet(self):
        if len(self.binary) is not 0:
            self.binary += "0" * ((8-len(self.binary)))//8

        bitline = int(self.binary, 2).to_bytes(1, byteorder='big')
        self.binary = ""

        return bitline
