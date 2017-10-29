import collections

from .util import Stack, Queue, inv_dict_list
from .enums import LexType, ValueType, Value, Line
from .errors import ParserError

NB_REG = 8


class Parser(object):
    def __init__(self, lexer_gen, possible_transitions,
                 asr_specs, types_specs):

        self.lexer_gen = lexer_gen
        self.stack = Stack()
        self.out_stack = Stack()

        rev_types_specs = inv_dict_list(types_specs)

        self.functions = dict()

        for funcname, list_asr_funcname in possible_transitions.items():
            self.functions[funcname] = dict()
            for asr_funcname in list_asr_funcname:

                asr_args = asr_specs[asr_funcname]
                preasr_args = tuple(
                    map(lambda x: rev_types_specs[x], asr_args))

                self.functions[funcname][preasr_args] = (asr_funcname,
                                                         asr_args)

    def run(self):
        for token in self.lexer_gen:
            if token.typ is LexType.COMMENT:
                pass  # Ignore comments

            elif token.typ is LexType.NEWLINE:
                try:
                    self.handle_one()
                except ParserError as e:
                    print(f"line {token}")
                    print(e.message)
                    raise

                while not self.out_stack.is_empty():
                    yield self.out_stack.pop()

            else:
                self.stack.push(token)

    def unstack_until_operation(self):
        res = Queue()

        while not self.stack.is_empty():
            if self.stack[-1].typ is not LexType.OPERATION:

                if self.stack[-1].typ is LexType.NEWLINE:
                    if len(res) is 0:
                        return ()  # Empty tuple
                    else:
                        raise ParserError("No operand found in not empty line")

                res.push(self.stack.pop())
            else:
                break
        else:
            if len(res) is not 0:
                raise ParserError("The stack is empty : couldn't find operand")
            else:
                return ()

        res.push(self.stack.pop())
        return tuple(res)

    def handle_one(self):
        res = self.unstack_until_operation()

        if len(res) is 0:
            return None

        fun_name = res[0].value
        args_types = tuple(map(lambda x: x.typ, res[1:]))

        args_values = tuple(map(lambda x: x.value, res[1:]))

        if fun_name not in self.functions.keys():
            raise ParserError(f"Not found operation : {fun_name}")

        if args_types not in self.functions[fun_name].keys():
            raise ParserError(
                f"Arguments types don't match function : {fun_name}")

        funcname, goal_args_type = self.functions[fun_name][args_types]

        typed_args = []

        for value, goal_type in zip(args_values, goal_args_type):
            method_name = f"read_{goal_type.name.lower()}"
            if not hasattr(self, method_name):
                raise ParserError(f"Reader not found for {goal_type.name}")

            method = getattr(self, method_name)

            typed_args.append(Value(goal_type, method(value)))

        typed_args = tuple(typed_args)
        self.out_stack.push(Line(funcname, typed_args, res[0].line))

    def read_memcounter(self, value):
        return value

    def read_direction(self, value):
        return value

    def read_condition(self, value):
        return value

    def read_uconstant(self, value):
        assert value in range(0, 2**63)
        return value

    def read_sconstant(self, value):
        assert value in range(-2**63, 2**63)
        return value

    def read_aaddress(self, value):
        raise NotImplementedError()

    def read_raddress(self, value):
        assert value in range(-2**63, 2**63)
        return value

    def read_shiftval(self, value):
        assert value in range(2**6)
        return value

    def read_size(self, value):
        assert value in [1, 4, 8, 32, 64]
        return value

    def read_register(self, value):
        assert value in range(NB_REG)
        return value
