; ModuleID = 'test/LICM.c'
source_filename = "test/LICM.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local double @complex_test(i32 noundef %0, double noundef %1, double noundef %2, ptr noundef %3) #0 {
  %5 = alloca i32, align 4
  %6 = alloca double, align 8
  %7 = alloca double, align 8
  %8 = alloca ptr, align 8
  %9 = alloca double, align 8
  %10 = alloca double, align 8
  %11 = alloca i32, align 4
  %12 = alloca double, align 8
  %13 = alloca double, align 8
  %14 = alloca double, align 8
  %15 = alloca double, align 8
  %16 = alloca double, align 8
  store i32 %0, ptr %5, align 4
  store double %1, ptr %6, align 8
  store double %2, ptr %7, align 8
  store ptr %3, ptr %8, align 8
  store double 0.000000e+00, ptr %9, align 8
  store double 1.000000e+00, ptr %10, align 8
  store i32 0, ptr %11, align 4
  br label %17

17:                                               ; preds = %51, %4
  %18 = load i32, ptr %11, align 4
  %19 = load i32, ptr %5, align 4
  %20 = icmp slt i32 %18, %19
  br i1 %20, label %21, label %54

21:                                               ; preds = %17
  %22 = load double, ptr %6, align 8
  %23 = fmul double %22, 2.000000e+00
  store double %23, ptr %12, align 8
  %24 = load double, ptr %7, align 8
  %25 = fadd double %24, 3.000000e+00
  store double %25, ptr %13, align 8
  %26 = load double, ptr %12, align 8
  %27 = load double, ptr %13, align 8
  %28 = fadd double %26, %27
  store double %28, ptr %14, align 8
  %29 = load i32, ptr %11, align 4
  %30 = sitofp i32 %29 to double
  %31 = fmul double %30, 4.000000e+00
  store double %31, ptr %15, align 8
  %32 = load double, ptr %14, align 8
  %33 = load i32, ptr %11, align 4
  %34 = sitofp i32 %33 to double
  %35 = fmul double %32, %34
  store double %35, ptr %16, align 8
  %36 = load double, ptr %16, align 8
  %37 = load ptr, ptr %8, align 8
  %38 = load i32, ptr %11, align 4
  %39 = sext i32 %38 to i64
  %40 = getelementptr inbounds double, ptr %37, i64 %39
  store double %36, ptr %40, align 8
  %41 = load ptr, ptr %8, align 8
  %42 = load i32, ptr %11, align 4
  %43 = sext i32 %42 to i64
  %44 = getelementptr inbounds double, ptr %41, i64 %43
  %45 = load double, ptr %44, align 8
  %46 = load double, ptr %14, align 8
  %47 = fadd double %45, %46
  %48 = load double, ptr %9, align 8
  %49 = fadd double %48, %47
  store double %49, ptr %9, align 8
  %50 = load double, ptr %14, align 8
  store double %50, ptr %10, align 8
  br label %51

51:                                               ; preds = %21
  %52 = load i32, ptr %11, align 4
  %53 = add nsw i32 %52, 1
  store i32 %53, ptr %11, align 4
  br label %17, !llvm.loop !6

54:                                               ; preds = %17
  %55 = load double, ptr %9, align 8
  %56 = load double, ptr %10, align 8
  %57 = fadd double %55, %56
  ret double %57
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
