; File:     	C:\Users\maxwe\Source\Repos\compiler\current\myAwesomeJasminFile.j
; Author(s):	Max Hofmeyer, Ahmed Malik, 19 April 2024
; -------------------------------------------------------------------------

.class public mySuperCoolClass
.super java/lang/Object

.method public <init>()V
	aload_0
	invokenonvirtual java/lang/Object/<init>()V
	return
.end method

.method public static main([Ljava/lang/String;)V
	.limit locals 7
	.limit stack 8

	irem

	return
	ldc 0 ; init i
	istore 4

	ldc 0 ; init y
	istore 5

	iload 4 ; i
	ldc 5
	ldc 10
	iadd
	istore 4 ; i

	goto loopCheck0

loop0:
	getstatic java/lang/System.out Ljava/io/PrintStream;
	ldc "hi mom"
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V

	iload 4 ; i
	ldc 1
	isub
	istore 4 ; i

	goto loopCheck0

loopCheck0:
	iload 4 ; i
	ldc 1
	iload 4 ; i
	if_icmpgt	ldc 5
	if_icmpgt loop0
	getstatic java/lang/System.out Ljava/io/PrintStream;
	ldc "
"
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
	getstatic java/lang/System.out Ljava/io/PrintStream;
	ldc "input a value for y"
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V

	goto loopCheck1

loop1:
	getstatic java/lang/System.out Ljava/io/PrintStream;
	ldc "goodbye mom"
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V

	iload 5 ; y
	ldc 2
	isub
	istore 5 ; y

	goto loopCheck1

loopCheck1:
	iload 5 ; y
	ldc 2
	if_icmpgt loop1

	return
.end method
