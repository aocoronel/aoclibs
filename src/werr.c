#include <aoclibs/common.h>
#include <stddef.h>

Err werr(int code, const char msg[]) {
        if (msg == NULL) return (Err){ code, "undefined" };
        return (Err){ code, msg };
}

// clang-format off

echar   werr_char(Err err, char value)      { return (echar){err, value}; }
estr    werr_str(Err err, char* value)      { return (estr){err, value}; }
evoid   werr_void(Err err, void* value)      { return (evoid){err, value}; }
ebool   werr_bool(Err err, bool value)       { return (ebool){err, value}; }

ei8     werr_i8(Err err, i8 value)      { return (ei8){err, value}; }
ei16    werr_i16(Err err, i16 value)     { return (ei16){err, value}; }
ei32    werr_i32(Err err, i32 value)     { return (ei32){err, value}; }
ei64    werr_i64(Err err, i64 value)     { return (ei64){err, value}; }

eu8     werr_u8(Err err, u8 value)      { return (eu8){err, value}; }
eu16    werr_u16(Err err, u16 value)    { return (eu16){err, value}; }
eu32    werr_u32(Err err, u32 value)    { return (eu32){err, value}; }
eu64    werr_u64(Err err, u64 value)    { return (eu64){err, value}; }

ef32    werr_f32(Err err, f32 value)       { return (ef32){err, value}; }
ef64    werr_f64(Err err, f64 value)      { return (ef64){err, value}; }

eisize  werr_isize(Err err, intptr_t value)  { return (eisize){err, value}; }
eusize  werr_usize(Err err, uintptr_t value) { return (eusize){err, value}; }

// clang-format on
