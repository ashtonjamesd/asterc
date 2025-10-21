#ifndef object_h
#define object_h

typedef enum {
    OBJ_I32
} ObjectType;

typedef struct {
    ObjectType type;

    union {
        int32_t i32;
    };
} Object;

#endif