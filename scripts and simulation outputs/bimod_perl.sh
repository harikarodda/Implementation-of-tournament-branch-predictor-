#!/bin/bash
t_folder="bimodal_perl"
f_width=8
t_ruu=32
LSQ=32
branch="bimod"
mkdir -p "$t_folder"
./sim-outorder -cache:il1 il1:128:32:1:l -cache:il2 none -cache:dl1 dl1:128:32:1:l -cache:dl2 none -cache:dl1lat 1 -cache:il1lat 1 -mem:lat 18 2 -fetch:ifqsize "$f_width" -ruu:size "$t_ruu" -lsq:size "$LSQ" -bpred "$branch"  -redir:sim "$t_folder"/perl.sim /opt/SimpleScalar/BenchMarks_Little/Programs/perl.ss words < /opt/SimpleScalar/BenchMarks_Little/Input/perl-tests.pl 2> "$t_folder"/perl.stderr 1 > "$t_folder"/perl.stdout
