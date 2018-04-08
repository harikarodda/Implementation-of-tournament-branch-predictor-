Tournament predictor: 
Replace the 4 files bpred.c,bpred.h,sim-bpred.c and sim-outorder.c in the simplecalar folder.

DO not forget to "make:" before execution.
The shell scripts that are used for the performance evaluation are included. 


In simulator: The option for the tournament branch predictor is defined as follows.

-bpred tournament -bpred:tournament <sel_size> <global_regsize> <local_htb_size> <local_hrsize> <optional>

eg: -bpred tournament -bpred:tournament 4096 12 1024 10 0

We have included scripts for 3 branchmarks and 3 predictors-bimodal|Tournament|2-level for comparisiojn.

