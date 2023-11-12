; BEGIN_TEST
; name: func
; type: float
; args: 0
; blocks: 1
; instructions: 4
; return: -101
; END_TEST
define float @func() {
0:
    %0 = float 42.0
    %1 = float 143.0
    %2 = fsub float %0, %1
    ret float %2
}
