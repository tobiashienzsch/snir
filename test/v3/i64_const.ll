; BEGIN_TEST
; name: func
; type: i64
; args: 0
; blocks: 1
; instructions: 2
; END_TEST
define i64 @func() {
0:
    %0 = i64 42
    ret i64 %0
}
