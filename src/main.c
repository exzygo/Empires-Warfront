#include "raylib.h"

#include "headers/unit.h"

int main(void) {
    InitWindow(800, 600, "Empires Warfront");

    // Carregar texturas para os diferentes tipos de unidades
    Texture2D infantry_texture = LoadTexture("assets/textures/infantry_t01-FR.png");
    Texture2D infantry_texture_selec = LoadTexture("assets/textures/infantry-selec_t01-FR.png");

    Texture2D cannons_texture = LoadTexture("assets/textures/twlb_t01-FR.png");
    Texture2D cannons_texture_selec = LoadTexture("assets/textures/twlb-selec_t01-FR.png");

    s_unit units[2];

    units[0] = init_unit(
        L_INFANTRY, 100, 10, 5,
        (Vector2){100, 100}, 2.0f,
        infantry_texture, infantry_texture_selec
    );

    units[1] = init_unit(
        TWLB_ART, 80, 30, 3,
        (Vector2){200, 200}, 1.5f,
        cannons_texture, cannons_texture_selec
    );

    int unit_count = 2;

    SetTargetFPS(60);

while (!WindowShouldClose()) {

    // --- SELEÇÃO VIA DRAG BOX OU CLIQUE SIMPLES ---

    // Começou a clicar → pode ser clique ou início de drag
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        start_selection(GetMousePosition());
    }

    // Segurando o botão → arrastando
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && is_dragging) {
        update_selection(GetMousePosition());
    }

    // Soltou o botão → decidir entre clique ou drag box
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        Rectangle box = get_selection_box();

        // Caixa muito pequena = trata como clique normal
        if (box.width < 4 && box.height < 4) {
            select_units(units, unit_count, GetMousePosition());
        } 
        else {
            // Drag box = seleção múltipla
            deselect_all(units, unit_count);
            drag_select_units(units, unit_count, box);
        }

        is_dragging = false;
    }

    BeginDrawing();
        ClearBackground(GREEN);

        // --- DESENHAR O DRAG BOX ---
        if (is_dragging) {
            Rectangle box = get_selection_box();
            DrawRectangleLinesEx(box, 2, BLUE);
            DrawRectangle(box.x, box.y, box.width, box.height, Fade(BLUE, 0.2f));
        }

        // --- DESENHAR UNIDADES ---
        for (int i = 0; i < unit_count; i++) {
            Texture2D tex = units[i].selected ? units[i].selec_texture : units[i].texture;
            DrawTexture(tex, units[i].position.x, units[i].position.y, WHITE);
        }

    EndDrawing();
}

    UnloadTexture(infantry_texture);
    UnloadTexture(infantry_texture_selec);

    UnloadTexture(cannons_texture);
    UnloadTexture(cannons_texture_selec);

    CloseWindow();

    return 0;
}

