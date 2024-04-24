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


	getstatic java/lang/System.out Ljava/io/PrintStream;
	iload 0 ; x
	invokestatic java/lang/String/valueOf(I)Ljava/lang/String;
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V

	getstatic java/lang/System.out Ljava/io/PrintStream;
	ldc "what?"
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V

	return

.end method

