// Very basic pageit algorithm, only allows one process at a time, and one page in memory

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    for (int proc = 0; proc < MAXPROCESSES; proc++) {
	    if (q[proc].active != 1) continue;          // select only active processes    

        int pc = q[proc].pc; 		            	// program counter for process
        int page = pc/PAGESIZE; 		        	// current page the PC is on

        if (q[proc].pages[page]) break;        	    // if the page is already in memory, then we're done
	    if (pagein(proc,page)) break;          	    // if not in memory, then page it in

   	    for (int vic = 0; vic++ < MAXPROCPAGES;) {  // if the pagein fails, find pages to pageout
            if (q[proc].pages[page] != 1) continue; // only interested in pages that are in memory
            if (vic == page) continue;              // make sure the incoming page isn't considered
            if (pageout(proc,vic) == 1) break;      // page it out and we're done if successful
        }
    }
} 
