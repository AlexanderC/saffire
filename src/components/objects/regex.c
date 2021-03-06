/*
 Copyright (c) 2012-2013, The Saffire Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the Saffire Group the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <pcre.h>
#include "objects/object.h"
#include "objects/objects.h"
#include "general/smm.h"
#include "general/md5.h"
#include "debug.h"
#include "general/output.h"


/* ======================================================================
 *   Supporting functions
 * ======================================================================
 */


/* ======================================================================
 *   Object methods
 * ======================================================================
 */


/**
 * Saffire method: constructor
 */
SAFFIRE_METHOD(regex, ctor) {
    RETURN_SELF;
}


/**
 * Saffire method: destructor
 */
SAFFIRE_METHOD(regex, dtor) {
    RETURN_NULL;
}


/**
 * Saffire method: Returns string regex
 */
SAFFIRE_METHOD(regex, regex) {
    RETURN_STRING(self->regex_string);
}


/**
 * Saffire method: match a string against (compiled) regex
 */
SAFFIRE_METHOD(regex, match) {
    t_string_object *str;
    int ovector[OVECCOUNT];
    int rc;

    // Parse the arguments
    if (! object_parse_arguments(SAFFIRE_METHOD_ARGS, "s", &str)) {
        return NULL;
    }

    // Convert to utf8 and execute regex
    rc = pcre_exec(self->regex, 0, str->value, strlen(str->value), 0, 0, ovector, OVECCOUNT);

    // Check result
    if (rc < 0) {
        switch (rc) {
            case PCRE_ERROR_NOMATCH:
                // No match found, is not an exception
                rc = 0;
                break;
            default :
                object_raise_exception(Object_SystemException, 1, "Error while matching: error code %d\n", rc);
                return NULL;
        }
    }

    // Display result
    for (int i=0; i<rc; i++) {
        DEBUG_PRINT("%2d: %.*s\n", i, ovector[2*i+1] - ovector[2*i], str->value + ovector[2*i]);
    }

    // Return number of matches
    RETURN_NUMERICAL(rc);
}


/**
 *
 */
SAFFIRE_METHOD(regex, conv_boolean) {
    if (strlen(self->regex_string) == 0) {
        RETURN_FALSE;
    } else {
        RETURN_TRUE;
    }
}


/**
 *
 */
SAFFIRE_METHOD(regex, conv_null) {
    RETURN_NULL;
}


/**
 *
 */
SAFFIRE_METHOD(regex, conv_numerical) {
    RETURN_NUMERICAL(0);
}


/**
 *
 */
SAFFIRE_METHOD(regex, conv_string) {
    RETURN_STRING(self->regex_string);
}

/* ======================================================================
 *   Global object management functions and data
 * ======================================================================
 */

/**
 * Initializes regex methods and properties, these are used
 */
void object_regex_init(void) {
    Object_Regex_struct.attributes = ht_create();
    object_add_internal_method((t_object *)&Object_Regex_struct, "__ctor",        ATTRIB_METHOD_CTOR, ATTRIB_VISIBILITY_PUBLIC, object_regex_method_ctor);
    object_add_internal_method((t_object *)&Object_Regex_struct, "__dtor",        ATTRIB_METHOD_DTOR, ATTRIB_VISIBILITY_PUBLIC, object_regex_method_dtor);

    object_add_internal_method((t_object *)&Object_Regex_struct, "__boolean",     ATTRIB_METHOD_STATIC, ATTRIB_VISIBILITY_PUBLIC, object_regex_method_conv_boolean);
    object_add_internal_method((t_object *)&Object_Regex_struct, "__null",        ATTRIB_METHOD_STATIC, ATTRIB_VISIBILITY_PUBLIC, object_regex_method_conv_null);
    object_add_internal_method((t_object *)&Object_Regex_struct, "__numerical",   ATTRIB_METHOD_STATIC, ATTRIB_VISIBILITY_PUBLIC, object_regex_method_conv_numerical);
    object_add_internal_method((t_object *)&Object_Regex_struct, "__string",      ATTRIB_METHOD_STATIC, ATTRIB_VISIBILITY_PUBLIC, object_regex_method_conv_string);

    object_add_internal_method((t_object *)&Object_Regex_struct, "match",       ATTRIB_METHOD_STATIC, ATTRIB_VISIBILITY_PUBLIC, object_regex_method_match);
    object_add_internal_method((t_object *)&Object_Regex_struct, "regex",       ATTRIB_METHOD_STATIC, ATTRIB_VISIBILITY_PUBLIC, object_regex_method_regex);

    vm_populate_builtins("regex", (t_object *)&Object_Regex_struct);
}

/**
 * Frees memory for a regex object
 */
void object_regex_fini(void) {
    // Free attributes
    object_free_internal_object((t_object *)&Object_Regex_struct);
}


static t_object *obj_new(t_object *self) {
    t_regex_object *obj = smm_malloc(sizeof(t_regex_object));
    memcpy(obj, Object_Regex, sizeof(t_regex_object));

    // Dynamically allocated
    obj->flags |= OBJECT_FLAG_ALLOCATED;

    // These are instances
    obj->flags &= ~OBJECT_TYPE_MASK;
    obj->flags |= OBJECT_TYPE_INSTANCE;

    return (t_object *)obj;
}


static void obj_populate(t_object *obj, t_dll *arg_list) {
    t_regex_object *re_obj = (t_regex_object *)obj;
    const char *error;
    int erroffset;

    t_dll_element *e = DLL_HEAD(arg_list);
    re_obj->regex_string = (char *)e->data;
    e = DLL_NEXT(e);

    int pcre_options = (int)e->data;

    re_obj->regex = pcre_compile(re_obj->regex_string, PCRE_UTF8 | pcre_options, &error, &erroffset, 0);
    if (! re_obj->regex) {
        // @TODO: How do we detect an exception that has been thrown here!??
        object_raise_exception(Object_ArgumentException, 1, "Error while compiling regular expression at offset %d: %s", erroffset, error);
        // @TODO: We must return NULL
        //return NULL;
    }
}

static void obj_free(t_object *obj) {
   t_regex_object *re_obj = (t_regex_object *)obj;

   if (re_obj->regex) {
       free(re_obj->regex);
   }
}

static void obj_destroy(t_object *obj) {
    smm_free(obj);
}

#ifdef __DEBUG
char global_buf[1024];
static char *obj_debug(t_object *obj) {
    if (OBJECT_TYPE_IS_CLASS(obj)) {
        strcpy(global_buf, "Regex");
    } else {
        sprintf(global_buf, "regex(%s)", ((t_regex_object *)obj)->regex_string);
    }
    return global_buf;
}
#endif

// Regex object management functions
t_object_funcs regex_funcs = {
        obj_new,              // Allocate a new regex object
        obj_populate,         // Populate
        obj_free,             // Free a regex object
        obj_destroy,          // Clone a regex object
        NULL,                 // Clone
        NULL,                 // Cache
#ifdef __DEBUG
        obj_debug
#endif
};

// Intial object
t_regex_object Object_Regex_struct = {
    OBJECT_HEAD_INIT("regex", objectTypeRegex, OBJECT_TYPE_CLASS, &regex_funcs),
    NULL,
    '\0',
};
