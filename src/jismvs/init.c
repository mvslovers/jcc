extern char __dd_name []; // JCC00xxx
extern char __subtask []; // SUBTSKxx

int __load_env () { // JCC Special function to do pre-initialisations.
    
    __dd_name [2] = '1'; // JC1 instead of JCC (and SUBTS1 instead of SUBTSK)
    __subtask [4] = '1'; // (To allow other JCC programs to run in this address space.)

    return (0);
};
