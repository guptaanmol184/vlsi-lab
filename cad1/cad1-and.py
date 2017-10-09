#!/usr/bin/env python3
# Stack for postfix conversion and 
# postfix evaluation
class Stack:
     def __init__(self):
         self.items = []

     def isEmpty(self):
         return self.items == []

     def push(self, item):
         self.items.append(item)

     def pop(self):
         return self.items.pop()

     def peek(self):
         return self.items[len(self.items)-1]

     def size(self):
         return len(self.items)

# Infix to postfix conversion
def infixToPostfix(infixexpr):
    prec = {}
    prec["!"] = 4
    prec["."] = 3
    prec["+"] = 2
    prec["("] = 1
    opStack = Stack()
    postfixList = []
    tokenList = list(infixexpr)

    for token in tokenList:
        if token in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" :
            postfixList.append(token)
        elif token == '(':
            opStack.push(token)
        elif token == ')':
            topToken = opStack.pop()
            while topToken != '(':
                postfixList.append(topToken)
                topToken = opStack.pop()
        else:
            while (not opStack.isEmpty()) and (prec[opStack.peek()] >= prec[token]):
                postfixList.append(opStack.pop())
            opStack.push(token)

    while not opStack.isEmpty():
        postfixList.append(opStack.pop())
    return "".join(postfixList)

# postfix evaluation to netlist generation
def postfixEval(postfixExpr):
    # two stacks one for nmos and one for pmos
    nmosStack = Stack()
    pmosStack = Stack()
    tokenList = list(postfixExpr)

    for token in tokenList:
        if token in "ABCDEFGHIJKLMNRSTUVWXYZabcdefghijklmnrstuvwxyz":
            nmosStack.push(token)
            pmosStack.push(token)
        else:
            # for nmos
            result = nmosDoOp(token,nmosStack)
            nmosStack.push(result)
            # for pmos
            result = pmosDoOp(token,pmosStack)
            pmosStack.push(result)

    # print output
    global out_ctr
    print("OUTPUT O" + str(out_ctr) )

# global counter variables
out_ctr=0
p_ctr=0
q_ctr=0

# nmos Operations
def nmosDoOp(op, nmosStack):
    # use global variables
    global out_ctr
    global p_ctr
    global q_ctr

    # define output pin
    out_ctr=out_ctr+1
    output = "O" + str(out_ctr)
    
    # perform output
    if op == ".":
        operand2 = nmosStack.pop()
        operand1 = nmosStack.pop()

        # NMOS(S,G,D)
        # Series
        p_ctr=p_ctr+1
        q_ctr=q_ctr+1
        print("NMOS ( GND, " + str(operand2) + ", P" + str(p_ctr) + " )" )
        print("NMOS ( P" + str(p_ctr) + ", " + str(operand1) + ", Q" + str(q_ctr) + " )" )

        # Invert output
        print("NMOS ( GND, Q" + str(q_ctr) + ", " + str(output) + " )" )
    elif op == "+":
        operand2 = nmosStack.pop()
        operand1 = nmosStack.pop()

        # NMOS(S,G,D)
        # Parallel
        p_ctr=p_ctr+1
        q_ctr=q_ctr+1
        print("NMOS ( GND, " + str(operand2) + ", Q" + str(q_ctr) + " )" )
        print("NMOS ( GND, " + str(operand1) + ", Q" + str(q_ctr) + " )" )

        # Invert output
        print("NMOS ( GND, Q" + str(q_ctr) + ", " + str(output) + " )" )
    else: # not case
        operand1 = nmosStack.pop()

        # Invert output
        print("NMOS ( GND, " + str(operand1) + ", " + str(output) + " )" )
    return output

# pmos Operations 
def pmosDoOp(op, pmosStack):
    # use global variables
    global out_ctr
    global p_ctr
    global q_ctr

    # define output pin
    # Already incremented in pmos
    output = "O" + str(out_ctr)
    
    # perform output
    if op == ".":
        operand2 = pmosStack.pop()
        operand1 = pmosStack.pop()

        # PMOS(S,G,D)
        # Parallel
        print("PMOS ( VDD, " + str(operand2) + ", Q" + str(q_ctr) + " )" )
        print("PMOS ( VDD, " + str(operand1) + ", Q" + str(q_ctr) + " )" )

        # Invert output
        print("PMOS ( VDD, Q" + str(q_ctr) + ", " + str(output) + " )" )
    elif op == "+":
        operand2 = pmosStack.pop()
        operand1 = pmosStack.pop()

        # PMOS(S,G,D)
        # Series
        print("PMOS ( VDD, " + str(operand2) + ", P" + str(p_ctr) + " )" )
        print("PMOS ( P" + str(p_ctr) + ", " + str(operand1) + ", Q" + str(q_ctr) + " )" )

        # Invert output
        print("PMOS ( VDD, Q" + str(q_ctr) + ", " + str(output) + " )" )
    else: # not case
        operand1 = pmosStack.pop()

        # Invert output
        print("PMOS ( VDD, " + str(operand1) + ", " + str(output) + " )" )
    return output

def input_signals(expression):
    print("INPUT", end=' ')
    for i in expression:
        if i.isidentifier():
            print(i, end=' ')
    print()

def main():
    welcome_string="""

    Welcom to Boolean Expression to Netlist Converter v1.0
    Author: Anmol Gupta CED15I021
    
    Please inter a valid Boolean Expression.
    Spaces are not valid in input expression.
    Input signals can be form A-Z,a-z excluding O,P,Q.
    
    Operator:
    '+' = OR
    '.' = AND
    '!' = NOT
    
    Example:
    !A+(B.C)
    ((!(A+B).C)+!B) 

    OUTPUT FORMAT:
    NMOS ( Source, Gate, Drain )
    PMOS ( Source, Gate, Drain )
    """

    print(welcome_string)
    input_expression = str(input("Enter input expression: "))
    print()
    print("Postfix: " + infixToPostfix(input_expression))
    print()
    print("Netlist:")
    input_signals(input_expression)
    postfixEval(infixToPostfix(input_expression))

if __name__ == "__main__":
    main()
