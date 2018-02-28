#include "memory.h"

#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#define NOT_IN_MEMORY -1

#define CLEAR_INTERVAL 1000 /* How often the R bits are cleaned. */

/********** Functions **********/

Algo AlgoOfChoice(char *str) {
    assert(str);
    if (strcmp(str, "FIFO") == 0 || strcmp(str,"fifo") == 0) {
        return FIFO;
    } else if (strcmp(str, "LRU") == 0 || strcmp(str, "lru") == 0) {
        return LRU;
    } else if (strcmp(str, "VMS") == 0 || strcmp(str, "vms") == 0) {
        return VMS;
    } else {
        printf("ERROR: Algorithm '%s' of Page Replacement invalid.\n", str);
        exit(EXIT_FAILURE);
    }
}

int fifo(Memory *mem){
    int i, first_access;
    list *frames_age_desc = EMPTY_LIST;

    for (i = 0; i < mem->numPF; i++) {
        frames_age_desc = listInsert(frames_age_desc, i, mem->frames[i].first_element);
    }

    DEBUG listPrint(frames_age_desc);

    frames_age_desc = listRemoveFirst(frames_age_desc, &i, &first_access);

    DEBUG printf("First frame: %d\n", i);

    frames_age_desc = freeList(frames_age_desc);

    return i;

}

int vms(Memory *mem) {
    int i, first_access; 
    list *frames_age_desc = EMPTY_LIST;

    for (i = 0; i < mem->numPF; i++) {
        frames_age_desc = listInsert(frames_age_desc, i, mem->frames[i].first_element);
    }

    DEBUG listPrint(frames_age_desc);

    while (1) {
        frames_age_desc = listRemoveFirst(frames_age_desc, &i, &first_access);

        DEBUG printf("Retired frame %d from the list, choosePage these: ", i);

        if (!mem->frames[i].ref) break;
        else {
            DEBUG printf("No, put it back.\n");
            mem->frames[i].first_element = mem->time_counter;
            mem->frames[i].ref = FALSE;
            frames_age_desc = listInsert(frames_age_desc, i, mem->frames[i].first_element);
        }

        DEBUG listPrint(frames_age_desc);
    }

    DEBUG printf("Simulate!\n");

    frames_age_desc = freeList(frames_age_desc);

    return i;
}

int lru(Memory *mem) {
    int i, last_element;
    list *frames_usage_desc = EMPTY_LIST;

    for (i = 0; i < mem->numPF; i++) {
        frames_usage_desc = listInsert(frames_usage_desc, i, mem->frames[i].last_element);
    }
    DEBUG listPrint(frames_usage_desc);

    frames_usage_desc = listRemoveFirst(frames_usage_desc, &i, &last_element);

    DEBUG printf("Most recently used frame: %d\n", i);

    frames_usage_desc = freeList(frames_usage_desc);

    return i;
}

/* Choose a page to remove from memory. */
int choosePage(Memory *mem) {
    int frame = -1;

    if (mem->algo == FIFO) {
        DEBUG printf("[!] Executing Algorithm 'FIFO'\n");
        frame = fifo(mem);
    } else if(mem->algo == VMS) {
        DEBUG printf("[!] Executing Algorithm 'Second Chance'\n");
        frame = vms(mem);
    } else if(mem->algo == LRU) {
        DEBUG printf("[!] Executing Algorithm 'LRU'\n");
        frame = lru(mem);
    }

    DEBUG printf("Frame chosen: %d\n", frame);

    assert(frame != -1);

    return frame;
}

/* Removes a page frame from physical memory. */
void removePage(Memory *mem, int frame_i) {
    assert(0 <= frame_i && frame_i < mem->numPF);

    DEBUG printf("Removendo quadro %d...\n", frame_i);

    mem->frames[frame_i].virtual_index = NOT_IN_MEMORY;
    mem->frames[frame_i].ref = FALSE;
    mem->frames[frame_i].mod = FALSE;
    mem->frames[frame_i].last_element = -1;
    mem->frames[frame_i].first_element = -1;
}

/*  Checks whether the page frame with index frame_i has been mod and
    simulates writing to disk.
    In this case, to simulate writing, simply increment a counter. */
void checkPage(Memory *mem, int frame_i) {
    DEBUG printf("Page frame %d has been mod? ", frame_i);
    if (mem->frames[frame_i].mod) {
        DEBUG printf("Simulation! +1 Written page \n");

        mem->stats.writes_to_disk += 1;

        DEBUG printvar(mem->stats.writes_to_disk);
    }else{
        DEBUG printf("Simulation! +1 Read page \n");
        mem->stats.reads_to_disk += 1;
        DEBUG printvar(mem->stats.writes_to_disk);
    }
    DEBUG printf("+1 Page read\n");
    mem->stats.page_faults += 1;
}

/* Simulates loading a page frame into physical memory. */
void loadPage(Memory *mem, int vir_i, int frame_i, char rw) {

    DEBUG printf("\n""Loading new frame %d on virtual_index=%d, rw=%c\n", frame_i, vir_i, rw);

    mem->frames[frame_i].virtual_index = vir_i;
    mem->frames[frame_i].last_element = mem->time_counter;
    mem->frames[frame_i].first_element = mem->time_counter;
    mem->frames[frame_i].ref = TRUE;

    if (rw == 'W') {
        mem->frames[frame_i].mod = TRUE;
    }
}

/* Look for the page frame with the virtual_index virtual index.
   If found, returns the index of the page in the frames vector.
   Note: This is only necessary in the simulation, in practice we would have another vector
   representing the virtual page table. */
int findPage(Memory *mem, int vir_i) {
    int i;

    DEBUG printf("Searching page with virtual_index=%d... ", vir_i);

    for (i = 0; i < mem->numPF; i++) {
        if (mem->frames[i].virtual_index == vir_i) {
            DEBUG printf("frame = %d\n", i);
            return i;
        }
    }

    DEBUG printf("not found!\n");

    return NOT_IN_MEMORY;
}

/********** Public Functions **********/

Memory *memInit(char *algo, int nframes) {
    int i;
    Memory *mem = (Memory *) malloc(sizeof(Memory));
    memCheck(mem);

    srand(time(NULL));

    mem->algo = AlgoOfChoice(algo);

    mem->stats.writes_to_disk = 0;
    mem->stats.page_faults = 0 ;
    mem->stats.reads_to_disk = 0;

    mem->time_counter = 0;
    mem->numUPF = 0;

    mem->numPF = nframes;

    mem->frames = (PageFrame *) malloc(mem->numPF * sizeof(PageFrame));
    memCheck(mem->frames);

    for (i = 0; i < mem->numPF; i++) {
        mem->frames[i].ref = FALSE;
        mem->frames[i].mod = FALSE;
        mem->frames[i].last_element = -1;
        mem->frames[i].first_element = -1;
        mem->frames[i].virtual_index = NOT_IN_MEMORY;
    }

    return mem;
}

void freeMem(Memory *mem) {
    free(mem->frames);
    free(mem);
}

/*void memClockInturrupt(Memory *mem) {
    int i;

    mem->time_counter += 1;

    if (mem->time_counter % CLEAR_INTERVAL == 0) {
        DEBUG printf("[clock interrupt] CLEANING BITS R\n\n");

        for (i = 0; i < mem->numPF; i++) {
            mem->frames[i].ref =  FALSE;
        }
    }
}*/

void accessMem(Memory *mem, unsigned vir_i, char rw) {
    int frame_i;

    frame_i = findPage(mem, vir_i);

    /* Page not in page table */
    if (frame_i == NOT_IN_MEMORY) {
        DEBUG printf("==> PAGE FAULT!\n"); 

        /* Physical memory has space */
        if (mem->numUPF < mem->numPF) {
            DEBUG printf("[!] \n""Algorithm does not start action yet: %d empty pages.\n", mem->numPF - mem->numUPF);
            loadPage(mem, vir_i, mem->numUPF, rw);
            mem->numUPF += 1;
        } else { // Physical memory full
            int chosen_frame_i; 

            /* When a page fault occurs, you should choosePage a page... */
            chosen_frame_i = choosePage(mem);
            /* if the page has been mod, you need to rewrite it in HD,
            to update the copy that was on the disk. */
            checkPage(mem, chosen_frame_i);
            /* ...to be removed from memory. */
            removePage(mem, chosen_frame_i);
            /* Put the new page in memory. */
            loadPage(mem, vir_i, chosen_frame_i, rw);

            mem->frames[chosen_frame_i].ref = TRUE;
        }
    } else { // Page is in table
        DEBUG printf("==> Page HIT!\n");

        mem->frames[frame_i].ref = TRUE ;
        mem->frames[frame_i].last_element = mem->time_counter;

        if (rw == 'W') {
            mem->frames[frame_i].mod = TRUE;
            DEBUG printf("frames[%d].mod = TRUE\n", frame_i);
        }
    }
}

void printFrames(Memory *mem) {
    int i;

    printf("\nTime Counter: %d\n+", mem->time_counter);
    for (i = 0; i < 50; i++) printf("-");
    printf("\n|\tframe_i\tvirtual_index\tref\tmod\tfirst\tlast\n");
    for (i = 0; i < mem->numPF; i++) {
        printf("|\t%d\t%d\t%d\t%d\t%d\t%d\n", i, 
                                         mem->frames[i].virtual_index,
                                         mem->frames[i].ref,
                                         mem->frames[i].mod,
                                         mem->frames[i].first_element,
                                         mem->frames[i].last_element);
    } 

    printf("+");
    for (i = 0; i < 50; i++) printf("-");
    printf("\n");
}

int memTime(Memory *mem) {
    return mem->time_counter;
}
