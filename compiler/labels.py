from .errors import BackEndError, ImpossibleError
from .back_end import CleartextBitcodeBackEnd, BinaryBitcodeBackEnd
from .enums import Line


class LabelsClearTextBackEnd(CleartextBitcodeBackEnd):
    bit_cost = {8: 9, 16: 18, 32: 35, 64: 67}
    bit_prefix = {8: "0", 16: "10", 32: "110", 64: "111"}

    def get_fullcode(self):
        #  List containning (len, bitcode/jump/label)
        fullcode = [(0, "")]
        # Not empty for special case with a call to a
        # label in 0

        acc = ""

        for line in self.line_gene:
            if line.funcname not in ("jumpl", "jumpifl", "calll", "label"):
                CleartextBitcodeBackEnd.handle_line(self, line, space=False)

                while not self.out_queue.is_empty():
                    acc += self.out_queue.pop()

            else:
                fullcode.append((len(acc), acc))

                if line.funcname is "label":
                    bitcode = ""
                else:
                    bitcode = self.huffman_tree[line.funcname[:-1]]

                if line.funcname is "jumpl":
                    fullcode.append((len(bitcode), line))
                elif line.funcname is "jumpifl":
                    fullcode.append((len(bitcode) + 3, line))
                elif line.funcname is "calll":
                    fullcode.append((len(bitcode), line))
                elif line.funcname is "label":
                    fullcode.append((0, line))

                acc = ""

        fullcode.append((len(acc), acc))

        return fullcode

    def get_label_pos(self, fullcode):

        # Get position for all the labels
        label_dict = dict()
        for i, (l, x) in enumerate(fullcode):
            if type(x) is Line:
                if x.funcname is "label":
                    label = x.typed_args[0].raw_value
                    label_dict[label] = i

        return label_dict

    def count_bytes(self, fullcode, addr_values, i, j):
        # Forward jump Can be call too
        if j < i:
            s = 0
            for k in range(j+1, i):
                s += fullcode[k][0]
                if k in addr_values:
                    nb_bit = addr_values[k][0]
                    s += self.bit_cost[nb_bit]

            return s

        # Backward jump
        elif j > i:
            s = 0
            for k in range(i, j+1):
                s += fullcode[k][0]
                if k in addr_values:
                    nb_bit = addr_values[k][0]
                    s += self.bit_cost[nb_bit]
            return -s

    def packets(self):

        from pprint import pprint

        fullcode = self.get_fullcode()

        # Get position for all the labels
        label_dict = self.get_label_pos(fullcode)

        addr_values = dict()

        # addr init
        for j, (l, x) in enumerate(fullcode):
            if type(x) is Line and x.funcname in ("jumpl", "jumpifl", "calll"):
                addr_values[j] = (8, 0)

        while True:

            for j, (l, x) in enumerate(fullcode):
                if type(x) is Line and x.funcname in ("jumpl", "jumpifl"):
                    if x.funcname is "jumpl":
                        label = x.typed_args[0].raw_value
                    elif x.funcname is "jumpifl":
                        label = x.typed_args[1].raw_value

                    if label not in label_dict:
                        raise BackEndError("Undefined label")

                    i = label_dict[label]

                    nb_bit, old_s = addr_values[j]

                    s = self.count_bytes(fullcode, addr_values, i, j)

                    if s not in range(-2**(nb_bit-1), 2**(nb_bit-1)):
                        if nb_bit == 64:
                            raise BackEndError("Jump too long")
                        addr_values[j] = (nb_bit * 2, s)
                        break
                    else:
                        addr_values[j] = (nb_bit, s)

                if type(x) is Line and x.funcname is "calll":
                    label = x.typed_args[0].raw_value

                    if label not in label_dict:
                        raise BackEndError("Undefined label")

                    i = label_dict[label_dict]

                    nb_bit, old_s = addr_values[j]

                    s = self.count_bytes(fullcode, addr_values, i, 0)

                    if s not in range(-2**(nb_bit-1), 2**(nb_bit-1)):
                        if nb_bit == 64:
                            raise BackEndError("Address too big")
                        addr_values[j] = (nb_bit*2, s)
                        break
                    else:
                        addr_values[j] = (nb_bit, s)

            else:
                break

        pprint(addr_values)
        pprint(list(map(lambda x: x[0], fullcode)))

        endcode = []
        for i, (l, x) in enumerate(fullcode):
            if type(x) is str:
                endcode.append(x)
            elif type(x) is Line and x.funcname is "label":
                pass

            # x.funcname is "jumpl" or "jumpifl" or "call"
            elif type(x) is Line:
                bitcode = self.huffman_tree[x.funcname[:-1]]

                endcode.append(bitcode)
                if x.funcname is "jumpifl":
                    cond = x.typed_args[0].raw_value
                    endcode.append(self.bin_condition(cond))
                k, n = addr_values[i]
                endcode.append(self.bit_prefix[k] + self.binary_repr(n, k, signed=True))
                # print(endcode)

        return endcode


class LabelsBinaryBackEnd(LabelsClearTextBackEnd):
    write_mode = "wb"

    def to_file(self, filename):
        bitcode = "".join(self.packets())
        bitcode = bitcode + "0"*((8-len(bitcode)) % 8)
        q = len(bitcode)//8

        with open(filename, self.write_mode) as f:
            f.write(q.to_bytes(8, byteorder="big"))
            for k in range(q):
                lul = "0b" + bitcode[8*k:8*(k+1)]
                # print(bytes([int(lul, 2)]))
                f.write(bytes([int(lul, 2)]))
