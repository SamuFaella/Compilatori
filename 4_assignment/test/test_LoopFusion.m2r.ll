; ModuleID = 'test/test_LoopFusion.ll'
source_filename = "test/test_LoopFusion.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [9 x i8] c"between\0A\00", align 1
@.str.1 = private unnamed_addr constant [15 x i8] c"between nests\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @simple_fusible_local() #0 {
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
  br label %3, !llvm.loop !6

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
  br label %11, !llvm.loop !8

19:                                               ; preds = %11
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @zero_distance_local() #0 {
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
  br label %3, !llvm.loop !9

10:                                               ; preds = %3
  br label %11

11:                                               ; preds = %20, %10
  %.01 = phi i32 [ 0, %10 ], [ %21, %20 ]
  %12 = icmp slt i32 %.01, 10
  br i1 %12, label %13, label %22

13:                                               ; preds = %11
  %14 = sext i32 %.01 to i64
  %15 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %14
  %16 = load i32, ptr %15, align 4
  %17 = add nsw i32 %16, 1
  %18 = sext i32 %.01 to i64
  %19 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %18
  store i32 %17, ptr %19, align 4
  br label %20

20:                                               ; preds = %13
  %21 = add nsw i32 %.01, 1
  br label %11, !llvm.loop !10

22:                                               ; preds = %11
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @negative_offset_plus_one() #0 {
  %1 = alloca [11 x i32], align 16
  %2 = alloca [10 x i32], align 16
  br label %3

3:                                                ; preds = %8, %0
  %.0 = phi i32 [ 0, %0 ], [ %9, %8 ]
  %4 = icmp slt i32 %.0, 10
  br i1 %4, label %5, label %10

5:                                                ; preds = %3
  %6 = sext i32 %.0 to i64
  %7 = getelementptr inbounds [11 x i32], ptr %1, i64 0, i64 %6
  store i32 %.0, ptr %7, align 4
  br label %8

8:                                                ; preds = %5
  %9 = add nsw i32 %.0, 1
  br label %3, !llvm.loop !11

10:                                               ; preds = %3
  br label %11

11:                                               ; preds = %21, %10
  %.01 = phi i32 [ 0, %10 ], [ %22, %21 ]
  %12 = icmp slt i32 %.01, 10
  br i1 %12, label %13, label %23

13:                                               ; preds = %11
  %14 = add nsw i32 %.01, 1
  %15 = sext i32 %14 to i64
  %16 = getelementptr inbounds [11 x i32], ptr %1, i64 0, i64 %15
  %17 = load i32, ptr %16, align 4
  %18 = add nsw i32 %17, 1
  %19 = sext i32 %.01 to i64
  %20 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %19
  store i32 %18, ptr %20, align 4
  br label %21

21:                                               ; preds = %13
  %22 = add nsw i32 %.01, 1
  br label %11, !llvm.loop !12

23:                                               ; preds = %11
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @different_trip_count_simple() #0 {
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
  br label %3, !llvm.loop !13

10:                                               ; preds = %3
  br label %11

11:                                               ; preds = %17, %10
  %.01 = phi i32 [ 0, %10 ], [ %18, %17 ]
  %12 = icmp slt i32 %.01, 8
  br i1 %12, label %13, label %19

13:                                               ; preds = %11
  %14 = add nsw i32 %.01, 1
  %15 = sext i32 %.01 to i64
  %16 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %15
  store i32 %14, ptr %16, align 4
  br label %17

17:                                               ; preds = %13
  %18 = add nsw i32 %.01, 1
  br label %11, !llvm.loop !14

19:                                               ; preds = %11
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @not_adjacent_printf() #0 {
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
  br label %3, !llvm.loop !15

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
  br label %12, !llvm.loop !16

20:                                               ; preds = %12
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @not_cfe_simple(i32 noundef %0) #0 {
  %2 = alloca [10 x i32], align 16
  %3 = alloca [10 x i32], align 16
  %4 = icmp sgt i32 %0, 0
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
define dso_local void @two_guards_same_condition(i32 noundef %0) #0 {
  %2 = alloca [10 x i32], align 16
  %3 = alloca [10 x i32], align 16
  %4 = icmp sgt i32 %0, 0
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
  %15 = icmp sgt i32 %0, 0
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
define dso_local void @decreasing_fusible() #0 {
  %1 = alloca [11 x i32], align 16
  %2 = alloca [11 x i32], align 16
  br label %3

3:                                                ; preds = %8, %0
  %.0 = phi i32 [ 10, %0 ], [ %9, %8 ]
  %4 = icmp sgt i32 %.0, 0
  br i1 %4, label %5, label %10

5:                                                ; preds = %3
  %6 = sext i32 %.0 to i64
  %7 = getelementptr inbounds [11 x i32], ptr %1, i64 0, i64 %6
  store i32 %.0, ptr %7, align 4
  br label %8

8:                                                ; preds = %5
  %9 = add nsw i32 %.0, -1
  br label %3, !llvm.loop !21

10:                                               ; preds = %3
  br label %11

11:                                               ; preds = %20, %10
  %.01 = phi i32 [ 10, %10 ], [ %21, %20 ]
  %12 = icmp sgt i32 %.01, 0
  br i1 %12, label %13, label %22

13:                                               ; preds = %11
  %14 = sext i32 %.01 to i64
  %15 = getelementptr inbounds [11 x i32], ptr %1, i64 0, i64 %14
  %16 = load i32, ptr %15, align 4
  %17 = add nsw i32 %16, 1
  %18 = sext i32 %.01 to i64
  %19 = getelementptr inbounds [11 x i32], ptr %2, i64 0, i64 %18
  store i32 %17, ptr %19, align 4
  br label %20

20:                                               ; preds = %13
  %21 = add nsw i32 %.01, -1
  br label %11, !llvm.loop !22

22:                                               ; preds = %11
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @decreasing_negative() #0 {
  %1 = alloca [11 x i32], align 16
  %2 = alloca [11 x i32], align 16
  br label %3

3:                                                ; preds = %8, %0
  %.0 = phi i32 [ 10, %0 ], [ %9, %8 ]
  %4 = icmp sgt i32 %.0, 0
  br i1 %4, label %5, label %10

5:                                                ; preds = %3
  %6 = sext i32 %.0 to i64
  %7 = getelementptr inbounds [11 x i32], ptr %1, i64 0, i64 %6
  store i32 %.0, ptr %7, align 4
  br label %8

8:                                                ; preds = %5
  %9 = add nsw i32 %.0, -1
  br label %3, !llvm.loop !23

10:                                               ; preds = %3
  br label %11

11:                                               ; preds = %21, %10
  %.01 = phi i32 [ 10, %10 ], [ %22, %21 ]
  %12 = icmp sgt i32 %.01, 0
  br i1 %12, label %13, label %23

13:                                               ; preds = %11
  %14 = sub nsw i32 %.01, 1
  %15 = sext i32 %14 to i64
  %16 = getelementptr inbounds [11 x i32], ptr %1, i64 0, i64 %15
  %17 = load i32, ptr %16, align 4
  %18 = add nsw i32 %17, 1
  %19 = sext i32 %.01 to i64
  %20 = getelementptr inbounds [11 x i32], ptr %2, i64 0, i64 %19
  store i32 %18, ptr %20, align 4
  br label %21

21:                                               ; preds = %13
  %22 = add nsw i32 %.01, -1
  br label %11, !llvm.loop !24

23:                                               ; preds = %11
  ret void
}

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
  br label %7, !llvm.loop !25

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
  br label %15, !llvm.loop !26

27:                                               ; preds = %15
  br label %28

28:                                               ; preds = %27, %4
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @fusible_symbolic_start(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca [100 x i32], align 16
  %4 = alloca [100 x i32], align 16
  %5 = icmp sle i32 0, %0
  br i1 %5, label %6, label %31

6:                                                ; preds = %2
  %7 = icmp slt i32 %0, %1
  br i1 %7, label %8, label %31

8:                                                ; preds = %6
  %9 = icmp sle i32 %1, 100
  br i1 %9, label %10, label %31

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %16, %10
  %.01 = phi i32 [ %0, %10 ], [ %17, %16 ]
  %12 = icmp slt i32 %.01, %1
  br i1 %12, label %13, label %18

13:                                               ; preds = %11
  %14 = sext i32 %.01 to i64
  %15 = getelementptr inbounds [100 x i32], ptr %3, i64 0, i64 %14
  store i32 %.01, ptr %15, align 4
  br label %16

16:                                               ; preds = %13
  %17 = add nsw i32 %.01, 1
  br label %11, !llvm.loop !27

18:                                               ; preds = %11
  br label %19

19:                                               ; preds = %28, %18
  %.0 = phi i32 [ %0, %18 ], [ %29, %28 ]
  %20 = icmp slt i32 %.0, %1
  br i1 %20, label %21, label %30

21:                                               ; preds = %19
  %22 = sext i32 %.0 to i64
  %23 = getelementptr inbounds [100 x i32], ptr %3, i64 0, i64 %22
  %24 = load i32, ptr %23, align 4
  %25 = add nsw i32 %24, 1
  %26 = sext i32 %.0 to i64
  %27 = getelementptr inbounds [100 x i32], ptr %4, i64 0, i64 %26
  store i32 %25, ptr %27, align 4
  br label %28

28:                                               ; preds = %21
  %29 = add nsw i32 %.0, 1
  br label %19, !llvm.loop !28

30:                                               ; preds = %19
  br label %31

31:                                               ; preds = %30, %8, %6, %2
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @pointer_may_alias_unknown(ptr noundef %0, ptr noundef %1, i32 noundef %2) #0 {
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
  br label %6, !llvm.loop !29

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
  br label %14, !llvm.loop !30

25:                                               ; preds = %14
  br label %26

26:                                               ; preds = %25, %3
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @nest_fusible(i32 noundef %0) #0 {
  %2 = alloca [100 x [100 x i32]], align 16
  %3 = alloca [100 x [100 x i32]], align 16
  %4 = alloca [100 x [100 x i32]], align 16
  %5 = icmp sgt i32 %0, 0
  br i1 %5, label %6, label %53

6:                                                ; preds = %1
  %7 = icmp sle i32 %0, 100
  br i1 %7, label %8, label %53

8:                                                ; preds = %6
  br label %9

9:                                                ; preds = %23, %8
  %.01 = phi i32 [ 0, %8 ], [ %24, %23 ]
  %10 = icmp slt i32 %.01, %0
  br i1 %10, label %11, label %25

11:                                               ; preds = %9
  br label %12

12:                                               ; preds = %20, %11
  %.02 = phi i32 [ 0, %11 ], [ %21, %20 ]
  %13 = icmp slt i32 %.02, %0
  br i1 %13, label %14, label %22

14:                                               ; preds = %12
  %15 = add nsw i32 %.01, %.02
  %16 = sext i32 %.01 to i64
  %17 = getelementptr inbounds [100 x [100 x i32]], ptr %2, i64 0, i64 %16
  %18 = sext i32 %.02 to i64
  %19 = getelementptr inbounds [100 x i32], ptr %17, i64 0, i64 %18
  store i32 %15, ptr %19, align 4
  br label %20

20:                                               ; preds = %14
  %21 = add nsw i32 %.02, 1
  br label %12, !llvm.loop !31

22:                                               ; preds = %12
  br label %23

23:                                               ; preds = %22
  %24 = add nsw i32 %.01, 1
  br label %9, !llvm.loop !32

25:                                               ; preds = %9
  br label %26

26:                                               ; preds = %50, %25
  %.03 = phi i32 [ 0, %25 ], [ %51, %50 ]
  %27 = icmp slt i32 %.03, %0
  br i1 %27, label %28, label %52

28:                                               ; preds = %26
  br label %29

29:                                               ; preds = %47, %28
  %.0 = phi i32 [ 0, %28 ], [ %48, %47 ]
  %30 = icmp slt i32 %.0, %0
  br i1 %30, label %31, label %49

31:                                               ; preds = %29
  %32 = sext i32 %.03 to i64
  %33 = getelementptr inbounds [100 x [100 x i32]], ptr %2, i64 0, i64 %32
  %34 = sext i32 %.0 to i64
  %35 = getelementptr inbounds [100 x i32], ptr %33, i64 0, i64 %34
  %36 = load i32, ptr %35, align 4
  %37 = sext i32 %.03 to i64
  %38 = getelementptr inbounds [100 x [100 x i32]], ptr %4, i64 0, i64 %37
  %39 = sext i32 %.0 to i64
  %40 = getelementptr inbounds [100 x i32], ptr %38, i64 0, i64 %39
  %41 = load i32, ptr %40, align 4
  %42 = add nsw i32 %36, %41
  %43 = sext i32 %.03 to i64
  %44 = getelementptr inbounds [100 x [100 x i32]], ptr %3, i64 0, i64 %43
  %45 = sext i32 %.0 to i64
  %46 = getelementptr inbounds [100 x i32], ptr %44, i64 0, i64 %45
  store i32 %42, ptr %46, align 4
  br label %47

47:                                               ; preds = %31
  %48 = add nsw i32 %.0, 1
  br label %29, !llvm.loop !33

49:                                               ; preds = %29
  br label %50

50:                                               ; preds = %49
  %51 = add nsw i32 %.03, 1
  br label %26, !llvm.loop !34

52:                                               ; preds = %26
  br label %53

53:                                               ; preds = %52, %6, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @nest_negative_inner_dimension(i32 noundef %0) #0 {
  %2 = alloca [100 x [101 x i32]], align 16
  %3 = alloca [100 x [100 x i32]], align 16
  %4 = icmp sgt i32 %0, 0
  br i1 %4, label %5, label %48

5:                                                ; preds = %1
  %6 = icmp sle i32 %0, 100
  br i1 %6, label %7, label %48

7:                                                ; preds = %5
  br label %8

8:                                                ; preds = %22, %7
  %.01 = phi i32 [ 0, %7 ], [ %23, %22 ]
  %9 = icmp slt i32 %.01, %0
  br i1 %9, label %10, label %24

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %19, %10
  %.02 = phi i32 [ 0, %10 ], [ %20, %19 ]
  %12 = icmp slt i32 %.02, %0
  br i1 %12, label %13, label %21

13:                                               ; preds = %11
  %14 = add nsw i32 %.01, %.02
  %15 = sext i32 %.01 to i64
  %16 = getelementptr inbounds [100 x [101 x i32]], ptr %2, i64 0, i64 %15
  %17 = sext i32 %.02 to i64
  %18 = getelementptr inbounds [101 x i32], ptr %16, i64 0, i64 %17
  store i32 %14, ptr %18, align 4
  br label %19

19:                                               ; preds = %13
  %20 = add nsw i32 %.02, 1
  br label %11, !llvm.loop !35

21:                                               ; preds = %11
  br label %22

22:                                               ; preds = %21
  %23 = add nsw i32 %.01, 1
  br label %8, !llvm.loop !36

24:                                               ; preds = %8
  br label %25

25:                                               ; preds = %45, %24
  %.03 = phi i32 [ 0, %24 ], [ %46, %45 ]
  %26 = icmp slt i32 %.03, %0
  br i1 %26, label %27, label %47

27:                                               ; preds = %25
  br label %28

28:                                               ; preds = %42, %27
  %.0 = phi i32 [ 0, %27 ], [ %43, %42 ]
  %29 = icmp slt i32 %.0, %0
  br i1 %29, label %30, label %44

30:                                               ; preds = %28
  %31 = sext i32 %.03 to i64
  %32 = getelementptr inbounds [100 x [101 x i32]], ptr %2, i64 0, i64 %31
  %33 = add nsw i32 %.0, 1
  %34 = sext i32 %33 to i64
  %35 = getelementptr inbounds [101 x i32], ptr %32, i64 0, i64 %34
  %36 = load i32, ptr %35, align 4
  %37 = add nsw i32 %36, 1
  %38 = sext i32 %.03 to i64
  %39 = getelementptr inbounds [100 x [100 x i32]], ptr %3, i64 0, i64 %38
  %40 = sext i32 %.0 to i64
  %41 = getelementptr inbounds [100 x i32], ptr %39, i64 0, i64 %40
  store i32 %37, ptr %41, align 4
  br label %42

42:                                               ; preds = %30
  %43 = add nsw i32 %.0, 1
  br label %28, !llvm.loop !37

44:                                               ; preds = %28
  br label %45

45:                                               ; preds = %44
  %46 = add nsw i32 %.03, 1
  br label %25, !llvm.loop !38

47:                                               ; preds = %25
  br label %48

48:                                               ; preds = %47, %5, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @nest_negative_outer_dimension(i32 noundef %0) #0 {
  %2 = alloca [101 x [100 x i32]], align 16
  %3 = alloca [100 x [100 x i32]], align 16
  %4 = icmp sgt i32 %0, 0
  br i1 %4, label %5, label %48

5:                                                ; preds = %1
  %6 = icmp sle i32 %0, 100
  br i1 %6, label %7, label %48

7:                                                ; preds = %5
  br label %8

8:                                                ; preds = %22, %7
  %.01 = phi i32 [ 0, %7 ], [ %23, %22 ]
  %9 = icmp slt i32 %.01, %0
  br i1 %9, label %10, label %24

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %19, %10
  %.02 = phi i32 [ 0, %10 ], [ %20, %19 ]
  %12 = icmp slt i32 %.02, %0
  br i1 %12, label %13, label %21

13:                                               ; preds = %11
  %14 = add nsw i32 %.01, %.02
  %15 = sext i32 %.01 to i64
  %16 = getelementptr inbounds [101 x [100 x i32]], ptr %2, i64 0, i64 %15
  %17 = sext i32 %.02 to i64
  %18 = getelementptr inbounds [100 x i32], ptr %16, i64 0, i64 %17
  store i32 %14, ptr %18, align 4
  br label %19

19:                                               ; preds = %13
  %20 = add nsw i32 %.02, 1
  br label %11, !llvm.loop !39

21:                                               ; preds = %11
  br label %22

22:                                               ; preds = %21
  %23 = add nsw i32 %.01, 1
  br label %8, !llvm.loop !40

24:                                               ; preds = %8
  br label %25

25:                                               ; preds = %45, %24
  %.03 = phi i32 [ 0, %24 ], [ %46, %45 ]
  %26 = icmp slt i32 %.03, %0
  br i1 %26, label %27, label %47

27:                                               ; preds = %25
  br label %28

28:                                               ; preds = %42, %27
  %.0 = phi i32 [ 0, %27 ], [ %43, %42 ]
  %29 = icmp slt i32 %.0, %0
  br i1 %29, label %30, label %44

30:                                               ; preds = %28
  %31 = add nsw i32 %.03, 1
  %32 = sext i32 %31 to i64
  %33 = getelementptr inbounds [101 x [100 x i32]], ptr %2, i64 0, i64 %32
  %34 = sext i32 %.0 to i64
  %35 = getelementptr inbounds [100 x i32], ptr %33, i64 0, i64 %34
  %36 = load i32, ptr %35, align 4
  %37 = add nsw i32 %36, 1
  %38 = sext i32 %.03 to i64
  %39 = getelementptr inbounds [100 x [100 x i32]], ptr %3, i64 0, i64 %38
  %40 = sext i32 %.0 to i64
  %41 = getelementptr inbounds [100 x i32], ptr %39, i64 0, i64 %40
  store i32 %37, ptr %41, align 4
  br label %42

42:                                               ; preds = %30
  %43 = add nsw i32 %.0, 1
  br label %28, !llvm.loop !41

44:                                               ; preds = %28
  br label %45

45:                                               ; preds = %44
  %46 = add nsw i32 %.03, 1
  br label %25, !llvm.loop !42

47:                                               ; preds = %25
  br label %48

48:                                               ; preds = %47, %5, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @nest_different_inner_trip_count(i32 noundef %0) #0 {
  %2 = alloca [100 x [100 x i32]], align 16
  %3 = alloca [100 x [100 x i32]], align 16
  %4 = icmp sgt i32 %0, 1
  br i1 %4, label %5, label %43

5:                                                ; preds = %1
  %6 = icmp sle i32 %0, 100
  br i1 %6, label %7, label %43

7:                                                ; preds = %5
  br label %8

8:                                                ; preds = %22, %7
  %.01 = phi i32 [ 0, %7 ], [ %23, %22 ]
  %9 = icmp slt i32 %.01, %0
  br i1 %9, label %10, label %24

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %19, %10
  %.02 = phi i32 [ 0, %10 ], [ %20, %19 ]
  %12 = icmp slt i32 %.02, %0
  br i1 %12, label %13, label %21

13:                                               ; preds = %11
  %14 = add nsw i32 %.01, %.02
  %15 = sext i32 %.01 to i64
  %16 = getelementptr inbounds [100 x [100 x i32]], ptr %2, i64 0, i64 %15
  %17 = sext i32 %.02 to i64
  %18 = getelementptr inbounds [100 x i32], ptr %16, i64 0, i64 %17
  store i32 %14, ptr %18, align 4
  br label %19

19:                                               ; preds = %13
  %20 = add nsw i32 %.02, 1
  br label %11, !llvm.loop !43

21:                                               ; preds = %11
  br label %22

22:                                               ; preds = %21
  %23 = add nsw i32 %.01, 1
  br label %8, !llvm.loop !44

24:                                               ; preds = %8
  br label %25

25:                                               ; preds = %40, %24
  %.03 = phi i32 [ 0, %24 ], [ %41, %40 ]
  %26 = icmp slt i32 %.03, %0
  br i1 %26, label %27, label %42

27:                                               ; preds = %25
  br label %28

28:                                               ; preds = %37, %27
  %.0 = phi i32 [ 0, %27 ], [ %38, %37 ]
  %29 = sub nsw i32 %0, 1
  %30 = icmp slt i32 %.0, %29
  br i1 %30, label %31, label %39

31:                                               ; preds = %28
  %32 = add nsw i32 %.0, 1
  %33 = sext i32 %.03 to i64
  %34 = getelementptr inbounds [100 x [100 x i32]], ptr %3, i64 0, i64 %33
  %35 = sext i32 %.0 to i64
  %36 = getelementptr inbounds [100 x i32], ptr %34, i64 0, i64 %35
  store i32 %32, ptr %36, align 4
  br label %37

37:                                               ; preds = %31
  %38 = add nsw i32 %.0, 1
  br label %28, !llvm.loop !45

39:                                               ; preds = %28
  br label %40

40:                                               ; preds = %39
  %41 = add nsw i32 %.03, 1
  br label %25, !llvm.loop !46

42:                                               ; preds = %25
  br label %43

43:                                               ; preds = %42, %5, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @nest_not_adjacent(i32 noundef %0) #0 {
  %2 = alloca [100 x [100 x i32]], align 16
  %3 = alloca [100 x [100 x i32]], align 16
  %4 = icmp sgt i32 %0, 0
  br i1 %4, label %5, label %43

5:                                                ; preds = %1
  %6 = icmp sle i32 %0, 100
  br i1 %6, label %7, label %43

7:                                                ; preds = %5
  br label %8

8:                                                ; preds = %22, %7
  %.01 = phi i32 [ 0, %7 ], [ %23, %22 ]
  %9 = icmp slt i32 %.01, %0
  br i1 %9, label %10, label %24

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %19, %10
  %.02 = phi i32 [ 0, %10 ], [ %20, %19 ]
  %12 = icmp slt i32 %.02, %0
  br i1 %12, label %13, label %21

13:                                               ; preds = %11
  %14 = add nsw i32 %.01, %.02
  %15 = sext i32 %.01 to i64
  %16 = getelementptr inbounds [100 x [100 x i32]], ptr %2, i64 0, i64 %15
  %17 = sext i32 %.02 to i64
  %18 = getelementptr inbounds [100 x i32], ptr %16, i64 0, i64 %17
  store i32 %14, ptr %18, align 4
  br label %19

19:                                               ; preds = %13
  %20 = add nsw i32 %.02, 1
  br label %11, !llvm.loop !47

21:                                               ; preds = %11
  br label %22

22:                                               ; preds = %21
  %23 = add nsw i32 %.01, 1
  br label %8, !llvm.loop !48

24:                                               ; preds = %8
  %25 = call i32 (ptr, ...) @printf(ptr noundef @.str.1)
  br label %26

26:                                               ; preds = %40, %24
  %.03 = phi i32 [ 0, %24 ], [ %41, %40 ]
  %27 = icmp slt i32 %.03, %0
  br i1 %27, label %28, label %42

28:                                               ; preds = %26
  br label %29

29:                                               ; preds = %37, %28
  %.0 = phi i32 [ 0, %28 ], [ %38, %37 ]
  %30 = icmp slt i32 %.0, %0
  br i1 %30, label %31, label %39

31:                                               ; preds = %29
  %32 = add nsw i32 %.0, 1
  %33 = sext i32 %.03 to i64
  %34 = getelementptr inbounds [100 x [100 x i32]], ptr %3, i64 0, i64 %33
  %35 = sext i32 %.0 to i64
  %36 = getelementptr inbounds [100 x i32], ptr %34, i64 0, i64 %35
  store i32 %32, ptr %36, align 4
  br label %37

37:                                               ; preds = %31
  %38 = add nsw i32 %.0, 1
  br label %29, !llvm.loop !49

39:                                               ; preds = %29
  br label %40

40:                                               ; preds = %39
  %41 = add nsw i32 %.03, 1
  br label %26, !llvm.loop !50

42:                                               ; preds = %26
  br label %43

43:                                               ; preds = %42, %5, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @inner_sibling_fusible(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca [100 x [100 x i32]], align 16
  %4 = alloca [100 x [100 x i32]], align 16
  %5 = icmp sgt i32 %0, 0
  br i1 %5, label %6, label %40

6:                                                ; preds = %2
  %7 = icmp sle i32 %0, 100
  br i1 %7, label %8, label %40

8:                                                ; preds = %6
  %9 = icmp sgt i32 %1, 0
  br i1 %9, label %10, label %40

10:                                               ; preds = %8
  %11 = icmp sle i32 %1, 100
  br i1 %11, label %12, label %40

12:                                               ; preds = %10
  br label %13

13:                                               ; preds = %37, %12
  %.02 = phi i32 [ 0, %12 ], [ %38, %37 ]
  %14 = icmp slt i32 %.02, %0
  br i1 %14, label %15, label %39

15:                                               ; preds = %13
  br label %16

16:                                               ; preds = %23, %15
  %.01 = phi i32 [ 0, %15 ], [ %24, %23 ]
  %17 = icmp slt i32 %.01, %1
  br i1 %17, label %18, label %25

18:                                               ; preds = %16
  %19 = sext i32 %.02 to i64
  %20 = getelementptr inbounds [100 x [100 x i32]], ptr %3, i64 0, i64 %19
  %21 = sext i32 %.01 to i64
  %22 = getelementptr inbounds [100 x i32], ptr %20, i64 0, i64 %21
  store i32 %.01, ptr %22, align 4
  br label %23

23:                                               ; preds = %18
  %24 = add nsw i32 %.01, 1
  br label %16, !llvm.loop !51

25:                                               ; preds = %16
  br label %26

26:                                               ; preds = %34, %25
  %.0 = phi i32 [ 0, %25 ], [ %35, %34 ]
  %27 = icmp slt i32 %.0, %1
  br i1 %27, label %28, label %36

28:                                               ; preds = %26
  %29 = add nsw i32 %.0, 1
  %30 = sext i32 %.02 to i64
  %31 = getelementptr inbounds [100 x [100 x i32]], ptr %4, i64 0, i64 %30
  %32 = sext i32 %.0 to i64
  %33 = getelementptr inbounds [100 x i32], ptr %31, i64 0, i64 %32
  store i32 %29, ptr %33, align 4
  br label %34

34:                                               ; preds = %28
  %35 = add nsw i32 %.0, 1
  br label %26, !llvm.loop !52

36:                                               ; preds = %26
  br label %37

37:                                               ; preds = %36
  %38 = add nsw i32 %.02, 1
  br label %13, !llvm.loop !53

39:                                               ; preds = %13
  br label %40

40:                                               ; preds = %39, %10, %8, %6, %2
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @inner_sibling_negative(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca [100 x [101 x i32]], align 16
  %4 = alloca [100 x [100 x i32]], align 16
  %5 = icmp sgt i32 %0, 0
  br i1 %5, label %6, label %46

6:                                                ; preds = %2
  %7 = icmp sle i32 %0, 100
  br i1 %7, label %8, label %46

8:                                                ; preds = %6
  %9 = icmp sgt i32 %1, 0
  br i1 %9, label %10, label %46

10:                                               ; preds = %8
  %11 = icmp sle i32 %1, 100
  br i1 %11, label %12, label %46

12:                                               ; preds = %10
  br label %13

13:                                               ; preds = %43, %12
  %.02 = phi i32 [ 0, %12 ], [ %44, %43 ]
  %14 = icmp slt i32 %.02, %0
  br i1 %14, label %15, label %45

15:                                               ; preds = %13
  br label %16

16:                                               ; preds = %23, %15
  %.01 = phi i32 [ 0, %15 ], [ %24, %23 ]
  %17 = icmp slt i32 %.01, %1
  br i1 %17, label %18, label %25

18:                                               ; preds = %16
  %19 = sext i32 %.02 to i64
  %20 = getelementptr inbounds [100 x [101 x i32]], ptr %3, i64 0, i64 %19
  %21 = sext i32 %.01 to i64
  %22 = getelementptr inbounds [101 x i32], ptr %20, i64 0, i64 %21
  store i32 %.01, ptr %22, align 4
  br label %23

23:                                               ; preds = %18
  %24 = add nsw i32 %.01, 1
  br label %16, !llvm.loop !54

25:                                               ; preds = %16
  br label %26

26:                                               ; preds = %40, %25
  %.0 = phi i32 [ 0, %25 ], [ %41, %40 ]
  %27 = icmp slt i32 %.0, %1
  br i1 %27, label %28, label %42

28:                                               ; preds = %26
  %29 = sext i32 %.02 to i64
  %30 = getelementptr inbounds [100 x [101 x i32]], ptr %3, i64 0, i64 %29
  %31 = add nsw i32 %.0, 1
  %32 = sext i32 %31 to i64
  %33 = getelementptr inbounds [101 x i32], ptr %30, i64 0, i64 %32
  %34 = load i32, ptr %33, align 4
  %35 = add nsw i32 %34, 1
  %36 = sext i32 %.02 to i64
  %37 = getelementptr inbounds [100 x [100 x i32]], ptr %4, i64 0, i64 %36
  %38 = sext i32 %.0 to i64
  %39 = getelementptr inbounds [100 x i32], ptr %37, i64 0, i64 %38
  store i32 %35, ptr %39, align 4
  br label %40

40:                                               ; preds = %28
  %41 = add nsw i32 %.0, 1
  br label %26, !llvm.loop !55

42:                                               ; preds = %26
  br label %43

43:                                               ; preds = %42
  %44 = add nsw i32 %.02, 1
  br label %13, !llvm.loop !56

45:                                               ; preds = %13
  br label %46

46:                                               ; preds = %45, %10, %8, %6, %2
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @nest_safe_offset_backward(i32 noundef %0) #0 {
  %2 = alloca [101 x [101 x i32]], align 16
  %3 = alloca [100 x [100 x i32]], align 16
  %4 = icmp sgt i32 %0, 1
  br i1 %4, label %5, label %49

5:                                                ; preds = %1
  %6 = icmp sle i32 %0, 100
  br i1 %6, label %7, label %49

7:                                                ; preds = %5
  br label %8

8:                                                ; preds = %24, %7
  %.01 = phi i32 [ 0, %7 ], [ %25, %24 ]
  %9 = icmp slt i32 %.01, %0
  br i1 %9, label %10, label %26

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %21, %10
  %.02 = phi i32 [ 0, %10 ], [ %22, %21 ]
  %12 = icmp slt i32 %.02, %0
  br i1 %12, label %13, label %23

13:                                               ; preds = %11
  %14 = add nsw i32 %.01, %.02
  %15 = add nsw i32 %.01, 1
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds [101 x [101 x i32]], ptr %2, i64 0, i64 %16
  %18 = add nsw i32 %.02, 1
  %19 = sext i32 %18 to i64
  %20 = getelementptr inbounds [101 x i32], ptr %17, i64 0, i64 %19
  store i32 %14, ptr %20, align 4
  br label %21

21:                                               ; preds = %13
  %22 = add nsw i32 %.02, 1
  br label %11, !llvm.loop !57

23:                                               ; preds = %11
  br label %24

24:                                               ; preds = %23
  %25 = add nsw i32 %.01, 1
  br label %8, !llvm.loop !58

26:                                               ; preds = %8
  br label %27

27:                                               ; preds = %46, %26
  %.03 = phi i32 [ 0, %26 ], [ %47, %46 ]
  %28 = icmp slt i32 %.03, %0
  br i1 %28, label %29, label %48

29:                                               ; preds = %27
  br label %30

30:                                               ; preds = %43, %29
  %.0 = phi i32 [ 0, %29 ], [ %44, %43 ]
  %31 = icmp slt i32 %.0, %0
  br i1 %31, label %32, label %45

32:                                               ; preds = %30
  %33 = sext i32 %.03 to i64
  %34 = getelementptr inbounds [101 x [101 x i32]], ptr %2, i64 0, i64 %33
  %35 = sext i32 %.0 to i64
  %36 = getelementptr inbounds [101 x i32], ptr %34, i64 0, i64 %35
  %37 = load i32, ptr %36, align 4
  %38 = add nsw i32 %37, 1
  %39 = sext i32 %.03 to i64
  %40 = getelementptr inbounds [100 x [100 x i32]], ptr %3, i64 0, i64 %39
  %41 = sext i32 %.0 to i64
  %42 = getelementptr inbounds [100 x i32], ptr %40, i64 0, i64 %41
  store i32 %38, ptr %42, align 4
  br label %43

43:                                               ; preds = %32
  %44 = add nsw i32 %.0, 1
  br label %30, !llvm.loop !59

45:                                               ; preds = %30
  br label %46

46:                                               ; preds = %45
  %47 = add nsw i32 %.03, 1
  br label %27, !llvm.loop !60

48:                                               ; preds = %27
  br label %49

49:                                               ; preds = %48, %5, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @nest_two_guards_same_condition(i32 noundef %0) #0 {
  %2 = alloca [100 x [100 x i32]], align 16
  %3 = alloca [100 x [100 x i32]], align 16
  %4 = icmp sgt i32 %0, 0
  br i1 %4, label %5, label %25

5:                                                ; preds = %1
  %6 = icmp sle i32 %0, 100
  br i1 %6, label %7, label %25

7:                                                ; preds = %5
  br label %8

8:                                                ; preds = %22, %7
  %.01 = phi i32 [ 0, %7 ], [ %23, %22 ]
  %9 = icmp slt i32 %.01, %0
  br i1 %9, label %10, label %24

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %19, %10
  %.02 = phi i32 [ 0, %10 ], [ %20, %19 ]
  %12 = icmp slt i32 %.02, %0
  br i1 %12, label %13, label %21

13:                                               ; preds = %11
  %14 = add nsw i32 %.01, %.02
  %15 = sext i32 %.01 to i64
  %16 = getelementptr inbounds [100 x [100 x i32]], ptr %2, i64 0, i64 %15
  %17 = sext i32 %.02 to i64
  %18 = getelementptr inbounds [100 x i32], ptr %16, i64 0, i64 %17
  store i32 %14, ptr %18, align 4
  br label %19

19:                                               ; preds = %13
  %20 = add nsw i32 %.02, 1
  br label %11, !llvm.loop !61

21:                                               ; preds = %11
  br label %22

22:                                               ; preds = %21
  %23 = add nsw i32 %.01, 1
  br label %8, !llvm.loop !62

24:                                               ; preds = %8
  br label %25

25:                                               ; preds = %24, %5, %1
  %26 = icmp sgt i32 %0, 0
  br i1 %26, label %27, label %47

27:                                               ; preds = %25
  %28 = icmp sle i32 %0, 100
  br i1 %28, label %29, label %47

29:                                               ; preds = %27
  br label %30

30:                                               ; preds = %44, %29
  %.03 = phi i32 [ 0, %29 ], [ %45, %44 ]
  %31 = icmp slt i32 %.03, %0
  br i1 %31, label %32, label %46

32:                                               ; preds = %30
  br label %33

33:                                               ; preds = %41, %32
  %.0 = phi i32 [ 0, %32 ], [ %42, %41 ]
  %34 = icmp slt i32 %.0, %0
  br i1 %34, label %35, label %43

35:                                               ; preds = %33
  %36 = add nsw i32 %.0, 1
  %37 = sext i32 %.03 to i64
  %38 = getelementptr inbounds [100 x [100 x i32]], ptr %3, i64 0, i64 %37
  %39 = sext i32 %.0 to i64
  %40 = getelementptr inbounds [100 x i32], ptr %38, i64 0, i64 %39
  store i32 %36, ptr %40, align 4
  br label %41

41:                                               ; preds = %35
  %42 = add nsw i32 %.0, 1
  br label %33, !llvm.loop !63

43:                                               ; preds = %33
  br label %44

44:                                               ; preds = %43
  %45 = add nsw i32 %.03, 1
  br label %30, !llvm.loop !64

46:                                               ; preds = %30
  br label %47

47:                                               ; preds = %46, %27, %25
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @nest_decreasing_fusible(i32 noundef %0) #0 {
  %2 = alloca [101 x [101 x i32]], align 16
  %3 = alloca [101 x [101 x i32]], align 16
  %4 = icmp sgt i32 %0, 0
  br i1 %4, label %5, label %47

5:                                                ; preds = %1
  %6 = icmp sle i32 %0, 100
  br i1 %6, label %7, label %47

7:                                                ; preds = %5
  br label %8

8:                                                ; preds = %22, %7
  %.01 = phi i32 [ %0, %7 ], [ %23, %22 ]
  %9 = icmp sgt i32 %.01, 0
  br i1 %9, label %10, label %24

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %19, %10
  %.02 = phi i32 [ %0, %10 ], [ %20, %19 ]
  %12 = icmp sgt i32 %.02, 0
  br i1 %12, label %13, label %21

13:                                               ; preds = %11
  %14 = add nsw i32 %.01, %.02
  %15 = sext i32 %.01 to i64
  %16 = getelementptr inbounds [101 x [101 x i32]], ptr %2, i64 0, i64 %15
  %17 = sext i32 %.02 to i64
  %18 = getelementptr inbounds [101 x i32], ptr %16, i64 0, i64 %17
  store i32 %14, ptr %18, align 4
  br label %19

19:                                               ; preds = %13
  %20 = add nsw i32 %.02, -1
  br label %11, !llvm.loop !65

21:                                               ; preds = %11
  br label %22

22:                                               ; preds = %21
  %23 = add nsw i32 %.01, -1
  br label %8, !llvm.loop !66

24:                                               ; preds = %8
  br label %25

25:                                               ; preds = %44, %24
  %.03 = phi i32 [ %0, %24 ], [ %45, %44 ]
  %26 = icmp sgt i32 %.03, 0
  br i1 %26, label %27, label %46

27:                                               ; preds = %25
  br label %28

28:                                               ; preds = %41, %27
  %.0 = phi i32 [ %0, %27 ], [ %42, %41 ]
  %29 = icmp sgt i32 %.0, 0
  br i1 %29, label %30, label %43

30:                                               ; preds = %28
  %31 = sext i32 %.03 to i64
  %32 = getelementptr inbounds [101 x [101 x i32]], ptr %2, i64 0, i64 %31
  %33 = sext i32 %.0 to i64
  %34 = getelementptr inbounds [101 x i32], ptr %32, i64 0, i64 %33
  %35 = load i32, ptr %34, align 4
  %36 = add nsw i32 %35, 1
  %37 = sext i32 %.03 to i64
  %38 = getelementptr inbounds [101 x [101 x i32]], ptr %3, i64 0, i64 %37
  %39 = sext i32 %.0 to i64
  %40 = getelementptr inbounds [101 x i32], ptr %38, i64 0, i64 %39
  store i32 %36, ptr %40, align 4
  br label %41

41:                                               ; preds = %30
  %42 = add nsw i32 %.0, -1
  br label %28, !llvm.loop !67

43:                                               ; preds = %28
  br label %44

44:                                               ; preds = %43
  %45 = add nsw i32 %.03, -1
  br label %25, !llvm.loop !68

46:                                               ; preds = %25
  br label %47

47:                                               ; preds = %46, %5, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @nest_decreasing_negative_inner(i32 noundef %0) #0 {
  %2 = alloca [101 x [101 x i32]], align 16
  %3 = alloca [101 x [101 x i32]], align 16
  %4 = icmp sgt i32 %0, 1
  br i1 %4, label %5, label %48

5:                                                ; preds = %1
  %6 = icmp sle i32 %0, 100
  br i1 %6, label %7, label %48

7:                                                ; preds = %5
  br label %8

8:                                                ; preds = %22, %7
  %.01 = phi i32 [ %0, %7 ], [ %23, %22 ]
  %9 = icmp sgt i32 %.01, 0
  br i1 %9, label %10, label %24

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %19, %10
  %.02 = phi i32 [ %0, %10 ], [ %20, %19 ]
  %12 = icmp sgt i32 %.02, 0
  br i1 %12, label %13, label %21

13:                                               ; preds = %11
  %14 = add nsw i32 %.01, %.02
  %15 = sext i32 %.01 to i64
  %16 = getelementptr inbounds [101 x [101 x i32]], ptr %2, i64 0, i64 %15
  %17 = sext i32 %.02 to i64
  %18 = getelementptr inbounds [101 x i32], ptr %16, i64 0, i64 %17
  store i32 %14, ptr %18, align 4
  br label %19

19:                                               ; preds = %13
  %20 = add nsw i32 %.02, -1
  br label %11, !llvm.loop !69

21:                                               ; preds = %11
  br label %22

22:                                               ; preds = %21
  %23 = add nsw i32 %.01, -1
  br label %8, !llvm.loop !70

24:                                               ; preds = %8
  br label %25

25:                                               ; preds = %45, %24
  %.03 = phi i32 [ %0, %24 ], [ %46, %45 ]
  %26 = icmp sgt i32 %.03, 0
  br i1 %26, label %27, label %47

27:                                               ; preds = %25
  br label %28

28:                                               ; preds = %42, %27
  %.0 = phi i32 [ %0, %27 ], [ %43, %42 ]
  %29 = icmp sgt i32 %.0, 0
  br i1 %29, label %30, label %44

30:                                               ; preds = %28
  %31 = sext i32 %.03 to i64
  %32 = getelementptr inbounds [101 x [101 x i32]], ptr %2, i64 0, i64 %31
  %33 = sub nsw i32 %.0, 1
  %34 = sext i32 %33 to i64
  %35 = getelementptr inbounds [101 x i32], ptr %32, i64 0, i64 %34
  %36 = load i32, ptr %35, align 4
  %37 = add nsw i32 %36, 1
  %38 = sext i32 %.03 to i64
  %39 = getelementptr inbounds [101 x [101 x i32]], ptr %3, i64 0, i64 %38
  %40 = sext i32 %.0 to i64
  %41 = getelementptr inbounds [101 x i32], ptr %39, i64 0, i64 %40
  store i32 %37, ptr %41, align 4
  br label %42

42:                                               ; preds = %30
  %43 = add nsw i32 %.0, -1
  br label %28, !llvm.loop !71

44:                                               ; preds = %28
  br label %45

45:                                               ; preds = %44
  %46 = add nsw i32 %.03, -1
  br label %25, !llvm.loop !72

47:                                               ; preds = %25
  br label %48

48:                                               ; preds = %47, %5, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @three_consecutive_fusible() #0 {
  %1 = alloca [10 x i32], align 16
  %2 = alloca [10 x i32], align 16
  %3 = alloca [10 x i32], align 16
  br label %4

4:                                                ; preds = %9, %0
  %.0 = phi i32 [ 0, %0 ], [ %10, %9 ]
  %5 = icmp slt i32 %.0, 10
  br i1 %5, label %6, label %11

6:                                                ; preds = %4
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %7
  store i32 %.0, ptr %8, align 4
  br label %9

9:                                                ; preds = %6
  %10 = add nsw i32 %.0, 1
  br label %4, !llvm.loop !73

11:                                               ; preds = %4
  br label %12

12:                                               ; preds = %18, %11
  %.01 = phi i32 [ 0, %11 ], [ %19, %18 ]
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
  br label %12, !llvm.loop !74

20:                                               ; preds = %12
  br label %21

21:                                               ; preds = %27, %20
  %.02 = phi i32 [ 0, %20 ], [ %28, %27 ]
  %22 = icmp slt i32 %.02, 10
  br i1 %22, label %23, label %29

23:                                               ; preds = %21
  %24 = add nsw i32 %.02, 2
  %25 = sext i32 %.02 to i64
  %26 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %25
  store i32 %24, ptr %26, align 4
  br label %27

27:                                               ; preds = %23
  %28 = add nsw i32 %.02, 1
  br label %21, !llvm.loop !75

29:                                               ; preds = %21
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
!33 = distinct !{!33, !7}
!34 = distinct !{!34, !7}
!35 = distinct !{!35, !7}
!36 = distinct !{!36, !7}
!37 = distinct !{!37, !7}
!38 = distinct !{!38, !7}
!39 = distinct !{!39, !7}
!40 = distinct !{!40, !7}
!41 = distinct !{!41, !7}
!42 = distinct !{!42, !7}
!43 = distinct !{!43, !7}
!44 = distinct !{!44, !7}
!45 = distinct !{!45, !7}
!46 = distinct !{!46, !7}
!47 = distinct !{!47, !7}
!48 = distinct !{!48, !7}
!49 = distinct !{!49, !7}
!50 = distinct !{!50, !7}
!51 = distinct !{!51, !7}
!52 = distinct !{!52, !7}
!53 = distinct !{!53, !7}
!54 = distinct !{!54, !7}
!55 = distinct !{!55, !7}
!56 = distinct !{!56, !7}
!57 = distinct !{!57, !7}
!58 = distinct !{!58, !7}
!59 = distinct !{!59, !7}
!60 = distinct !{!60, !7}
!61 = distinct !{!61, !7}
!62 = distinct !{!62, !7}
!63 = distinct !{!63, !7}
!64 = distinct !{!64, !7}
!65 = distinct !{!65, !7}
!66 = distinct !{!66, !7}
!67 = distinct !{!67, !7}
!68 = distinct !{!68, !7}
!69 = distinct !{!69, !7}
!70 = distinct !{!70, !7}
!71 = distinct !{!71, !7}
!72 = distinct !{!72, !7}
!73 = distinct !{!73, !7}
!74 = distinct !{!74, !7}
!75 = distinct !{!75, !7}
