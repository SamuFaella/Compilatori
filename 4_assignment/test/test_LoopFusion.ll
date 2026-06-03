; ModuleID = 'test/test_LoopFusion.c'
source_filename = "test/test_LoopFusion.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [25 x i8] c"L0 guarded adjacent: %d\0A\00", align 1
@.str.1 = private unnamed_addr constant [15 x i8] c"Between loops\0A\00", align 1
@.str.2 = private unnamed_addr constant [25 x i8] c"L1 guarded adjacent: %d\0A\00", align 1

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
  br i1 %8, label %9, label %21

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
  %20 = call i32 (ptr, ...) @printf(ptr noundef @.str.1)
  br label %21

21:                                               ; preds = %19, %2
  store i32 0, ptr %6, align 4
  br label %22

22:                                               ; preds = %28, %21
  %23 = load i32, ptr %6, align 4
  %24 = icmp slt i32 %23, 10
  br i1 %24, label %25, label %31

25:                                               ; preds = %22
  %26 = load i32, ptr %6, align 4
  %27 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %26)
  br label %28

28:                                               ; preds = %25
  %29 = load i32, ptr %6, align 4
  %30 = add nsw i32 %29, 1
  store i32 %30, ptr %6, align 4
  br label %22, !llvm.loop !8

31:                                               ; preds = %22
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
