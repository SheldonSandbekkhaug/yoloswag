This is an interpreter for a parody programming language called Yoloswag. Yoloswag is
currently under development and aspects of it may change in future updates.

Table of Contents

1. Running the Interpreter
2. About Yoloswag
3. Examples and Notes
4. Troubleshooting and Known Bugs


1. Running the Interpreter

To compile the interpreter, run the following command:

> make

You can run the interpreter with or without a file:

> ./yoloswag
> ./yoloswag hello_world.ys


2. About Yoloswag

The goal of Yoloswag is to poke fun at terms such as "YOLO" (You Only Live Once) and
"swag" (an adjective used to compliment someone or something). Yoloswag is not
designed to be like other programming languages, nor is it intended to be extremely
efficient. Rather, it is designed to be simple. Though extensions of Yoloswag are
under consideration, there are only four keywords in the original version of
Yoloswag (plus the fifth "experimental" keyword). These keywords are:

yolo
	Declares an 8-bit variable with the given name.

swag
	Increments a variable by one.

ratchet
	Decrements a variable by one.

holla
	Prints the ASCII character corresponding to this variable's value.

hollaNumber
	(Experimental) Prints the value of this variable.

It is important to note that there are no operands to add, subtract, or assign
the value of one variable to another. This is intended to increase the amount of swag
found in Yoloswag files (file extension .ys).

Another important issue to note is that the '#' character (pronounced "hash tag")
indicates the end of a statement. By Yoloswag convention, the '#' should be placed at
the beginning of a line before the first keyword, but this is not required. It is
also customary to place a hash tag before the first statement in a file.

Because '#' acts similarly to a semicolon, the interpreter will not stop reading a
statement until it reaches a hash tag. This is to allow multiple lines to be used
in a single statement (see hello_world.ys for an example of this).


3. Examples and Notes

Declare the variable foo:
#yolo foo
#

Increment foo:
#swag foo
#

Add 2 to the value of foo:
#swag swag swag swag swag foo
#

Add 9 to the value of foo:
#swag
swag swag swag swag swag
swag swag swag
foo
#

NOTE: Because each variable is represented by eight bits, the programmer must be
careful not to swag too many times. Since an eight-bit variable can only hold values
up to 255 (if used as an unsigned integer), swag'ing a variable 256 times results in
a SWAG OVERFLOW, in which the variable's value is effectively reset to 0.

Decrement foo:
#ratchet foo

NOTE: Ratchet'ing a variable with a value of 0 causes a RATCHET UNDERFLOW, in which
the variable's value is effectively set to 255.

Print foo as an ASCII character
#holla foo


4. Troubleshooting and Known Bugs

"The interpreter isn't working when I give it statements on the command line!"
This isn't a bug. The interpreter considers a statement to be everything between two
hash tags (except the first statement).

"Holla isn't printing anything!"
This isn't a bug. Check that you're trying to print a valid ASCII value. Values under
32 probably won't appear. Try value 48, that should print a '0'.

As Yoloswag is still under development, there are probably other bugs in the codebase.
Send all complaints to sheldon@sandbekkhaug.net.
