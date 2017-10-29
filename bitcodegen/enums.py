import enum
import collections

Token = collections.namedtuple('Token', ['typ', 'value', 'line', 'column'])
Value = collections.namedtuple("Value", ("typ", "raw_value"))
Line = collections.namedtuple("Line", ("funcname", "typed_args", "linenumber"))


class LexType(enum.Enum):
    """ Type enum for lexer's token """
    MEMCOUNTER = enum.auto()
    OPERATION = enum.auto()
    DIRECTION = enum.auto()
    CONDITION = enum.auto()
    REGISTER = enum.auto()
    COMMENT = enum.auto()
    NEWLINE = enum.auto()
    ENDFILE = enum.auto()
    NUMBER = enum.auto()
    LABEL = enum.auto()
    SKIP = enum.auto()
    MISMATCH = enum.auto()


class ValueType(enum.Enum):
    MEMCOUNTER = enum.auto()
    DIRECTION = enum.auto()
    CONDITION = enum.auto()
    UCONSTANT = enum.auto()  # Unsigned Constant
    SCONSTANT = enum.auto()  # Signed Constant
    RADDRESS = enum.auto()  # Relative address
    AADDRESS = enum.auto()  # Absolute address
    SHIFTVAL = enum.auto()
    SIZE = enum.auto()
    REGISTER = enum.auto()
