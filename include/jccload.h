/*********************************************************************
 * JCC With Dynamic Load Libraries                                   *
 * -------------------------------                                   *
 *                                                                   *
 * NOTE: These library routines are only supported in JCC RENT mode. *
 *                                                                   *
 * A JCC dynamic module is the standard output from the compiler in  *
 * -rent/-r mode.  This is XSD OMF (ESD only allows 8 characters for *
 * externals, but the loader here allows up to 40. ESD unsupported.) *
 *                                                                   *
 * WARNING: Pre-linked objects are not supported via this interface, *
 *          although concatenated objects are and will be 'grouped'. *
 *                                                                   *
 *    Copyright (C) 2004 Jason Paul Winter, All Rights Reserved.     *
 *                                                                   *
 *********************************************************************/

#include <errno.h>

void * _jcc_load   (char * file);
// Returns: a handle to a loaded object
//
// file is the pathname & optionally a membername of the object to read
// - Once loaded, new symbols will be available with _jcc_deref.
//
// Errors:
// -------
// ENOENT        - File not found / couldn't be opened.
// ENOSPC        - The internal limit (1024) has been reached.
// EINVAL        - The Object was corrupt.
// EADDRNOTAVAIL - A missing reference prevents loading.
//
// char * _jcc_load_reference [256]; contains thread-specific missing
// references for when errno has been set to EADDRNOTAVAIL but the
// following #define _jcc_load_myref will also access this pointer.

extern char * _jcc_load_reference [256];
#define _jcc_load_myref _jcc_load_reference [athreadid()]

int    _jcc_unload (void * handle);
// Returns: Boolean 0-OK 1-Error
//
// WARNING:
// It would be a good assumption to make unloads order specific since any
// new object may refer to previously loaded ext.identifiers...
// - The library will delete code for which pointers are still held elsewhere.

void * _jcc_deref  (char * identifier);
// Returns: The identifier address or NULL if unknown.

int    _jcc_adref  (char * name, void * entry);
// Returns: Boolean 0-OK
//
// Adds a user-address to the run-time symbol table, and may replace
// existing identifiers or remove them (using entry = NULL.)
// - user-addresses are not deleted when an object is unloaded.

