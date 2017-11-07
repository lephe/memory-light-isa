from .errors import BackEndError, ImpossibleError
from .back_end import CleartextBitcodeBackEnd, BinaryBitcodeBackEnd
from .enums import Line


class LabelsClearTextBackEnd(CleartextBitcodeBackEnd):

    def packets(self):
        it = iter(self.line_gene)

        #  List containning (len, bitcode/jump/label)
        fullcode = []

        acc = ""

        for line in it:
            print(line)
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
                    fullcode.append((len(bitcode) + 9, line))
                elif line.funcname is "jumpifl":
                    fullcode.append((len(bitcode) + 9 + 3, line))
                elif line.funcname is "calll":
                    raise NotImplementedError()
                elif line.funcname is "label":
                    fullcode.append((0, line))

                acc = ""

        label_dict = dict()

        for i, (l, x) in enumerate(fullcode):
            if type(x) is Line:
                if x.funcname is "label":
                    label = x.typed_args[0].raw_value
                    label_dict[label] = i

        addr_values = dict()

        for j, (l, x) in enumerate(fullcode):
            if type(x) is Line:
                if x.funcname is "jumpl":
                    label = x.typed_args[0].raw_value
                elif x.funcname is "jumpifl":
                    label = x.typed_args[1].raw_value

                if x.funcname in ("jumpl", "jumpifl"):
                    if label not in label_dict:
                        raise BackEndError("Undefined label")

                    i = label_dict[label]

                    if j < i:
                        # Forward jump

                        s = 0
                        for k in range(j+1, i):
                            s += fullcode[k][0]

                        if s in range(-2**7, 2**7):
                            addr_values[j] = (8, s)
                            continue

                    elif j > i:
                        # Backward jump
                        s = 0
                        for k in range(i, j+1):
                            s += fullcode[k][0]
                        if s in range(-2**7, 2**7):
                            addr_values[j] = (8, s)
                            continue

                    else:
                        raise ImpossibleError("jump can't be a label")
        else:
            endcode = []
            for i, (l, x) in enumerate(fullcode):
                if type(x) is str:
                    endcode.append(x)
                elif type(x) is Line and x.funcname is "label":
                    pass

                elif type(x) is Line and x.funcname in ("jumpifl", "jumpl"):
                    bitcode = self.huffman_tree[x.funcname[:-1]]
                    print(bitcode)
                    endcode.append(bitcode)
                    if x.funcname is "jumpifl":
                        cond = x.typed_args[0].raw_value
                        endcode.append(self.bin_condition(cond))
                    k, n = addr_values[i]
                    endcode.append(self.binary_repr(n, k, signed=True))

            return endcode


class LabelsBinaryBitcodeBackEnd(LabelsClearTextBackEnd, BinaryBitcodeBackEnd):
    pass
