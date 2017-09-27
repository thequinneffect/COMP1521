// PageTable.c ... implementation of Page Table operations
// COMP1521 17s2 Assignment 2
// Written by John Shepherd, September 2017

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
   int  page_number; // may need this if cant keep track of heads's page numbers any other way
   struct PTE *next;
   struct PTE *prev;
   // TODO: add more fields here, if needed ...
};

typedef struct PTE PTE;

// The virtual address space of the process is managed
//  by an array of Page Table Entries (PTEs)
// The Page Table is not directly accessible outside
//  this file (hence the static declaration)

static PTE *PageTable;      // array of page table entries
struct PTE *head;
struct PTE *tail;
struct PTE *temp;
struct PTE *page_to_update;
PTE **page_getter;
static int  nPages;         // # entries in page table
static int  replacePolicy;  // how to do page replacement

// Forward refs for private functions

static int findVictim(int, PTE *);

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
   page_getter = malloc(sizeof(PTE*)*nPages);
   //fifoRead = 0;
   //fifoWrite = nPages-1;
   head = NULL;
   tail = NULL;
   for (int i = 0; i < nPages; i++) {
      PTE *p = &PageTable[i];
      page_getter[i] = p;
      p->status = NOT_USED;
      p->modified = 0;
      p->frame = NONE;
      p->accessTime = NONE;
      p->loadTime = NONE;
      p->nPeeks = p->nPokes = 0;
      p->page_number = i;
      p->next = NULL;
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
      fprintf(stderr,"Invalid page reference\n");
      exit(EXIT_FAILURE);
   }
   PTE *p = &PageTable[pno];
   int fno; // frame number
   switch (p->status) {
   case NOT_USED:
   case ON_DISK:

      // TODO: add stats collection  
      countPageFault();

      fno = findFreeFrame();

      // if there is a free frame then add to victim list
      if (fno != NONE) {
         if (head == NULL) {                             // if victim list has not started to be built
            head = p;
            tail = p;                             // head and tail both have the address of the specified page
            //head->next = head->prev = tail->next = NULL; // set all pointers coming from head/tail to NULL as only 1 page in list
         } else {                // if victim list is partially built
            temp = tail;    // save current tail so it can be doubly linked
            tail->next = p;      // add new page after tail
            tail = tail->next;   // make new page the tail
            tail->prev = temp;   // link new tail to old tail via prev pointer
            //tail->next = NULL;   // set tails next to point to nothing
         }
      }
      // no free frames, so victim list is fully built, so get victim from it
      // NOTE: could i just do away with the find victim function and just make it grab head? 
         // maybe do that if thats the only thing needed there (no book-keeping etc.)
      else {
         int vno = findVictim(time, p);
#ifdef DBUG
         printf("Evict page %d\n",vno);
#endif
         // TODO:
         PTE *vic = &PageTable[vno];
         // if victim page modified, save its frame
         if (vic->modified) {
            countPageSave();
         }
         // collect frame# (fno) for victim page
         fno = vic->frame;
         // update PTE for victim page
         // - new status
         // - no longer modified
         // - no frame mapping
         // - not accessed, not loaded
         vic->status = ON_DISK;
         vic->modified = 0;
         vic->frame = NONE;
         vic->accessTime = NONE;
         vic->loadTime = NONE;
      }
      printf("Page %d given frame %d\n",pno,fno);
      // TODO:
      // load page pno into frame fno
      loadFrame(fno, pno, time);
      // update PTE for page
      // - new status
      p->status = IN_MEMORY;
      // - not yet modified
      p->modified = 0;
      // - associated with frame fno
      p->frame = fno;
      // - just loaded
      p->loadTime = time;
      break;
   case IN_MEMORY:
      // TODO: add stats collection
      countPageHit();
      switch (replacePolicy) {
      case REPL_LRU:
         // update lru list
         page_to_update = page_getter[pno];
         if (page_to_update == tail) break;
         if (page_to_update == head) {
            head = head->next;
            head->prev = NULL;
            temp = tail;    // save current tail so it can be doubly linked
            tail->next = page_to_update;      // add new page after tail
            tail = tail->next;   // make new page the tail
            tail->prev = temp;   // link new tail to old tail via prev pointer
            tail->next = NULL;
         } else {
            temp = page_to_update->prev;
            page_to_update->prev->next = page_to_update->next; // snip it out of current place
            page_to_update->next->prev = temp;
            temp = tail;    // save current tail so it can be doubly linked
            tail->next = page_to_update;      // add new page after tail
            tail = tail->next;   // make new page the tail
            tail->prev = temp;   // link new tail to old tail via prev pointer
            tail->next = NULL;
         }
      }
      break;
   default:
      fprintf(stderr,"Invalid page status\n");
      exit(EXIT_FAILURE);
   }
   if (mode == 'r')
      p->nPeeks++;
   else if (mode == 'w') {
      p->nPokes++;
      p->modified = 1;
   }
   p->accessTime = time;
   return p->frame;
}

// findVictim: find a page to be replaced
// uses the configured replacement policy

static int findVictim(int time, PTE *p)
{
   int victim = 0;
   switch (replacePolicy) {
   case REPL_LRU:
      // TODO: implement LRU strategy
      victim = head->page_number;
      head = head->next;   // pop first in from list
      temp = tail;    // save current tail so it can be doubly linked
      tail->next = p;      // add new page after tail
      tail = tail->next;   // make new page the tail
      tail->prev = temp;   // link new tail to old tail via prev pointer
      break;
   case REPL_FIFO:
      // TODO: implement FIFO strategy
      // ret head page number
      victim = head->page_number;
      head = head->next;   // pop first in from list
      temp = tail;    // save current tail so it can be doubly linked
      tail->next = p;      // add new page after tail
      tail = tail->next;   // make new page the tail
      tail->prev = temp;   // link new tail to old tail via prev pointer
      break;
   case REPL_CLOCK:
      return 0;
   }
   return victim;
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
