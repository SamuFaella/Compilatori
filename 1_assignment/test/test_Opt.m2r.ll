; ModuleID = 'test_Opt.ll'
source_filename = "test_Opt.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
entry:
  %add = add nsw i32 42, 0
  %mul = mul nsw i32 1, 42
  %mul1 = mul nsw i32 42, 16
  %mul2 = mul nsw i32 42, 15
  %div = sdiv i32 42, 8
  %add3 = add nsw i32 42, 1
  %sub = sub nsw i32 %add3, 1
  %add4 = add nsw i32 %add, %mul
  %add5 = add nsw i32 %add4, %mul1
  %add6 = add nsw i32 %add5, %div
  %add7 = add nsw i32 %add6, %sub
  %add8 = add nsw i32 %add7, %mul2
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
