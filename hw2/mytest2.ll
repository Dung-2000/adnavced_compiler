; ModuleID = 'mytest2.c'
source_filename = "mytest2.c"
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
  store ptr %x, ptr %p, align 8
  %0 = load ptr, ptr %pp, align 8
  %1 = load ptr, ptr %0, align 8
  store ptr %1, ptr %p, align 8
  %2 = load ptr, ptr %p2, align 8
  store ptr %2, ptr %p, align 8
  %3 = load ptr, ptr %pp, align 8
  store ptr %x, ptr %3, align 8
  %4 = load ptr, ptr %p, align 8
  %5 = load ptr, ptr %pp, align 8
  store ptr %4, ptr %5, align 8
  %6 = load ptr, ptr %pp2, align 8
  %7 = load ptr, ptr %6, align 8
  %8 = load ptr, ptr %pp, align 8
  store ptr %7, ptr %8, align 8
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