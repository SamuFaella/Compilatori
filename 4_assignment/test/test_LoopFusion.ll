; ModuleID = 'test/test_LoopFusion.c'
source_filename = "test/test_LoopFusion.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [15 x i8] c"Between loops\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @fusible_test(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 %1, ptr %4, align 4
  %9 = load i32, ptr %3, align 4
  %10 = load i32, ptr %4, align 4
  %11 = icmp slt i32 %9, %10
  br i1 %11, label %12, label %44

12:                                               ; preds = %2
  %13 = load i32, ptr %3, align 4
  store i32 %13, ptr %7, align 4
  br label %14

14:                                               ; preds = %24, %12
  %15 = load i32, ptr %7, align 4
  %16 = load i32, ptr %4, align 4
  %17 = icmp slt i32 %15, %16
  br i1 %17, label %18, label %27

18:                                               ; preds = %14
  %19 = load i32, ptr %7, align 4
  %20 = load ptr, ptr %5, align 8
  %21 = load i32, ptr %7, align 4
  %22 = sext i32 %21 to i64
  %23 = getelementptr inbounds i32, ptr %20, i64 %22
  store i32 %19, ptr %23, align 4
  br label %24

24:                                               ; preds = %18
  %25 = load i32, ptr %7, align 4
  %26 = add nsw i32 %25, 1
  store i32 %26, ptr %7, align 4
  br label %14, !llvm.loop !6

27:                                               ; preds = %14
  %28 = load i32, ptr %3, align 4
  store i32 %28, ptr %8, align 4
  br label %29

29:                                               ; preds = %40, %27
  %30 = load i32, ptr %8, align 4
  %31 = load i32, ptr %4, align 4
  %32 = icmp slt i32 %30, %31
  br i1 %32, label %33, label %43

33:                                               ; preds = %29
  %34 = load i32, ptr %8, align 4
  %35 = add nsw i32 %34, 1
  %36 = load ptr, ptr %6, align 8
  %37 = load i32, ptr %8, align 4
  %38 = sext i32 %37 to i64
  %39 = getelementptr inbounds i32, ptr %36, i64 %38
  store i32 %35, ptr %39, align 4
  br label %40

40:                                               ; preds = %33
  %41 = load i32, ptr %8, align 4
  %42 = add nsw i32 %41, 1
  store i32 %42, ptr %8, align 4
  br label %29, !llvm.loop !8

43:                                               ; preds = %29
  br label %44

44:                                               ; preds = %43, %2
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @negative_dep_test(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca [11 x i32], align 16
  %4 = alloca [10 x i32], align 16
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %7 = load i32, ptr %2, align 4
  %8 = icmp sgt i32 %7, 10
  br i1 %8, label %9, label %39

9:                                                ; preds = %1
  store i32 0, ptr %5, align 4
  br label %10

10:                                               ; preds = %18, %9
  %11 = load i32, ptr %5, align 4
  %12 = icmp slt i32 %11, 10
  br i1 %12, label %13, label %21

13:                                               ; preds = %10
  %14 = load i32, ptr %5, align 4
  %15 = load i32, ptr %5, align 4
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds [11 x i32], ptr %3, i64 0, i64 %16
  store i32 %14, ptr %17, align 4
  br label %18

18:                                               ; preds = %13
  %19 = load i32, ptr %5, align 4
  %20 = add nsw i32 %19, 1
  store i32 %20, ptr %5, align 4
  br label %10, !llvm.loop !9

21:                                               ; preds = %10
  store i32 0, ptr %6, align 4
  br label %22

22:                                               ; preds = %35, %21
  %23 = load i32, ptr %6, align 4
  %24 = icmp slt i32 %23, 10
  br i1 %24, label %25, label %38

25:                                               ; preds = %22
  %26 = load i32, ptr %6, align 4
  %27 = add nsw i32 %26, 1
  %28 = sext i32 %27 to i64
  %29 = getelementptr inbounds [11 x i32], ptr %3, i64 0, i64 %28
  %30 = load i32, ptr %29, align 4
  %31 = add nsw i32 %30, 1
  %32 = load i32, ptr %6, align 4
  %33 = sext i32 %32 to i64
  %34 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 %33
  store i32 %31, ptr %34, align 4
  br label %35

35:                                               ; preds = %25
  %36 = load i32, ptr %6, align 4
  %37 = add nsw i32 %36, 1
  store i32 %37, ptr %6, align 4
  br label %22, !llvm.loop !10

38:                                               ; preds = %22
  br label %39

39:                                               ; preds = %38, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @zero_distance_dep_test(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca [10 x i32], align 16
  %4 = alloca [10 x i32], align 16
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %7 = load i32, ptr %2, align 4
  %8 = icmp sgt i32 %7, 10
  br i1 %8, label %9, label %38

9:                                                ; preds = %1
  store i32 0, ptr %5, align 4
  br label %10

10:                                               ; preds = %18, %9
  %11 = load i32, ptr %5, align 4
  %12 = icmp slt i32 %11, 10
  br i1 %12, label %13, label %21

13:                                               ; preds = %10
  %14 = load i32, ptr %5, align 4
  %15 = load i32, ptr %5, align 4
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %16
  store i32 %14, ptr %17, align 4
  br label %18

18:                                               ; preds = %13
  %19 = load i32, ptr %5, align 4
  %20 = add nsw i32 %19, 1
  store i32 %20, ptr %5, align 4
  br label %10, !llvm.loop !11

21:                                               ; preds = %10
  store i32 0, ptr %6, align 4
  br label %22

22:                                               ; preds = %34, %21
  %23 = load i32, ptr %6, align 4
  %24 = icmp slt i32 %23, 10
  br i1 %24, label %25, label %37

25:                                               ; preds = %22
  %26 = load i32, ptr %6, align 4
  %27 = sext i32 %26 to i64
  %28 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %27
  %29 = load i32, ptr %28, align 4
  %30 = add nsw i32 %29, 1
  %31 = load i32, ptr %6, align 4
  %32 = sext i32 %31 to i64
  %33 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 %32
  store i32 %30, ptr %33, align 4
  br label %34

34:                                               ; preds = %25
  %35 = load i32, ptr %6, align 4
  %36 = add nsw i32 %35, 1
  store i32 %36, ptr %6, align 4
  br label %22, !llvm.loop !12

37:                                               ; preds = %22
  br label %38

38:                                               ; preds = %37, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @not_adjacent_test(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca [10 x i32], align 16
  %4 = alloca [10 x i32], align 16
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %7 = load i32, ptr %2, align 4
  %8 = icmp sgt i32 %7, 10
  br i1 %8, label %9, label %36

9:                                                ; preds = %1
  store i32 0, ptr %5, align 4
  br label %10

10:                                               ; preds = %18, %9
  %11 = load i32, ptr %5, align 4
  %12 = icmp slt i32 %11, 10
  br i1 %12, label %13, label %21

13:                                               ; preds = %10
  %14 = load i32, ptr %5, align 4
  %15 = load i32, ptr %5, align 4
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %16
  store i32 %14, ptr %17, align 4
  br label %18

18:                                               ; preds = %13
  %19 = load i32, ptr %5, align 4
  %20 = add nsw i32 %19, 1
  store i32 %20, ptr %5, align 4
  br label %10, !llvm.loop !13

21:                                               ; preds = %10
  %22 = call i32 (ptr, ...) @printf(ptr noundef @.str)
  store i32 0, ptr %6, align 4
  br label %23

23:                                               ; preds = %32, %21
  %24 = load i32, ptr %6, align 4
  %25 = icmp slt i32 %24, 10
  br i1 %25, label %26, label %35

26:                                               ; preds = %23
  %27 = load i32, ptr %6, align 4
  %28 = add nsw i32 %27, 1
  %29 = load i32, ptr %6, align 4
  %30 = sext i32 %29 to i64
  %31 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 %30
  store i32 %28, ptr %31, align 4
  br label %32

32:                                               ; preds = %26
  %33 = load i32, ptr %6, align 4
  %34 = add nsw i32 %33, 1
  store i32 %34, ptr %6, align 4
  br label %23, !llvm.loop !14

35:                                               ; preds = %23
  br label %36

36:                                               ; preds = %35, %1
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @different_trip_count_test(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca [10 x i32], align 16
  %4 = alloca [10 x i32], align 16
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %7 = load i32, ptr %2, align 4
  %8 = icmp sgt i32 %7, 10
  br i1 %8, label %9, label %35

9:                                                ; preds = %1
  store i32 0, ptr %5, align 4
  br label %10

10:                                               ; preds = %18, %9
  %11 = load i32, ptr %5, align 4
  %12 = icmp slt i32 %11, 10
  br i1 %12, label %13, label %21

13:                                               ; preds = %10
  %14 = load i32, ptr %5, align 4
  %15 = load i32, ptr %5, align 4
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %16
  store i32 %14, ptr %17, align 4
  br label %18

18:                                               ; preds = %13
  %19 = load i32, ptr %5, align 4
  %20 = add nsw i32 %19, 1
  store i32 %20, ptr %5, align 4
  br label %10, !llvm.loop !15

21:                                               ; preds = %10
  store i32 0, ptr %6, align 4
  br label %22

22:                                               ; preds = %31, %21
  %23 = load i32, ptr %6, align 4
  %24 = icmp slt i32 %23, 8
  br i1 %24, label %25, label %34

25:                                               ; preds = %22
  %26 = load i32, ptr %6, align 4
  %27 = add nsw i32 %26, 1
  %28 = load i32, ptr %6, align 4
  %29 = sext i32 %28 to i64
  %30 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 %29
  store i32 %27, ptr %30, align 4
  br label %31

31:                                               ; preds = %25
  %32 = load i32, ptr %6, align 4
  %33 = add nsw i32 %32, 1
  store i32 %33, ptr %6, align 4
  br label %22, !llvm.loop !16

34:                                               ; preds = %22
  br label %35

35:                                               ; preds = %34, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @not_control_flow_equivalent_test(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca [10 x i32], align 16
  %4 = alloca [10 x i32], align 16
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %7 = load i32, ptr %2, align 4
  %8 = icmp sgt i32 %7, 10
  br i1 %8, label %9, label %22

9:                                                ; preds = %1
  store i32 0, ptr %5, align 4
  br label %10

10:                                               ; preds = %18, %9
  %11 = load i32, ptr %5, align 4
  %12 = icmp slt i32 %11, 10
  br i1 %12, label %13, label %21

13:                                               ; preds = %10
  %14 = load i32, ptr %5, align 4
  %15 = load i32, ptr %5, align 4
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %16
  store i32 %14, ptr %17, align 4
  br label %18

18:                                               ; preds = %13
  %19 = load i32, ptr %5, align 4
  %20 = add nsw i32 %19, 1
  store i32 %20, ptr %5, align 4
  br label %10, !llvm.loop !17

21:                                               ; preds = %10
  br label %22

22:                                               ; preds = %21, %1
  store i32 0, ptr %6, align 4
  br label %23

23:                                               ; preds = %32, %22
  %24 = load i32, ptr %6, align 4
  %25 = icmp slt i32 %24, 10
  br i1 %25, label %26, label %35

26:                                               ; preds = %23
  %27 = load i32, ptr %6, align 4
  %28 = add nsw i32 %27, 1
  %29 = load i32, ptr %6, align 4
  %30 = sext i32 %29 to i64
  %31 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 %30
  store i32 %28, ptr %31, align 4
  br label %32

32:                                               ; preds = %26
  %33 = load i32, ptr %6, align 4
  %34 = add nsw i32 %33, 1
  store i32 %34, ptr %6, align 4
  br label %23, !llvm.loop !18

35:                                               ; preds = %23
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @two_separate_guards_same_condition_test(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca [10 x i32], align 16
  %4 = alloca [10 x i32], align 16
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %7 = load i32, ptr %2, align 4
  %8 = icmp sgt i32 %7, 10
  br i1 %8, label %9, label %22

9:                                                ; preds = %1
  store i32 0, ptr %5, align 4
  br label %10

10:                                               ; preds = %18, %9
  %11 = load i32, ptr %5, align 4
  %12 = icmp slt i32 %11, 10
  br i1 %12, label %13, label %21

13:                                               ; preds = %10
  %14 = load i32, ptr %5, align 4
  %15 = load i32, ptr %5, align 4
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %16
  store i32 %14, ptr %17, align 4
  br label %18

18:                                               ; preds = %13
  %19 = load i32, ptr %5, align 4
  %20 = add nsw i32 %19, 1
  store i32 %20, ptr %5, align 4
  br label %10, !llvm.loop !19

21:                                               ; preds = %10
  br label %22

22:                                               ; preds = %21, %1
  %23 = load i32, ptr %2, align 4
  %24 = icmp sgt i32 %23, 10
  br i1 %24, label %25, label %39

25:                                               ; preds = %22
  store i32 0, ptr %6, align 4
  br label %26

26:                                               ; preds = %35, %25
  %27 = load i32, ptr %6, align 4
  %28 = icmp slt i32 %27, 10
  br i1 %28, label %29, label %38

29:                                               ; preds = %26
  %30 = load i32, ptr %6, align 4
  %31 = add nsw i32 %30, 1
  %32 = load i32, ptr %6, align 4
  %33 = sext i32 %32 to i64
  %34 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 %33
  store i32 %31, ptr %34, align 4
  br label %35

35:                                               ; preds = %29
  %36 = load i32, ptr %6, align 4
  %37 = add nsw i32 %36, 1
  store i32 %37, ptr %6, align 4
  br label %26, !llvm.loop !20

38:                                               ; preds = %26
  br label %39

39:                                               ; preds = %38, %22
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @two_separate_guards_different_conditions_test(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca [10 x i32], align 16
  %6 = alloca [10 x i32], align 16
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 %1, ptr %4, align 4
  %9 = load i32, ptr %3, align 4
  %10 = icmp sgt i32 %9, 10
  br i1 %10, label %11, label %24

11:                                               ; preds = %2
  store i32 0, ptr %7, align 4
  br label %12

12:                                               ; preds = %20, %11
  %13 = load i32, ptr %7, align 4
  %14 = icmp slt i32 %13, 10
  br i1 %14, label %15, label %23

15:                                               ; preds = %12
  %16 = load i32, ptr %7, align 4
  %17 = load i32, ptr %7, align 4
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds [10 x i32], ptr %5, i64 0, i64 %18
  store i32 %16, ptr %19, align 4
  br label %20

20:                                               ; preds = %15
  %21 = load i32, ptr %7, align 4
  %22 = add nsw i32 %21, 1
  store i32 %22, ptr %7, align 4
  br label %12, !llvm.loop !21

23:                                               ; preds = %12
  br label %24

24:                                               ; preds = %23, %2
  %25 = load i32, ptr %4, align 4
  %26 = icmp sgt i32 %25, 10
  br i1 %26, label %27, label %41

27:                                               ; preds = %24
  store i32 0, ptr %8, align 4
  br label %28

28:                                               ; preds = %37, %27
  %29 = load i32, ptr %8, align 4
  %30 = icmp slt i32 %29, 10
  br i1 %30, label %31, label %40

31:                                               ; preds = %28
  %32 = load i32, ptr %8, align 4
  %33 = add nsw i32 %32, 1
  %34 = load i32, ptr %8, align 4
  %35 = sext i32 %34 to i64
  %36 = getelementptr inbounds [10 x i32], ptr %6, i64 0, i64 %35
  store i32 %33, ptr %36, align 4
  br label %37

37:                                               ; preds = %31
  %38 = load i32, ptr %8, align 4
  %39 = add nsw i32 %38, 1
  store i32 %39, ptr %8, align 4
  br label %28, !llvm.loop !22

40:                                               ; preds = %28
  br label %41

41:                                               ; preds = %40, %24
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @backward_read_not_negative_test(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca [10 x i32], align 16
  %4 = alloca [10 x i32], align 16
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %7 = load i32, ptr %2, align 4
  %8 = icmp sgt i32 %7, 10
  br i1 %8, label %9, label %38

9:                                                ; preds = %1
  store i32 0, ptr %5, align 4
  br label %10

10:                                               ; preds = %18, %9
  %11 = load i32, ptr %5, align 4
  %12 = icmp slt i32 %11, 9
  br i1 %12, label %13, label %21

13:                                               ; preds = %10
  %14 = load i32, ptr %5, align 4
  %15 = load i32, ptr %5, align 4
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %16
  store i32 %14, ptr %17, align 4
  br label %18

18:                                               ; preds = %13
  %19 = load i32, ptr %5, align 4
  %20 = add nsw i32 %19, 1
  store i32 %20, ptr %5, align 4
  br label %10, !llvm.loop !23

21:                                               ; preds = %10
  store i32 0, ptr %6, align 4
  br label %22

22:                                               ; preds = %34, %21
  %23 = load i32, ptr %6, align 4
  %24 = icmp slt i32 %23, 9
  br i1 %24, label %25, label %37

25:                                               ; preds = %22
  %26 = load i32, ptr %6, align 4
  %27 = sext i32 %26 to i64
  %28 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %27
  %29 = load i32, ptr %28, align 4
  %30 = add nsw i32 %29, 1
  %31 = load i32, ptr %6, align 4
  %32 = sext i32 %31 to i64
  %33 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 %32
  store i32 %30, ptr %33, align 4
  br label %34

34:                                               ; preds = %25
  %35 = load i32, ptr %6, align 4
  %36 = add nsw i32 %35, 1
  store i32 %36, ptr %6, align 4
  br label %22, !llvm.loop !24

37:                                               ; preds = %22
  br label %38

38:                                               ; preds = %37, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @negative_dep_plus_three_test(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca [13 x i32], align 16
  %4 = alloca [10 x i32], align 16
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %7 = load i32, ptr %2, align 4
  %8 = icmp sgt i32 %7, 10
  br i1 %8, label %9, label %39

9:                                                ; preds = %1
  store i32 0, ptr %5, align 4
  br label %10

10:                                               ; preds = %18, %9
  %11 = load i32, ptr %5, align 4
  %12 = icmp slt i32 %11, 10
  br i1 %12, label %13, label %21

13:                                               ; preds = %10
  %14 = load i32, ptr %5, align 4
  %15 = load i32, ptr %5, align 4
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds [13 x i32], ptr %3, i64 0, i64 %16
  store i32 %14, ptr %17, align 4
  br label %18

18:                                               ; preds = %13
  %19 = load i32, ptr %5, align 4
  %20 = add nsw i32 %19, 1
  store i32 %20, ptr %5, align 4
  br label %10, !llvm.loop !25

21:                                               ; preds = %10
  store i32 0, ptr %6, align 4
  br label %22

22:                                               ; preds = %35, %21
  %23 = load i32, ptr %6, align 4
  %24 = icmp slt i32 %23, 10
  br i1 %24, label %25, label %38

25:                                               ; preds = %22
  %26 = load i32, ptr %6, align 4
  %27 = add nsw i32 %26, 3
  %28 = sext i32 %27 to i64
  %29 = getelementptr inbounds [13 x i32], ptr %3, i64 0, i64 %28
  %30 = load i32, ptr %29, align 4
  %31 = add nsw i32 %30, 1
  %32 = load i32, ptr %6, align 4
  %33 = sext i32 %32 to i64
  %34 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 %33
  store i32 %31, ptr %34, align 4
  br label %35

35:                                               ; preds = %25
  %36 = load i32, ptr %6, align 4
  %37 = add nsw i32 %36, 1
  store i32 %37, ptr %6, align 4
  br label %22, !llvm.loop !26

38:                                               ; preds = %22
  br label %39

39:                                               ; preds = %38, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @pointer_may_alias_unknown_test(ptr noundef %0, ptr noundef %1, i32 noundef %2) #0 {
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  store ptr %0, ptr %4, align 8
  store ptr %1, ptr %5, align 8
  store i32 %2, ptr %6, align 4
  %9 = load i32, ptr %6, align 4
  %10 = icmp sgt i32 %9, 10
  br i1 %10, label %11, label %43

11:                                               ; preds = %3
  store i32 0, ptr %7, align 4
  br label %12

12:                                               ; preds = %21, %11
  %13 = load i32, ptr %7, align 4
  %14 = icmp slt i32 %13, 10
  br i1 %14, label %15, label %24

15:                                               ; preds = %12
  %16 = load i32, ptr %7, align 4
  %17 = load ptr, ptr %4, align 8
  %18 = load i32, ptr %7, align 4
  %19 = sext i32 %18 to i64
  %20 = getelementptr inbounds i32, ptr %17, i64 %19
  store i32 %16, ptr %20, align 4
  br label %21

21:                                               ; preds = %15
  %22 = load i32, ptr %7, align 4
  %23 = add nsw i32 %22, 1
  store i32 %23, ptr %7, align 4
  br label %12, !llvm.loop !27

24:                                               ; preds = %12
  store i32 0, ptr %8, align 4
  br label %25

25:                                               ; preds = %39, %24
  %26 = load i32, ptr %8, align 4
  %27 = icmp slt i32 %26, 10
  br i1 %27, label %28, label %42

28:                                               ; preds = %25
  %29 = load ptr, ptr %5, align 8
  %30 = load i32, ptr %8, align 4
  %31 = sext i32 %30 to i64
  %32 = getelementptr inbounds i32, ptr %29, i64 %31
  %33 = load i32, ptr %32, align 4
  %34 = add nsw i32 %33, 1
  %35 = load ptr, ptr %5, align 8
  %36 = load i32, ptr %8, align 4
  %37 = sext i32 %36 to i64
  %38 = getelementptr inbounds i32, ptr %35, i64 %37
  store i32 %34, ptr %38, align 4
  br label %39

39:                                               ; preds = %28
  %40 = load i32, ptr %8, align 4
  %41 = add nsw i32 %40, 1
  store i32 %41, ptr %8, align 4
  br label %25, !llvm.loop !28

42:                                               ; preds = %25
  br label %43

43:                                               ; preds = %42, %3
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @non_guarded_fusible_test() #0 {
  %1 = alloca [10 x i32], align 16
  %2 = alloca [10 x i32], align 16
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 0, ptr %3, align 4
  br label %5

5:                                                ; preds = %13, %0
  %6 = load i32, ptr %3, align 4
  %7 = icmp slt i32 %6, 10
  br i1 %7, label %8, label %16

8:                                                ; preds = %5
  %9 = load i32, ptr %3, align 4
  %10 = load i32, ptr %3, align 4
  %11 = sext i32 %10 to i64
  %12 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %11
  store i32 %9, ptr %12, align 4
  br label %13

13:                                               ; preds = %8
  %14 = load i32, ptr %3, align 4
  %15 = add nsw i32 %14, 1
  store i32 %15, ptr %3, align 4
  br label %5, !llvm.loop !29

16:                                               ; preds = %5
  store i32 0, ptr %4, align 4
  br label %17

17:                                               ; preds = %26, %16
  %18 = load i32, ptr %4, align 4
  %19 = icmp slt i32 %18, 10
  br i1 %19, label %20, label %29

20:                                               ; preds = %17
  %21 = load i32, ptr %4, align 4
  %22 = add nsw i32 %21, 1
  %23 = load i32, ptr %4, align 4
  %24 = sext i32 %23 to i64
  %25 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %24
  store i32 %22, ptr %25, align 4
  br label %26

26:                                               ; preds = %20
  %27 = load i32, ptr %4, align 4
  %28 = add nsw i32 %27, 1
  store i32 %28, ptr %4, align 4
  br label %17, !llvm.loop !30

29:                                               ; preds = %17
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @non_guarded_not_adjacent_test() #0 {
  %1 = alloca [10 x i32], align 16
  %2 = alloca [10 x i32], align 16
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 0, ptr %3, align 4
  br label %5

5:                                                ; preds = %13, %0
  %6 = load i32, ptr %3, align 4
  %7 = icmp slt i32 %6, 10
  br i1 %7, label %8, label %16

8:                                                ; preds = %5
  %9 = load i32, ptr %3, align 4
  %10 = load i32, ptr %3, align 4
  %11 = sext i32 %10 to i64
  %12 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %11
  store i32 %9, ptr %12, align 4
  br label %13

13:                                               ; preds = %8
  %14 = load i32, ptr %3, align 4
  %15 = add nsw i32 %14, 1
  store i32 %15, ptr %3, align 4
  br label %5, !llvm.loop !31

16:                                               ; preds = %5
  %17 = call i32 (ptr, ...) @printf(ptr noundef @.str)
  store i32 0, ptr %4, align 4
  br label %18

18:                                               ; preds = %27, %16
  %19 = load i32, ptr %4, align 4
  %20 = icmp slt i32 %19, 10
  br i1 %20, label %21, label %30

21:                                               ; preds = %18
  %22 = load i32, ptr %4, align 4
  %23 = add nsw i32 %22, 1
  %24 = load i32, ptr %4, align 4
  %25 = sext i32 %24 to i64
  %26 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %25
  store i32 %23, ptr %26, align 4
  br label %27

27:                                               ; preds = %21
  %28 = load i32, ptr %4, align 4
  %29 = add nsw i32 %28, 1
  store i32 %29, ptr %4, align 4
  br label %18, !llvm.loop !32

30:                                               ; preds = %18
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @fusible_decreasing(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca [11 x i32], align 16
  %4 = alloca [11 x i32], align 16
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %7 = load i32, ptr %2, align 4
  %8 = icmp sgt i32 %7, 10
  br i1 %8, label %9, label %38

9:                                                ; preds = %1
  store i32 10, ptr %5, align 4
  br label %10

10:                                               ; preds = %18, %9
  %11 = load i32, ptr %5, align 4
  %12 = icmp sgt i32 %11, 0
  br i1 %12, label %13, label %21

13:                                               ; preds = %10
  %14 = load i32, ptr %5, align 4
  %15 = load i32, ptr %5, align 4
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds [11 x i32], ptr %3, i64 0, i64 %16
  store i32 %14, ptr %17, align 4
  br label %18

18:                                               ; preds = %13
  %19 = load i32, ptr %5, align 4
  %20 = add nsw i32 %19, -1
  store i32 %20, ptr %5, align 4
  br label %10, !llvm.loop !33

21:                                               ; preds = %10
  store i32 10, ptr %6, align 4
  br label %22

22:                                               ; preds = %34, %21
  %23 = load i32, ptr %6, align 4
  %24 = icmp sgt i32 %23, 0
  br i1 %24, label %25, label %37

25:                                               ; preds = %22
  %26 = load i32, ptr %6, align 4
  %27 = sext i32 %26 to i64
  %28 = getelementptr inbounds [11 x i32], ptr %3, i64 0, i64 %27
  %29 = load i32, ptr %28, align 4
  %30 = add nsw i32 %29, 1
  %31 = load i32, ptr %6, align 4
  %32 = sext i32 %31 to i64
  %33 = getelementptr inbounds [11 x i32], ptr %4, i64 0, i64 %32
  store i32 %30, ptr %33, align 4
  br label %34

34:                                               ; preds = %25
  %35 = load i32, ptr %6, align 4
  %36 = add nsw i32 %35, -1
  store i32 %36, ptr %6, align 4
  br label %22, !llvm.loop !34

37:                                               ; preds = %22
  br label %38

38:                                               ; preds = %37, %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @negative_decreasing(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca [11 x i32], align 16
  %4 = alloca [11 x i32], align 16
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %7 = load i32, ptr %2, align 4
  %8 = icmp sgt i32 %7, 10
  br i1 %8, label %9, label %39

9:                                                ; preds = %1
  store i32 10, ptr %5, align 4
  br label %10

10:                                               ; preds = %18, %9
  %11 = load i32, ptr %5, align 4
  %12 = icmp sgt i32 %11, 0
  br i1 %12, label %13, label %21

13:                                               ; preds = %10
  %14 = load i32, ptr %5, align 4
  %15 = load i32, ptr %5, align 4
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds [11 x i32], ptr %3, i64 0, i64 %16
  store i32 %14, ptr %17, align 4
  br label %18

18:                                               ; preds = %13
  %19 = load i32, ptr %5, align 4
  %20 = add nsw i32 %19, -1
  store i32 %20, ptr %5, align 4
  br label %10, !llvm.loop !35

21:                                               ; preds = %10
  store i32 10, ptr %6, align 4
  br label %22

22:                                               ; preds = %35, %21
  %23 = load i32, ptr %6, align 4
  %24 = icmp sgt i32 %23, 0
  br i1 %24, label %25, label %38

25:                                               ; preds = %22
  %26 = load i32, ptr %6, align 4
  %27 = sub nsw i32 %26, 1
  %28 = sext i32 %27 to i64
  %29 = getelementptr inbounds [11 x i32], ptr %3, i64 0, i64 %28
  %30 = load i32, ptr %29, align 4
  %31 = add nsw i32 %30, 1
  %32 = load i32, ptr %6, align 4
  %33 = sext i32 %32 to i64
  %34 = getelementptr inbounds [11 x i32], ptr %4, i64 0, i64 %33
  store i32 %31, ptr %34, align 4
  br label %35

35:                                               ; preds = %25
  %36 = load i32, ptr %6, align 4
  %37 = add nsw i32 %36, -1
  store i32 %37, ptr %6, align 4
  br label %22, !llvm.loop !36

38:                                               ; preds = %22
  br label %39

39:                                               ; preds = %38, %1
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
