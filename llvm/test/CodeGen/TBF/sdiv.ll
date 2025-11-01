; RUN: llc -march=tbf < %s | FileCheck %s -check-prefixes=CHECK-TBF
; RUN: llc -march=tbf -mcpu=v2 < %s | FileCheck %s -check-prefixes=CHECK-TBFV2

; Function Attrs: norecurse nounwind readnone
define i32 @test(i32 %len) #0 {
  %1 = sdiv i32 %len, 15
; CHECK-TBF: call __divdi3
; CHECK-TBFV2: sdiv32 w0, 15
  ret i32 %1
}
