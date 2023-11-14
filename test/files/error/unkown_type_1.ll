; BEGIN_TEST
; error: failed to parse 'ret some_type' as an instruction
; END_TEST
define i64 @func() {
0:
  ret some_type
}
