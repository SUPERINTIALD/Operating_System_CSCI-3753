/*
 * This file contains the skeleton for an LRU pager
 */

#include <stdio.h> 
#include <stdlib.h>
#include "simulator.h"
void pageit(Pentry q[MAXPROCESSES]) {

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    int proc;
    int page;
    int old;
    int LRU;
    int pc;
    /* initialize static vars on first run */
    if (!initialized) {
        for (int i = 0; i < MAXPROCPAGES; i++) {
            for (int j = 0; j < MAXPROCPAGES; j++) {
                timestamps[i][j] = 0;
            }
        }
        initialized = 1;
    }
  // Every time pageit() is called
    for (proc = 0; proc < MAXPROCESSES; proc++) {

        // 1. Go through all ACTIVE processes
        if (q[proc].active == 1) {

            // a. Calculate the wanted page based on PC and PAGESIZE
            pc = q[proc].pc;
            page = (pc)/ PAGESIZE;

            // b. Update timestamp of the wanted page
            timestamps[proc][page] = tick;

            // c. Check if the wanted page is already in memory?
            if (!q[proc].pages[page]) {

                // ii. If not => try to swap in the wanted page by using pagein()
                // 1. If pagein() success => continue to the next process
                // 2. If not, LRU kicks in

                // a. Get the swapped in old recently used page of the current process (local)
                if(!pagein(proc, page)){
                    old = tick;
                    LRU = -1;
                    for (int vic = 0; ++vic < MAXPROCPAGES;) {
                        if (vic == page) continue;
                        // if (pageout(proc,vic) == 1) break;  
                        if (timestamps[proc][vic] < old && q[proc].pages[vic] && q[proc].pages[page] != 1) {
                            LRU = vic;
                            old = timestamps[proc][vic];
                        }
                    }
                // b. Pageout the least recently used page
                if (LRU != -1) {
                        pageout(proc, LRU);
                }                
                }
            }
        }

        // Advance time for the next pageit iteration
        tick++;
    }
}
/*
 notes:\\


Every time pageit() is called
1. Go through all ACTIVE processes
    a. Calculate the wanted page based on PC and PAGESIZE
    b. Update timestamp of the wanted page
    c. Check if the wanted page is already in the memory ?
        i. If the wanted page is swapped in => continue to the next process
        ii. If not => try to swap in the wanted page by using pagein()
            1. If pagein() success => continue to the next process
            2. If not, LRU kicks in
                a. get the swapped in old recently used page of the current process (local) using
                timestamp and pages array => pageout()
*/