#!/bin/bash
lcov --zerocounters
cd source/tools/testing/selftests/sigaltstack

echo "TEST sas.c"
gcc --coverage sas.c -o sas
./sas
echo "TEST_32bit sas.c"
gcc --coverage -m32 sas.c -o sas
./sas

cd ../x86

echo "TEST mov_ss_trap.c"
gcc --coverage mov_ss_trap.c -o mov_ss_trap
./mov_ss_trap
echo "TEST_32bit mov_ss_trap.c"
gcc --coverage -m32 mov_ss_trap.c -o mov_ss_trap
./mov_ss_trap

echo "TEST sigreturn.c"
gcc --coverage -no-pie sigreturn.c -o sigreturn
./sigreturn
echo "TEST_32bit sigreturn.c"
gcc --coverage -m32 -no-pie sigreturn.c -o sigreturn
./sigreturn

echo "TEST single_step_syscall.c"
gcc --coverage single_step_syscall.c -o single_step_syscall
./single_step_syscall
echo "TEST_32bit single_step_syscall.c"
gcc --coverage -m32 single_step_syscall.c -o single_step_syscall
./single_step_syscall

echo "TEST syscall_arg_fault.c"
gcc --coverage syscall_arg_fault.c -o syscall_arg_fault
./syscall_arg_fault
echo "TEST_32bit syscall_arg_fault.c"
gcc --coverage -m32 syscall_arg_fault.c -o syscall_arg_fault
./syscall_arg_fault

cd ../../../../..
echo "LCOV KERNEL"
lcov -o signal.info -c
echo "GENHTML"
genhtml -o report signal.info
                         

               




