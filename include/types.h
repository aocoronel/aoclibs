#ifndef AOC_TYPES_H_
#define AOC_TYPES_H_

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

#ifndef AOC_BUDDY_H_
$declare(struct)(Buddy_Block);
$declare(struct)(Buddy_Page);
$declare(struct)(Buddy_Header);
$declare(struct)(Buddy);
#endif // AOC_BUDDY_H_

#ifndef AOC_CSTR_H_
$declare(struct)(Slice);
#endif // AOC_CSTR_H_

#ifndef AOC_FILE_H_
$declare(enum)(File_Type);
$declare(struct)(Dir_Walker);
#endif // AOC_FILE_H_

#ifndef AOC_FORK_H_
$declare(struct)(Fork_Result);
$declare(struct)(Fork_Options);
$declare(struct)(Cmd_Result);
#endif // AOC_FORK_H_

#ifndef AOC_FQUERY_H_
$declare(struct)(FQuery);
#endif // AOC_FQUERY_H_

#ifndef AOC_HASHMAP_H_
$declare(struct)(Hash_Entry_Tmpl);
$declare(struct)(Hash_Map_Tmpl);
#endif // AOC_HASHMAP_H_

#ifndef AOC_MAP_H_
$declare(struct)(Map_Tmpl);
#endif // AOC_MAP_H_

#ifndef AOC_PP_H_
$declare(struct)(Pointer_Pool);
#endif // AOC_PP_H_

#ifndef AOC_RC_H_
$declare(struct)(rcs);
$declare(struct)(rc);
#endif // AOC_RC_H_

#ifndef AOC_STACK_H_
$declare(struct)(Stack);
#endif // AOC_STACK_H_

#endif // AOC_TYPES_H_
