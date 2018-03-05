To run the program run:
$: make
$: ./memsim <tracefile> <nframes> <lru|fifo|vms> <debug|quiet>

Where <tracefile> is the .trace file to be analyzed, <nframes> is the number
of frames to be used for the Page Table, <lru|fifo|vms> specified the algorithm
to be run, and <debug|quiet> gives detailed output or final output respectively.

NOTE: This program was tested on C4 labs machines, there is no guarantee it will
compile on other systems.
