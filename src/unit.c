#include "headers/unit.h"
#include "raylib.h"
#include "raymath.h"
#include "math.h"

s_unit init_unit(unit_t type, int health, int attack_power,
                 int defense, Vector2 position, float speed,
                 Texture2D texture, Texture2D texture_selected) {

    s_unit unit = {
        type,
        health,
        attack_power,
        defense,
        position,
        speed,
        false,
        texture,
        texture_selected,
        position,
        false,
        FORM_NONE
    };

    return unit;
}

bool unit_check_selection(s_unit *unit, Vector2 mouse_pos) {
    Rectangle hitbox = {
        unit->position.x,
        unit->position.y,
        unit->texture.width,
        unit->texture.height
    };

    unit->selected = CheckCollisionPointRec(mouse_pos, hitbox);

    return unit->selected;
}

bool is_dragging = false;
static Vector2 dragStart = {0};

bool unit_is_clicked(s_unit *unit, Vector2 mouse_pos) {
    Rectangle hitbox = {
        unit->position.x,
        unit->position.y,
        unit->texture.width,
        unit->texture.height
    };

    return CheckCollisionPointRec(mouse_pos, hitbox);
}

void deselect_all(s_unit units[], int count) {
    for (int i = 0; i < count; i++)
        units[i].selected = false;
}

void select_units(s_unit units[], int count, Vector2 mouse_pos) {
    bool clickedSomething = false;

    for (int i = 0; i < count; i++) {
        if (unit_is_clicked(&units[i], mouse_pos)) {

            if (units[i].selected) {
                units[i].selected = false;
            } else {
                deselect_all(units, count);
                units[i].selected = true;
            }

            clickedSomething = true;
            break;
        }
    }

    if (!clickedSomething)
        deselect_all(units, count);
}

void start_selection(Vector2 mouse_pos) {
    dragStart = mouse_pos;
    is_dragging = true;
}

void update_selection(Vector2 mouse_pos) {
    // apenas atualiza o estado; o cálculo real é feito em get_selection_box()
}

Rectangle get_selection_box() {
    if (!is_dragging) return (Rectangle){0};

    Vector2 mouse_now = GetMousePosition();

    Rectangle box = {
        fmin(dragStart.x, mouse_now.x),
        fmin(dragStart.y, mouse_now.y),
        fabs(mouse_now.x - dragStart.x),
        fabs(mouse_now.y - dragStart.y)
    };

    return box;
}

void drag_select_units(s_unit units[], int count, Rectangle selection_box) {
    for (int i = 0; i < count; i++) {

        Rectangle unitRect = {
            units[i].position.x,
            units[i].position.y,
            units[i].texture.width,
            units[i].texture.height
        };

        if (CheckCollisionRecs(unitRect, selection_box))
            units[i].selected = true;
    }
}

void move_unit_target(s_unit *u, float dt) {
    if (!u->is_moving) return;

    float dist = Vector2Distance(u->position, u->target);

    if (dist < 1.0f) {
        u->is_moving = false;
        return;
    }

    Vector2 dir = Vector2Normalize(Vector2Subtract(u->target, u->position));
    u->position = Vector2Add(u->position, Vector2Scale(dir, u->speed * dt));
}

void apply_formation(s_unit units[], int count, formation_t f) {
    int sel_count = 0;
    for (int i = 0; i < count; i++)
        if (units[i].selected)
            sel_count++;

    if (sel_count == 0) return;

    // Pegar o ponto médio entre todas as unidades selecionadas
    Vector2 center = {0};

    for (int i = 0; i < count; i++)
        if (units[i].selected)
            center = Vector2Add(center, units[i].position);

    center = Vector2Scale(center, 1.0f / sel_count);

    int index = 0;

    for (int i = 0; i < count; i++) {
        if (!units[i].selected) continue;

        Vector2 offset = {0};

        switch (f) {

            case FORM_LINE:
                offset = (Vector2){ index * 80, 0 };
                break;

            case FORM_COLUMN:
                offset = (Vector2){ 0, index * 60 };
                break;

            case FORM_WEDGE:
                offset = (Vector2){
                    (index - sel_count / 2) * 50,
                    fabsf(index - sel_count / 2) * 35
                };
                break;

            default:
                break;
        }

        units[i].target = Vector2Add(center, offset);
        units[i].is_moving = true;
        units[i].formation = f;

        index++;
    }
}
