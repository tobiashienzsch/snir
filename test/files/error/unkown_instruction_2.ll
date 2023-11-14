; BEGIN_TEST
; error: failed to parse 'bar label %1' as an instruction
; END_TEST
define i64 @func() {
0:
  bar label %1
}
