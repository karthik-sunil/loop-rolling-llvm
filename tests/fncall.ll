; ModuleID = 'tests/toy.c'
source_filename = "tests/toy.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @foo(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  store i32 0, ptr %3, align 4
  br label %4

4:                                                ; preds = %10, %1
  %5 = load i32, ptr %3, align 4
  %6 = icmp slt i32 %5, 3
  br i1 %6, label %7, label %13

7:                                                ; preds = %4
  %8 = load i32, ptr %3, align 4
  %9 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %8)
  br label %10

10:                                               ; preds = %7
  %11 = load i32, ptr %3, align 4
  %12 = add nsw i32 %11, 1
  store i32 %12, ptr %3, align 4
  br label %4, !llvm.loop !6

13:                                               ; preds = %4
  %14 = load i32, ptr %2, align 4
  %15 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %14)
  ret i32 0
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @bar(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %3)
  ret i32 0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca [15 x i32], align 16
  %3 = alloca [6 x i32], align 16
  store i32 0, ptr %1, align 4
  %4 = getelementptr inbounds [15 x i32], ptr %2, i64 0, i64 0
  store i32 1, ptr %4, align 16
  %5 = getelementptr inbounds [15 x i32], ptr %2, i64 0, i64 1
  store i32 1, ptr %5, align 4
  %6 = getelementptr inbounds [15 x i32], ptr %2, i64 0, i64 2
  store i32 1, ptr %6, align 8
  %7 = getelementptr inbounds [15 x i32], ptr %2, i64 0, i64 3
  store i32 1, ptr %7, align 4
  %8 = getelementptr inbounds [15 x i32], ptr %2, i64 0, i64 4
  store i32 1, ptr %8, align 16
  %9 = getelementptr inbounds [15 x i32], ptr %2, i64 0, i64 5
  store i32 1, ptr %9, align 4
  %10 = getelementptr inbounds [15 x i32], ptr %2, i64 0, i64 6
  store i32 1, ptr %10, align 8
  %11 = getelementptr inbounds [6 x i32], ptr %3, i64 0, i64 0
  store i32 2, ptr %11, align 16
  %12 = getelementptr inbounds [6 x i32], ptr %3, i64 0, i64 2
  store i32 2, ptr %12, align 8
  %13 = getelementptr inbounds [6 x i32], ptr %3, i64 0, i64 4
  store i32 2, ptr %13, align 16
  ret i32 5
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
!5 = !{!"clang version 17.0.6"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
