; ModuleID = 'test/test_LoopFusion.ll'
source_filename = "test/test_LoopFusion.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [15 x i8] c"Between loops\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @fusible_test(i32 noundef %0, i32 noundef %1) #0 {
  %3 = icmp slt i32 %0, %1
  br i1 %3, label %4, label %22

4:                                                ; preds = %2
  br label %5

5:                                                ; preds = %10, %4
  %.01 = phi i32 [ %0, %4 ], [ %11, %10 ]
  %6 = icmp slt i32 %.01, %1
  br i1 %6, label %7, label %12

7:                                                ; preds = %5
  %8 = sext i32 %.01 to i64
  %9 = getelementptr inbounds i32, ptr undef, i64 %8
  store i32 %.01, ptr %9, align 4
  br label %10

10:                                               ; preds = %7
  %11 = add nsw i32 %.01, 1
  br label %5, !llvm.loop !6

12:                                               ; preds = %5
  br label %13

13:                                               ; preds = %19, %12
  %.0 = phi i32 [ %0, %12 ], [ %20, %19 ]
  %14 = icmp slt i32 %.0, %1
  br i1 %14, label %15, label %21

15:                                               ; preds = %13
  %16 = add nsw i32 %.0, 1
  %17 = sext i32 %.0 to i64
  %18 = getelementptr inbounds i32, ptr undef, i64 %17
  store i32 %16, ptr %18, align 4
  br label %19

19:                                               ; preds = %15
  %20 = add nsw i32 %.0, 1
  br label %13, !llvm.loop !8

21:                                               ; preds = %13
  br label %22

22:                                               ; preds = %21, %2
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @negative_dep_test(i32 noundef %0) #0 {
  %2 = alloca [11 x i32], align 16
  %3 = alloca [10 x i32], align 16
  %4 = icmp sgt i32 %0, 10
  br i1 %4, label %5, label %27

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %11, %5
  %.01 = phi i32 [ 0, %5 ], [ %12, %11 ]
  %7 = icmp slt i32 %.01, 10
  br i1 %7, label %8, label %13

8:                                                ; preds = %6
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds [11 x i32], ptr %2, i64 0, i64 %9
  store i32 %.01, ptr %10, align 4
  br label %11

11:                                               ; preds = %8
  %12 = add nsw i32 %.01, 1
  br label %6, !llvm.loop !9

13:                                               ; preds = %6
  br label %14

14:                                               ; preds = %24, %13
  %.0 = phi i32 [ 0, %13 ], [ %25, %24 ]
  %15 = icmp slt i32 %.0, 10
  br i1 %15, label %16, label %26

16:                                               ; preds = %14
  %17 = add nsw i32 %.0, 1
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds [11 x i32], ptr %2, i64 0, i64 %18
  %20 = load i32, ptr %19, align 4
  %21 = add nsw i32 %20, 1
  %22 = sext i32 %.0 to i64
  %23 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %22
  store i32 %21, ptr %23, align 4
  br label %24

24:                                               ; preds = %16
  %25 = add nsw i32 %.0, 1
  br label %14, !llvm.loop !10

26:                                               ; preds = %14
  br label %27

27:                                               ; preds = %26, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @zero_distance_dep_test(i32 noundef %0) #0 {
  %2 = alloca [10 x i32], align 16
  %3 = alloca [10 x i32], align 16
  %4 = icmp sgt i32 %0, 10
  br i1 %4, label %5, label %26

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %11, %5
  %.01 = phi i32 [ 0, %5 ], [ %12, %11 ]
  %7 = icmp slt i32 %.01, 10
  br i1 %7, label %8, label %13

8:                                                ; preds = %6
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %9
  store i32 %.01, ptr %10, align 4
  br label %11

11:                                               ; preds = %8
  %12 = add nsw i32 %.01, 1
  br label %6, !llvm.loop !11

13:                                               ; preds = %6
  br label %14

14:                                               ; preds = %23, %13
  %.0 = phi i32 [ 0, %13 ], [ %24, %23 ]
  %15 = icmp slt i32 %.0, 10
  br i1 %15, label %16, label %25

16:                                               ; preds = %14
  %17 = sext i32 %.0 to i64
  %18 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %17
  %19 = load i32, ptr %18, align 4
  %20 = add nsw i32 %19, 1
  %21 = sext i32 %.0 to i64
  %22 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %21
  store i32 %20, ptr %22, align 4
  br label %23

23:                                               ; preds = %16
  %24 = add nsw i32 %.0, 1
  br label %14, !llvm.loop !12

25:                                               ; preds = %14
  br label %26

26:                                               ; preds = %25, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @not_adjacent_test(i32 noundef %0) #0 {
  %2 = alloca [10 x i32], align 16
  %3 = alloca [10 x i32], align 16
  %4 = icmp sgt i32 %0, 10
  br i1 %4, label %5, label %24

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %11, %5
  %.01 = phi i32 [ 0, %5 ], [ %12, %11 ]
  %7 = icmp slt i32 %.01, 10
  br i1 %7, label %8, label %13

8:                                                ; preds = %6
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %9
  store i32 %.01, ptr %10, align 4
  br label %11

11:                                               ; preds = %8
  %12 = add nsw i32 %.01, 1
  br label %6, !llvm.loop !13

13:                                               ; preds = %6
  %14 = call i32 (ptr, ...) @printf(ptr noundef @.str)
  br label %15

15:                                               ; preds = %21, %13
  %.0 = phi i32 [ 0, %13 ], [ %22, %21 ]
  %16 = icmp slt i32 %.0, 10
  br i1 %16, label %17, label %23

17:                                               ; preds = %15
  %18 = add nsw i32 %.0, 1
  %19 = sext i32 %.0 to i64
  %20 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %19
  store i32 %18, ptr %20, align 4
  br label %21

21:                                               ; preds = %17
  %22 = add nsw i32 %.0, 1
  br label %15, !llvm.loop !14

23:                                               ; preds = %15
  br label %24

24:                                               ; preds = %23, %1
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @different_trip_count_test(i32 noundef %0) #0 {
  %2 = alloca [10 x i32], align 16
  %3 = alloca [10 x i32], align 16
  %4 = icmp sgt i32 %0, 10
  br i1 %4, label %5, label %23

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %11, %5
  %.01 = phi i32 [ 0, %5 ], [ %12, %11 ]
  %7 = icmp slt i32 %.01, 10
  br i1 %7, label %8, label %13

8:                                                ; preds = %6
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %9
  store i32 %.01, ptr %10, align 4
  br label %11

11:                                               ; preds = %8
  %12 = add nsw i32 %.01, 1
  br label %6, !llvm.loop !15

13:                                               ; preds = %6
  br label %14

14:                                               ; preds = %20, %13
  %.0 = phi i32 [ 0, %13 ], [ %21, %20 ]
  %15 = icmp slt i32 %.0, 8
  br i1 %15, label %16, label %22

16:                                               ; preds = %14
  %17 = add nsw i32 %.0, 1
  %18 = sext i32 %.0 to i64
  %19 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %18
  store i32 %17, ptr %19, align 4
  br label %20

20:                                               ; preds = %16
  %21 = add nsw i32 %.0, 1
  br label %14, !llvm.loop !16

22:                                               ; preds = %14
  br label %23

23:                                               ; preds = %22, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @not_control_flow_equivalent_test(i32 noundef %0) #0 {
  %2 = alloca [10 x i32], align 16
  %3 = alloca [10 x i32], align 16
  %4 = icmp sgt i32 %0, 10
  br i1 %4, label %5, label %14

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %11, %5
  %.01 = phi i32 [ 0, %5 ], [ %12, %11 ]
  %7 = icmp slt i32 %.01, 10
  br i1 %7, label %8, label %13

8:                                                ; preds = %6
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %9
  store i32 %.01, ptr %10, align 4
  br label %11

11:                                               ; preds = %8
  %12 = add nsw i32 %.01, 1
  br label %6, !llvm.loop !17

13:                                               ; preds = %6
  br label %14

14:                                               ; preds = %13, %1
  br label %15

15:                                               ; preds = %21, %14
  %.0 = phi i32 [ 0, %14 ], [ %22, %21 ]
  %16 = icmp slt i32 %.0, 10
  br i1 %16, label %17, label %23

17:                                               ; preds = %15
  %18 = add nsw i32 %.0, 1
  %19 = sext i32 %.0 to i64
  %20 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %19
  store i32 %18, ptr %20, align 4
  br label %21

21:                                               ; preds = %17
  %22 = add nsw i32 %.0, 1
  br label %15, !llvm.loop !18

23:                                               ; preds = %15
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @two_separate_guards_same_condition_test(i32 noundef %0) #0 {
  %2 = alloca [10 x i32], align 16
  %3 = alloca [10 x i32], align 16
  %4 = icmp sgt i32 %0, 10
  br i1 %4, label %5, label %14

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %11, %5
  %.01 = phi i32 [ 0, %5 ], [ %12, %11 ]
  %7 = icmp slt i32 %.01, 10
  br i1 %7, label %8, label %13

8:                                                ; preds = %6
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %9
  store i32 %.01, ptr %10, align 4
  br label %11

11:                                               ; preds = %8
  %12 = add nsw i32 %.01, 1
  br label %6, !llvm.loop !19

13:                                               ; preds = %6
  br label %14

14:                                               ; preds = %13, %1
  %15 = icmp sgt i32 %0, 10
  br i1 %15, label %16, label %26

16:                                               ; preds = %14
  br label %17

17:                                               ; preds = %23, %16
  %.0 = phi i32 [ 0, %16 ], [ %24, %23 ]
  %18 = icmp slt i32 %.0, 10
  br i1 %18, label %19, label %25

19:                                               ; preds = %17
  %20 = add nsw i32 %.0, 1
  %21 = sext i32 %.0 to i64
  %22 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %21
  store i32 %20, ptr %22, align 4
  br label %23

23:                                               ; preds = %19
  %24 = add nsw i32 %.0, 1
  br label %17, !llvm.loop !20

25:                                               ; preds = %17
  br label %26

26:                                               ; preds = %25, %14
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @two_separate_guards_different_conditions_test(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca [10 x i32], align 16
  %4 = alloca [10 x i32], align 16
  %5 = icmp sgt i32 %0, 10
  br i1 %5, label %6, label %15

6:                                                ; preds = %2
  br label %7

7:                                                ; preds = %12, %6
  %.01 = phi i32 [ 0, %6 ], [ %13, %12 ]
  %8 = icmp slt i32 %.01, 10
  br i1 %8, label %9, label %14

9:                                                ; preds = %7
  %10 = sext i32 %.01 to i64
  %11 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %10
  store i32 %.01, ptr %11, align 4
  br label %12

12:                                               ; preds = %9
  %13 = add nsw i32 %.01, 1
  br label %7, !llvm.loop !21

14:                                               ; preds = %7
  br label %15

15:                                               ; preds = %14, %2
  %16 = icmp sgt i32 %1, 10
  br i1 %16, label %17, label %27

17:                                               ; preds = %15
  br label %18

18:                                               ; preds = %24, %17
  %.0 = phi i32 [ 0, %17 ], [ %25, %24 ]
  %19 = icmp slt i32 %.0, 10
  br i1 %19, label %20, label %26

20:                                               ; preds = %18
  %21 = add nsw i32 %.0, 1
  %22 = sext i32 %.0 to i64
  %23 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 %22
  store i32 %21, ptr %23, align 4
  br label %24

24:                                               ; preds = %20
  %25 = add nsw i32 %.0, 1
  br label %18, !llvm.loop !22

26:                                               ; preds = %18
  br label %27

27:                                               ; preds = %26, %15
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @backward_read_not_negative_test(i32 noundef %0) #0 {
  %2 = alloca [10 x i32], align 16
  %3 = alloca [10 x i32], align 16
  %4 = icmp sgt i32 %0, 10
  br i1 %4, label %5, label %26

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %11, %5
  %.01 = phi i32 [ 0, %5 ], [ %12, %11 ]
  %7 = icmp slt i32 %.01, 9
  br i1 %7, label %8, label %13

8:                                                ; preds = %6
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %9
  store i32 %.01, ptr %10, align 4
  br label %11

11:                                               ; preds = %8
  %12 = add nsw i32 %.01, 1
  br label %6, !llvm.loop !23

13:                                               ; preds = %6
  br label %14

14:                                               ; preds = %23, %13
  %.0 = phi i32 [ 0, %13 ], [ %24, %23 ]
  %15 = icmp slt i32 %.0, 9
  br i1 %15, label %16, label %25

16:                                               ; preds = %14
  %17 = sext i32 %.0 to i64
  %18 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %17
  %19 = load i32, ptr %18, align 4
  %20 = add nsw i32 %19, 1
  %21 = sext i32 %.0 to i64
  %22 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %21
  store i32 %20, ptr %22, align 4
  br label %23

23:                                               ; preds = %16
  %24 = add nsw i32 %.0, 1
  br label %14, !llvm.loop !24

25:                                               ; preds = %14
  br label %26

26:                                               ; preds = %25, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @negative_dep_plus_three_test(i32 noundef %0) #0 {
  %2 = alloca [13 x i32], align 16
  %3 = alloca [10 x i32], align 16
  %4 = icmp sgt i32 %0, 10
  br i1 %4, label %5, label %27

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %11, %5
  %.01 = phi i32 [ 0, %5 ], [ %12, %11 ]
  %7 = icmp slt i32 %.01, 10
  br i1 %7, label %8, label %13

8:                                                ; preds = %6
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds [13 x i32], ptr %2, i64 0, i64 %9
  store i32 %.01, ptr %10, align 4
  br label %11

11:                                               ; preds = %8
  %12 = add nsw i32 %.01, 1
  br label %6, !llvm.loop !25

13:                                               ; preds = %6
  br label %14

14:                                               ; preds = %24, %13
  %.0 = phi i32 [ 0, %13 ], [ %25, %24 ]
  %15 = icmp slt i32 %.0, 10
  br i1 %15, label %16, label %26

16:                                               ; preds = %14
  %17 = add nsw i32 %.0, 3
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds [13 x i32], ptr %2, i64 0, i64 %18
  %20 = load i32, ptr %19, align 4
  %21 = add nsw i32 %20, 1
  %22 = sext i32 %.0 to i64
  %23 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %22
  store i32 %21, ptr %23, align 4
  br label %24

24:                                               ; preds = %16
  %25 = add nsw i32 %.0, 1
  br label %14, !llvm.loop !26

26:                                               ; preds = %14
  br label %27

27:                                               ; preds = %26, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @pointer_may_alias_unknown_test(ptr noundef %0, ptr noundef %1, i32 noundef %2) #0 {
  %4 = icmp sgt i32 %2, 10
  br i1 %4, label %5, label %26

5:                                                ; preds = %3
  br label %6

6:                                                ; preds = %11, %5
  %.01 = phi i32 [ 0, %5 ], [ %12, %11 ]
  %7 = icmp slt i32 %.01, 10
  br i1 %7, label %8, label %13

8:                                                ; preds = %6
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds i32, ptr %0, i64 %9
  store i32 %.01, ptr %10, align 4
  br label %11

11:                                               ; preds = %8
  %12 = add nsw i32 %.01, 1
  br label %6, !llvm.loop !27

13:                                               ; preds = %6
  br label %14

14:                                               ; preds = %23, %13
  %.0 = phi i32 [ 0, %13 ], [ %24, %23 ]
  %15 = icmp slt i32 %.0, 10
  br i1 %15, label %16, label %25

16:                                               ; preds = %14
  %17 = sext i32 %.0 to i64
  %18 = getelementptr inbounds i32, ptr %1, i64 %17
  %19 = load i32, ptr %18, align 4
  %20 = add nsw i32 %19, 1
  %21 = sext i32 %.0 to i64
  %22 = getelementptr inbounds i32, ptr %1, i64 %21
  store i32 %20, ptr %22, align 4
  br label %23

23:                                               ; preds = %16
  %24 = add nsw i32 %.0, 1
  br label %14, !llvm.loop !28

25:                                               ; preds = %14
  br label %26

26:                                               ; preds = %25, %3
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @non_guarded_fusible_test() #0 {
  %1 = alloca [10 x i32], align 16
  %2 = alloca [10 x i32], align 16
  br label %3

3:                                                ; preds = %8, %0
  %.0 = phi i32 [ 0, %0 ], [ %9, %8 ]
  %4 = icmp slt i32 %.0, 10
  br i1 %4, label %5, label %10

5:                                                ; preds = %3
  %6 = sext i32 %.0 to i64
  %7 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %6
  store i32 %.0, ptr %7, align 4
  br label %8

8:                                                ; preds = %5
  %9 = add nsw i32 %.0, 1
  br label %3, !llvm.loop !29

10:                                               ; preds = %3
  br label %11

11:                                               ; preds = %17, %10
  %.01 = phi i32 [ 0, %10 ], [ %18, %17 ]
  %12 = icmp slt i32 %.01, 10
  br i1 %12, label %13, label %19

13:                                               ; preds = %11
  %14 = add nsw i32 %.01, 1
  %15 = sext i32 %.01 to i64
  %16 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %15
  store i32 %14, ptr %16, align 4
  br label %17

17:                                               ; preds = %13
  %18 = add nsw i32 %.01, 1
  br label %11, !llvm.loop !30

19:                                               ; preds = %11
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @non_guarded_not_adjacent_test() #0 {
  %1 = alloca [10 x i32], align 16
  %2 = alloca [10 x i32], align 16
  br label %3

3:                                                ; preds = %8, %0
  %.0 = phi i32 [ 0, %0 ], [ %9, %8 ]
  %4 = icmp slt i32 %.0, 10
  br i1 %4, label %5, label %10

5:                                                ; preds = %3
  %6 = sext i32 %.0 to i64
  %7 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %6
  store i32 %.0, ptr %7, align 4
  br label %8

8:                                                ; preds = %5
  %9 = add nsw i32 %.0, 1
  br label %3, !llvm.loop !31

10:                                               ; preds = %3
  %11 = call i32 (ptr, ...) @printf(ptr noundef @.str)
  br label %12

12:                                               ; preds = %18, %10
  %.01 = phi i32 [ 0, %10 ], [ %19, %18 ]
  %13 = icmp slt i32 %.01, 10
  br i1 %13, label %14, label %20

14:                                               ; preds = %12
  %15 = add nsw i32 %.01, 1
  %16 = sext i32 %.01 to i64
  %17 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %16
  store i32 %15, ptr %17, align 4
  br label %18

18:                                               ; preds = %14
  %19 = add nsw i32 %.01, 1
  br label %12, !llvm.loop !32

20:                                               ; preds = %12
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
!23 = distinct !{!23, !7}
!24 = distinct !{!24, !7}
!25 = distinct !{!25, !7}
!26 = distinct !{!26, !7}
!27 = distinct !{!27, !7}
!28 = distinct !{!28, !7}
!29 = distinct !{!29, !7}
!30 = distinct !{!30, !7}
!31 = distinct !{!31, !7}
!32 = distinct !{!32, !7}
