LOAD 3 %a
LOAD 10 %b
while:
INCR %a
OUT %a
CMPL %a %b %cont
JMPC while %cont
END