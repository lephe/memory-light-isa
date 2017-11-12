#!/usr/bin/env python3
class TokenError(RuntimeError):
    pass


class ParserError(RuntimeError):
    pass


class BackEndError(RuntimeError):
    pass


class ImpossibleError(RuntimeError):
    pass
