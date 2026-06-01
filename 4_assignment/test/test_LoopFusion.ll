; ModuleID = 'test/test_LoopFusion.c'
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
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 %1, ptr %4, align 4
  %7 = load i32, ptr %3, align 4
  %8 = icmp sgt i32 %7, 0
  br i1 %8, label %9, label %20

9:                                                ; preds = %2
  store i32 0, ptr %5, align 4
  br label %10

10:                                               ; preds = %16, %9
  %11 = load i32, ptr %5, align 4
  %12 = icmp slt i32 %11, 10
  br i1 %12, label %13, label %19

13:                                               ; preds = %10
  %14 = load i32, ptr %5, align 4
  %15 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %14)
  br label %16

16:                                               ; preds = %13
  %17 = load i32, ptr %5, align 4
  %18 = add nsw i32 %17, 1
  store i32 %18, ptr %5, align 4
  br label %10, !llvm.loop !6

19:                                               ; preds = %10
  br label %20

20:                                               ; preds = %19, %2
  %21 = load i32, ptr %4, align 4
  %22 = icmp sgt i32 %21, 0
  br i1 %22, label %23, label %34

23:                                               ; preds = %20
  store i32 0, ptr %6, align 4
  br label %24

24:                                               ; preds = %30, %23
  %25 = load i32, ptr %6, align 4
  %26 = icmp slt i32 %25, 10
  br i1 %26, label %27, label %33

27:                                               ; preds = %24
  %28 = load i32, ptr %6, align 4
  %29 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %28)
  br label %30

30:                                               ; preds = %27
  %31 = load i32, ptr %6, align 4
  %32 = add nsw i32 %31, 1
  store i32 %32, ptr %6, align 4
  br label %24, !llvm.loop !8

33:                                               ; preds = %24
  br label %34

34:                                               ; preds = %33, %20
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @l0_guarded_l1_not_guarded_not_adjacent(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %5 = load i32, ptr %2, align 4
  %6 = icmp sgt i32 %5, 0
  br i1 %6, label %7, label %18

7:                                                ; preds = %1
  store i32 0, ptr %3, align 4
  br label %8

8:                                                ; preds = %14, %7
  %9 = load i32, ptr %3, align 4
  %10 = icmp slt i32 %9, 10
  br i1 %10, label %11, label %17

11:                                               ; preds = %8
  %12 = load i32, ptr %3, align 4
  %13 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %12)
  br label %14

14:                                               ; preds = %11
  %15 = load i32, ptr %3, align 4
  %16 = add nsw i32 %15, 1
  store i32 %16, ptr %3, align 4
  br label %8, !llvm.loop !9

17:                                               ; preds = %8
  br label %18

18:                                               ; preds = %17, %1
  %19 = call i32 @puts(ptr noundef @.str.3)
  store i32 0, ptr %4, align 4
  br label %20

20:                                               ; preds = %26, %18
  %21 = load i32, ptr %4, align 4
  %22 = icmp slt i32 %21, 10
  br i1 %22, label %23, label %29

23:                                               ; preds = %20
  %24 = load i32, ptr %4, align 4
  %25 = call i32 (ptr, ...) @printf(ptr noundef @.str.4, i32 noundef %24)
  br label %26

26:                                               ; preds = %23
  %27 = load i32, ptr %4, align 4
  %28 = add nsw i32 %27, 1
  store i32 %28, ptr %4, align 4
  br label %20, !llvm.loop !10

29:                                               ; preds = %20
  ret void
}

declare i32 @puts(ptr noundef) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @l0_guarded_l1_not_guarded_adjacent(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %5 = load i32, ptr %2, align 4
  %6 = icmp sgt i32 %5, 0
  br i1 %6, label %7, label %18

7:                                                ; preds = %1
  store i32 0, ptr %3, align 4
  br label %8

8:                                                ; preds = %14, %7
  %9 = load i32, ptr %3, align 4
  %10 = icmp slt i32 %9, 10
  br i1 %10, label %11, label %17

11:                                               ; preds = %8
  %12 = load i32, ptr %3, align 4
  %13 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %12)
  br label %14

14:                                               ; preds = %11
  %15 = load i32, ptr %3, align 4
  %16 = add nsw i32 %15, 1
  store i32 %16, ptr %3, align 4
  br label %8, !llvm.loop !11

17:                                               ; preds = %8
  br label %18

18:                                               ; preds = %17, %1
  store i32 0, ptr %4, align 4
  br label %19

19:                                               ; preds = %25, %18
  %20 = load i32, ptr %4, align 4
  %21 = icmp slt i32 %20, 10
  br i1 %21, label %22, label %28

22:                                               ; preds = %19
  %23 = load i32, ptr %4, align 4
  %24 = call i32 (ptr, ...) @printf(ptr noundef @.str.5, i32 noundef %23)
  br label %25

25:                                               ; preds = %22
  %26 = load i32, ptr %4, align 4
  %27 = add nsw i32 %26, 1
  store i32 %27, ptr %4, align 4
  br label %19, !llvm.loop !12

28:                                               ; preds = %19
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @l0_not_guarded_l1_guarded_adjacent(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  store i32 0, ptr %3, align 4
  br label %5

5:                                                ; preds = %11, %1
  %6 = load i32, ptr %3, align 4
  %7 = icmp slt i32 %6, 10
  br i1 %7, label %8, label %14

8:                                                ; preds = %5
  %9 = load i32, ptr %3, align 4
  %10 = call i32 (ptr, ...) @printf(ptr noundef @.str.6, i32 noundef %9)
  br label %11

11:                                               ; preds = %8
  %12 = load i32, ptr %3, align 4
  %13 = add nsw i32 %12, 1
  store i32 %13, ptr %3, align 4
  br label %5, !llvm.loop !13

14:                                               ; preds = %5
  %15 = load i32, ptr %2, align 4
  %16 = icmp sgt i32 %15, 0
  br i1 %16, label %17, label %28

17:                                               ; preds = %14
  store i32 0, ptr %4, align 4
  br label %18

18:                                               ; preds = %24, %17
  %19 = load i32, ptr %4, align 4
  %20 = icmp slt i32 %19, 10
  br i1 %20, label %21, label %27

21:                                               ; preds = %18
  %22 = load i32, ptr %4, align 4
  %23 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %22)
  br label %24

24:                                               ; preds = %21
  %25 = load i32, ptr %4, align 4
  %26 = add nsw i32 %25, 1
  store i32 %26, ptr %4, align 4
  br label %18, !llvm.loop !14

27:                                               ; preds = %18
  br label %28

28:                                               ; preds = %27, %14
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @l0_not_guarded_l1_guarded_not_adjacent(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  store i32 0, ptr %3, align 4
  br label %5

5:                                                ; preds = %11, %1
  %6 = load i32, ptr %3, align 4
  %7 = icmp slt i32 %6, 10
  br i1 %7, label %8, label %14

8:                                                ; preds = %5
  %9 = load i32, ptr %3, align 4
  %10 = call i32 (ptr, ...) @printf(ptr noundef @.str.7, i32 noundef %9)
  br label %11

11:                                               ; preds = %8
  %12 = load i32, ptr %3, align 4
  %13 = add nsw i32 %12, 1
  store i32 %13, ptr %3, align 4
  br label %5, !llvm.loop !15

14:                                               ; preds = %5
  %15 = call i32 @puts(ptr noundef @.str.3)
  %16 = load i32, ptr %2, align 4
  %17 = icmp sgt i32 %16, 0
  br i1 %17, label %18, label %29

18:                                               ; preds = %14
  store i32 0, ptr %4, align 4
  br label %19

19:                                               ; preds = %25, %18
  %20 = load i32, ptr %4, align 4
  %21 = icmp slt i32 %20, 10
  br i1 %21, label %22, label %28

22:                                               ; preds = %19
  %23 = load i32, ptr %4, align 4
  %24 = call i32 (ptr, ...) @printf(ptr noundef @.str.8, i32 noundef %23)
  br label %25

25:                                               ; preds = %22
  %26 = load i32, ptr %4, align 4
  %27 = add nsw i32 %26, 1
  store i32 %27, ptr %4, align 4
  br label %19, !llvm.loop !16

28:                                               ; preds = %19
  br label %29

29:                                               ; preds = %28, %14
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @both_guarded_not_adjacent(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 %1, ptr %4, align 4
  %7 = load i32, ptr %3, align 4
  %8 = icmp sgt i32 %7, 0
  br i1 %8, label %9, label %20

9:                                                ; preds = %2
  store i32 0, ptr %5, align 4
  br label %10

10:                                               ; preds = %16, %9
  %11 = load i32, ptr %5, align 4
  %12 = icmp slt i32 %11, 10
  br i1 %12, label %13, label %19

13:                                               ; preds = %10
  %14 = load i32, ptr %5, align 4
  %15 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %14)
  br label %16

16:                                               ; preds = %13
  %17 = load i32, ptr %5, align 4
  %18 = add nsw i32 %17, 1
  store i32 %18, ptr %5, align 4
  br label %10, !llvm.loop !17

19:                                               ; preds = %10
  br label %20

20:                                               ; preds = %19, %2
  %21 = call i32 @puts(ptr noundef @.str.9)
  %22 = load i32, ptr %4, align 4
  %23 = icmp sgt i32 %22, 0
  br i1 %23, label %24, label %35

24:                                               ; preds = %20
  store i32 0, ptr %6, align 4
  br label %25

25:                                               ; preds = %31, %24
  %26 = load i32, ptr %6, align 4
  %27 = icmp slt i32 %26, 10
  br i1 %27, label %28, label %34

28:                                               ; preds = %25
  %29 = load i32, ptr %6, align 4
  %30 = call i32 (ptr, ...) @printf(ptr noundef @.str.8, i32 noundef %29)
  br label %31

31:                                               ; preds = %28
  %32 = load i32, ptr %6, align 4
  %33 = add nsw i32 %32, 1
  store i32 %33, ptr %6, align 4
  br label %25, !llvm.loop !18

34:                                               ; preds = %25
  br label %35

35:                                               ; preds = %34, %20
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @both_not_guarded_adjacent() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  br label %3

3:                                                ; preds = %9, %0
  %4 = load i32, ptr %1, align 4
  %5 = icmp slt i32 %4, 10
  br i1 %5, label %6, label %12

6:                                                ; preds = %3
  %7 = load i32, ptr %1, align 4
  %8 = call i32 (ptr, ...) @printf(ptr noundef @.str.6, i32 noundef %7)
  br label %9

9:                                                ; preds = %6
  %10 = load i32, ptr %1, align 4
  %11 = add nsw i32 %10, 1
  store i32 %11, ptr %1, align 4
  br label %3, !llvm.loop !19

12:                                               ; preds = %3
  store i32 0, ptr %2, align 4
  br label %13

13:                                               ; preds = %19, %12
  %14 = load i32, ptr %2, align 4
  %15 = icmp slt i32 %14, 10
  br i1 %15, label %16, label %22

16:                                               ; preds = %13
  %17 = load i32, ptr %2, align 4
  %18 = call i32 (ptr, ...) @printf(ptr noundef @.str.5, i32 noundef %17)
  br label %19

19:                                               ; preds = %16
  %20 = load i32, ptr %2, align 4
  %21 = add nsw i32 %20, 1
  store i32 %21, ptr %2, align 4
  br label %13, !llvm.loop !20

22:                                               ; preds = %13
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @both_not_guarded_not_adjacent() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  br label %3

3:                                                ; preds = %9, %0
  %4 = load i32, ptr %1, align 4
  %5 = icmp slt i32 %4, 10
  br i1 %5, label %6, label %12

6:                                                ; preds = %3
  %7 = load i32, ptr %1, align 4
  %8 = call i32 (ptr, ...) @printf(ptr noundef @.str.7, i32 noundef %7)
  br label %9

9:                                                ; preds = %6
  %10 = load i32, ptr %1, align 4
  %11 = add nsw i32 %10, 1
  store i32 %11, ptr %1, align 4
  br label %3, !llvm.loop !21

12:                                               ; preds = %3
  %13 = call i32 @puts(ptr noundef @.str.3)
  store i32 0, ptr %2, align 4
  br label %14

14:                                               ; preds = %20, %12
  %15 = load i32, ptr %2, align 4
  %16 = icmp slt i32 %15, 10
  br i1 %16, label %17, label %23

17:                                               ; preds = %14
  %18 = load i32, ptr %2, align 4
  %19 = call i32 (ptr, ...) @printf(ptr noundef @.str.4, i32 noundef %18)
  br label %20

20:                                               ; preds = %17
  %21 = load i32, ptr %2, align 4
  %22 = add nsw i32 %21, 1
  store i32 %22, ptr %2, align 4
  br label %14, !llvm.loop !22

23:                                               ; preds = %14
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
