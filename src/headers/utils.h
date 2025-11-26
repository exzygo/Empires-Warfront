#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include "raylib.h"

static inline Vector2 NormalizeV2(Vector2 v) {
    float len = sqrtf(v.x*v.x + v.y*v.y);
    return (len > 0.0001f) ? (Vector2){v.x/len, v.y/len} : (Vector2){1,0};
}

static inline Vector2 Rotate(Vector2 v, float angle) {
    float cs = cosf(angle);
    float sn = sinf(angle);
    return (Vector2){ v.x*cs - v.y*sn, v.x*sn + v.y*cs };
}

static inline Vector2 RotateOffset(Vector2 v, float ang) {
    float cs = cosf(ang);
    float sn = sinf(ang);
    return (Vector2){
        v.x * cs - v.y * sn,
        v.x * sn + v.y * cs
    };
}

#endif // UTILS_H

