#!/usr/bin/env python
# from tabulate import tabulate

class Stack:
	def __init__(self):
		self.data=[]
	def push(self, item):
		self.data.append(item)
	def pop(self):
		return self.data.pop()
	def isEmpty(self):
		return self.data==[]
	def size(self):
		return len(self.data)
	def peek(self):
		return self.data[len(self.data)-1]

IpDict={}

# Converts the infix expression into a postfix expression
def InfixToPostfix(infix):
	precedence={"!":4, ".":3, "+":2, "&":3, "|":2, "(":1}
	OpStack=Stack()
	postfixlst=[]
	infixlst=list(infix)
	global IpDict

	for x in infixlst:
		if x in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz":
			postfixlst.append(x)
			IpDict[x]=int (input("Enter the value for " + str(x) + " (0/1) : "))
		elif x=='(':
			OpStack.push(x)
		elif x==')':
			top=OpStack.pop()
			while top!='(':
				postfixlst.append(top)
				top=OpStack.pop()
		else:
			while (not OpStack.isEmpty()) and (precedence[OpStack.peek()] >= precedence[x]):
				postfixlst.append(OpStack.pop())
			OpStack.push(x);

	while not OpStack.isEmpty():
		postfixlst.append(OpStack.pop())
	return "".join(postfixlst)

cmosStack=Stack()
def PostfixEval(postfix):
	global cmosStack
	postfixlst=list(postfix)
	for x in postfixlst:
		if x in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz":
			cmosStack.push(x)
		else:
			z=cmosOpns(x, cmosStack)
			cmosStack.push(z)

op_count=0
nmos_count=0
pmos_count=0

# Assigns the logical operations and their corresponding CMOS circuit
def cmosOpns(x, cmosStack):
	global op_count
	global nmos_count
	global pmos_count
	global IpDict

	op_count=op_count+1
	print(" ")
	if x=='&':	
		# NAND: two NMOS transistors in series, two PMOS in parallel
		operand2=cmosStack.pop()
		operand1=cmosStack.pop()
	
		print("NAND of " + str(operand1) + " and " + str(operand2) + ": ")
		print("----------------")
	
		pmos_count=pmos_count+1
		print("PMOS-" + str(pmos_count) + " ( VDD, " + str(operand1) + ", Y" + str(op_count) + ")")
		pmos_count=pmos_count+1
		print("PMOS-" + str(pmos_count) + " ( VDD, " + str(operand2) + ", Y" + str(op_count) + ")")

		nmos_count=nmos_count+1
		print("NMOS-" + str(nmos_count) + " ( drain( NMOS-" + str(nmos_count+1) + "), " + str(operand1) + ", Y" + str(op_count) + ")")
		nmos_count=nmos_count+1
		print("NMOS-" + str(nmos_count) + " ( GND, " + str(operand2) + ", source( NMOS-" + str(nmos_count-1) + " ))")

		if (IpDict[str(operand1)] == 1) and (IpDict[str(operand2)] == 1):
			IpDict["Y" + str(op_count)]=0
			print("NMOS network is ON and PMOS network is OFF")
		else:
			IpDict["Y" + str(op_count)]=1
			print("NMOS network is OFF and PMOS network is ON")

		print(str(operand1) + " = " + str(IpDict[str(operand1)]) + ", "+ str(operand2) + " = " + str(IpDict[str(operand2)]) + ", Y" + str(op_count) + " = " + str(IpDict["Y" + str(op_count)]))


	elif x=='|':
		# NOR: two NMOS transistors in parallel, two PMOS in series
		operand2=cmosStack.pop()
		operand1=cmosStack.pop()
	
		print("NOR of " + str(operand1) + " and " + str(operand2) + ": ")
		print("----------------")
	
		pmos_count=pmos_count+1
		print("PMOS-" + str(pmos_count) + " ( VDD, " + str(operand1) + ", source( PMOS-" + str(pmos_count+1) + " ))")
		pmos_count=pmos_count+1
		print("PMOS-" + str(pmos_count) + " ( drain( PMOS-" + str(pmos_count-1) + " ), " + str(operand2) + ", Y" + str(op_count) + ")")

		nmos_count=nmos_count+1
		print("NMOS-" + str(nmos_count) + " ( GND, " + str(operand1) + ", Y" + str(op_count) + ")")
		nmos_count=nmos_count+1
		print("NMOS-" + str(nmos_count) + " ( GND, " + str(operand2) + ", Y" + str(op_count) + ")")

		if (IpDict[str(operand1)]==0) and (IpDict[str(operand2)]==0):
			IpDict["Y" + str(op_count)]=1
			print("NMOS network is OFF and PMOS network is ON")
		else:
			IpDict["Y" + str(op_count)]=0
			print("NMOS network is ON and PMOS network is OFF")
	
		print(str(operand1) + " = " + str(IpDict[str(operand1)]) + ", "+ str(operand2) + " = " + str(IpDict[str(operand2)]) + ", Y" + str(op_count) + " = " + str(IpDict["Y" + str(op_count)]))

	elif x=='.':
		# AND: NAND followed by NOT
		
		operand2=cmosStack.pop()
		operand1=cmosStack.pop()
	
		print("AND of " + str(operand1) + " and " + str(operand2) + ": ")
		print("----------------")
	
		# NAND: two NMOS transistors in series, two PMOS in parallel
		pmos_count=pmos_count+1
		print("PMOS-" + str(pmos_count) + " ( VDD, " + str(operand1) + ", Y" + str(op_count) + ")")
		pmos_count=pmos_count+1
		print("PMOS-" + str(pmos_count) + " ( VDD, " + str(operand2) + ", Y" + str(op_count) + ")")

		nmos_count=nmos_count+1
		print("NMOS-" + str(nmos_count) + " ( drain( NMOS-" + str(nmos_count+1) + "), " + str(operand1) + ", Y" + str(op_count) + ")")
		nmos_count=nmos_count+1
		print("NMOS-" + str(nmos_count) + " ( GND, " + str(operand2) + ", source( NMOS-" + str(nmos_count-1) + " ))")

		if (IpDict[str(operand1)]==1) and (IpDict[str(operand2)]==1):
			IpDict["Y" + str(op_count)]=0
			print("NMOS network is ON and PMOS network is OFF")
		else:
			IpDict["Y" + str(op_count)]=1
			print("NMOS network is OFF and PMOS network is ON")

		# NOT: 1 PMOS and NMOS transistor in parallel
		op_count=op_count+1
		pmos_count=pmos_count+1
		print("PMOS-" + str(pmos_count) + " ( VDD, Y" + str(op_count-1) + ", Y" + str(op_count) + ")")
		nmos_count=nmos_count+1
		print("NMOS-" + str(nmos_count) + " ( GND, Y" + str(op_count-1) + ", Y" + str(op_count) + ")")

		if IpDict["Y" + str(op_count-1)]==1:
			IpDict["Y" + str(op_count)]=0
			print("NMOS-" + str(nmos_count) + " is ON and PMOS-" + str(pmos_count) + " is OFF")
		else:
			IpDict["Y" + str(op_count)]=1
			print("NMOS-" + str(nmos_count) + " is OFF and PMOS-" + str(pmos_count) + " is ON")
	
		print(str(operand1) + " = " + str(IpDict[str(operand1)]) + ", "+ str(operand2) + " = " + str(IpDict[str(operand2)]) + ", Y" + str(op_count) + " = " + str(IpDict["Y" + str(op_count)]))

	elif x=='+':
		# OR: NOR followed by NOT
	
		operand2=cmosStack.pop()
		operand1=cmosStack.pop()

		print("OR of " + str(operand1) + " and " + str(operand2) + ": ")
		print("------------")	

		# NOR: two NMOS transistors in parallel, two PMOS in series
		pmos_count=pmos_count+1
		print("PMOS-" + str(pmos_count) + " ( VDD, " + str(operand1) + ", source( PMOS-" + str(pmos_count+1) + " ))")
		pmos_count=pmos_count+1
		print("PMOS-" + str(pmos_count) + " ( drain( PMOS-" + str(pmos_count-1) + " ), " + str(operand2) + ", Y" + str(op_count) + ")")

		nmos_count=nmos_count+1
		print("NMOS-" + str(nmos_count) + " ( GND, " + str(operand1) + ", Y" + str(op_count) + ")")
		nmos_count=nmos_count+1
		print("NMOS-" + str(nmos_count) + " ( GND, " + str(operand2) + ", Y" + str(op_count) + ")")

		if (IpDict[str(operand1)]==0) and (IpDict[str(operand2)]==0):
			IpDict["Y" + str(op_count)]=1
			print("NMOS network is OFF and PMOS network is ON")
		else:
			IpDict["Y" + str(op_count)]=0
			print("NMOS network is ON and PMOS network is OFF")

		# NOT: 1 PMOS and NMOS transistor in parallel
		op_count=op_count+1
		pmos_count=pmos_count+1
		print("PMOS-" + str(pmos_count) + " ( VDD, Y" + str(op_count-1) + ", Y" + str(op_count) + ")")
		nmos_count=nmos_count+1
		print("NMOS-" + str(nmos_count) + " ( GND, Y" + str(op_count-1) + ", Y" + str(op_count) + ")")

		if IpDict["Y" + str(op_count-1)]==1:
			IpDict["Y" + str(op_count)]=0
			print("NMOS-" + str(nmos_count) + " is ON and PMOS-" + str(pmos_count) + " is OFF")
		else:
			IpDict["Y" + str(op_count)]=1
			print("NMOS-" + str(nmos_count) + " is OFF and PMOS-" + str(pmos_count) + " is ON")

		print(str(operand1) + " = " + str(IpDict[str(operand1)]) + ", "+ str(operand2) + " = " + str(IpDict[str(operand2)]) + ", Y" + str(op_count) + " = " + str(IpDict["Y" + str(op_count)]))

	else:
		# NOT: 1 PMOS and NMOS transistor in parallel
		operand1=cmosStack.pop()
		op_count=op_count+1		
	
		print("NOT of " + str(operand1) + ": ")
		print("---------")

		pmos_count=pmos_count+1
		print("PMOS-" + str(pmos_count) + " ( VDD, " + str(operand1) + ", Y" + str(op_count) + ")")
		nmos_count=nmos_count+1
		print("NMOS-" + str(nmos_count) + " ( GND, " + str(operand1) + ", Y" + str(op_count) + ")")

		if IpDict[str(operand1)]==1:
			IpDict["Y" + str(op_count)]=0
			print("NMOS-" + str(nmos_count) + " is ON and PMOS-" + str(pmos_count) + " is OFF")
		else:
			IpDict["Y" + str(op_count)]=1
			print("NMOS-" + str(nmos_count) + " is OFF and PMOS-" + str(pmos_count) + " is ON")

		print(str(operand1) + " = " + str(IpDict[str(operand1)]) + ", Y" + str(op_count) + " = " + str(IpDict["Y" + str(op_count)]))
	output="Y"+str(op_count)
	return output

# Main function
print("Computer Aided Design (CAD) program for CMOS circuits-2")
print("-------------------------------------------------------")
infix=input("Enter the boolean expression (Note: '!' stands for NOT, '.' means AND, '&' means NAND, '+' stands for OR, '|' stands for NOR)\n")
print("Transistor(Source, Gate, Drain)")
print("-------------------------------")
PostfixEval(InfixToPostfix(infix))
# print(IpDict);
