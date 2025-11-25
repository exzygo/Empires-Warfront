#include "headers/unit.h"
#include "raylib.h"
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
        texture_selected
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


