typedef struct JTEST_tag {
    void * _jcc_obj_type;
    void * _jcc_obj_data;
    void * _jcc_obj_next;
    long libcarch;
    void * c_wrapper;
    void * c_Lock;
    void * c_Unlock;
    void * c_init;
    void * c_term;
    void * c_print;
    int v;
    int * vp;
    void (*Unlock) ();
    void (*Lock) ();
} JTEST;
