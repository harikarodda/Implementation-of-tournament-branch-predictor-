#ifndef BPRED_H
#define BPRED_H

#define dassert(a) assert(a)

#include <stdio.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "stats.h"


/* branch predictor types */
enum bpred_class {
  BPred2bit,			/* 2-bit saturating cntr pred (dir mapped) */
  BPredTaken,			/* static predict taken */
  BPredNotTaken,		/* static predict not taken */
  BPredComb,                    /* combined predictor (McFarling) */
  BPred2Level,			/* 2-level correlating pred w/2-bit counters */
  BPredTournament,              /* tournament predictor */  
  BPredGlobal,                  /* global predictor */
  BPredLocal,                   /* local predictor*/
  BPredSelector,                  /* selector */
  BPred_NUM
};

/* an entry in a BTB */
struct bpred_btb_ent_t {
  md_addr_t addr;		/* address of branch being tracked */
  enum md_opcode op;		/* opcode of branch corresp. to addr */
  md_addr_t target;		/* last destination of branch when taken */
  struct bpred_btb_ent_t *prev, *next; /* lru chaining pointers */
};

/* direction predictor def */
struct bpred_dir_t {
  enum bpred_class class;	/* type of predictor */
  union {
    struct {
      unsigned int size;	/* number of entries in direct-mapped table */
      unsigned char *table;	/* prediction state table */
    } bimod;
    struct {
      int l1size;		/* level-1 size, number of history regs */
      int l2size;		/* level-2 size, number of pred states */
      int shift_width;		/* amount of history in level-1 shift regs */
      int xor;			/* history xor address flag */
      int *shiftregs;		/* level-1 history table */
      unsigned char *l2table;	/* level-2 prediction state table */
    } two;

/* CSCE 614 */
    struct {
      int global_regsize;          /* size of the global branch history reg */
      int shiftreg_global;         /* global branch history table */
      unsigned char *global_table; /* global prediction state table */
      int optional;                /* optional */ 
    } global_pred;
    struct {
      int local_htb_size;          /* size of the history table for local branch history regs */
      int local_hrsize;            /* size of each local branch history regs*/
      int *shiftregs_local;        /* local branch history table */
      unsigned char *local_table;  /* local prediction state table */
    } local_pred; 
    struct {
      int sel_size;                /* selector size, number of 2-bit selector entries */
      unsigned char *seltable;     /* selctor prediction state table */
    } selector;   
  } config;
};

/* branch predictor def */
struct bpred_t {
  enum bpred_class class;	     /* type of predictor */
  struct {
    struct bpred_dir_t *bimod;	     /* first direction predictor */
    struct bpred_dir_t *twolev;	     /* second direction predictor */

  
    struct bpred_dir_t *global_pred; /* global predictor */
struct bpred_dir_t *selector;    /* selector */

    struct bpred_dir_t *meta;	     /* meta predictor */
    struct bpred_dir_t *local_pred;  /* local predictor */
    
  } dirpred;

  struct {
    int sets;			     /* num BTB sets */
    int assoc;			     /* BTB associativity */
    struct bpred_btb_ent_t *btb_data;/* BTB addr-prediction table */
  } btb;

  struct {
    int size;			/* return-address stack size */
    int tos;			/* top-of-stack */
    struct bpred_btb_ent_t *stack; /* return-address stack */
  } retstack;

  /* stats */
  counter_t addr_hits;		/* num correct addr-predictions */
  counter_t dir_hits;		/* num correct dir-predictions (incl addr) */
  counter_t used_ras;		/* num RAS predictions used */
  counter_t used_bimod;		/* num bimodal predictions used (BPredComb) */
  counter_t used_2lev;		/* num 2-level predictions used (BPredComb) */


  
  counter_t jr_seen;		/* num JR's seen */
  counter_t jr_non_ras_hits;	/* num correct addr-preds for non-RAS JR's */
counter_t used_local_pred;    /* num local predictions used (Tournament) */
  counter_t used_global_pred;   /* num global predictions used (Tournament) */
 counter_t jr_non_ras_seen;	/* num non-RAS JR's seen */
  counter_t misses;		/* num incorrect predictions */
  counter_t jr_hits;		/* num correct addr-predictions for JR's */
  counter_t retstack_pops;	/* number of times a value was popped */
  counter_t retstack_pushes;	/* number of times a value was pushed */

  counter_t lookups;		/* num lookups */
 
  counter_t ras_hits;		/* num correct return-address predictions */
};

/* branch predictor update information */
struct bpred_update_t {
  char *pdir1;		/* direction-1 predictor counter */
  char *pdir2;		/* direction-2 predictor counter */
  char *sel_dir;        /* direction to selector counter*/ 
  char *pmeta;		/* meta predictor counter */
  


                                                                          // bug
  struct {		/* predicted directions */
    unsigned int ras         : 1;    /* RAS used */
    unsigned int bimod       : 1;    /* bimodal predictor */
    unsigned int twolev      : 1;    /* 2-level predictor */

/* CSCE 614 */
    unsigned int local_pred  : 1;    /* local predictor */
    unsigned int global_pred : 1;    /* global predictor */

    unsigned int meta        : 1;    /* meta predictor (0..bimod / 1..2lev) */
  } dir;
};

struct bpred_t *			/* branch predictory instance */
bpred_create(enum bpred_class class,	/* type of predictor to create */
	     unsigned int bimod_size,	/* bimod table size */
	     unsigned int l1size,	/* level-1 table size */
	     unsigned int l2size,	/* level-2 table size */
	     unsigned int meta_size,	/* meta predictor table size */
	     unsigned int shift_width,	/* history register width */          // bug
	     unsigned int xor,		/* history xor address flag */
	     unsigned int btb_sets,	/* number of sets in BTB */ 
	     unsigned int btb_assoc,	/* BTB associativity */
	     unsigned int retstack_size,/* num entries in ret-addr stack */
             unsigned int sel_size,       /* num of 2-bit selector entries */
             unsigned int global_regsize, /* size of global branch history reg */
             unsigned int local_htb_size, /* size of the history table */
             unsigned int local_hrsize,   /* size of each local branch register */
             unsigned int optional);      /* optional */
/* CSCE 614 */

/* create a branch direction predictor */
struct bpred_dir_t *		/* branch direction predictor instance */
bpred_dir_create (
  enum bpred_class class,	/* type of predictor to create */
 unsigned int xor,	   	/* history xor address flag */
  unsigned int l1size,		/* level-1 table size */
 unsigned int local_htb_size,  /* size of the history table */
  unsigned int local_hrsize,    /* size of each local branch register */
  unsigned int optional);       /* optional */
  unsigned int sel_size,        /* num of 2-bit selector entries */
  unsigned int global_regsize,  /* size of global branch history reg */
  unsigned int l2size,		/* level-2 table size (if relevant) */
  unsigned int shift_width,	/* history register width */
 

 

/* print branch predictor configuration */
void
bpred_config(struct bpred_t *pred,	/* branch predictor instance */
	     FILE *stream);		/* output stream */

/* print predictor stats */
void
bpred_stats(struct bpred_t *pred,	/* branch predictor instance */
	    FILE *stream);		/* output stream */

/* register branch predictor stats */
void
bpred_reg_stats(struct bpred_t *pred,	/* branch predictor instance */
		struct stat_sdb_t *sdb);/* stats database */

/* reset stats after priming, if appropriate */
void bpred_after_priming(struct bpred_t *bpred);

/* probe a predictor for a next fetch address, the predictor is probed
   with branch address BADDR, the branch target is BTARGET (used for
   static predictors), and OP is the instruction opcode (used to simulate
   predecode bits; a pointer to the predictor state entry (or null for jumps)
   is returned in *DIR_UPDATE_PTR (used for updating predictor state),
   and the non-speculative top-of-stack is returned in stack_recover_idx 
   (used for recovering ret-addr stack after mis-predict).  */
md_addr_t				/* predicted branch target addr */
bpred_lookup(struct bpred_t *pred,	/* branch predictor instance */
	     md_addr_t baddr,		/* branch address */
	     md_addr_t btarget,		/* branch target if taken */
	     enum md_opcode op,		/* opcode of instruction */
	     int is_call,		/* non-zero if inst is fn call */
	     int is_return,		/* non-zero if inst is fn return */
	     struct bpred_update_t *dir_update_ptr, /* pred state pointer */
	     int *stack_recover_idx);	/* Non-speculative top-of-stack;
					 * used on mispredict recovery */

/* Speculative execution can corrupt the ret-addr stack.  So for each
 * lookup we return the top-of-stack (TOS) at that point; a mispredicted
 * branch, as part of its recovery, restores the TOS using this value --
 * hopefully this uncorrupts the stack. */
void
bpred_recover(struct bpred_t *pred,	/* branch predictor instance */
	      md_addr_t baddr,		/* branch address */
	      int stack_recover_idx);	/* Non-speculative top-of-stack;
					 * used on mispredict recovery */

/* update the branch predictor, only useful for stateful predictors; updates
   entry for instruction type OP at address BADDR.  BTB only gets updated
   for branches which are taken.  Inst was determined to jump to
   address BTARGET and was taken if TAKEN is non-zero.  Predictor 
   statistics are updated with result of prediction, indicated by CORRECT and 
   PRED_TAKEN, predictor state to be updated is indicated by *DIR_UPDATE_PTR 
   (may be NULL for jumps, which shouldn't modify state bits).  Note if
   bpred_update is done speculatively, branch-prediction may get polluted. */
void
bpred_update(struct bpred_t *pred,	/* branch predictor instance */
	     md_addr_t baddr,		/* branch address */
	     md_addr_t btarget,		/* resolved branch target */
	     int taken,			/* non-zero if branch was taken */
	     int pred_taken,		/* non-zero if branch was pred taken */
	     int correct,		/* was earlier prediction correct? */
	     enum md_opcode op,		/* opcode of instruction */
	     struct bpred_update_t *dir_update_ptr); /* pred state pointer */


#ifdef foo0
/* OBSOLETE */
/* dump branch predictor state (for debug) */
void
bpred_dump(struct bpred_t *pred,	/* branch predictor instance */
	   FILE *stream);		/* output stream */
#endif

#endif /* BPRED_H */
