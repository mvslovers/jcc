/*
 * JIS MVS3.8j WWW Application Server.  (C)2004 Jason Paul Winter, All Rights Reserved.
 *
 * jasonwinter@hotmail.com, use "Subject" wisely - spam gets deleted without being read.
 *
 */

#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <jclass.h>

extern void(*__libc_ECS)(LPCRITICAL_SECTION lock);
extern void(*__libc_LCS)(LPCRITICAL_SECTION lock);

// Error when keys don't exist for before and after, and duplicates too!

#define Collection
#include "asp.h"

void * NewObject ();     // A Real Collection (Safe Array)...

// Collection object.

int Collection_init (void * me);

static list_ptr c_add (VARIANT * item, char * key, char * before, char * after);
static void c_remove ();
static VARIANT * c_item (char * key);
static VARIANT * c_for ();
static VARIANT * c_next ();

// Properties

static int Count = 0;

static int No_Errors = 1;

// Methods

static VARIANT * (*Default) ();

static list_ptr (*Add) ();
static list_ptr (*AddKeyed) ();
static list_ptr (*AddUpdate) ();
static void (*Remove) ();

static VARIANT * (*Item) ();
static VARIANT * (*ItemIndexed) ();
static VARIANT * (*ItemKeyed) ();
static int       (*HasKeys) ();

static VARIANT * (*For) ();
static VARIANT * (*Next) ();

// Globals

static CRITICAL_SECTION lock = 0;
static list_ptr head = NULL;

// Code

// c_add must accept a Variant...  Increase it's instance_count (#references)
// - but only when the item is added properly to the collection.
static list_ptr c_add (VARIANT * item, char * key, char * before, char * after) {
    list_ptr  a;
    list_ptr  s;
    int       i;
    int       j;
    char      buf [32];
    VARIANT * v;
    VARIANT * v2;
    void    * z;

    if ((before) && (after)) { // Raise an exception?
        if (No_Errors == 0)
            vb_error ("Collection", "Add", "Invalid parameters of both before and after.");
        return (NULL);
    };

    if (key == NULL) {
        sprintf (buf, "%d", Count + 1);
        key = buf;
    };

    if (after)
        before = after;

    if (before) {

        if ((*before == 0) || (strspnp (before, "0123456789"))) {

            s = head;
            while (s) {
                if (strcmp (s->key, before) == 0) break;
                s = s->next;
            };

        } else {
            i = atol (before);
            s = head;
            while (s) {
                if (s->position == i) break;
                s = s->next;
            };
        };

        if (s == NULL) { // Raise an exception?
            if (after) {
                if (No_Errors == 0)
                    vb_error ("Collection", "Add", "Item not found looking for after=\"%s\".", after);
                return (NULL);
            } else {
                if (No_Errors == 0)
                    vb_error ("Collection", "Add", "Item not found looking for before=\"%s\".", before);
                return (NULL);
            };
        };

        if (after)
            i = s->position + 1;
        else
            i = s->position;

        if (*key) {

            s = head;
            while (s) {
                if (strcmp (s->key, key) == 0) break;
                s = s->next;
            };

            if (s) { // Raise an exception?
                if (No_Errors == 0)
                    vb_error ("Collection", "Add", "Item=\"%s\" already exists.", key);
                return (NULL);
            };

            j = strlen (key) + 1;

        } else
            j = 0;

        goto foa;
    };

    if ((*key == 0) || (strspnp (key, "0123456789"))) {

        s = head;
        while (s) {
            if (strcmp (s->key, key) == 0) break;
            s = s->next;
        };

        if ((s) && (s->item->SubType != 2)) {

            v = NewObject (&Collection_init); // Create a new Variant of type Collection
            v->SubType = 2;
            v2 = s->item;      // Get old 'String'
            s->item = v;       // Set to new Collection

            _push_object (v->Value.ov);   // Use new Collection
            c_add (v2, NULL, NULL, NULL); // Add + Increase Reference Count
            _pop_object (v->Value.ov);    // Unuse new Collection
            _del_object (v2);             // Decrease Reference Count again
        };

        if ((s) && (s->item->SubType == 2)) {
            _push_object (s->item->Value.ov);
            a = c_add (item, NULL, NULL, NULL);
            _pop_object (s->item->Value.ov);
            return (a);
        };

        if (__libc_ECS) (*__libc_ECS) (&lock);

        a = (list_ptr)malloc (sizeof (list_sz) + strlen (key) + 1);
        if (a) {
            a->item = item;
            a->key = &(((char *)a) [sizeof (list_sz)]);
            strcpy (a->key, key);
            a->position = ++Count;
        };
    } else {
        i = atol (key);
        j = 0;
foa:
        if (__libc_ECS) (*__libc_ECS) (&lock);

        a = (list_ptr)malloc (sizeof (list_sz) + j);
        if (a) {
            a->item = item;
            if (j) {
                a->key = &(((char *)a) [sizeof (list_sz)]);
                strcpy (a->key, key);
            } else
                a->key = NULL;

            s = head;
            while (s) {
                if (s->position >= i) (s->position)++;
                s = s->next;
            };
            a->position = i;
            Count++;
        };
    };

    if (a) {
        a->item->instance_count_inc (1);

        a->next = head;
        head = a;
    };

    if (__libc_LCS) (*__libc_LCS) (&lock);

#ifdef DEBUG
    printf ("Collection Item Added.\n");
#endif

    return (a);
};

static list_ptr c_add_update (VARIANT * item, char * key) {
    list_ptr  a;
    list_ptr  s;
    int       i;
    int       j;
    char      buf [32];
    VARIANT * v;
    VARIANT * v2;
    void    * z;

    if (key == NULL) {
        sprintf (buf, "%d", Count + 1);
        key = buf;
    };

    if ((*key == 0) || (strspnp (key, "0123456789"))) {

        s = head;
        while (s) {
            if (strcmp (s->key, key) == 0) {
                
                // Update...
                
                if (__libc_ECS) (*__libc_ECS) (&lock);

                _del_object (s->item);
                s->item = item;
                s->item->instance_count_inc (1);
                
                if (__libc_LCS) (*__libc_LCS) (&lock);

                return (s);
            };
            s = s->next;
        };

        if (__libc_ECS) (*__libc_ECS) (&lock);

        a = (list_ptr)malloc (sizeof (list_sz) + strlen (key) + 1);
        if (a) {
            a->item = item;
            a->key = &(((char *)a) [sizeof (list_sz)]);
            strcpy (a->key, key);
            a->position = ++Count;
        };
    } else {

        i = atol (key);
        s = head;
        while (s) {
            if (s->position == i) {
                
                // Update...
                
                if (__libc_ECS) (*__libc_ECS) (&lock);

                _del_object (s->item);
                s->item = item;
                s->item->instance_count_inc (1);

                if (__libc_LCS) (*__libc_LCS) (&lock);

                return (s);
            };
            s = s->next;
        };

        if (__libc_ECS) (*__libc_ECS) (&lock);

        a = (list_ptr)malloc (sizeof (list_sz));
        if (a) {
            a->item = item;
            a->key = NULL;

            s = head;
            while (s) {
                if (s->position >= i) (s->position)++;
                s = s->next;
            };
            a->position = i;
            Count++;
        };
    };

    if (a) {
        a->item->instance_count_inc (1);

        a->next = head;
        head = a;
    };

    if (__libc_LCS) (*__libc_LCS) (&lock);

    return (a);
};

static list_ptr c_add_keyed (VARIANT * item, char * key, char * key2) {
    list_ptr  a;
    list_ptr  s;
    int       i;
    int       j;
    char      buf [32];
    VARIANT * v;
    VARIANT * v2;
    void    * z;

    if (key == NULL) {
        sprintf (buf, "%d", Count + 1);
        key = buf;
    };

    if ((*key == 0) || (strspnp (key, "0123456789"))) {

        s = head;
        while (s) {
            if (strcmp (s->key, key) == 0) break;
            s = s->next;
        };
    } else {

        i = atol (key);
        s = head;
        while (s) {
            if (s->position == i) break;
            s = s->next;
        };
    };

    if ((s) && (s->item->SubType != 2)) {

        v = NewObject (&Collection_init); // Create a new Variant of type Collection
        v->SubType = 2;
        v2 = s->item;      // Get old 'String'
        s->item = v;       // Set to new Collection

        _push_object (v->Value.ov);   // Use new Collection
        c_add (v2, NULL, NULL, NULL); // Increase Reference Count
        _pop_object (v->Value.ov);    // Unuse new Collection
        _del_object (v2);             // Decrease Reference Count again

    } else if (s == NULL) { // Make a new 'empty' collection...

        v = NewObject (&Collection_init); // Create a new Variant of type Collection
        v->SubType = 2;
        s = c_add (v, key, NULL, NULL); // Increase Reference Count
        _del_object (v);                // Decrease Reference Count again
    };

    _push_object (s->item->Value.ov);
    a = c_add (item, key2, NULL, NULL);
    _pop_object (s->item->Value.ov);
    return (a);
};

static void c_remove (char * key) {
    list_ptr p;
    list_ptr s;
    int i;

    if ((*key == 0) || (strspnp (key, "0123456789"))) {
        p = NULL;
        s = head;
        while (s) {
            if (strcmp (s->key, key) == 0) break;
            p = s;
            s = s->next;
        };
    } else {
        i = atol (key);
        p = NULL;
        s = head;
        while (s) {
            if (s->position == i) break;
            p = s;
            s = s->next;
        };
    };

    if (s) {

        if (__libc_ECS) (*__libc_ECS) (&lock);

        if (p)
            p->next = s->next;
        else {
            head = s->next;
        };

        s->item->instance_count_inc (-1);
        if (s->item->instance_count == 0)
            _del_object (s->item);

        free (s);

        s = head;
        while (s) {
            if (s->position > i) (s->position)--;
            s = s->next;
        };
        Count--;

        if (__libc_LCS) (*__libc_LCS) (&lock);

    } else { // Raise an exception?
        if (No_Errors == 0)
            vb_error ("Collection", "Remove", "Item=\"%s\" doesn't exist.", key);
        return;
    };

#ifdef DEBUG
    printf ("Collection Item Removed.\n");
#endif
};

static VARIANT * c_item (char * key) { // *** DEFAULT METHOD ***
    list_ptr p;
    list_ptr s;
    int i;

    if ((*key == 0) || (strspnp (key, "0123456789"))) {
        p = NULL;
        s = head;
        while (s) {
            if (strcmp (s->key, key) == 0) break;
            p = s;
            s = s->next;
        };
    } else {
        i = atol (key);
        p = NULL;
        s = head;
        while (s) {
            if (s->position == i) break;
            p = s;
            s = s->next;
        };
    };

    if (s == NULL) { // Raise an exception?
        if (No_Errors == 0)
            vb_error ("Collection", "Item", "Item=\"%s\" doesn't exist.", key);
        return (NULL);
    };

#ifdef DEBUG
    printf ("Collection Item Found.\n");
#endif

    return (s->item);
};

static int c_item_haskeys (char * key) {
    list_ptr p;
    list_ptr s;
    int i;

    if ((*key == 0) || (strspnp (key, "0123456789"))) {
        p = NULL;
        s = head;
        while (s) {
            if (strcmp (s->key, key) == 0) break;
            p = s;
            s = s->next;
        };
    } else {
        i = atol (key);
        p = NULL;
        s = head;
        while (s) {
            if (s->position == i) break;
            p = s;
            s = s->next;
        };
    };

    if (s == NULL) { // Raise an exception?
        if (No_Errors == 0)
            vb_error ("Collection", "HasKeys", "Item=\"%s\" doesn't exist.", key);
        return (NULL);
    };

#ifdef DEBUG
    printf ("Collection Item Found.\n");
#endif

    return (s->item->SubType == 2);
};

//
// INDEXED VERSION - This is needed when a Collection points through a Variant into a Collection!
//

static VARIANT * c_item_indexed (char * key, int index) {
    list_ptr p;
    list_ptr s;
    int i;
    VARIANT * a;
    char      buf [12];

    if ((*key == 0) || (strspnp (key, "0123456789"))) {
        p = NULL;
        s = head;
        while (s) {
            if (strcmp (s->key, key) == 0) break;
            p = s;
            s = s->next;
        };
    } else {
        i = atol (key);
        p = NULL;
        s = head;
        while (s) {
            if (s->position == i) break;
            p = s;
            s = s->next;
        };
    };

    if (s == NULL) { // Raise an exception?
        if (No_Errors == 0)
            vb_error ("Collection", "ItemIndexed", "Item=\"%s\" doesn't exist.", key);
        return (NULL);
    };

#ifdef DEBUG
    printf ("Collection Item Found.\n");
#endif

    if (s->item->SubType == 2) {
        itoa (index, buf, 10);
        _push_object (s->item->Value.ov);
        a = c_item (buf);
        _pop_object (s->item->Value.ov);
        return (a);
    } else if (index == 1)
        return (s->item);
    else
        return (NULL);
};

//
// KEYED VERSION - This is needed for COOKIE Dictionary strings.
//

static VARIANT * c_item_keyed (char * key, char * key2) {
    list_ptr p;
    list_ptr s;
    int i;
    VARIANT * a;

    if ((*key == 0) || (strspnp (key, "0123456789"))) {
        p = NULL;
        s = head;
        while (s) {
            if (strcmp (s->key, key) == 0) break;
            p = s;
            s = s->next;
        };
    } else {
        i = atol (key);
        p = NULL;
        s = head;
        while (s) {
            if (s->position == i) break;
            p = s;
            s = s->next;
        };
    };

    if (s == NULL) { // Raise an exception?
        if (No_Errors == 0)
            vb_error ("Collection", "ItemIndexed", "Item=\"%s\" doesn't exist.", key);
        return (NULL);
    };

#ifdef DEBUG
    printf ("Collection Item Found.\n");
#endif

    if (s->item->SubType == 2) {
        _push_object (s->item->Value.ov);
        a = c_item (key2);
        _pop_object (s->item->Value.ov);
        return (a);
    } else
        return (NULL);
};

static VARIANT * c_for (list_ptr * last) {

    *last = head;
    if (head)
        return (head->item);
    else
        return (NULL);
};

static VARIANT * c_next (list_ptr * last) {

    if (*last)
        *last = (*last)->next;
    if (*last)
        return ((*last)->item);
    else
        return (NULL);
};

int Collection_init (void * me) {

    // Set up object methods
    Default = _make_f_vector (&c_Wrap, &c_item, me);
    Add = _make_f_vector (&c_Wrap, &c_add, me);
    AddKeyed = _make_f_vector (&c_Wrap, &c_add_keyed, me);
    AddUpdate = _make_f_vector (&c_Wrap, &c_add_update, me);
    Remove = _make_f_vector (&c_Wrap, &c_remove, me);
    
    Item = _make_f_vector (&c_Wrap, &c_item, me);
    ItemIndexed = _make_f_vector (&c_Wrap, &c_item_indexed, me);
    ItemKeyed = _make_f_vector (&c_Wrap, &c_item_keyed, me);
    HasKeys = _make_f_vector (&c_Wrap, &c_item_haskeys, me);

    For = _make_f_vector (&c_Wrap, &c_for, me);
    Next = _make_f_vector (&c_Wrap, &c_next, me);

#ifdef DEBUG
    printf ("Collection Created.\n");
#endif

    return (1);
};

int Collection_exit () {
    list_ptr a;

    while (a = head) {
        head = head->next;

        a->item->instance_count_inc (-1);
        if (a->item->instance_count == 0)
            _del_object (a->item);

        free (a);
    };

    // Delete: function vectors
    _del_f_vector (Default);
    _del_f_vector (Add);
    _del_f_vector (AddKeyed);
    _del_f_vector (AddUpdate);
    _del_f_vector (Remove);
    
    _del_f_vector (Item);
    _del_f_vector (ItemIndexed);
    _del_f_vector (ItemKeyed);
    _del_f_vector (HasKeys);

    _del_f_vector (For);
    _del_f_vector (Next);

#ifdef DEBUG
    printf ("Collection Deleted.\n");
#endif

    return (1);
};
