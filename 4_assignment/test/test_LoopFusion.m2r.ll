; ModuleID = 'test/test_LoopFusion.ll'
source_filename = "test/test_LoopFusion.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [16 x i8] c"Loop 1: i = %d\0A\00", align 1
@.str.1 = private unnamed_addr constant [16 x i8] c"Loop 2: j = %d\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @test(i32 noundef %0, i32 noundef %1) #0 {
  br label %3

3:                                                ; preds = %7, %2
  %.01 = phi i32 [ 0, %2 ], [ %8, %7 ]
  %4 = icmp slt i32 %.01, 10
  br i1 %4, label %5, label %9

5:                                                ; preds = %3
  %6 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %.01)
  br label %7

7:                                                ; preds = %5
  %8 = add nsw i32 %.01, 1
  br label %3, !llvm.loop !6

9:                                                ; preds = %3
  br label %10

10:                                               ; preds = %14, %9
  %.0 = phi i32 [ 0, %9 ], [ %15, %14 ]
  %11 = icmp slt i32 %.0, 10
  br i1 %11, label %12, label %16

12:                                               ; preds = %10
  %13 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %.0)
  br label %14

14:                                               ; preds = %12
  %15 = add nsw i32 %.0, 1
  br label %10, !llvm.loop !8

16:                                               ; preds = %10
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

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
!8 = distinct !{!8, !7}
