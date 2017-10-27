import collections
import re

from .enums import Token, LexType, inverse_possible_transition
from .errors import TokenError, ParserError
from .util import Queue, Stack, huffman


class Parser(object):
	def __init__(self, lexer_gen):
		self.lexer_gen = lexer_gen
		self.stack = Stack()

	def run(self):
		pass