; File:     	C:\Users\maxwe\Source\Repos\compiler\current\out\myAwesomeJasminFile.j
; Author(s):	Max Hofmeyer, Ahmed Malik, 19 April 2024
; -------------------------------------------------------------------------

.class public mySuperCoolClass
.super java/lang/Object

.method public <init>()V
	aload_0
	invokenonvirtual java/lang/Object/<init>()V
	return
.end method


.method public static isEven(II)I
	.limit locals 5
	.limit stack 5

	iload 0 ; n
	iload 1 ; m

	ineg
	ldc 2
	idiv
	ldc 0
	if_icmpeq Ltrue0
	goto Lfalse0

	Ltrue0:
	ldc 1
	goto Lend0

	Lfalse0:
	ldc 0
	Lend0:

	ireturn

.end method

.method public static hello()I
	.limit locals 5
	.limit stack 5

	ldc 0

	ireturn

.end method

.method public static goodbye()I
	.limit locals 5
	.limit stack 5


	getstatic java/lang/System.out Ljava/io/PrintStream;
	ldc "hello"
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
	ldc 0

	ireturn

.end method

.method public static main([Ljava/lang/String;)V
	.limit locals 5
	.limit stack 5

	ldc 0 ; init i
	istore 0

	iload 0 ; i
	invokestatic mySuperCoolClass/goodbye()I
	istore 0 ; i

	iload 0 ; i

	return

.end method

