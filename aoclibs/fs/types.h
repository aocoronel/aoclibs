#ifndef AOCLIBS_FS_TYPES_H
#define AOCLIBS_FS_TYPES_H

typedef enum FileType {
        F_NULL,
        F_REG,
        F_DIR,
        F_LNK,
        F_CHR,
        F_BLK,
        F_FIFO,
        F_SOCK
} FileType;

#endif
