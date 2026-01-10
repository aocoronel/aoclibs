#ifndef AOCLIBS_ATTRIBUTES_H_
#define AOCLIBS_ATTRIBUTES_H_

/*
 * Modify prefixes in all functions
*/
#define AOCLIBS_PREFIX

/*
 * The concept of _Nonnull and _Nullable is fascinating and is interesting when combined with
 * assertions, or even with the Clang compiler, thus enforcing if a pointer can or cannot be NULL.
 *
 * This is specially useful, perhaps when a function is never supposed to return NULL or take NULL.
 * A great example is the free function. If we define a custom free wrapper to the free function with
 * a _Nonnull to the pointer and an assertion, the program will immediately stop and warn you have a
 * double free issue.
*/

/*
 * ref (reference) :: aliased to _Nonnull.
 * null :: aliased to _Nullable
*/

// clang-format off

#ifndef __clang__
        #define ref
        #define xref
        #define null
        #define xnull
#else
        #define ref _Nonnull
        #define xref _Nonnull restrict
        #define null _Nullable
        #define xnull _Nullable restrict
#endif

// clang-format on

/*
 * Convenient macros to improve user experience
*/

#define FN_DEPRECATED(fn_to_use_instead) __attribute_deprecated_msg__(fn_to_use_instead)
#define FN_WARN_UNUSED __attribute__((warn_unused_result))

#endif // AOCLIBS_ATTRIBUTES_H_
