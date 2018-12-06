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

        FRM TO

LOAD    L   REG
MOVE    REG REG

ADD     REG REG
SUB     REG REG
MUL     REG REG
DIV     REG REG

OUT     REG
ASK         REG

CMP     REG REG

JMPE    L
JMPL    L 
JMPG    L 
JPGE    L 
JPLE    L 
JMP     L

DBG
END
LABEL   L

AND     REG REG
OR      REG REG
XOR     REG REG
NOT     REG REG

PUSH    REG
POP         REG

; COMMENT