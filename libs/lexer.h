#ifndef LEXER_H_
#define LEXER_H_

#include "base.h"
#include "io.h"
#include "cstr.h"
#include "colors.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

// lexer.h
//
// There were notable times I tried to parse text, for whatever reason: try to parse code, try to
// make a configuration file format, parse input and so on. I always did it manually, byte by byte.
// And this is one of my biggest headaches I have, specially in C. This library aims to remove this
// pain once and for all, so instead of work with bytes, I can work with tokens.
//
// Missing:
//
//   Important:
//     - Print errors
//     - Remove abort() macros
//
//   Should I even care?:
//     - lexer_char only computes _char.val based on UTF-8
//       L'日' will overflow
//       In this case, the user still have the intact string, and can calculate by theirselves

typedef enum {
    Lex_Tok_EOF,

    Lex_Tok_Identifier,
    Lex_Tok_Int,
    Lex_Tok_Float,
    Lex_Tok_String,
    Lex_Tok_Char,

    Lex_Tok_LParen,
    Lex_Tok_RParen,
    Lex_Tok_LBracket,
    Lex_Tok_RBracket,
    Lex_Tok_LBrace,
    Lex_Tok_RBrace,

    Lex_Tok_Comma,
    Lex_Tok_Semicolon,
    Lex_Tok_Dot,
    Lex_Tok_Colon,
    Lex_Tok_QuestionMark,
    Lex_Tok_Apostrophe,

    Lex_Tok_DoubleDot,
    Lex_Tok_Ellipsis,
    Lex_Tok_RArrow,

    Lex_Tok_Operator,

    Lex_Tok_Unknown,
} Lexer_Token_Kind;

typedef enum {
    Lex_Num_Decimal,
    Lex_Num_Hex,
    Lex_Num_Octal,
    Lex_Num_Binary,
} Lexer_Num_Base;

typedef enum {
    Lex_Encoding_U8,
    Lex_Encoding_U16,
    Lex_Encoding_U32,
    Lex_Encoding_Wide,
} Lexer_String_Encoding;

typedef enum {
    Lex_Suffix_None,
    Lex_Suffix_U,
    Lex_Suffix_L,
    Lex_Suffix_LL,
    Lex_Suffix_UL,
    Lex_Suffix_ULL,
    Lex_Suffix_F,
    Lex_Suffix_LF,
} Lexer_Num_Suffix;

typedef enum {
    Lex_Op_Eq, // =
    Lex_Op_Not, // !
    Lex_Op_Lt, // <
    Lex_Op_Gt, // >
    Lex_Op_Plus, // +
    Lex_Op_Minus, // -
    Lex_Op_Mul, // *
    Lex_Op_Div, // /
    Lex_Op_Xor, // ^
    Lex_Op_And, // &
    Lex_Op_Or, // |
    Lex_Op_Modulo, // %
    Lex_Op_Mask, // ~

    Lex_Op_EqEq, // ==
    Lex_Op_NotEq, // !=
    Lex_Op_LtEq, // <=
    Lex_Op_GtEq, // >=
    Lex_Op_PlusEq, // +=
    Lex_Op_MinusEq, // -=
    Lex_Op_MulEq, // *=
    Lex_Op_DivEq, // /=
    Lex_Op_XorEq, // ^=
    Lex_Op_ShlEq, // <<=
    Lex_Op_ShrEq, // >>=
    Lex_Op_AndEq, // &=
    Lex_Op_OrEq, // |=
    Lex_Op_ModuloEq, // %=

    Lex_Op_PlusPlus, // ++
    Lex_Op_MinusMinus, // --
    Lex_Op_AndAnd, // &&
    Lex_Op_OrOr, // ||
    Lex_Op_Shl, // <<
    Lex_Op_Shr, // >>
} Lexer_Operator;

typedef struct {
    size_t line;
    size_t column;
    const char *filename;
} Lexer_Source_Location;

typedef struct {
    Lexer_Source_Location loc;
    Lexer_Token_Kind kind;
    Slice text;

    union {
        struct {
            Lexer_Num_Base base;
            Lexer_Num_Suffix suffix;
            size_t val;
        } _int;
        struct {
            Lexer_Num_Base base;
            Lexer_Num_Suffix suffix;
            double val;
        } _float;
        struct {
            Lexer_String_Encoding encode;
        } _string;
        struct {
            Lexer_String_Encoding encode;
            unsigned val;
        } _char;
        Lexer_Operator op;
    };
} Lexer_Token;

typedef struct {
    Slice inline_comment;
    Slice comment_begin;
    Slice comment_end;

    bool istty;
    bool print_errors;
} Lexer_Config;

typedef struct {
    const char *begin;
    const char *pos;
    const char *end;

    Lexer_Source_Location loc;
    Lexer_Config settings;
    size_t error_count;
} Lexer;

bool lexer_eof(const Lexer *l) {
    return l->pos >= l->end;
}

unsigned char lexer_peek(const Lexer *l) {
    return lexer_eof(l) ? '\0' : *l->pos;
}

unsigned char lexer_peek2(const Lexer *l) {
    return l->pos + 1 >= l->end ? '\0' : l->pos[1];
}

char lexer_next_char(Lexer *l) {
    if (lexer_eof(l)) return '\0';

    char c = *l->pos++;

    if (c == '\n') {
        l->loc.line++;
        l->loc.column = 0;
    } else {
        l->loc.column++;
    }

    return c;
}

char lexer_jump(Lexer *l, size_t n) {
    char c undefined;
    $range(0, n, i) {
        if (lexer_eof(l)) return '\0';

        c = *l->pos++;

        if (c == '\n') {
            l->loc.line++;
            l->loc.column = 0;
        } else {
            l->loc.column++;
        }
    }
    return c;
}

void lexer_skip_whitespace(Lexer *l) {
    while (!lexer_eof(l) && isspace(lexer_peek(l))) {
        lexer_next_char(l);
    }
}

void lexer_skip_newline(Lexer *l) {
    while (!lexer_eof(l) && '\n' != lexer_peek(l)) {
        lexer_next_char(l);
    }
}

void lex_printf(
    Lexer *l, const Lexer_Source_Location *loc, const char *color, const char *level,
    const char *fmt, ...) $attr_printf(5, 6);
void lex_printf(
    Lexer *l, const Lexer_Source_Location *loc, const char *color, const char *level,
    const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    if (l->settings.istty) {
        fprintf(
            stderr, "%s:%zu:%zu: %s%s:%s ", loc->filename, loc->line, loc->column, color, level,
            COLOR_RESET);
        vfprintf(stderr, fmt, args);
        fputc('\n', stderr);
    } else {
        fprintf(stderr, "%s:%zu:%zu: %s: ", loc->filename, loc->line, loc->column, level);
        vfprintf(stderr, fmt, args);
        fputc('\n', stderr);
    }

    va_end(args);
}

#define lexer_error(l, loc, ...)                                 \
    do {                                                         \
        lex_printf((l), (loc), COLOR_RED, "error", __VA_ARGS__); \
        (l)->error_count += 1;                                   \
    } while (0)

char lexer_expect(Lexer *l, char expected) {
    Lexer_Source_Location loc = l->loc;
    char got = lexer_peek(l);
    if (expected == got) {
        return lexer_next_char(l);
    }
    lexer_error(l, &loc, "expected '%c', but got '%c'", expected, got);
    return '\0';
}

Lexer_Token lexer_unknown(Lexer *l);
Lexer_Token lexer_lit(Lexer *l, const char *lit, Lexer_Token_Kind kind) {
    $assert(strlen(lit) == 1, "literals passed to lexer_lit() should be one byte");
    Lexer_Source_Location loc = l->loc;

    char err = lexer_expect(l, *lit);
    if (err == '\0') return lexer_unknown(l);

    return (Lexer_Token){
		.loc = loc,
        .kind = kind,
        .text = {
            .data = lit,
            .len = strlen(lit),
        },
    };
}

unsigned hex_val_of_char(char c);
Lexer_Token lexer_char(Lexer *l, Lexer_String_Encoding encode) {
    Lexer_Source_Location loc = l->loc;

    if ('\'' == *l->pos)
        lexer_next_char(l);
    else
        $assert(('\'' == *(l->pos - 1)), "expected position to be at start of character");

    const char *begin = l->pos;

    unsigned value = 0;
    unsigned units = 0;

    while (!lexer_eof(l)) {
        char c = lexer_next_char(l);

        if (c == '\\') {
            if (lexer_eof(l)) $panic("unterminated escape");
            char esc = lexer_next_char(l);

            switch (esc) {
            case 'n':
                value = (value << 8) | '\n';
                break;
            case 't':
                value = (value << 8) | '\t';
                break;
            case 'r':
                value = (value << 8) | '\r';
                break;
            case '\\':
                value = (value << 8) | '\\';
                break;
            case '\'':
                value = (value << 8) | '\'';
                break;
            case '\"':
                value = (value << 8) | '\"';
                break;
            case '0':
                value = (value << 8) | '\0';
                break;
            case 'a':
                value = (value << 8) | '\a';
                break;
            case 'b':
                value = (value << 8) | '\b';
                break;
            case 'f':
                value = (value << 8) | '\f';
                break;
            case 'v':
                value = (value << 8) | '\v';
                break;
            case 'x': {
                if (lexer_eof(l)) $panic("invalid hex escape");
                unsigned hex = 0;
                while (!lexer_eof(l) && isxdigit(lexer_peek(l)))
                    hex = hex * 16 + hex_val_of_char(lexer_next_char(l));
                value = (value << 8) | hex;
                break;
            }
            default:
                value = (value << 8) | esc;
                break;
            }
        } else if (c == '\'') {
            Slice s = { .data = begin, .len = (size_t)(l->pos - begin - 1) };
            return (Lexer_Token){
                .loc = loc,
                .kind = Lex_Tok_Char,
                .text = s,
                ._char = {
                    .encode = encode,
                    .val = value,
                },
            };
        } else {
            value = (value << 8) | (unsigned char)c;
        }

        if (++units > 4) {
            $panic("character literal exceeds encoding capacity");
            break;
        }
    }

    $panic("unterminated char literal");
}

unsigned hex_val_of_char(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

Lexer_Token lexer_string(Lexer *l, Lexer_String_Encoding encode) {
    Lexer_Source_Location loc = l->loc;
    lexer_expect(l, '"');

    const char *begin = l->pos;

    while (!lexer_eof(l)) {
        char c = lexer_next_char(l);

        if (c == '\\') {
            if (!lexer_eof(l)) lexer_next_char(l);
        } else if (c == '"') {
            return (Lexer_Token){
			.loc = loc,
            .kind = Lex_Tok_String,
            .text = {
                .data = begin,
                .len = (size_t)((l->pos - 1) - begin),
            },
			._string = {
				.encode = encode,
			},
        };
        }
    }

    $panic("unterminated string");
}

bool lexer_is_identifier(char c) {
    return (c == '_' || isalpha((unsigned char)c));
}

bool lexer_is_identifier2(char c) {
    return (c == '_' || isalnum((unsigned char)c));
}

Lexer_Token lexer_unknown(Lexer *l) {
    Lexer_Source_Location loc = l->loc;
    lexer_next_char(l);
    return (Lexer_Token){
        .loc = loc,
        .kind = Lex_Tok_Unknown,
        .text = { .data = l->pos - 1, .len = 1 },
    };
}

Lexer_Token lexer_operator(Lexer *l) {
    Lexer_Source_Location loc = l->loc;
    const char *begin = l->pos;

    char first = lexer_peek(l);
    char second = lexer_peek2(l);

    Lexer_Operator op undefined;
    Lexer_Token_Kind kind = Lex_Tok_Operator;

    switch (first) {
    case '=':
        lexer_next_char(l);
        if (second == '=') {
            lexer_next_char(l);
            op = Lex_Op_EqEq;
        } else {
            op = Lex_Op_Eq;
        }
        break;

    case '!':
        lexer_next_char(l);
        if (second == '=') {
            lexer_next_char(l);
            op = Lex_Op_NotEq;
        } else {
            op = Lex_Op_Not;
        }
        break;

    case '<':
        lexer_next_char(l);
        if (second == '=') {
            lexer_next_char(l);
            op = Lex_Op_LtEq;
        } else if (second == '<') {
            lexer_next_char(l);
            char third = lexer_peek(l);
            if (third == '=') {
                lexer_next_char(l);
                op = Lex_Op_ShlEq;
            } else {
                op = Lex_Op_Shl;
            }
        } else {
            op = Lex_Op_Lt;
        }
        break;

    case '>':
        lexer_next_char(l);
        if (second == '=') {
            lexer_next_char(l);
            op = Lex_Op_GtEq;
        } else if (second == '>') {
            lexer_next_char(l);
            char third = lexer_peek(l);
            if (third == '=') {
                lexer_next_char(l);
                op = Lex_Op_ShrEq;
            } else {
                op = Lex_Op_Shr;
            }
        } else {
            op = Lex_Op_Gt;
        }
        break;

    case '+':
        lexer_next_char(l);
        if (second == '=') {
            lexer_next_char(l);
            op = Lex_Op_PlusEq;
        } else if (second == '+') {
            lexer_next_char(l);
            op = Lex_Op_PlusPlus;
        } else {
            op = Lex_Op_Plus;
        }
        break;

    case '-':
        lexer_next_char(l);
        if (second == '=') {
            lexer_next_char(l);
            op = Lex_Op_MinusEq;
        } else if (second == '-') {
            lexer_next_char(l);
            op = Lex_Op_MinusMinus;
        } else if (second == '>') {
            lexer_next_char(l);
            kind = Lex_Tok_RArrow;
            op = 0;
        } else {
            op = Lex_Op_Minus;
        }
        break;

    case '*':
        lexer_next_char(l);
        if (second == '=') {
            lexer_next_char(l);
            op = Lex_Op_MulEq;
        } else {
            op = Lex_Op_Mul;
        }
        break;

    case '/':
        lexer_next_char(l);
        if (second == '=') {
            lexer_next_char(l);
            op = Lex_Op_DivEq;
        } else {
            op = Lex_Op_Div;
        }
        break;

    case '^':
        lexer_next_char(l);
        if (second == '=') {
            lexer_next_char(l);
            op = Lex_Op_XorEq;
        } else {
            op = Lex_Op_Xor;
        }
        break;

    case '&':
        lexer_next_char(l);
        if (second == '&') {
            lexer_next_char(l);
            op = Lex_Op_AndAnd;
        } else if (second == '=') {
            lexer_next_char(l);
            op = Lex_Op_AndEq;
        } else {
            op = Lex_Op_And;
        }
        break;

    case '|':
        lexer_next_char(l);
        if (second == '|') {
            lexer_next_char(l);
            op = Lex_Op_OrOr;
        } else if (second == '=') {
            lexer_next_char(l);
            op = Lex_Op_OrEq;
        } else {
            op = Lex_Op_Or;
        }
        break;

    case '%':
        lexer_next_char(l);
        if (second == '=') {
            lexer_next_char(l);
            op = Lex_Op_ModuloEq;
        } else {
            op = Lex_Op_Modulo;
        }
        break;

    case '~':
        lexer_next_char(l);
        op = Lex_Op_Mask;
        break;
    default:
        return lexer_unknown(l);
    }

    return (Lexer_Token){
		.loc = loc,
        .kind = kind,
        .text = {
            .data = begin,
            .len = (size_t)(l->pos - begin),
        },
        .op = op,
    };
}

bool lexer_is_comment(Lexer *l, Slice comment) {
    if (comment.len > 4) return false;
    Slice c = { .data = l->pos, .len = comment.len };
    return slice_eq(c, comment);
}

void lexer_comment(Lexer *l) {
    Slice begin = l->settings.comment_begin;
    Slice end = l->settings.comment_end;

    size_t comments = 0;
    while (!lexer_eof(l)) {
        char c = lexer_peek(l);
        if (c == '\'') {
            lexer_char(l, Lex_Encoding_U8);
            continue;
        } else if (c == '"') {
            lexer_string(l, Lex_Encoding_U8);
            continue;
        } else if (c == end.data[0]) {
            if (lexer_is_comment(l, end)) {
                lexer_jump(l, end.len);
                comments -= 1;
                if (comments == 0) {
                    return;
                }
            }
        } else if (c == begin.data[0]) {
            if (lexer_is_comment(l, begin)) {
                lexer_jump(l, begin.len);
                comments += 1;
            }
        } else {
            lexer_next_char(l);
        }
    }
}

static const uint8_t encodings[128] = {
    ['L'] = Lex_Encoding_Wide,
    ['u'] = Lex_Encoding_U16,
    ['U'] = Lex_Encoding_U32,
};

Lexer_Token lexer_identifier(Lexer *l) {
    Lexer_Source_Location loc = l->loc;
    const char *begin = l->pos;

    {
        unsigned char quote undefined;
        uint8_t encoding undefined;
        encoding = encodings[lexer_peek(l)];
        if (encoding == 0) goto identifier;

        quote = lexer_peek2(l);
        if (quote == '\'') {
            lexer_next_char(l);
            return lexer_char(l, encoding);
        } else if (quote == '"') {
            lexer_next_char(l);
            return lexer_string(l, encoding);
        }
    }

identifier:

    while (!lexer_eof(l)) {
        char c = lexer_peek(l);
        if (lexer_is_identifier2(c)) {
            lexer_next_char(l);
            continue;
        }
        break;
    }
    return (Lexer_Token){
		.loc = loc,
		.kind = Lex_Tok_Identifier,
		.text = {
			.data = begin,
			.len = (size_t)((l->pos) - begin),
		},
	};
}

void lexer_skip_digits(Lexer *l, int base) {
    while (!lexer_eof(l)) {
        unsigned char c = lexer_peek(l);

        switch (base) {
        case 10:
            if (!isdigit(c)) return;
            break;

        case 16:
            if (!isxdigit(c)) return;
            break;

        case 8:
            if (c < '0' || c > '7') return;
            break;

        default:
            return;
        }

        lexer_next_char(l);
    }
}

// Pretty much cloned from my s-expr repo
Lexer_Token lexer_number(Lexer *l) {
    Lexer_Source_Location loc = l->loc;
    const char *begin = l->pos;

    bool is_float = false;

    Lexer_Num_Base nbase = Lex_Num_Decimal;
    Lexer_Num_Suffix nsuffix = Lex_Suffix_None;

    int base = 10;

    // Hex, octal, and binary
    if (lexer_peek(l) == '0') {
        lexer_next_char(l);

        if (lexer_peek(l) == 'x' || lexer_peek(l) == 'X') {
            lexer_next_char(l);
            if (!isxdigit(lexer_peek(l))) {
                return lexer_unknown(l);
            }
            base = 16;
            nbase = Lex_Num_Hex;
            lexer_next_char(l);
        } else if (lexer_peek(l) == 'o') {
            lexer_next_char(l);
            if (!isdigit(lexer_peek(l))) {
                return lexer_unknown(l);
            }
            base = 8;
            nbase = Lex_Num_Octal;
            lexer_next_char(l);
        } else if (lexer_peek(l) == 'b' || lexer_peek(l) == 'B') {
            lexer_next_char(l);
            if (lexer_peek(l) != '0' && lexer_peek(l) != '1') {
                return lexer_unknown(l);
            }
            base = 2;
            nbase = Lex_Num_Binary;
            lexer_next_char(l);
        }
    }

    lexer_skip_digits(l, base);

    // Float
    if (base == 10 && lexer_peek(l) == '.' && isdigit(lexer_peek2(l))) {
        is_float = true;
        lexer_next_char(l);
        lexer_skip_digits(l, 10);
    }

    // Scientific notation
    if (base == 10 && (lexer_peek(l) == 'e' || lexer_peek(l) == 'E')) {
        is_float = true;

        const char *save = l->pos;
        Lexer_Source_Location save_loc = l->loc;

        lexer_next_char(l); // e

        {
            char c = lexer_peek(l);
            if (c == '+' || c == '-') lexer_next_char(l);
        }

        if (!isdigit(lexer_peek(l))) {
            l->pos = save;
            l->loc = save_loc;
            // TODO: register error
            goto done;
        }

        lexer_skip_digits(l, 10);
    }

    // strtol, strtod don't care about suffixes
    const char *end = l->pos;

    // Suffixes
    while (!lexer_eof(l)) {
        switch (lexer_peek(l)) {
        case 'u':
        case 'U':
            lexer_next_char(l);
            switch (nsuffix) {
            case Lex_Suffix_None:
                nsuffix = Lex_Suffix_U;
                break;
            case Lex_Suffix_L:
                nsuffix = Lex_Suffix_UL;
                break;
            case Lex_Suffix_LL:
                nsuffix = Lex_Suffix_ULL;
                break;
            default:
                break;
            }
            break;

        case 'l':
        case 'L':
            lexer_next_char(l);
            switch (nsuffix) {
            case Lex_Suffix_None:
                nsuffix = Lex_Suffix_L;
                break;
            case Lex_Suffix_U:
                nsuffix = Lex_Suffix_UL;
                break;
            case Lex_Suffix_UL:
                nsuffix = Lex_Suffix_ULL;
                break;
            case Lex_Suffix_L:
                nsuffix = Lex_Suffix_LL;
                break;
            case Lex_Suffix_F:
                nsuffix = Lex_Suffix_LF;
                break;
            default:
                break;
            }
        case 'f':
        case 'F':
            is_float = true;
            if (nsuffix == Lex_Suffix_L)
                nsuffix = Lex_Suffix_LF;
            else
                nsuffix = Lex_Suffix_F;
            lexer_next_char(l);
            break;
        default:
            goto done;
        }
    }

done:
    NULL;

    Slice val_s = { .data = begin, .len = (size_t)(end - begin) };
    if (is_float) {
        return (Lexer_Token){
            .loc = loc,
            .kind = Lex_Tok_Float,
            .text = val_s,
            ._float = { .base = nbase, .suffix = nsuffix, .val = slice_to_double(val_s, 0) },
        };
    } else {
        return (Lexer_Token){
            .loc = loc,
            .kind = Lex_Tok_Int,
            .text = val_s,
            ._int = { .base = nbase, .suffix = nsuffix, .val = slice_to_long(val_s, 0) },
        };
    }
}

bool lexer_next(Lexer *l, Lexer_Token *out) {
    lexer_skip_whitespace(l);

    if (lexer_eof(l)) {
        out->kind = Lex_Tok_EOF;
        out->text = (Slice){ 0 };
        return false;
    }

    char peek = lexer_peek(l);
    switch (peek) {
    case '(':
        *out = lexer_lit(l, "(", Lex_Tok_LParen);
        return true;
    case ')':
        *out = lexer_lit(l, ")", Lex_Tok_RParen);
        return true;
    case '[':
        *out = lexer_lit(l, "[", Lex_Tok_LBracket);
        return true;
    case ']':
        *out = lexer_lit(l, "]", Lex_Tok_RBracket);
        return true;
    case '{':
        *out = lexer_lit(l, "{", Lex_Tok_LBrace);
        return true;
    case '}':
        *out = lexer_lit(l, "}", Lex_Tok_RBrace);
        return true;
    case ',':
        *out = lexer_lit(l, ",", Lex_Tok_Comma);
        return true;
    case ';':
        *out = lexer_lit(l, ";", Lex_Tok_Semicolon);
        return true;
    case ':':
        *out = lexer_lit(l, ":", Lex_Tok_Colon);
        return true;
    case '?':
        *out = lexer_lit(l, ":", Lex_Tok_QuestionMark);
        return true;
    case '.': {
        Lexer_Source_Location loc = l->loc;
        if (lexer_peek2(l) == '.') {
            lexer_next_char(l);
            if (lexer_peek2(l) == '.') {
                lexer_next_char(l);
                *out = (Lexer_Token){ .loc = loc, .text = "...", .kind = Lex_Tok_Ellipsis };
            } else {
                lexer_next_char(l);
                *out = (Lexer_Token){ .loc = loc, .text = "..", .kind = Lex_Tok_DoubleDot };
            }
            return true;
        }
        *out = lexer_lit(l, ".", Lex_Tok_Dot);
        return true;
    }
    case '\'':
        *out = lexer_lit(l, "'", Lex_Tok_Apostrophe);
        return true;
    case '"':
        *out = lexer_string(l, Lex_Encoding_U8);
        return true;
    default:
        if (lexer_is_identifier(peek)) {
            *out = lexer_identifier(l);
            return true;
        }
        if (isdigit(peek)) {
            *out = lexer_number(l);
            return true;
        }
        if (lexer_is_comment(l, l->settings.inline_comment)) {
            lexer_skip_newline(l);
            return lexer_next(l, out);
        }
        if (lexer_is_comment(l, l->settings.comment_begin)) {
            lexer_comment(l);
            return lexer_next(l, out);
        }
        *out = lexer_operator(l);
        return true;
    }
    return false;
}

bool lexer_next_peak(Lexer *l, Lexer_Token *out, Lexer_Config *cfg) {
    Lexer_Source_Location loc = l->loc;
    const char *save = l->pos;
    bool ret = lexer_next(l, out);
    l->pos = save;
    l->loc = loc;
    return ret;
}

Lexer lexer_init(const char *filename, const char *contents, size_t length, Lexer_Config settings) {
    Lexer_Source_Location loc = { .filename = filename, .line = 0, .column = 0 };
    return (Lexer){ .loc = loc,
                    .begin = contents,
                    .end = contents + length,
                    .pos = contents,
                    .settings = settings };
}

const char *lexer_kind_to_string(Lexer_Token_Kind k) {
    switch (k) {
    case Lex_Tok_EOF:
        return "EOF";
    case Lex_Tok_Identifier:
        return "Identifier";
    case Lex_Tok_Int:
        return "Int";
    case Lex_Tok_Float:
        return "Float";
    case Lex_Tok_String:
        return "String";
    case Lex_Tok_Char:
        return "Char";
    case Lex_Tok_LParen:
        return "LParen";
    case Lex_Tok_RParen:
        return "RParen";
    case Lex_Tok_LBracket:
        return "LBracket";
    case Lex_Tok_RBracket:
        return "RBracket";
    case Lex_Tok_LBrace:
        return "LBrace";
    case Lex_Tok_RBrace:
        return "RBrace";
    case Lex_Tok_Comma:
        return "Comma";
    case Lex_Tok_Semicolon:
        return "Semicolon";
    case Lex_Tok_Colon:
        return "Colon";
    case Lex_Tok_QuestionMark:
        return "QuestionMark";
    case Lex_Tok_Apostrophe:
        return "Apostrophe";
    case Lex_Tok_Dot:
        return "Dot";
    case Lex_Tok_DoubleDot:
        return "DoubleDot";
    case Lex_Tok_Ellipsis:
        return "Ellipsis";
    case Lex_Tok_RArrow:
        return "RArrow";
    case Lex_Tok_Operator:
        return "Operator";
    case Lex_Tok_Unknown:
        return "Unknown";
    }
}

// Example:
// void pretty_print_token(Lexer_Token token) {
// 	const char *kind_text = lexer_kind_to_string(token.kind);
// 	size_t kind_len = strlen(kind_text);
// 	size_t padding = 12 - kind_len; // 12 is QuestionMark length (the largest one)
// 	printf("%s", kind_text);
// 	fputw(stdout, padding);
// 	printf(": %.*s", $view_slice(token.text));
// 	switch (token.kind) {
// 	case Lex_Tok_Int:
// 		printf(" (%zu)", token._int.val);
// 		break;
// 	case Lex_Tok_Float:
// 		printf(" (%lf)", token._float.val);
// 		break;
// 	case Lex_Tok_String:
// 		printf(" (%d)", token._string.encode);
// 		break;
// 	case Lex_Tok_Char:
// 		printf(" (%d)", token._char.val);
// 		break;
// 	default:
// 		break;
// 	}
// 	fputc('\n', stdout);
// }
//
// int main(int argc, char *argv[]) {
// 	// handwritten like if it was file contents
// 	const char *text = "let urmon = {20.25}\n"
// 					   "int main(void) {\n"
// 					   "    let w = 235\n"
// 					   "    let x = 235u\n"
// 					   "    let y = 25.500f\n"
// 					   "    let z = 200UL\n"
// 					   "    let a = 0xFF\n"
// 					   "    // let b = \"hello // pseudo comment world\"\n"
// 					   "    // let b = \"hello /* pseudo comment */ world\"\n"
// 					   "    let b = \"hello \\\"world\\\"\"\n"
// 					   "    /*\n"
// 					   "      /*\n"
// 					   "        let b = \"hello /* pseudo comment */ world\"\n"
// 					   "      */\n"
// 					   "    */\n"
// 					   "    let c = 25.500\n"
// 					   "    let d = 25.50e+2\n"
// 					   "    let e = { 1, 2, 3, 4 };\n"
// 					   "    let f = e[3]\n"
// 					   "    let g = L\"日本語\"\n"
// 					   "    let g = L'日'\n"
// 					   "    let h = f->g\n"
// 					   "    i = 'c' // this becomes apostrophe and the user can decide\n"
// 					   "            // what to do with it\n"
// 					   "    i = L'c' // this one won't become apostrophe\n"
// 					   "    ###\n"
// 					   "}\n";
// 	Lexer_Config settings = { .inline_comment = $slice("//"),
// 							  .comment_begin = $slice("/*"),
// 							  .comment_end = $slice("*/"),
// 							  .print_errors = true,
// 							  .istty = false };
// 	Lexer l = lexer_init("none", text, strlen(text), settings);
// 	Lexer_Token tok undefined;
//
// 	while (lexer_next(&l, &tok)) {
// 		if (tok.kind == Lex_Tok_Apostrophe) {
// 			// Treat all apostrophes as begin os character
// 			// The user may or may not do this, so they can use
// 			// the apostrophe for something else, depending on context
// 			tok = lexer_char(&l, Lex_Encoding_U8);
// 		}
// 		pretty_print_token(tok);
// 	}
// 	return 0;
// }

#endif // LEXER_H_
