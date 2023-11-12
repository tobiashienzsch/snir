; BEGIN_TEST
; name: func
; type: double
; args: 0
; blocks: 1
; instructions: 5
; return: 185
; END_TEST
define double @func() {
0:
    ; nop
    %0 = double 42
    %1 = double 143
    %2 = fadd double %0, %1
    ret double %2
}
