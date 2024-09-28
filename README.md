## Table Of Contents

- [About]
- [Build Instructions]
- [ToyC Specifications]
- [Implementation]
- [Conclusion]

## About 
---
This was a semester long project for a graduate compiler construction course.  The compiler uses a  LL(1) recursive decent parser, and generates a Jasmin file. The file can then be assembled into JVM byte code, which will run as a Java class file. Meaning this compiler takes in a ToyC program  and outputs a Java program.

### Limitations
While the scanner and parser are fully featured for ToyC, the generator only produces operations for integer and string types. Additionally, this compiler will only generate 2 level namespaces. Meaning a single class with function definitions will be generated, but not multiple classes. 

### Problems
As this was my first major C++ project, I learned quite a lot of lessons...

I go into more detail in the conclusion section. 

The biggest problem is how I tracked/managed stack memory during the semantic analysis phase. I didn't have a great way of calculating the overall size of a namespace, meaning the generated code can sometimes fail to assemble or run. This is largely due to my symbol table implementation, as it doesn't properly calculate memory allocations for iterations or recursions. The solution is relatively trivial, but would require rewriting the semantic analyzer and the symbol table. Since this was an academic exercise, I haven't been compelled to do this. If I were to, I'd pretty much rewrite this whole compiler. 

## Build Instructions
---
### Prerequisites
To build the ToyC compiler, CMake must be installed.
#### Mac  
CMake uses the Homebrew package manager. If installed, simply run in the terminal: 
- ``sh brew install cmake``
#### Linux
Cmake uses apt-get, to install run: 
-  `` sudo apt-get install cmake ``
#### Windows
- You can download it [here](https://cmake.org/download/)

### Building
You must change into the `current` directory, then the build folder
1. `cd current`
2. `cd build`

Then run these 2 commands:
1. `cmake ..`
2. `cmake --build .`
### Running 
To run the compiler without a ToyC file, use:
`./current

This will generate an error as the compiler expects a ToyC file. This can be done by running: 
`./current test.tc`
#### Command Line Options
The following options can be passed through the command line. These flags control various aspects of output, debugging, and compiler behavior: 
- `-help` Displays a usage message explaining available options and usage.
- `-output <file>` Specifies the target file name for output. 
- `-class <file>` Specifies the class file name. 
- `-debug <level>` Displays messages that aid in tracing the compilation process. The debug levels are as follows: 
	- `0` - Displays all messages. 
	- `1` - Displays scanner messages only. 
	- `2` - Displays parser messages only. 
	- `3` - Displays code generation messages only. 
- `-abstract` Dumps the abstract syntax tree for inspection. 
- `-symbol` Dumps the symbol table(s). 
- `-code` Dumps the generated program code. 
- `-verbose` Displays all information during compilation. 

##### Using The Jasmin File
If you want to assemble the .j output file, you must use the [Jasmin assembler.](https://jasmin.sourceforge.net/)This will generate a .class file, and make it runnable on JVM. The [[#Example Output]] section demonstrates the various input/output files and the resulting Java class.  

## ToyC Specifications
---
### Basic Compiler Theory
Since its hard to understand a compiler without being privy to some of the background theory, I've included some background knowledge. However this is nowhere near comprehensive, and if you're interested in learning more I'd recommend skimming the [Dragon Book.](https://github.com/muthukumarse/books/blob/master/Dragon%20Book%20Compilers%20Principle%20Techniques%20and%20Tools%202nd%20Edtion.pdf)
#### Tokens & Lexemes
A token is essentially storing a regular expression. It is the most primitive definition of a language. For example, this is what declaring a string is in the form of a regular expression:
![stringstatemachine](https://github.com/user-attachments/assets/4e910005-5291-44c2-b579-d5afa7513ac5)

#### Syntax & Grammar
Forming the syntax of a language is typically very time intensive. As the author has to consider many different rules and productions in-order to get a fully fleshed out language. Take the specification for a definition: 
- ` Definition → Type identifier (FunctionDefinition | ;)` 

This can be read as:
- "A definition must have a type with an identifier, followed by either a `FunctionDefinition` or semicolon"

In code this translates to:
``` C
int myVar;
int myVar() {}
```

Syntax is most clearly understood by starting at the base, and working up to the final production. In ToyC the base is defined as `Primary` and the final production is defined as `ToyCProgram`.

Grammar is used to convey what is legal or illegal for language. It abstracts the specifics of syntax, and makes it easier to understand how to use the language. In the context of compiler design, grammar can be ignored as the syntax is what actually defines the production.  
### Keywords:
- int, char, return, newline, default
- if, else, switch, case, break
- do, while, continue, for, write, read

### Tokens:
- **Identifier**
	- letter -> [a-zA-Z]
	- digit -> [0-9]
	- identifier -> letter (letter | digit)*

- **Number**
	- digits -> digit digit*
	- optional_fraction -> (. digits) | ɛ
	- optional_exponent -> (E (+ | - | ɛ) digits) | ɛ
	- number -> digits optional_fraction optional_exponent
	
- **CharLiteral**: A terminated character without newlines, delimited by single quotes
- **String**: A terminated character stream without newlines, delimited by double quotes
- **Relop**: ==, !=, <, <=, >=, >
- **Addop**: +,-,||
- **Mulop**: *,/,%, &&
- **Assignop**: = 
- Others:
	- **Lparen** (, **Rparen** ), **Lcurly** {, **Rcurly** }, **Comma** ,, **Semicolon** ;, **Not** !, **Colon** :

### Syntax Specifications:
- **ToyCProgram** → { Definition } EOF 
- **Definition** → Type identifier ( FunctionDefinition | ; ) 
- **Type** → int | char 
- **FunctionDefinition** → FunctionHeader FunctionBody 
- **FunctionHeader** → ( [ FormalParamList ] ) 
- **FunctionBody** → CompoundStatement 
- **FormalParamList** → Type identifier { , Type identifier } 
- **Statement** → ExpressionStatement | BreakStatement | CompoundStatement | IfStatement | NullStatement | ReturnStatement | WhileStatement | ReadStatement | WriteStatement | NewLineStatement 
- **ExpressionStatement** → Expression ; 
- **BreakStatement** → break ; 
- **CompoundStatement** → { { Type identifier ; } { Statement } } 
- **IfStatement** → if ( Expression ) Statement [ else Statement ] 
- **NullStatement** → ; 
- **ReturnStatement** → return [ Expression ] ; 
- **WhileStatement** → while ( Expression ) Statement 
- **ReadStatement** → read ( identifier { , identifier } ) ; 
- **WriteStatement** → write ( ActualParameters ) ; 
- **NewLineStatement** → newline ; 
- **Expression** → RelopExpression { assignop RelopExpression } 
- **RelopExpression** → SimpleExpression { relop SimpleExpression } 
- **SimpleExpression** → Term { addop Term } 
- **Term** → Primary { mulop Primary } 
- **Primary** → identifier [ FunctionCall ] | number | stringConstant | charConstant | ( Expression ) | ( − | not ) 
- **Primary FunctionCall** → ( [ ActualParameters ] ) ActualParameters → Expression { , Expression }

### Grammar:
**Program**: 
	= prog(Definition∗) 

**Definition**: 
	= funcDef(Id, Type, varDef(Id+, Type)∗, Statement) | varDef(Id+, Type) 

**Statement**: 
	= exprState(Expression) 
	| breakState() 
	| blockState(varDef(Id+, Type)∗, Statement∗) 
	| ifState(Expression, Statement, Statement?) 
	| nullState() 
	| returnState(Expression?) 
	| whileState(Expression, Statement) 
	| readState(Id+) 
	| writeState(Expression+) 
	| newLineState() 

**Expression**:
	= Number 
	| Identifier
	| CharLiteral 
	| StringLiteral 
	| funcCall(Identifier, Expression∗) 
	| expr(Operator, Expression, Expression)
	| minus(Expression) 
	| not(Expression) 

**Operator**: 
	= + 
	| −
	| ∗
	| /
	| %
	| ||
	| &&
	| <=
	| <
	| =
	| >
	| >= 
	| !=

### Example Output

Source file in conventional C: 
``` C
int factorial(int n) { 
	if (n == 0) return 1; 
	return n * factorial(n - 1); 
} 
int isEven(int number) {
	if ((number % 2) == 0) return 1;
	else return 0; 
} 
int main() {
	int n = 5; 
	int result = factorial(n); 
	printf("The factorial of %d is %d", n, result);
	
	if (isEven(result) == 1) {
		printf(" which is even");
	}
	else {
		printf(" which is odd"); 
	}
	printf("\n"); 
	return 0;
 }
```

Source file in ToyC:
``` C
int factorial(int n) {
   read(n);
    if (n == 0)
        return 1;
    else
	    n = n - 1;
        return n * factorial(n);
}

int isEven(int number) {
    if ((number % 2) == 0)
        return 1;
    else
        return 0;
}

int main() {
    int n;
    int result;

    n = 5; 
    result = factorial(n);

    write("The factorial of ");
    write(n);
    write(" is ");
    write(result);
    read(result);

    if (isEven(result) == 1) {
        write(" which is even");
    } else {
        write(" which is odd");
    }
    newline;

    return;
}
```

Output .j file
``` Java
; File: C:\Users\maxwe\Source\Repos\compiler\current\out\myAwesomeJasminFile.j
; Author: Max Hofmeyer
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

```

Java file after assembling the .j output file using Jasmin
``` Java
public class Main {  
    public static int factorial (int n) {  
        if (n == 0) return 1;  
        else {
		    n = n - 1;  
            return n * factorial(n);  
        }  
    }  
  
    public static int isEven(int number){  
        if ((number % 2 == 0)) {  
            return 1;  
        }  
        else {  
            return 0;  
        }  
    }  
  
    public static void main(String[] args) {  
        int n = 5;  
        int result = factorial(n);  
  
        System.out.print("The factorial of ");  
        System.out.print(n);  
        System.out.print(" is ");  
        System.out.print(result);  
  
        if (isEven(result) == 1) {  
            System.out.print(" which is even");  
        }  
        else {  
            System.out.print(" which is odd");  
        }  
        System.out.println();  
    }  
}
```


## Implementation 
---
### Front End
#### Scanner / Lexical Analyzer
Responsible for reading the input file and generating tokens. It tracks line count and checks for basic errors (illegal characters, unterminated strings). 
#### Parser
Responsible for constructing the abstract syntax using token streams. This compiler is parser driven, meaning it is the parsers responsibility to determine when the scanner needs to read in another token. The parser is responsible for most of the error handling, and will enforce the rules laid out by the syntax.   
#### AST (Abstract Syntax Tree)
This is the stored structure of the input file, and is the final product of the front end of the compiler. 

### Back End
#### Code Generator
Is fed the cleaned AST and recursively walks down the tree to generate assembly code for a target language. It uses the symbol table to lookup the specifics of an identifier and create correct labels for branches and jumps.

### Helpers
#### Symbol Table
Stores the specifics of an identifier to aid in code generation
#### Semantic Analyzer
Performs more complex checks on the AST (identifier is declared before use, functions return the correct type, ect) and populates the symbol table for the code generator.
#### Config
Reads in the command line to configure the compiler based on the flags. Also performs basic checks on the passed file and stores it for the scanner. 
## Conclusion
---
### Lessons Learned
This project deserves a rewrite, and I am only posting it to demonstrate how my skills have largely improved over the years. Here are some major refactors: 
#### The Use of std::variant<>
I made the rookie mistake it seems a lot of first time C++ devs do, and that is loading up on language features expecting no trade-offs. For example inside AST.hpp: 

``` C++
class NodePrimary : public Node {
public:
	using PrimaryVal = std::variant <
		token, //handles num,string,char,id, - and not
		std::unique_ptr<NodeExpression>,
		std::pair<token, std::unique_ptr<NodePrimary>>
	>;
	PrimaryVal val;
	std::optional<std::unique_ptr<NodeFunctionCall>> rhs;

	
	explicit NodePrimary(token t) : val(std::move(t)) {}

	NodePrimary(token t, std::unique_ptr<NodeFunctionCall> func)
		: val(std::move(t)), rhs(std::move(func)) {}

	NodePrimary(std::unique_ptr<NodeExpression> expr) : val(std::move(expr)) {}
	NodePrimary(token t, std::unique_ptr<NodePrimary> p)
	     : val(std::make_pair(std::move(t), std::move(p))) {}

	void print(std::ostream& out) const override;
};
	```

I used `std::varient<>` because a primary could be either a token, a NodeExpression, or a token with a primary. Since `std::varient` offers a type-safe union, I figured this was the perfect tool for my application. However, as I used it later I realized how much complexity it added when I attempted to unpack the specific types. This is largely from `std::visit` as its the mechanism for accessing the varying types. It required an overloaded struct to handle the different encapsulated types within the variant.  For example in jasmineGenerator.cpp: 
``` c++
template <class... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

void AnalyseSemantics::analyzePrimary(NodePrimary& primary) {
	if (hasError) return;
	std::string lastUsedID;
	std::visit(overloaded{
	   [this, &lastUsedID](token& t) {
		   if (t.type == Tokens::ID) {
			   if (!_table.checkForSymbol(t.value)) {
				   reportError(
					   td::to_string(t.lineLoc) + ": Identifier '" + t.value +
					   "' was referenced before being declared");
			   }
			   lastUsedID = t.value;
		   }

	   },
	   [this](const std::unique_ptr<NodeExpression>& expr) {
		   analyzeExpression(*expr);
	   },
	   [this](const std::pair<token, std::unique_ptr<NodePrimary>>& p) {
		   analyzePrimary(*p.second);
	   }
   }, primary.val);

	if (primary.rhs) {
		analyzeFunctionCall(lastUsedID, *primary.rhs.value());
	}
}
```
What I should've done instead was embrace polymorphism. I did this correctly with how I printed the AST: 
``` c++

/* AST.hpp */
class Node {
public:
	virtual ~Node() {}
	virtual void print(std::ostream& out) const = 0;
};

class NodeToyCProgram : public Node {
public:
	std::vector<std::unique_ptr<NodeDefinition>> definitions;
	void addRHS(std::unique_ptr<NodeDefinition> def) {
		definitions.emplace_back(std::move(def));
	}

	void print(std::ostream& out) const override;
};

class NodeDefinition : public Node {
public:
	std::unique_ptr<NodeDeclaration> lhs;
	std::optional<std::unique_ptr<NodeFunctionDefinition>> rhs;
	explicit NodeDefinition(std::unique_ptr<NodeDeclaration> decl)
		: lhs(std::move(decl)) {}

	NodeDefinition(std::unique_ptr<NodeDeclaration> decl,         std::unique_ptr<NodeFunctionDefinition> funcdef)
	: lhs(std::move(decl)), rhs(std::move(funcdef)) {}

	void print(std::ostream& out) const override;
};

/* AST.cpp without print code */
void NodeToyCProgram::print(std::ostream& out) const {
    for (const auto& def : definitions) {
        def->print(out);
        out << "\n";
    }
}

void NodeDefinition::print(std::ostream& out) const {
    lhs->print(out);
	if (rhs.has_value()) 
        rhs.value()->print(out);
}
```

If I just created a node for each possible `NodePrimary` instead of using variant, it would've made it much simpler to actually use the node. 

#### No mechanism to extract primitive types from the AST
While I was building this compiler, it was in distinct stages. Stage 1 was getting the scanner working, stage 2 was getting the parser working, and stage 3 was the code generation. The issue with that is I built the AST (stage 2) without having to immediately use it (stage 3). So I lacked the foresight in providing a polymorphic mechanism to extract types for a given node. This is especially needed in performing semantic analysis.

``` c++
//semanticAnalysis.cpp
//removed print messages for the sake of brevity
void AnalyseSemantics::analyzeReturnStatement(const NodeReturnStatement& returnStatement) {
	if (hasError) return;
	
	const auto func = _table.lookupSymbol(_table.getFirstSymbol());
	if (!returnStatement.lhs.has_value()) {
		reportError();
		return;
	}
	
	std::vector<token> returnT;
	if (func == nullptr) return;
	extractTypesFromExpression(*returnStatement.lhs.value(), returnT, false);
	
	if (returnT.empty()) {
		reportError();
	}
	if (returnT[0].type != func->type.type) {
		reportError();
		return;
	}
	
	analyzeExpression(*returnStatement.lhs.value());
}

```

In order to check if a return statement is the correct type, you must descend the tree all the way down to `NodePrimary`. This should be a simple check, but because of the deeply nested tree it can get messy quickly (as seen above...). Instead what would've been better was to implement some polymorphic traversal mechanism, allowing a parent to get all of its children primitive tokens. Rewritten this could've been as simple as: 
``` c++
//semanticAnalysis.cpp
//removed print messages for the sake of brevity
void AnalyseSemantics::analyzeReturnStatement(const NodeReturnStatement& returnStatement) {
	if (hasError) return;
	
	const auto func = _table.lookupSymbol(_table.getFirstSymbol());
	if (!returnStatement.lhs.has_value()) {
		reportError();
		return;
	}
	const auto tok = returnStatement.GetPrimitives();

	if (tok.type != func->type.type) {
		reportError();
		return;
	}
	
	analyzeExpression(*returnStatement.lhs.value());
}

```
Since now the hypothetical `GetPrimitives()` would return a token or a vector of tokens depending on the specific implementation. 
