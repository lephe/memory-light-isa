from .util import Queue


class BackEnd(object):
    write_mode = "w+"

    def __init__(self, line_gene):
        self.line_gene = line_gene
        self.out_queue = Queue()

    def to_file(self, filename):
        with open(filename, self.write_mode) as f:
            for packet in self.packets():
                f.write(packet)

    def to_output(self):
        for packet in self.packets():
            print(packet)

    def packets(self):
        for line in self.line_gene:
            self.handle_line(line)

            while not self.out_queue.is_empty():
                yield self.out_queue.pop()

    def handle_line(self, line):
        raise NotImplementedError()


class MemonicBackEnd(BackEnd):
    write_mode = "w+"

    def handle_line(self, line):
        funcname, typed_args, linenumber = line

        funcname = funcname + " "*(7-len(funcname))

        realise_line = [funcname] + [str(x.raw_value) for x in typed_args]

        self.out_queue.push(" ".join(realise_line))


class BitcodeBackEnd(object):
    pass
