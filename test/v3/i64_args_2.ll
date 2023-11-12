; BEGIN_TEST
; name: func
; type: i64
; args: 2
; blocks: 1
; instructions: 2
; END_TEST
define i64 @func(i64 %0, i64 %1) {
2:
    %2 = add i64 %0, %1
    ret i64 %2
}
