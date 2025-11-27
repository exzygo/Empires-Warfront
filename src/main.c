#include "raylib.h"
#include "raymath.h"
#include "headers/unit.h"

#define DEFAULT_FONT_PATH "assets/fonts/GamePausedDEMO.otf"
#define DEFAULT_FONT_SIZE 20
#define SPRITESHEET_WIDTH   512
#define SPRITESHEET_HEIGHT  512

Font default_font;

void DrawTextDefault(const char *text, int x, int y, int size, Color color) {
    DrawTextEx(
        default_font, text, (Vector2){x, y}, 
        size, 1, color
    );
}

int main(void) {
    InitWindow(800, 600, "Empires Warfront");

    // Carregando texturas das unidades
    Texture2D infantry_texture = LoadTexture("assets/textures/infantry_t01-FR.png");
    Texture2D infantry_texture_selec = LoadTexture("assets/textures/infantry-selec_t01-FR.png");

    Texture2D cannons_texture = LoadTexture("assets/textures/twlb_t01-FR.png");
    Texture2D cannons_texture_selec = LoadTexture("assets/textures/twlb-selec_t01-FR.png");

    Texture2D hussars_texture = LoadTexture("assets/textures/cav_t01-FR.png");
    Texture2D hussars_texture_selec = LoadTexture("assets/textures/cav-selec_t01-FR.png");

    // Carregando spritesheet
    Texture2D ui_spritsheet = LoadTexture("assets/ui/fantasy-ui-spritesheet.png");
    Rectangle uiss_source_rec = { 810, 700, 110, 80 };
    Vector2 uiss_position = { 690, 510 };

    // Carregando fonte
    default_font = LoadFont(DEFAULT_FONT_PATH);
    SetTextureFilter(default_font.texture, TEXTURE_FILTER_BILINEAR);

    formation_t current_formation = FORM_NONE;

    s_unit units[3];

    units[0] = init_unit(
        L_INFANTRY, 100, 10, 5, 
        (Vector2){100, 100}, 3.5f, 
        infantry_texture, infantry_texture_selec
    );

    units[1] = init_unit(
        TWLB_ART, 80, 30, 3, 
        (Vector2){200, 200}, 2.0f, 
        cannons_texture, cannons_texture_selec
    );

    units[2] = init_unit(
        HUSSARS, 125, 25, 5, 
        (Vector2){300, 300}, 5.5f, 
        hussars_texture, hussars_texture_selec
    );

    int unit_count = 3;

    SetTargetFPS(60);

    // Elementos da UI
    Rectangle ui_bar = {0, 530, 800, 80};

    Rectangle btn_formations = {20, 550, 120, 30};
    Rectangle btn_line      = {160, 550, 80, 30};
    Rectangle btn_column    = {250, 550, 80, 30};
    Rectangle btn_wedge     = {340, 550, 80, 30};

    bool show_formation_menu = false;

    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        // SELEÇÃO VIA DRAG BOX OU CLIQUE SIMPLES

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

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 click = GetMousePosition();

            // 1) coletar índices das unidades selecionadas
            int sel_indices[512]; // se tiver muitas unidades aumente
            int sel_count = 0;
            for (int i = 0; i < unit_count; i++) {
                if (units[i].selected) {
                    sel_indices[sel_count++] = i;
                }
            }

            if (sel_count == 0) {
                // sem unidades selecionadas -> nada a fazer (ou mover unidade individual)
            } else {
                Vector2 center = (Vector2){0.0f, 0.0f};
                for (int k = 0; k < sel_count; k++) {
                    int idx = sel_indices[k];
                    Vector2 unit_center = (Vector2){
                        units[idx].position.x + units[idx].texture.width * 0.5f,
                        units[idx].position.y + units[idx].texture.height * 0.5f
                    };
                    center = Vector2Add(center, unit_center);
                }
                center = Vector2Scale(center, 1.0f / (float)sel_count);

                Vector2 forward = Vector2Subtract(click, center);
                float forward_len = sqrtf(forward.x*forward.x + forward.y*forward.y);
                if (forward_len < 0.0001f) {
                    forward = (Vector2){1.0f, 0.0f};
                } else {
                    forward = Vector2Scale(forward, 1.0f / forward_len);
                }

                const float spacing_x = 80.0f;      // linha horizontal spacing
                const float spacing_y = 60.0f;      // coluna vertical spacing
                const float wedge_forward = 40.0f;  // profundidade da cunha por "nível"
                const float wedge_side = 50.0f;     // afastamento lateral por "nível"

                float mid = (sel_count - 1) / 2.0f;

                for (int k = 0; k < sel_count; k++) {
                    int idx = sel_indices[k];

                    Vector2 baseOffset = {0.0f, 0.0f};

                    if (current_formation == FORM_LINE) {
                        baseOffset.x = ((float)k - mid) * spacing_x;
                        baseOffset.y = 0.0f;
                    }
                    else if (current_formation == FORM_COLUMN) {
                        baseOffset.x = 0.0f;
                        baseOffset.y = ((float)k - mid) * spacing_y;
                    }
                    else if (current_formation == FORM_WEDGE) {
                        float rel = (float)k - mid;
                        int level = (int)floorf(fabsf(rel) + 0.5f); // aproximação de nível
                        float side_dir = (rel >= 0.0f) ? 1.0f : -1.0f;

                        baseOffset.x = side_dir * (level * wedge_side);
                        baseOffset.y = (level) * wedge_forward; // profundidade cresce com o level
                    }
                    else { // FORM_NONE
                        baseOffset.x = 0.0f;
                        baseOffset.y = 0.0f;
                    }

                    // Sem rotação aplicada
                    Vector2 rotated = baseOffset;

                    Vector2 destination = Vector2Add(click, rotated);

                    units[idx].target = destination;
                    units[idx].is_moving = true;
                    units[idx].formation = current_formation;
                }
            }
        }

        // Interação com o menu
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

            Vector2 m = GetMousePosition();

            if (CheckCollisionPointRec(m, btn_formations)) {
                show_formation_menu = !show_formation_menu;
            }

            if (show_formation_menu) {
                if (CheckCollisionPointRec(m, btn_line)) {
                    current_formation = FORM_LINE;
                }

                if (CheckCollisionPointRec(m, btn_column)) {
                    current_formation = FORM_COLUMN;
                }

                if (CheckCollisionPointRec(m, btn_wedge)) {
                    current_formation = FORM_WEDGE;
                }
            }
        }

        for (int i = 0; i < unit_count; i++) {
            move_unit_target(&units[i], dt);
        }

        BeginDrawing();
            ClearBackground(GREEN);

            // UI BAR
            DrawRectangleRec(ui_bar, DARKGRAY);
            DrawTextureRec(ui_spritsheet, uiss_source_rec, uiss_position, RAYWHITE);

            // Botão principal
            DrawRectangleRec(btn_formations, RAYWHITE);
            DrawTextDefault("Formations", btn_formations.x + 4, btn_formations.y + 4, DEFAULT_FONT_SIZE, BLACK);

            // Menu secundário
            if (show_formation_menu) {
                Color lineColor   = (current_formation == FORM_LINE)   ? YELLOW : LIGHTGRAY;
                Color columnColor = (current_formation == FORM_COLUMN) ? YELLOW : LIGHTGRAY;
                Color wedgeColor  = (current_formation == FORM_WEDGE)  ? YELLOW : LIGHTGRAY;

                DrawRectangleRec(btn_line, lineColor);
                DrawTextDefault("Line", btn_line.x + 20, btn_line.y + 5, DEFAULT_FONT_SIZE, BLACK);

                DrawRectangleRec(btn_column, columnColor);
                DrawTextDefault("Column", btn_column.x + 5, btn_column.y + 5, DEFAULT_FONT_SIZE, BLACK);

                DrawRectangleRec(btn_wedge, wedgeColor);
                DrawTextDefault("Wedge", btn_wedge.x + 7, btn_wedge.y + 5, DEFAULT_FONT_SIZE, BLACK);
            }


            // DESENHAR O DRAG BOX
            if (is_dragging) {
                Rectangle box = get_selection_box();
                DrawRectangleLinesEx(box, 2, BLUE);
                DrawRectangle(box.x, box.y, box.width, box.height, Fade(BLUE, 0.2f));
            }

            // DESENHAR UNIDADES
            for (int i = 0; i < unit_count; i++) {
                Texture2D tex = units[i].selected ? units[i].selec_texture : units[i].texture;
                DrawTexture(tex, units[i].position.x, units[i].position.y, WHITE);
            }

            // DESENHAR TRAJETÓRIA DAS UNIDADES SELECIONADAS
            for (int i = 0; i < unit_count; i++) {
                if (units[i].selected && units[i].is_moving) {

                    Vector2 pos = units[i].position;

                    // Centro da unidade
                    Vector2 center = (Vector2){
                        pos.x + units[i].texture.width / 2,
                        pos.y + units[i].texture.height / 2
                    };

                    // Direção para o target
                    Vector2 dir = Vector2Normalize(Vector2Subtract(units[i].target, center));

                    // Ponto de início deslocado para frente da unidade
                    Vector2 start = Vector2Add(center, Vector2Scale(dir, 20)); // 20px à frente

                    // Ponto final
                    Vector2 end = units[i].target;

                    // Linha principal
                    DrawLineEx(start, end, 2.0f, YELLOW);

                    // Círculo de destino
                    DrawCircleV(end, 8, YELLOW);
                    DrawCircleV(end, 4, GREEN);

                    Vector2 arrow_dir = Vector2Normalize(Vector2Subtract(end, start));
                    Vector2 perp = (Vector2){-arrow_dir.y, arrow_dir.x};

                    float arrow_size = 12;

                    Vector2 p1 = end;
                    Vector2 p2 = Vector2Add(end, Vector2Scale(arrow_dir, -arrow_size));
                    Vector2 p3 = Vector2Add(p2, Vector2Scale(perp, 5));
                    Vector2 p4 = Vector2Add(p2, Vector2Scale(perp, -5));

                    DrawTriangle(p1, p3, p4, RED);
                }
            }

        EndDrawing();
    }

    UnloadTexture(infantry_texture);
    UnloadTexture(infantry_texture_selec);

    UnloadTexture(cannons_texture);
    UnloadTexture(cannons_texture_selec);

    UnloadTexture(hussars_texture);
    UnloadTexture(hussars_texture_selec);

    UnloadTexture(ui_spritsheet);

    UnloadFont(default_font);

    CloseWindow();

    return 0;
}

