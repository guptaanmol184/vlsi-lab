#!/usr/bin/env python3
# pylint disable=CODE


''' Convert Boolean to Netlist with CMOS logic

!(!(A+B).C+!D)
means
NOT (NOT(A OR B) AND C + NOT D)
'''
print(
    '''
Convert Boolean to Netlist with CMOS logic

!(!(A+B).C+!D)
means
NOT (NOT(A OR B) AND C + NOT D)
''')

SYMLIST = ['(',
           ')',
           '+',
           '.',
           '!',
           ' '
          ]

PRECDICT = {'!':3,
            '.':2,
            '+':1,
            '(':0,
            ')':0
           }

print("Enter Boolean Expression")
expr = input().strip()

tokens = []
substr_start = 0

for char in range(len(expr)):
# for char,expr in enumerate(expr):
    if expr[char] in SYMLIST:

        if substr_start < char:
            tokens.append(expr[substr_start:char])

        if expr[char] != ' ':
            tokens.append(expr[char])

        substr_start = char+1

if substr_start < len(expr):
    tokens.append(expr[substr_start:len(expr)])

stack = []
postfixList = []

for token in tokens:

    if token in PRECDICT.keys():

        if token == '(':
            stack.append(token)

        elif token == ')':
            while (stack[-1] != '('):
                postfixList.append(stack.pop())

            stack.pop()

        else:
            while (len(stack)!=0) and (stack[-1] != '(') and (PRECDICT[stack[-1]] >= PRECDICT[token]):
                postfixList.append(stack.pop())

            stack.append(token)

    else:
        postfixList.append(token)

while len(stack) !=0 :
    postfixList.append(stack.pop())


postfixList_pmos = postfixList.copy()

postfixList_nmos = postfixList.copy()
postfixList_nmos.append('!')


def invert(postfixList, invert_index):

    if postfixList[invert_index] not in PRECDICT.keys():

        if postfixList[invert_index][0] == '!':
            postfixList[invert_index] = postfixList[invert_index][1:]

        else:
            postfixList[invert_index] = '!'+ postfixList[invert_index]

        return invert_index;

    else:

        if postfixList[invert_index] == '.':
            postfixList[invert_index] = '+'

        elif postfixList[invert_index] == '+':
            postfixList[invert_index] = '.';

        chain_end = invert(postfixList, invert_index-1)
        return invert(postfixList, chain_end - 1)



token_index = 0

while token_index < len(postfixList_pmos):

    if postfixList_pmos[token_index] == '!':
        invert(postfixList_pmos, token_index - 1)
        postfixList_pmos.pop(token_index)

    else:
        token_index += 1


token_index = 0

while token_index < len(postfixList_nmos):

    if postfixList_nmos[token_index] == '!':
        invert(postfixList_nmos, token_index - 1)
        postfixList_nmos.pop(token_index)

    else:
        token_index += 1


nmos = []
nmos_index = 0
nmos_jn_index = 0 

for token in postfixList_nmos:

    if token not in ['.','+']:

        nmos.append([{ "name":("NMOS"+str(nmos_index)), "drain":nmos_jn_index,"src":nmos_jn_index+1, "gate":token}])
        nmos_jn_index += 2
        nmos_index += 1

    elif token == '.':

        old_drain = nmos[-1][-1]["drain"]
        new_drain = nmos[-2][0]["src"]

        for transistor in nmos[-1]:
            if transistor["drain"] == old_drain:
                transistor["drain"] = new_drain

        nmos[-1].extend(nmos.pop(-2))
        
    elif token == '+':
        
        old_src = nmos[-1][0]["src"]
        new_src = nmos[-2][0]["src"]

        old_drain = nmos[-1][-1]["drain"]
        new_drain = nmos[-2][-1]["drain"]

        for transistor in nmos[-1]:

            if transistor["src"] == old_src:
                transistor["src"] = new_src

            if transistor["drain"] == old_drain:
                transistor["drain"] = new_drain

        nmos[-1].extend(nmos.pop(-2))


ground = nmos[0][-1]["drain"]
out_nmos = nmos[0][0]["src"]

for transistor in nmos[0]:

    if transistor["src"] == out_nmos:
        transistor["src"] = "OUTPUT"

    if transistor["drain"] == ground:
        transistor["drain"] = "GND"

def invert(token):

    if token[0] == '!':
        return token[1:]

    else:
        return "!"+token


pmos = []
pmos_index = 0
pmos_jn_index = nmos_jn_index 

for token in postfixList_pmos:

    if token not in ['.','+']:

        pmos.append([{ "name":("PMOS"+str(pmos_index)), "src":pmos_jn_index,"drain":pmos_jn_index+1, "gate":invert(token)}])
        pmos_jn_index += 2
        pmos_index += 1

    elif token == '.':

        old_src = pmos[-1][0]["src"]
        new_src = pmos[-2][-1]["drain"]

        for transistor in pmos[-1]:
            if transistor["src"] == old_src:
                transistor["src"] = new_src

        pmos[-2].extend(pmos.pop(-1))
        
    elif token == '+':
        
        old_src = pmos[-1][0]["src"]
        new_src = pmos[-2][0]["src"]

        old_drain = pmos[-1][-1]["drain"]
        new_drain = pmos[-2][-1]["drain"]

        for transistor in pmos[-1]:

            if transistor["src"] == old_src:
                transistor["src"] = new_src

            if transistor["drain"] == old_drain:
                transistor["drain"] = new_drain

        pmos[-2].extend(pmos.pop(-1))

vdd = pmos[0][0]["src"]
out_pmos = pmos[0][-1]["drain"]

for transistor in pmos[0]:

    if transistor["src"] == vdd:
        transistor["src"] = "VSS"

    if transistor["drain"] == out_pmos:
        transistor["drain"] = "OUTPUT"

print("Transistor [Src, Gate, Drain]")

print("pmos Network:")
for transistor in pmos[0]:
    print(transistor["src"], transistor["gate"], transistor["drain"], sep='\t')

print("\nnmos Network:")
for transistor in nmos[0]:
    print(transistor["src"], transistor["gate"], transistor["drain"], sep='\t')
