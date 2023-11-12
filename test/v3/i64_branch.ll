; BEGIN_TEST
; name: func
; type: i64
; args: 0
; blocks: 2
; instructions: 3
; END_TEST
define i64 @func() {
0:
    br label %1
1:
    %2 = i64 9
    ret i64 %2
}
