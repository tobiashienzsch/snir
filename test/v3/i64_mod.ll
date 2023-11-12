; BEGIN_TEST
; name: func
; type: i64
; args: 0
; blocks: 1
; instructions: 4
; return: 2
; END_TEST
define i64 @func() {
0:
    %0 = i64 42
    %1 = i64 4
    %2 = mod i64 %0, %1
    ret i64 %2
}
