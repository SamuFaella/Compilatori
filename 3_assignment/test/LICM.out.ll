; ModuleID = 'test/LICM.m2r.ll'
source_filename = "test/LICM.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local double @complex_test(i32 noundef %0, double noundef %1, double noundef %2, ptr noundef %3) #0 {
  %5 = fmul double %1, 2.000000e+00
  %6 = fadd double %2, 3.000000e+00
  %7 = fadd double %5, %6
  br label %8

8:                                                ; preds = %22, %4
  %.02 = phi double [ 0.000000e+00, %4 ], [ %21, %22 ]
  %.01 = phi double [ 1.000000e+00, %4 ], [ %7, %22 ]
  %.0 = phi i32 [ 0, %4 ], [ %23, %22 ]
  %9 = icmp slt i32 %.0, %0
  br i1 %9, label %10, label %24

10:                                               ; preds = %8
  %11 = sitofp i32 %.0 to double
  %12 = fmul double %11, 4.000000e+00
  %13 = sitofp i32 %.0 to double
  %14 = fmul double %7, %13
  %15 = sext i32 %.0 to i64
  %16 = getelementptr inbounds double, ptr %3, i64 %15
  store double %14, ptr %16, align 8
  %17 = sext i32 %.0 to i64
  %18 = getelementptr inbounds double, ptr %3, i64 %17
  %19 = load double, ptr %18, align 8
  %20 = fadd double %19, %7
  %21 = fadd double %.02, %20
  br label %22

22:                                               ; preds = %10
  %23 = add nsw i32 %.0, 1
  br label %8, !llvm.loop !6

24:                                               ; preds = %8
  %25 = fadd double %.02, %.01
  ret double %25
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
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
