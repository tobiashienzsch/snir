; BEGIN_TEST
; name: func
; type: double
; args: 0
; blocks: 1
; instructions: 4
; return: -101
; END_TEST
define double @func() {
0:
    %0 = double 42.0
    %1 = double 143.0
    %2 = fsub double %0, %1
    ret double %2
}
