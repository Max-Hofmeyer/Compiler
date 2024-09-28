; File:     	C:\Users\maxwe\Source\Repos\compiler\current\out\testJasminFile.j
; Author(s):	Max Hofmeyer
; -------------------------------------------------------------------------

.class public mySuperCoolClass
.super java/lang/Object

.method public <init>()V
	aload_0
	invokenonvirtual java/lang/Object/<init>()V
	return
.end method


.method public static factorial(I)I
	.limit locals 10
	.limit stack 10

	iload 0 ; n

	
	new java/util/Scanner
	dup
	getstatic java/lang/System/in Ljava/io/InputStream;
	invokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V
	astore 2
	aload 2
	invokevirtual java/util/Scanner.nextInt()I
	istore 0 ; n

	ldc 0
	if_icmpeq Ltrue0
	goto Lfalse0

	Ltrue0:
	ldc 1
	goto Lend0

	Lfalse0:
	iload 0 ; n
	ldc 1
	isub
	istore 0 ; n

	Lend0:
	iload 0 ; n
	iload 0 ; n
	invokestatic mySuperCoolClass/factorial(I)I
	imul

	ireturn

.end method

.method public static isEven(I)I
	.limit locals 15
	.limit stack 15

	iload 0 ; number

	ldc 2
	irem
	ldc 0
	if_icmpeq Ltrue1
	goto Lfalse1

	Ltrue1:
	ldc 1
	goto Lend1

	Lfalse1:
	ldc 0
	Lend1:

	ireturn

.end method

.method public static main([Ljava/lang/String;)V
	.limit locals 15
	.limit stack 15

	ldc 0 ; init result
	istore 1

	iload 0 ; n
	ldc 5
	istore 0 ; n

	iload 1 ; result
	iload 0 ; factorial
	iload 0 ; n
	invokestatic mySuperCoolClass/factorial(I)I
	istore 1 ; result


	getstatic java/lang/System.out Ljava/io/PrintStream;
	ldc "The factorial of "
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V

	getstatic java/lang/System.out Ljava/io/PrintStream;
	iload 0 ; n
	invokestatic java/lang/String/valueOf(I)Ljava/lang/String;
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V

	getstatic java/lang/System.out Ljava/io/PrintStream;
	ldc " is "
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V

	getstatic java/lang/System.out Ljava/io/PrintStream;
	iload 1 ; result
	invokestatic java/lang/String/valueOf(I)Ljava/lang/String;
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
	
	new java/util/Scanner
	dup
	getstatic java/lang/System/in Ljava/io/InputStream;
	invokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V
	astore 3
	aload 3
	invokevirtual java/util/Scanner.nextInt()I
	istore 1 ; result

	iload 1 ; result
	invokestatic mySuperCoolClass/isEven(I)I
	ldc 1
	if_icmpeq Ltrue2
	goto Lfalse2

	Ltrue2:

	getstatic java/lang/System.out Ljava/io/PrintStream;
	ldc " which is even"
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
	goto Lend2

	Lfalse2:

	getstatic java/lang/System.out Ljava/io/PrintStream;
	ldc " which is odd"
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
	Lend2:

	getstatic java/lang/System.out Ljava/io/PrintStream;
	ldc "
	"
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
	ldc 0

	return

.end method

