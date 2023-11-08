define i64 @func() {
0:
    %0 = i64 42
    %1 = i64 0
    %2 = shr i64 %0 %1
    ret %2
}
