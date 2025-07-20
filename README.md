# Brainfuck Interpreter

This is a lightweight and configurabe Brainfuck interpreter written in C.

## Features

- Fully supports standard Brainfuck
- Allows for configurable memory tape length (default: 30,000 cells)
- Allows for nested while loops
- Optional debug warnings of memory overflows
- Ignored new lines and spaces

## Build

To compile the interpreter, run:
```bash
./build
```
The warnings are expected.

## Examples

Example brainfuck programs can be found in the [examples](./examples)
* echo.bf
	* Echo command
	
* echo-small.bf 
	* A test of the reading and writing -- handles a few echos

* helloworld.bf 
	* An explicit implementation of print("Hello World!")

* helloworld-while.bf
	* An implementation of print("Hello World!") that uses while loops

* overflow.bf
	* Should be run with -m 
	* Shows the warning message for overflowing a cell
