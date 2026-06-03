; ModuleID = 'test/test_LoopFusion.ll'
source_filename = "test/test_LoopFusion.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [25 x i8] c"L0 guarded adjacent: %d\0A\00", align 1
@.str.1 = private unnamed_addr constant [25 x i8] c"L1 guarded adjacent: %d\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @both_guarded_adjacent(i32 noundef %0, i32 noundef %1) #0 {
  %3 = icmp sgt i32 %0, 0
  br i1 %3, label %4, label %12

4:                                                ; preds = %2
  br label %5

5:                                                ; preds = %9, %4
  %.01 = phi i32 [ 0, %4 ], [ %10, %9 ]
  %6 = icmp slt i32 %.01, 11
  br i1 %6, label %7, label %11

7:                                                ; preds = %5
  %8 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %.01)
  br label %9

9:                                                ; preds = %7
  %10 = add nsw i32 %.01, 1
  br label %5, !llvm.loop !6

11:                                               ; preds = %5
  br label %12

12:                                               ; preds = %11, %2
  %13 = icmp sgt i32 %1, 0
  br i1 %13, label %14, label %22

14:                                               ; preds = %12
  br label %15

15:                                               ; preds = %19, %14
  %.0 = phi i32 [ 0, %14 ], [ %20, %19 ]
  %16 = icmp slt i32 %.0, 21
  br i1 %16, label %17, label %21

17:                                               ; preds = %15
  %18 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %.0)
  br label %19

19:                                               ; preds = %17
  %20 = add nsw i32 %.0, 1
  br label %15, !llvm.loop !8

21:                                               ; preds = %15
  br label %22

22:                                               ; preds = %21, %12
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
