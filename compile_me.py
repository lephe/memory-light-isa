import bitcodegen

if __name__ == "__main__":
    l = bitcodegen.Lexer()

    statements = '''\
        add r3 r5 r6
        add r1 343
        sub r1 r2 -13 ; Ceci est un commentaire
        shift left r3 -5
        add3 r1 r1 r2      r2    '''

    for token in l.lex(statements):
        print(token)
