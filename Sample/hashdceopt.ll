; ModuleID = 'Sample/hashdceopt.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

define i32 @main() nounwind uwtable {
entry:
  %a = alloca i32, align 4
  %j = alloca i32, align 4
  %x = alloca i32, align 4
  %call = call i32 (i8*, ...)* @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8]* @.str, i32 0, i32 0), i32* %a)
  %call1 = call i32 (i8*, ...)* @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8]* @.str, i32 0, i32 0), i32* %x)
  %0 = load i32* %a, align 4
  %1 = load i32* %x, align 4
  %tobool = icmp ne i32 %1, 0
  br i1 %tobool, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %add8 = add nsw i32 %0, 10
  %add9 = add nsw i32 %add8, %0
  store i32 %add9, i32* %j, align 4
  br label %if.end

if.else:                                          ; preds = %entry
  %add12 = add nsw i32 %0, 10
  %add13 = add nsw i32 %add12, %0
  store i32 %add13, i32* %j, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %2 = load i32* %j, align 4
  %call14 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([3 x i8]* @.str, i32 0, i32 0), i32 %2)
  ret i32 0
}

declare i32 @__isoc99_scanf(i8*, ...)

declare i32 @printf(i8*, ...)
