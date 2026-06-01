; ModuleID = 'test/test_LoopFusion.ll'
source_filename = "test/test_LoopFusion.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [25 x i8] c"L0 guarded adjacent: %d\0A\00", align 1
@.str.1 = private unnamed_addr constant [25 x i8] c"L1 guarded adjacent: %d\0A\00", align 1
@.str.2 = private unnamed_addr constant [16 x i8] c"L0 guarded: %d\0A\00", align 1
@.str.3 = private unnamed_addr constant [24 x i8] c"statement between loops\00", align 1
@.str.4 = private unnamed_addr constant [20 x i8] c"L1 non guarded: %d\0A\00", align 1
@.str.5 = private unnamed_addr constant [29 x i8] c"L1 non guarded adjacent: %d\0A\00", align 1
@.str.6 = private unnamed_addr constant [29 x i8] c"L0 non guarded adjacent: %d\0A\00", align 1
@.str.7 = private unnamed_addr constant [20 x i8] c"L0 non guarded: %d\0A\00", align 1
@.str.8 = private unnamed_addr constant [16 x i8] c"L1 guarded: %d\0A\00", align 1
@.str.9 = private unnamed_addr constant [32 x i8] c"statement between guarded loops\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @both_guarded_adjacent(i32 noundef %0, i32 noundef %1) #0 {
  %3 = icmp sgt i32 %0, 0
  br i1 %3, label %4, label %12

4:                                                ; preds = %2
  br label %5

5:                                                ; preds = %9, %4
  %.01 = phi i32 [ 0, %4 ], [ %10, %9 ]
  %6 = icmp slt i32 %.01, 10
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
  %16 = icmp slt i32 %.0, 10
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

; Function Attrs: noinline nounwind uwtable
define dso_local void @l0_guarded_l1_not_guarded_not_adjacent(i32 noundef %0) #0 {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %3, label %11

3:                                                ; preds = %1
  br label %4

4:                                                ; preds = %8, %3
  %.01 = phi i32 [ 0, %3 ], [ %9, %8 ]
  %5 = icmp slt i32 %.01, 10
  br i1 %5, label %6, label %10

6:                                                ; preds = %4
  %7 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %.01)
  br label %8

8:                                                ; preds = %6
  %9 = add nsw i32 %.01, 1
  br label %4, !llvm.loop !9

10:                                               ; preds = %4
  br label %11

11:                                               ; preds = %10, %1
  %12 = call i32 @puts(ptr noundef @.str.3)
  br label %13

13:                                               ; preds = %17, %11
  %.0 = phi i32 [ 0, %11 ], [ %18, %17 ]
  %14 = icmp slt i32 %.0, 10
  br i1 %14, label %15, label %19

15:                                               ; preds = %13
  %16 = call i32 (ptr, ...) @printf(ptr noundef @.str.4, i32 noundef %.0)
  br label %17

17:                                               ; preds = %15
  %18 = add nsw i32 %.0, 1
  br label %13, !llvm.loop !10

19:                                               ; preds = %13
  ret void
}

declare i32 @puts(ptr noundef) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @l0_guarded_l1_not_guarded_adjacent(i32 noundef %0) #0 {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %3, label %11

3:                                                ; preds = %1
  br label %4

4:                                                ; preds = %8, %3
  %.01 = phi i32 [ 0, %3 ], [ %9, %8 ]
  %5 = icmp slt i32 %.01, 10
  br i1 %5, label %6, label %10

6:                                                ; preds = %4
  %7 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %.01)
  br label %8

8:                                                ; preds = %6
  %9 = add nsw i32 %.01, 1
  br label %4, !llvm.loop !11

10:                                               ; preds = %4
  br label %11

11:                                               ; preds = %10, %1
  br label %12

12:                                               ; preds = %16, %11
  %.0 = phi i32 [ 0, %11 ], [ %17, %16 ]
  %13 = icmp slt i32 %.0, 10
  br i1 %13, label %14, label %18

14:                                               ; preds = %12
  %15 = call i32 (ptr, ...) @printf(ptr noundef @.str.5, i32 noundef %.0)
  br label %16

16:                                               ; preds = %14
  %17 = add nsw i32 %.0, 1
  br label %12, !llvm.loop !12

18:                                               ; preds = %12
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @l0_not_guarded_l1_guarded_adjacent(i32 noundef %0) #0 {
  br label %2

2:                                                ; preds = %6, %1
  %.01 = phi i32 [ 0, %1 ], [ %7, %6 ]
  %3 = icmp slt i32 %.01, 10
  br i1 %3, label %4, label %8

4:                                                ; preds = %2
  %5 = call i32 (ptr, ...) @printf(ptr noundef @.str.6, i32 noundef %.01)
  br label %6

6:                                                ; preds = %4
  %7 = add nsw i32 %.01, 1
  br label %2, !llvm.loop !13

8:                                                ; preds = %2
  %9 = icmp sgt i32 %0, 0
  br i1 %9, label %10, label %18

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %15, %10
  %.0 = phi i32 [ 0, %10 ], [ %16, %15 ]
  %12 = icmp slt i32 %.0, 10
  br i1 %12, label %13, label %17

13:                                               ; preds = %11
  %14 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %.0)
  br label %15

15:                                               ; preds = %13
  %16 = add nsw i32 %.0, 1
  br label %11, !llvm.loop !14

17:                                               ; preds = %11
  br label %18

18:                                               ; preds = %17, %8
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @l0_not_guarded_l1_guarded_not_adjacent(i32 noundef %0) #0 {
  br label %2

2:                                                ; preds = %6, %1
  %.01 = phi i32 [ 0, %1 ], [ %7, %6 ]
  %3 = icmp slt i32 %.01, 10
  br i1 %3, label %4, label %8

4:                                                ; preds = %2
  %5 = call i32 (ptr, ...) @printf(ptr noundef @.str.7, i32 noundef %.01)
  br label %6

6:                                                ; preds = %4
  %7 = add nsw i32 %.01, 1
  br label %2, !llvm.loop !15

8:                                                ; preds = %2
  %9 = call i32 @puts(ptr noundef @.str.3)
  %10 = icmp sgt i32 %0, 0
  br i1 %10, label %11, label %19

11:                                               ; preds = %8
  br label %12

12:                                               ; preds = %16, %11
  %.0 = phi i32 [ 0, %11 ], [ %17, %16 ]
  %13 = icmp slt i32 %.0, 10
  br i1 %13, label %14, label %18

14:                                               ; preds = %12
  %15 = call i32 (ptr, ...) @printf(ptr noundef @.str.8, i32 noundef %.0)
  br label %16

16:                                               ; preds = %14
  %17 = add nsw i32 %.0, 1
  br label %12, !llvm.loop !16

18:                                               ; preds = %12
  br label %19

19:                                               ; preds = %18, %8
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @both_guarded_not_adjacent(i32 noundef %0, i32 noundef %1) #0 {
  %3 = icmp sgt i32 %0, 0
  br i1 %3, label %4, label %12

4:                                                ; preds = %2
  br label %5

5:                                                ; preds = %9, %4
  %.01 = phi i32 [ 0, %4 ], [ %10, %9 ]
  %6 = icmp slt i32 %.01, 10
  br i1 %6, label %7, label %11

7:                                                ; preds = %5
  %8 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %.01)
  br label %9

9:                                                ; preds = %7
  %10 = add nsw i32 %.01, 1
  br label %5, !llvm.loop !17

11:                                               ; preds = %5
  br label %12

12:                                               ; preds = %11, %2
  %13 = call i32 @puts(ptr noundef @.str.9)
  %14 = icmp sgt i32 %1, 0
  br i1 %14, label %15, label %23

15:                                               ; preds = %12
  br label %16

16:                                               ; preds = %20, %15
  %.0 = phi i32 [ 0, %15 ], [ %21, %20 ]
  %17 = icmp slt i32 %.0, 10
  br i1 %17, label %18, label %22

18:                                               ; preds = %16
  %19 = call i32 (ptr, ...) @printf(ptr noundef @.str.8, i32 noundef %.0)
  br label %20

20:                                               ; preds = %18
  %21 = add nsw i32 %.0, 1
  br label %16, !llvm.loop !18

22:                                               ; preds = %16
  br label %23

23:                                               ; preds = %22, %12
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @both_not_guarded_adjacent() #0 {
  br label %1

1:                                                ; preds = %5, %0
  %.0 = phi i32 [ 0, %0 ], [ %6, %5 ]
  %2 = icmp slt i32 %.0, 10
  br i1 %2, label %3, label %7

3:                                                ; preds = %1
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str.6, i32 noundef %.0)
  br label %5

5:                                                ; preds = %3
  %6 = add nsw i32 %.0, 1
  br label %1, !llvm.loop !19

7:                                                ; preds = %1
  br label %8

8:                                                ; preds = %12, %7
  %.01 = phi i32 [ 0, %7 ], [ %13, %12 ]
  %9 = icmp slt i32 %.01, 10
  br i1 %9, label %10, label %14

10:                                               ; preds = %8
  %11 = call i32 (ptr, ...) @printf(ptr noundef @.str.5, i32 noundef %.01)
  br label %12

12:                                               ; preds = %10
  %13 = add nsw i32 %.01, 1
  br label %8, !llvm.loop !20

14:                                               ; preds = %8
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @both_not_guarded_not_adjacent() #0 {
  br label %1

1:                                                ; preds = %5, %0
  %.0 = phi i32 [ 0, %0 ], [ %6, %5 ]
  %2 = icmp slt i32 %.0, 10
  br i1 %2, label %3, label %7

3:                                                ; preds = %1
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str.7, i32 noundef %.0)
  br label %5

5:                                                ; preds = %3
  %6 = add nsw i32 %.0, 1
  br label %1, !llvm.loop !21

7:                                                ; preds = %1
  %8 = call i32 @puts(ptr noundef @.str.3)
  br label %9

9:                                                ; preds = %13, %7
  %.01 = phi i32 [ 0, %7 ], [ %14, %13 ]
  %10 = icmp slt i32 %.01, 10
  br i1 %10, label %11, label %15

11:                                               ; preds = %9
  %12 = call i32 (ptr, ...) @printf(ptr noundef @.str.4, i32 noundef %.01)
  br label %13

13:                                               ; preds = %11
  %14 = add nsw i32 %.01, 1
  br label %9, !llvm.loop !22

15:                                               ; preds = %9
  ret void
}

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
!9 = distinct !{!9, !7}
!10 = distinct !{!10, !7}
!11 = distinct !{!11, !7}
!12 = distinct !{!12, !7}
!13 = distinct !{!13, !7}
!14 = distinct !{!14, !7}
!15 = distinct !{!15, !7}
!16 = distinct !{!16, !7}
!17 = distinct !{!17, !7}
!18 = distinct !{!18, !7}
!19 = distinct !{!19, !7}
!20 = distinct !{!20, !7}
!21 = distinct !{!21, !7}
!22 = distinct !{!22, !7}
