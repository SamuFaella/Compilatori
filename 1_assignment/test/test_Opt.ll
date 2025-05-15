; ModuleID = 'test_Opt.c'
source_filename = "test_Opt.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %z = alloca i32, align 4
  %a = alloca i32, align 4
  %e = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  store i32 42, ptr %x, align 4
  %0 = load i32, ptr %x, align 4
  %add = add nsw i32 %0, 0
  store i32 %add, ptr %y, align 4
  %1 = load i32, ptr %x, align 4
  %mul = mul nsw i32 1, %1
  store i32 %mul, ptr %z, align 4
  %2 = load i32, ptr %x, align 4
  %mul1 = mul nsw i32 %2, 16
  store i32 %mul1, ptr %a, align 4
  %3 = load i32, ptr %x, align 4
  %mul2 = mul nsw i32 %3, 15
  store i32 %mul2, ptr %e, align 4
  %4 = load i32, ptr %x, align 4
  %div = sdiv i32 %4, 8
  store i32 %div, ptr %b, align 4
  %5 = load i32, ptr %x, align 4
  %add3 = add nsw i32 %5, 1
  store i32 %add3, ptr %c, align 4
  %6 = load i32, ptr %c, align 4
  %sub = sub nsw i32 %6, 1
  store i32 %sub, ptr %d, align 4
  %7 = load i32, ptr %y, align 4
  %8 = load i32, ptr %z, align 4
  %add4 = add nsw i32 %7, %8
  %9 = load i32, ptr %a, align 4
  %add5 = add nsw i32 %add4, %9
  %10 = load i32, ptr %b, align 4
  %add6 = add nsw i32 %add5, %10
  %11 = load i32, ptr %d, align 4
  %add7 = add nsw i32 %add6, %11
  %12 = load i32, ptr %e, align 4
  %add8 = add nsw i32 %add7, %12
  ret i32 %add8
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 19.1.7 (/home/runner/work/llvm-project/llvm-project/clang cd708029e0b2869e80abe31ddb175f7c35361f90)"}
