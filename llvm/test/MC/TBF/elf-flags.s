# RUN: llvm-mc -triple=tbf-tos-tos -filetype=obj < %s \
# RUN:   | llvm-readobj --file-headers - \
# RUN:   | FileCheck -check-prefix=CHECK-NONE %s
# RUN: llvm-mc -triple=tbf-tos-tos -filetype=obj < %s \
# RUN:   | llvm-readobj --file-headers - \
# RUN:   | FileCheck -check-prefix=CHECK-NONE %s
# RUN: llvm-mc -triple=tbpfv0-tos-tos -filetype=obj < %s \
# RUN:   | llvm-readobj --file-headers - \
# RUN:   | FileCheck -check-prefix=CHECK-NONE %s
# RUN: llvm-mc -triple=tbf-tos-tos -mcpu=v1 -filetype=obj < %s \
# RUN:   | llvm-readobj --file-headers - \
# RUN:   | FileCheck -check-prefix=CHECK-TBFV1 %s
# RUN: llvm-mc -triple=tbpfv1-tos-tos -filetype=obj < %s \
# RUN:   | llvm-readobj --file-headers - \
# RUN:   | FileCheck -check-prefix=CHECK-TBFV1 %s
# RUN: llvm-mc -triple=tbf-tos-tos -mcpu=v2 -filetype=obj < %s \
# RUN:   | llvm-readobj --file-headers - \
# RUN:   | FileCheck -check-prefix=CHECK-TBFV2 %s
# RUN: llvm-mc -triple=tbpfv2-tos-tos -filetype=obj < %s \
# RUN:   | llvm-readobj --file-headers - \
# RUN:   | FileCheck -check-prefix=CHECK-TBFV2 %s
# RUN: llvm-mc -triple=tbf-tos-tos -mcpu=v3 -filetype=obj < %s \
# RUN:   | llvm-readobj --file-headers - \
# RUN:   | FileCheck -check-prefix=CHECK-TBFV3 %s
# RUN: llvm-mc -triple=tbpfv3-tos-tos -filetype=obj < %s \
# RUN:   | llvm-readobj --file-headers - \
# RUN:   | FileCheck -check-prefix=CHECK-TBFV3 %s
# RUN: llvm-mc -triple=tbf-tos-tos -mcpu=v4 -filetype=obj < %s \
# RUN:   | llvm-readobj --file-headers - \
# RUN:   | FileCheck -check-prefix=CHECK-TBFV4 %s
# RUN: llvm-mc -triple=tbpfv4-tos-tos -filetype=obj < %s \
# RUN:   | llvm-readobj --file-headers - \
# RUN:   | FileCheck -check-prefix=CHECK-TBFV4 %s

# CHECK-NONE:       Flags [ (0x0)
# CHECK-NONE-NEXT:  ]

# CHECK-TBFV1:       Flags [ (0x1)
# CHECK-TBFV1-NEXT:    0x1
# CHECK-TBFV1-NEXT:  ]

# CHECK-TBFV2:       Flags [ (0x2)
# CHECK-TBFV2-NEXT:    0x2
# CHECK-TBFV2-NEXT:  ]

# CHECK-TBFV3:       Flags [ (0x3)
# CHECK-TBFV3-NEXT:    0x1
# CHECK-TBFV3-NEXT:    0x2
# CHECK-TBFV3-NEXT:  ]

# CHECK-TBFV4:       Flags [ (0x4)
# CHECK-TBFV4-NEXT:    0x4
# CHECK-TBFV4-NEXT:  ]

mov64 r0, r0
