LOAD 1 %a
LOAD 10 %b
PUSH %a
PUSH %b
CALL XX
END
XX:
POP %b
POP %a
INCR %a
CMPE %a %b %end
JMPC toend %end
PUSH %a
PUSH %b
CALL XX
toend:
OUT %a
RETURN
END