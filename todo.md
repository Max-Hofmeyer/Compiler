# Part 2

## Tech Debt
* change scanner to work alongside other programs, instead of dumping everything inside a vector [DONE]
* Possibly remove std::optional from value string for tokens, instead manually assigning the values

## Parser
Probably LR(1)

### Build syntax understanding
* Line by line?

### Error messages
* Find what token is needed for a line if it isn't valid syntax, and output it

## Abstract Syntax Tree (future problem)

## Expand CLI
* New debug level, and lots of other features

##PrettyPrinting
* Outputs from the parser will have to respect the whitespace indent level
  
