; ModuleID = 'mytest1.c'
source_filename = "mytest1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @foo() #0 {
entry:
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %p = alloca ptr, align 8
  %p2 = alloca ptr, align 8
  %pp = alloca ptr, align 8
  %pp2 = alloca ptr, align 8
  %0 = load i32, ptr %y, align 4
  store i32 %0, ptr %x, align 4
  %1 = load ptr, ptr %p, align 8
  %2 = load i32, ptr %1, align 4
  store i32 %2, ptr %x, align 4
  %3 = load ptr, ptr %pp, align 8
  %4 = load ptr, ptr %3, align 8
  %5 = load i32, ptr %4, align 4
  store i32 %5, ptr %x, align 4
  %6 = load ptr, ptr %p2, align 8
  %7 = load i32, ptr %6, align 4
  %8 = load ptr, ptr %p, align 8
  store i32 %7, ptr %8, align 4
  %9 = load i32, ptr %x, align 4
  %10 = load ptr, ptr %p, align 8
  store i32 %9, ptr %10, align 4
  %11 = load ptr, ptr %pp, align 8
  %12 = load ptr, ptr %11, align 8
  %13 = load i32, ptr %12, align 4
  %14 = load ptr, ptr %p, align 8
  store i32 %13, ptr %14, align 4
  %15 = load ptr, ptr %pp2, align 8
  %16 = load ptr, ptr %15, align 8
  %17 = load i32, ptr %16, align 4
  %18 = load ptr, ptr %pp, align 8
  %19 = load ptr, ptr %18, align 8
  store i32 %17, ptr %19, align 4
  %20 = load ptr, ptr %p, align 8
  %21 = load i32, ptr %20, align 4
  %22 = load ptr, ptr %pp, align 8
  %23 = load ptr, ptr %22, align 8
  store i32 %21, ptr %23, align 4
  %24 = load i32, ptr %x, align 4
  %25 = load ptr, ptr %pp, align 8
  %26 = load ptr, ptr %25, align 8
  store i32 %24, ptr %26, align 4
  ret void
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 17.0.4 (++20231025124049+afbe3549af4d-1~exp1~20231025124151.58)"}
