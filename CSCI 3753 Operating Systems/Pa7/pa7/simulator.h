/*
 * File: simulator.h
 * 
 * Original Author: Dr. Alva Couch
 *                  http://www.cs.tufts.edu/~couch/
 */

#define TRUE  1
#define FALSE 0

#define MAXPROCPAGES    20 	            /* max pages per individual process */ 
#define MAXPROCESSES    20 	            /* max number of processes in runqueue */ 
#define PAGESIZE        128 		    /* size of an individual page */ 
#define PAGEWAIT        100 		    /* wait for paging in */ 
#define PHYSICALPAGES   100	            /* number of available physical pages */ 
#define QUEUESIZE       40              /* total number of processes to run */
#define PROGRAMS        5               /* total number of program types */

#define MAXBRANCHES     40	            /* number of branches in a program */ 
#define MAXEXITS        10	            /* number of maximum exits per program */ 
#define MAXBRINGS       100	            /* must be EVEN! data points in branch table */ 
#define MAXPC (MAXPROCPAGES*PAGESIZE)   /* largest PC value */ 

struct pentry {
    long active; 
    long pc; 
    long npages; 
    long pages[MAXPROCPAGES]; /* 0 if not allocated, 1 if allocated */ 
};

typedef struct pentry Pentry; 

/* int pagein (int process, int page)
 *   This pages in the requested page
 * Arguments:
 *   proc: process to work upon (0-19) 
 *   page: page to put in (0-19)
 * Returns:
 *   1 if pagein started, already running, or paged in
 *   0 if it can't start (e.g., swapping out) 
 */
extern int pagein (int process, int page); 

/* int pageout(int process, int page)
 *   This pages out the requested page.
 * Arguments:
 *   proc: process to work upon (0-19)
 *   page: page to swap out. 
 * Returns: 
 *   1 if pageout started, already running, or paged out
 *   0 if can't start (e.g., swapping in)
 */
extern int pageout(int process, int page); 

/* void pageit(Pentry q[MAXPROCESSES])
 *   This is called by the simulator
 *   every time something interesting occurs.
 *   It is where you implement the paging strategy.
 * Arguments:   
 *   q: state of every process
 * Returns:
 *   void 
 */
extern void pageit(Pentry q[MAXPROCESSES]); 

typedef enum { GOTO, FOR, NFOR, IF } BranchType;

/* abstract description of a branch 
   describes qualitative behavior, not actual branching */ 
typedef struct branch { 
   long wherefrom;
   long whereto; 
   BranchType btype; 
   long min, max; 
   double prob; 
   long extent; 
} Branch;

typedef struct program { 
   long size; 
   long nbranches; 
   Branch branches[MAXBRANCHES]; 
   long nexits; 
   long exits[MAXEXITS]; /* which statements are "halt" */ 
} Program; 


// branch context: determines which branch to 
// take next in a probabilistic situation...
typedef struct bcontext { 
   BranchType btype; 
   long boffset;   /* offset into brings array */ 
   long bsize; 	   /* size of brings array */ 
   long bvalue;    /* which branch direction to take */ 
   long bcount;    /* value of current decrement */ 
   long brings[MAXBRINGS]; 
} Bcontext; 

typedef struct process { 
   Program *program; 
   long nbcontexts; 
   Bcontext bcontexts[MAXBRANCHES]; 
   long pc; 	            	/* program counter */ 
   long npages; 
   long pages[MAXPROCPAGES]; 	/* whether page is available */ 
   long blocked[MAXPROCPAGES];	/* whether we've reported page state */ 
   long active;              	/* whether running now */ 
   long compute; 	    	/* number of compute ticks */ 
   long block; 		    	/* number of blocked ticks */ 
   long pid; 			/* unique process number */ 
   long kind; 			/* kind of process from table */ 
} Process;
