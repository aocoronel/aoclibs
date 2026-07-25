#ifndef AOC_TYPES_H_
#define AOC_TYPES_H_

#include "alloc.h"
#ifndef __cplusplus

// C forces you to do this, when you want to define a struct that uses itself
//   typedef struct Node {
//       struct Node *node;
//   }
//
// For a much better way to do this, and less repetitive
//   $declare(struct)(My_Very_Big_Node_Name_So_It_Makes_Sense);
//   // Expands to
//   typedef struct My_Very_Big_Node_Name_So_It_Makes_Sense My_Very_Big_Node_Name_So_It_Makes_Sense;
//
// Then you can use it like any decent language:
//   struct Node {
//       Node *node
//   }
#define $$declare(name) name name
#define $declare(type) typedef type $$declare

// alloc.h
$declare(struct)(General_Allocator);
$declare(struct)(Buffer_Allocator);
$declare(struct)(Fixed_Buffer);
$declare(struct)(Heap_Trace_Entry);

// base.h
$declare(struct)(Source_Code_Location);

// buddy.h
$declare(struct)(Buddy_Block);
$declare(struct)(Buddy_Page);
$declare(struct)(Buddy_Header);
$declare(struct)(Buddy);

// cstr.h
$declare(struct)(Slice);

// file.h
$declare(enum)(File_Type);
$declare(struct)(Dir_Walker);

// fork.h
$declare(struct)(Fork_Result);
$declare(struct)(Fork_Options);
$declare(struct)(Cmd_Result);

// fquery.h
$declare(struct)(FQuery);

// hmap.h
$declare(struct)(Hash_Entry_Tmpl);
$declare(struct)(Hash_Map_Tmpl);

// map.h
$declare(struct)(Map_Tmpl);

// pp.h
$declare(struct)(Pointer_Pool);

// rc.h
$declare(struct)(rcs);
$declare(struct)(rc);

// stack.h
$declare(struct)(Stack);

#endif // __cplusplus

#endif // AOC_TYPES_H_
