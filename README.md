# DisASM
A faster > ? < (I checked and it's just barely faster lol) knock off of Lqpletsp/GALang. It's meant to replicate some behaviour of assembly, but is capable of performing multiple operations in the same clock-cycle due to the comma syntax, with the main difference from GALang being a slight change of syntax to make the interpreter a one-pass system, and that it's written in cpp

## Usage
Build by running `./build.sh` in any POSIX-compliant shell with your working directory being the cloned directory.

> [!NOTE]
> `build.sh` should warn you about unmet dependencies, and will create a build directory to store the compiled binaries.

Run the demo files with `./build/src.main ./examples/fib.slang` or `./build/src.main ./examples/strings.slang`.

## Sample Program and Explanation
```asm
decf fibo x, y, depth, maxD int
  decv m int
  inc depth

  add x y m
  set x, y y, m

  out m
  outch delim

  // exit if depth 10
  // the second param to decl is the offset from l1 when labelling l1
  decl l1 2
  cmpe depth maxD l1
  call fibo x, y, depth, maxD
endf

decv delim str
set delim ", "

out 1
outch delim
out 1
outch delim

call fibo 1, 1, 1, 10

exit
```

### Basic Syntax
Each operation is written as 
> [ operation ] [ operand, ...] [ operand, ... ] [ OR MORE OPERANDS ]

For general operations like add, set, out, outch, it performs the operations on corresponding elements of the operands, so 
> add 1, 2, 3 2 x, y, z
will add 2 to 1, 2, 3 and store them to x, y, z correspondingly.

`decf` defines a function, and is ignored unless called explicitly using `call`. 
`decl` defines a label, and the second parameter specifies the offset from the label's position. This is typically used along with `cmp{x}` to perform complex logic.

> [!NOTE]
> Labels have been given offsets solely to make the system one-pass rather than two.

> [!NOTE]
> Previously, `outch` expected the ascii code of a character and displays that, which is why I've implemented strings as ascii value arrays. 
