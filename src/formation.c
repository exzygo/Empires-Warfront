#include "raylib.h"
#include "headers/formation.h"
#include "headers/utils.h"

Vector2 formation_offset(formation_alt_t f, int index, float spacing)
{
    switch (f) {
        case FORMATION_LINE:
            return (Vector2){ index * spacing, 0 };

        case FORMATION_COLUMN:
            return (Vector2){ 0, index * spacing };

        default:
        case FORMATION_NONE:
            return (Vector2){ 0, 0 };
    }
}

Vector2 dest_formation(Vector2 clickPos, Vector2 unitPos, formation_alt_t f, int index)
{
    if (f == FORMATION_NONE) return clickPos;

    const float spacing = 45.0f;

    // 1. offset base
    Vector2 base_offset = formation_offset(f, index, spacing);

    // 2. direção do movimento
    Vector2 forward = NormalizeV2((Vector2){clickPos.x - unitPos.x,
                                          clickPos.y - unitPos.y});

    // 3. ângulo da formação baseado no clique
    float angle = atan2f(forward.y, forward.x);

    // 4. rotaciona offset
    Vector2 rotated = Rotate(base_offset, angle);

    // 5. destino final
    return (Vector2){ clickPos.x + rotated.x, clickPos.y + rotated.y };
}

