define i64 @func() {
0:
    %0 = i64 42
    %1 = i64 2
    %2 = shl i64 %0 %1
    ret i64 %2
}
