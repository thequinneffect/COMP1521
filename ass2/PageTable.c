// PageTable.c ... implementation of Page Table operations
// COMP1521 17s2 Assignment 2
// Written by John Shepherd, September 2017
// modified by Nicholas Quinn (z5117408), September 2017

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Memory.h"
#include "Stats.h"
#include "PageTable.h"

// Symbolic constants

#define NOT_USED 0
#define IN_MEMORY 1
#define ON_DISK 2

// PTE = Page Table Entry

struct PTE {
   char status;      // NOT_USED, IN_MEMORY, ON_DISK
   char modified;    // boolean: changed since loaded
   int  frame;       // memory frame holding this page
   int  accessTime;  // clock tick for last access
   int  loadTime;    // clock tick for last time loaded (where load means allocated to a memory frame?)
   int  nPeeks;      // total number times this page read
   int  nPokes;      // total number times this page 
   int  page_number; // page number of page
   struct PTE *next; // pointer to next page in victim priority list
   struct PTE *prev; // pointer to prev page in victim priority list
};

typedef struct PTE PTE;

static PTE *PageTable;      // array of page table entries
struct PTE *head;           // head of victim priority list (where head is the victim)
struct PTE *tail;           // tail of victim priority list (where tail is furthest from being the victim)
struct PTE *temp;           // temporary pointer for performing list operations
static int  nPages;         // # entries in page table
static int  replacePolicy;  // how to do page replacement

// Forward refs for private functions

static int findVictim(int);
static void buildVictimList(PTE*);
static void updateVicPTE(PTE*);
static void updateNewPagePTE(PTE*, int, int);
static void updateLRU(PTE*);

// initPageTable: create/initialise Page Table data structures

void initPageTable(int policy, int np)
{
   PageTable = malloc(np * sizeof(PTE));
   if (PageTable == NULL) {
      fprintf(stderr, "Can't initialise Memory\n");
      exit(EXIT_FAILURE);
   }
   replacePolicy = policy;
   nPages = np;
   head = NULL; // init head/tail to point to no pages
   tail = NULL;
   for (int i = 0; i < nPages; i++) {
      PTE *p = &PageTable[i];
      p->status = NOT_USED;
      p->modified = 0;
      p->frame = NONE;
      p->accessTime = NONE;
      p->loadTime = NONE;
      p->nPeeks = p->nPokes = 0;
      p->page_number = i;
      p->next = NULL; // init page to point to no other pages
      p->prev = NULL;
   }
}

// requestPage: request access to page pno in mode
// returns memory frame holding this page
// page may have to be loaded
// PTE(status,modified,frame,accessTime,nextPage,nPeeks,nWrites)

int requestPage(int pno, char mode, int time)
{
   if (pno < 0 || pno >= nPages) {
      fprintf(stderr,"Invalid page reference \n");
      exit(EXIT_FAILURE);
   }
   PTE *p = &PageTable[pno]; // page
   int fno; // frame number
   switch (p->status) { // look at page's status
   case NOT_USED: // both these cases are deemed identical for this assignment
   case ON_DISK: 

      // add stats collection  
      countPageFault(); // has to be loaded, so increment page faults
      fno = findFreeFrame(); // find a free frame of memory for the page frame to go in

      // if there IS A FREE FRAME then ADD PAGE p TO VICTIM LIST
      if (fno != NONE) {
         buildVictimList(p);
      }
      // NO FREE FRAMES, so VICTIM LIST FULLY BUILT, so get victim from it
      else {
         int vno = findVictim(time);
#ifdef DBUG
         printf("Evict page %d\n",vno);
#endif
         PTE *vic = &PageTable[vno];
         // if victim page modified, save its frame
         if (vic->modified) {
            countPageSave();
         }
         // collect frame# (fno) for victim page
         fno = vic->frame;
         // update PTE for victim page 
         updateVicPTE(vic);
         // remove victim page from victim list and add new page to tail
         head = head->next;
         buildVictimList(p); 
         //appendToVictimList(p);
      }
      printf("Page %d given frame %d\n",pno,fno);
      // load page pno into frame fno
      loadFrame(fno, pno, time);
      // update PTE for page
      updateNewPagePTE(p, fno, time);
      break;
   case IN_MEMORY:
      // add stats collection
      countPageHit();
      switch (replacePolicy) {
      case REPL_LRU:
         // update lru list
         updateLRU(p);
      }
      break;
   default:
      fprintf(stderr,"Invalid page status\n");
      exit(EXIT_FAILURE);
   }
   if (mode == 'r') // reading so increment peeks
      p->nPeeks++;
   else if (mode == 'w') { // writing so increment pokes and set mod flag to true
      p->nPokes++;
      p->modified = 1;
   }
   p->accessTime = time; // update access time
   return p->frame; // return the allocated memory frame for the page
}

// adds page p to end of victim list - least likely to be the victim

void buildVictimList(PTE *p) {
   if (head == NULL) {     // if victim list has not started to be built
      head = p;            // head and tail both now point to the specified page
      tail = p;            
   } else {                // if victim list is partially built
      temp = tail;         // save current tail so it can be doubly linked
      tail->next = p;      // add new page after tail
      tail = tail->next;   // make new page the tail
      tail->prev = temp;   // link new tail to old tail via prev pointer to maintain victim list
   }
}

// findVictim: find a page to be replaced
// uses the configured replacement policy
// note: lists built/managed in such a way that head is always victim

static int findVictim(int time)
{
   int victim = 0;
   switch (replacePolicy) {
   case REPL_LRU:
      // head page is victim
      victim = head->page_number;
      break;
   case REPL_FIFO:
      // head page is victim
      victim = head->page_number;
      break;
   case REPL_CLOCK:
      return 0;
   }
   return victim;
}

// updates the PTE for the victim

void updateVicPTE(PTE *vic) {
   vic->status = ON_DISK;
   vic->modified = 0;
   vic->frame = NONE;
   vic->accessTime = NONE;
   vic->loadTime = NONE;
}

// updates the PTE for the new page

void updateNewPagePTE(PTE *p, int fno, int time) {
   p->status = IN_MEMORY;
   p->modified = 0;
   p->frame = fno;
   p->loadTime = time;
}

// updates the victim list specifically for memory accesses for LRU policy

void updateLRU(PTE *p) {
   if (p == tail) return; // already in least recently used position, so do nothing
   if (p == head) { 
      head = head->next; // pop head
      head->prev = NULL; // unlink new head from old head
   } else {
      temp = p->prev; // save page before page to be updated
      p->prev->next = p->next; // snip page out of current place
      p->next->prev = temp; // link node after page to be updated to one before it
   }
   // do this regardless of page location in list (as long as not tail)
   temp = tail;    // save current tail so it can be doubly linked
   tail->next = p;      // add new page after tail
   tail = tail->next;   // make new page the tail
   tail->prev = temp;   // link new tail to old tail via prev pointer
   tail->next = NULL;
}

// showPageTableStatus: dump page table
// PTE(status,modified,frame,accessTime,nextPage,nPeeks,nWrites)

void showPageTableStatus(void)
{
   char *s;
   printf("%4s %6s %4s %6s %7s %7s %7s %7s\n",
          "Page","Status","Mod?","Frame","Acc(t)","Load(t)","#Peeks","#Pokes");
   for (int i = 0; i < nPages; i++) {
      PTE *p = &PageTable[i];
      printf("[%02d]", i);
      switch (p->status) {
      case NOT_USED:  s = "-"; break;
      case IN_MEMORY: s = "mem"; break;
      case ON_DISK:   s = "disk"; break;
      }
      printf(" %6s", s);
      printf(" %4s", p->modified ? "yes" : "no");
      if (p->frame == NONE)
         printf(" %6s", "-");
      else
         printf(" %6d", p->frame);
      if (p->accessTime == NONE)
         printf(" %7s", "-");
      else
         printf(" %7d", p->accessTime);
      if (p->loadTime == NONE)
         printf(" %7s", "-");
      else
         printf(" %7d", p->loadTime);
      printf(" %7d", p->nPeeks);
      printf(" %7d", p->nPokes);
      printf("\n");
   }
}