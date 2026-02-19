// RUN: %clang -O2 %s -flto=thin -c -o %t.o
// RUN: llvm-lto -thinlto -o %t %t.o

// -fthinlto_index should be passed to cc1
// RUN: %clang -O2 -o %t1.o -x ir %t.o -c -fthinlto-index=%t.thinlto.bc -### \
// RUN:     2>&1 | FileCheck %s -check-prefix=CHECK-THINLTOBE-ACTION
// CHECK-THINLTOBE-ACTION: -fthinlto-index=
// CHECK-THINLTOBE-ACTION-SAME: {{"?-x"? "?ir"?}}

// Check that this also works without -x ir.
// RUN: %clang -O2 -o %t1.o %t.o -c -fthinlto-index=%t.thinlto.bc -### 2>&1 \
// RUN:     | FileCheck %s -check-prefix=CHECK-THINLTOBE-ACTION

// -save-temps should be passed to cc1
// RUN: %clang -O2 -o %t1.o -x ir %t.o -c -fthinlto-index=%t.thinlto.bc -save-temps -### 2>&1 | FileCheck %s -check-prefix=CHECK-SAVE-TEMPS -check-prefix=CHECK-SAVE-TEMPS-CWD
// RUN: %clang -O2 -o %t1.o -x ir %t.o -c -fthinlto-index=%t.thinlto.bc -save-temps=cwd -### 2>&1 | FileCheck %s -check-prefix=CHECK-SAVE-TEMPS -check-prefix=CHECK-SAVE-TEMPS-CWD
// RUN: %clang -O2 -o %t1.o -x ir %t.o -c -fthinlto-index=%t.thinlto.bc -save-temps=obj -### 2>&1 | FileCheck %s -check-prefix=CHECK-SAVE-TEMPS -check-prefix=CHECK-SAVE-TEMPS-OBJ
// CHECK-SAVE-TEMPS-NOT: -emit-llvm-bc
// CHECK-SAVE-TEMPS-CWD: -save-temps=cwd
// CHECK-SAVE-TEMPS-OBJ: -save-temps=obj
// CHECK-SAVE-TEMPS-NOT: -emit-llvm-bc

// Ensure clang driver gives the expected error for incorrect input type
// RUN: not %clang -O2 -o %t1.o %s -c -fthinlto-index=%t.thinlto.bc 2>&1 \
// RUN:     | FileCheck %s -check-prefix=CHECK-WARNING
// CHECK-WARNING: error: option '-fthinlto-index={{.*}}' requires input to be LLVM bitcode

// -fthinlto-CGO should also be passed to cc1.
// RUN: %clang -O2 -o %t1.o %t.o -c -fthinlto-index=%t.thinlto.bc \
// RUN:     -fthinlto-CGO1 -### 2>&1 \
// RUN:     | FileCheck %s -check-prefix=CHECK-THINLTOBE-CGO
// CHECK-THINLTOBE-CGO: -fthinlto-CGO1

// With no -fthinlto-CGO, codegen follows -O.
// RUN: %clang -O0 -x ir %t.o -c -fthinlto-index=%t.thinlto.bc \
// RUN:     -mllvm -debug-pass=Structure -o %t2.o 2>&1 \
// RUN:     | FileCheck %s -check-prefix=NOOPT
// RUN: %clang -O3 -x ir %t.o -c -fthinlto-index=%t.thinlto.bc \
// RUN:     -mllvm -debug-pass=Structure -o %t3.o 2>&1 \
// RUN:     | FileCheck %s -check-prefix=OPT
//
// Explicit -fthinlto-CGO overrides -O.
// RUN: %clang -O3 -x ir %t.o -c -fthinlto-index=%t.thinlto.bc \
// RUN:     -fthinlto-CGO0 -mllvm -debug-pass=Structure -o %t4.o 2>&1 \
// RUN:     | FileCheck %s -check-prefix=NOOPT
// RUN: %clang -O0 -x ir %t.o -c -fthinlto-index=%t.thinlto.bc \
// RUN:     -fthinlto-CGO2 -mllvm -debug-pass=Structure -o %t5.o 2>&1 \
// RUN:     | FileCheck %s -check-prefix=OPT
//
// Reject out-of-range values.
// RUN: not %clang -x ir %t.o -c -fthinlto-index=%t.thinlto.bc \
// RUN:     -fthinlto-CGO4 -o %t.bad.o 2>&1 \
// RUN:     | FileCheck %s -check-prefix=INVALID
// RUN: not %clang -x ir %t.o -c -fthinlto-index=%t.thinlto.bc \
// RUN:     -fthinlto-CGO8 -o %t.bad.o 2>&1 \
// RUN:     | FileCheck %s -check-prefix=INVALID
// INVALID: error: invalid value '{{.*}}' in '-fthinlto-CGO{{.*}}'
//
// NOOPT: Fast Register Allocator
// OPT: Greedy Register Allocator
