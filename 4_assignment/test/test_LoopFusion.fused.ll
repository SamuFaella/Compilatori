; ModuleID = 'test/test_LoopFusion.m2r.ll'
source_filename = "test/test_LoopFusion.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @negative_symbolic_start(ptr noundef %0, ptr noundef %1, i32 noundef %2, i32 noundef %3) #0 {
  %5 = icmp slt i32 %2, %3
  br i1 %5, label %6, label %28

6:                                                ; preds = %4
  br label %7

7:                                                ; preds = %12, %6
  %.01 = phi i32 [ %2, %6 ], [ %13, %12 ]
  %8 = icmp slt i32 %.01, %3
  br i1 %8, label %9, label %14

9:                                                ; preds = %7
  %10 = sext i32 %.01 to i64
  %11 = getelementptr inbounds i32, ptr %0, i64 %10
  store i32 %.01, ptr %11, align 4
  br label %12

12:                                               ; preds = %9
  %13 = add nsw i32 %.01, 1
  br label %7, !llvm.loop !6

14:                                               ; preds = %7
  br label %15

15:                                               ; preds = %25, %14
  %.0 = phi i32 [ %2, %14 ], [ %26, %25 ]
  %16 = icmp slt i32 %.0, %3
  br i1 %16, label %17, label %27

17:                                               ; preds = %15
  %18 = add nsw i32 %.0, 1
  %19 = sext i32 %18 to i64
  %20 = getelementptr inbounds i32, ptr %0, i64 %19
  %21 = load i32, ptr %20, align 4
  %22 = add nsw i32 %21, 1
  %23 = sext i32 %.0 to i64
  %24 = getelementptr inbounds i32, ptr %1, i64 %23
  store i32 %22, ptr %24, align 4
  br label %25

25:                                               ; preds = %17
  %26 = add nsw i32 %.0, 1
  br label %15, !llvm.loop !8

27:                                               ; preds = %15
  br label %28

28:                                               ; preds = %27, %4
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @fusible_symbolic_start(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca [100 x i32], align 16
  %4 = alloca [100 x i32], align 16
  %5 = icmp slt i32 %0, %1
  br i1 %5, label %6, label %22

6:                                                ; preds = %2
  br label %7

7:                                                ; preds = %12, %6
  %.01 = phi i32 [ %0, %6 ], [ %13, %12 ]
  %8 = icmp slt i32 %.01, %1
  br i1 %8, label %9, label %21

9:                                                ; preds = %7
  %10 = sext i32 %.01 to i64
  %11 = getelementptr inbounds [100 x i32], ptr %3, i64 0, i64 %10
  store i32 %.01, ptr %11, align 4
  br label %14

12:                                               ; preds = %14
  %13 = add nsw i32 %.01, 1
  br label %7, !llvm.loop !9

14:                                               ; preds = %9
  %15 = sext i32 %.01 to i64
  %16 = getelementptr inbounds [100 x i32], ptr %3, i64 0, i64 %15
  %17 = load i32, ptr %16, align 4
  %18 = add nsw i32 %17, 1
  %19 = sext i32 %.01 to i64
  %20 = getelementptr inbounds [100 x i32], ptr %4, i64 0, i64 %19
  store i32 %18, ptr %20, align 4
  br label %12

21:                                               ; preds = %7
  br label %22

22:                                               ; preds = %21, %2
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @negative_decreasing(i32 noundef %0) #0 {
  %2 = alloca [11 x i32], align 16
  %3 = alloca [11 x i32], align 16
  %4 = icmp sgt i32 %0, 10
  br i1 %4, label %5, label %27

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %11, %5
  %.01 = phi i32 [ 10, %5 ], [ %12, %11 ]
  %7 = icmp sgt i32 %.01, 0
  br i1 %7, label %8, label %13

8:                                                ; preds = %6
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds [11 x i32], ptr %2, i64 0, i64 %9
  store i32 %.01, ptr %10, align 4
  br label %11

11:                                               ; preds = %8
  %12 = add nsw i32 %.01, -1
  br label %6, !llvm.loop !10

13:                                               ; preds = %6
  br label %14

14:                                               ; preds = %24, %13
  %.0 = phi i32 [ 10, %13 ], [ %25, %24 ]
  %15 = icmp sgt i32 %.0, 0
  br i1 %15, label %16, label %26

16:                                               ; preds = %14
  %17 = sub nsw i32 %.0, 1
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds [11 x i32], ptr %2, i64 0, i64 %18
  %20 = load i32, ptr %19, align 4
  %21 = add nsw i32 %20, 1
  %22 = sext i32 %.0 to i64
  %23 = getelementptr inbounds [11 x i32], ptr %3, i64 0, i64 %22
  store i32 %21, ptr %23, align 4
  br label %24

24:                                               ; preds = %16
  %25 = add nsw i32 %.0, -1
  br label %14, !llvm.loop !11

26:                                               ; preds = %14
  br label %27

27:                                               ; preds = %26, %1
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
!5 = !{!"clang version 19.1.7 (/home/runner/work/llvm-project/llvm-project/clang cd708029e0b2869e80abe31ddb175f7c35361f90)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = distinct !{!8, !7}
!9 = distinct !{!9, !7}
!10 = distinct !{!10, !7}
!11 = distinct !{!11, !7}
