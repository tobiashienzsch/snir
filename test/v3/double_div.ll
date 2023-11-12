; BEGIN_TEST
; name: func
; type: double
; args: 0
; blocks: 1
; instructions: 4
; END_TEST
define double @func() {
0:
    %0 = double 42.0
    %1 = double 2.0
    %2 = fdiv double %0, %1
    ret double %2
}
