/*********************************************************************
 * JCC With Classes:                                                 *
 * -----------------                                                 *
 *                                                                   *
 * When RENT programs are constructed by the prelinker, each object  *
 * which is pre-linked owns a known location which its functions     *
 * access to fill-in needed information in accessing RENT variables. *
 *                                                                   *
 * This source module is an interface to allow the internal table    *
 * of object read/write data pointers to be updated safely and allow *
 * 'object' creation by allocating a chunk of memory for its own     *
 * internal use.  The limit on objects is bound by processor memory. *
 *                                                                   *
 * Any external references into that file.obj 'object' will either   *
 * point to a function address-start or will point to the default    *
 * initialisation data section used to create objects of that type.  *
 * (May be changed to allow runtime control of new objects data.)    *
 *                                                                   *
 * Addresses of data within file.obj 'object' will also point to the *
 * original source data copy which allows easy access to true Global *
 * memory across all objects of that type.  Your initialiser for     *
 * the class should otherwise be used for data-address assignments.  *
 *                                                                   *
 * This last point about constant-initialisers and data-addresses is *
 * going to be the sticking point for new-users of JCC with Classes. *
 *                                                                   *
 *    Copyright (C) 2004 Jason Paul Winter, All Rights Reserved.     *
 *                                                                   *
 *********************************************************************/

/* WHY?: *
 * Because some people don't like using structures for everything,
 * which involves passing around at least one pointer to everything.
 * JCC has the ability at little or no cost over RENT code generation.
 * Can be a powerful tool used in TP systems/reentrant code wise.
 * (Granuality of any/all RENT data is now programmer controlled.)
 */

/*******************/
/*  Registration.  */
/*******************/

int    _jcc_object  (void * startfunc, void * endfunc);
// Returns: True (1) if startfunc has been 'registered' else
// if already registered:(-1) else False (0) failed.
//
// startfunc is called for every _new_object instance created
// endfunc is called for every _del_object...  Both must return
// non-zero to indicate there were no errors at startup or there
// is no reason to prevent the object from being deleted.
// Both functions take 1 parameter, the 'object's void * value.

void   _end_object  (void * startfunc); // *ALL* OF THIS *TYPE* ***
// 'deregister' and call all obj.class_terminate functions.
// end-of-program clean-up for zombie objects - not really needed,
// unless your objects need to save data on termination and you
// forget to delete the object when it has no further use.

/*******************/
/* object creation */
/*******************/

void * _new_object  (void * startfunc);
// Returns: an initialised object + memory-chunk.
// NULL indicates startfunc isn't a registered object.

void   _del_object  (void * old_object); // *THIS* INSTANCE ONLY ***
// calls class_terminate,
// deallocate memory associated with an object.

/*******************/
/* stack functions */
/*******************/

void * _push_object (void * old_object);
// Returns: the last object being referenced.
// can be safely discarded, since it's still on the stack.
// NULL indicates old_object isn't a registered object.

void * _pop_object  (void * old_object);
// Returns: the last object being referenced.
// NULL indicates old_object isn't a registered object.

/*********************/
/* special functions */
/*********************/

void * _make_f_vector (void * func, void * param1, void * param2);
// Returns: a new function to call an old function with given parameters.
// the new function moves R1 to R0 before vectoring...

void * _get_vector_r (void);
// Returns: Caller's R0 from entry (saved through vector op. above.)

void _del_f_vector (void * v);
// Deletes the 'code area' created by _make_f_...

void * _exec_vector (void * f, void * R1);
// Returns: anything_object pointer...
// The function in the vector f is executed with parameter pointer R1

// ** Usage notes: **
//
// These function-revectoring routines actually create a new JCC
// internal & nameless function.  The parameters you supply to this
// function, when finally calling it, are sent to the function 'func'
// which calls _get_vector_r to obtain the original param-list area.
//
// _exec_vector allows a JCC function to be executed when you have
// a value for R1 instead of wanting the compiler to build parameters.
//
// Expected uses for _make_'s param1 & param2 are:
// param1 - another function pointer (which ends up using R0 as its R1)
// param2 - class-object pointer for any required 'push' initialisation.
//
// The example shows how to automate the push-pop sequence when using
// the vectoring routines.  Methods in class objects which need to
// be automated like this, should be defined as function pointers and
// initialised within the specified class_init function.
//
// Eg.
//     void * (*a) (); // a is a function pointer...
//     a = _make_f_vector (my_vec, my_method, my_obj);
//     (*a) (1, 2, 3); // execute the function referred to in a.
//
// void * my_vec (void (*f)(), void * o) {
//     void * r;
//     void * t = _get_vector_r ();
//     _push_obj (o);               // <<< THIS IS WHY WE VECTOR
//     r = exec_vector (f, t);
//     _pop_object (o);
//     return (r);
// }
//
// void * my_method (int a, int b, int c) {
//     printf ("a=%d b=%d c=%d\n", a, b, c);
//}
