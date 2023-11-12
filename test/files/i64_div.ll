; BEGIN_TEST
; name: func
; type: i64
; args: 0
; blocks: 1
; instructions: 4
; return: 21
; END_TEST
define i64 @func() {
0:
    %1 = i64 42
    %2 = i64 2
    %3 = div i64 %1, %2
    ret i64 %3
}
