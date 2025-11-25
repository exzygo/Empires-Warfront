#ifndef UNIT_H
#define UNIT_H

#include "raylib.h"

typedef enum {
    L_INFANTRY,               // Line Infantry
    TWLB_ART,                 // 12-lb Cannons Artillery
    HUSSARS                   // Hussars
} unit_t;

// Estrutura de Unidade
typedef struct {
    unit_t type;              // Tipo da unidade (infantaria, artilharia, etc.)
    int health;               // Saúde da unidade
    int attack_power;         // Poder de ataque
    int defense;              // Defesa da unidade
    Vector2 position;         // Posição no mapa
    float speed;              // Velocidade da unidade
    bool selected;            // Se a unidade está selecionada ou não
    Texture2D texture;        // Textura para desenhar a unidade
    Texture2D selec_texture;  // Textura para desenhar ao selecionar a unidade
} s_unit;

s_unit init_unit(unit_t type, int health, int attack_power,
                 int defense, Vector2 position, float speed,
                 Texture2D texture, Texture2D texture_selected);

bool unit_check_selection(s_unit *unit, Vector2 mouse_pos);
void select_units(s_unit units[], int count, Vector2 mouse_pos);
void deselect_all(s_unit units[], int count);
bool unit_is_clicked(s_unit *unit, Vector2 mouse_pos);

void drag_select_units(s_unit units[], int count, Rectangle selection_box);
void start_selection(Vector2 mouse_pos);
void update_selection(Vector2 mouse_pos);
Rectangle get_selection_box();
extern bool is_dragging;




#endif // UNIT_H
