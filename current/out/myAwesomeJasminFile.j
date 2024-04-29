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


.method public static isEven(I)I
	.limit locals 5
	.limit stack 5

	iload 0 ; n

	ldc 2
	irem
	ldc 0
	if_icmpeq Ltrue0
	goto Lfalse0

	Ltrue0:
	ldc 0
	goto Lend0

	Lfalse0:
	ldc 1
	Lend0:

	ireturn

.end method

.method public static isOdd()I
	.limit locals 5
	.limit stack 5

	ldc 0 ; init b
	istore 2

	iload 2 ; b
	ldc 5
	istore 2 ; b


	getstatic java/lang/System.out Ljava/io/PrintStream;
	ldc "x is odd"
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
	iload 2 ; b

	ireturn

.end method

.method public static main([Ljava/lang/String;)V
	.limit locals 5
	.limit stack 5

	ldc 0 ; init x
	istore 0

	iload 0 ; x
	ldc 1
	istore 0 ; x

	iload 0 ; x
	ldc 2
	iadd
	ldc 3
	iadd
	ldc 4
	iadd
	istore 0 ; x

	iload 0 ; x
	invokestatic mySuperCoolClass/isEven(I)I

	getstatic java/lang/System.out Ljava/io/PrintStream;
	ldc "Hello"
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V

	getstatic java/lang/System.out Ljava/io/PrintStream;
	ldc "
	"
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V

	getstatic java/lang/System.out Ljava/io/PrintStream;
	iload 0 ; x
	invokestatic java/lang/String/valueOf(I)Ljava/lang/String;
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
	invokestatic mySuperCoolClass/isOdd()I
	istore 0 ; x


	getstatic java/lang/System.out Ljava/io/PrintStream;
	iload 0 ; x
	invokestatic java/lang/String/valueOf(I)Ljava/lang/String;
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V

	return

.end method

