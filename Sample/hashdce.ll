; ModuleID = 'hashdce.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

define i32 @main() nounwind uwtable {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  %e = alloca i32, align 4
  %f = alloca i32, align 4
  %g = alloca i32, align 4
  %h = alloca i32, align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  %k = alloca i32, align 4
  %x = alloca i32, align 4
  store i32 0, i32* %retval
  %call = call i32 (i8*, ...)* @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8]* @.str, i32 0, i32 0), i32* %a)
  %call1 = call i32 (i8*, ...)* @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8]* @.str, i32 0, i32 0), i32* %x)
  %0 = load i32* %a, align 4
  store i32 %0, i32* %b, align 4
  %1 = load i32* %b, align 4
  store i32 %1, i32* %c, align 4
  %2 = load i32* %c, align 4
  store i32 %2, i32* %d, align 4
  %3 = load i32* %d, align 4
  store i32 %3, i32* %e, align 4
  %4 = load i32* %e, align 4
  store i32 %4, i32* %f, align 4
  %5 = load i32* %e, align 4
  %add = add nsw i32 %5, 10
  %6 = load i32* %d, align 4
  %add2 = add nsw i32 %add, %6
  store i32 %add2, i32* %g, align 4
  %7 = load i32* %a, align 4
  %add3 = add nsw i32 10, %7
  %8 = load i32* %b, align 4
  %add4 = add nsw i32 %add3, %8
  %9 = load i32* %f, align 4
  %add5 = add nsw i32 %add4, %9
  store i32 %add5, i32* %h, align 4
  %10 = load i32* %x, align 4
  %tobool = icmp ne i32 %10, 0
  br i1 %tobool, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %11 = load i32* %a, align 4
  store i32 %11, i32* %c, align 4
  %12 = load i32* %b, align 4
  store i32 %12, i32* %d, align 4
  %13 = load i32* %a, align 4
  %14 = load i32* %b, align 4
  %add6 = add nsw i32 %13, %14
  %15 = load i32* %d, align 4
  %add7 = add nsw i32 %add6, %15
  store i32 %add7, i32* %i, align 4
  %16 = load i32* %a, align 4
  %add8 = add nsw i32 %16, 10
  %17 = load i32* %d, align 4
  %add9 = add nsw i32 %add8, %17
  store i32 %add9, i32* %j, align 4
  store i32 10, i32* %c, align 4
  store i32 20, i32* %b, align 4
  br label %if.end

if.else:                                          ; preds = %entry
  %18 = load i32* %a, align 4
  %add10 = add nsw i32 %18, 10
  %add11 = add nsw i32 %add10, 25
  store i32 %add11, i32* %k, align 4
  %19 = load i32* %a, align 4
  %add12 = add nsw i32 %19, 10
  %20 = load i32* %d, align 4
  %add13 = add nsw i32 %add12, %20
  store i32 %add13, i32* %j, align 4
  %21 = load i32* %e, align 4
  store i32 %21, i32* %c, align 4
  %22 = load i32* %c, align 4
  store i32 %22, i32* %d, align 4
  %23 = load i32* %c, align 4
  store i32 %23, i32* %k, align 4
  store i32 10, i32* %b, align 4
  store i32 20, i32* %c, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %24 = load i32* %j, align 4
  %call14 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([3 x i8]* @.str, i32 0, i32 0), i32 %24)
  ret i32 0
}

declare i32 @__isoc99_scanf(i8*, ...)

declare i32 @printf(i8*, ...)
