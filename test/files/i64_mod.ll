define i64 @func() {
0:
    %0 = i64 42
    %1 = i64 3
    %2 = mod i64 %0 %1
    ret %2
}
