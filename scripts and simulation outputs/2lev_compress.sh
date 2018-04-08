#!/bin/bash
t_folder="2level_compress"
f_width=8
t_ruu=32
LSQ=32
branch="2lev"
mkdir -p "$t_folder"
./sim-outorder -cache:il1 il1:128:32:1:l -cache:il2 none -cache:dl1 dl1:128:32:1:l -cache:dl2 none -cache:dl1lat 1 -cache:il1lat 1 -mem:lat 18 2 -fetch:ifqsize "$f_width" -ruu:size "$t_ruu" -lsq:size "$LSQ" -bpred "$branch" -bpred:2lev 1 16384 14 1 -redir:sim "$t_folder"/compress95.sim /opt/SimpleScalar/BenchMarks_Little/Programs/compress95.ss words < /opt/SimpleScalar/BenchMarks_Little/Input/compress95.in 2> "$t_folder"/compress95.stderr 1 > "$t_folder"/compress95.stdout
