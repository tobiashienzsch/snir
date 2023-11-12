; BEGIN_TEST
; name: func
; type: float
; args: 0
; blocks: 1
; instructions: 4
; END_TEST
define float @func() {
0:
    %0 = float 42
    %1 = float 143
    %2 = fadd float %0, %1
    ret float %2
}
