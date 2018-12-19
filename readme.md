# MVM - Moritz virtual machine

## Getting started

Create CMake project:

```
cmake CMakeLists.txt
```

Compile and run tests:

```
make
./runTests
```

## Opt codes

LOAD    L   REG
MOVE    REG REG

ADD     REG REG REG
SUB     REG REG REG
MUL     REG REG REG
DIV     REG REG REG

OUT     REG
ASK     REG

CMPE    REG REG REG
CMPL    REG REG REG
CMLE    REG REG REG
CMPG    REG REG REG
CMGE    REG REG REG

JMPC    REG L 
JMP     L

DBG
END
LABEL   L

AND     REG REG REG
OR      REG REG REG
XOR     REG REG REG
NOT     REG REG REG

PUSH    REG
POP     REG

; COMMENT

### gdb

```
gdb runTests
catch throw
run
```