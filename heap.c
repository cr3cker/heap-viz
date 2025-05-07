#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <raylib.h>
#include <string.h>
#include <math.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 600
#define MAX_INPUT_CHARS 2

typedef struct {
    char value[MAX_INPUT_CHARS];
    bool in_use;
    Vector2 pos;
    Vector2 target_pos;
} node;

typedef struct {
    int size;
    int capacity;
    node *data;
} min_heap;

typedef struct {
    char *text;
    Vector2 pos;
} text;

int get_left_child_index(int index) {
    return 2 * index + 1;
}

int get_right_child_index(int index) {
    return 2 * index + 2;
}

int get_parent_index(int index) {
    return (index - 1) / 2;
}

int get_left_child_value(min_heap heap, int index) {
    return atoi(heap.data[get_left_child_index(index)].value);
}

int get_right_child_value(min_heap heap, int index) {
    return atoi(heap.data[get_right_child_index(index)].value);
}

int get_parent_value(min_heap heap, int index) {
    return atoi(heap.data[get_parent_index(index)].value);
}

bool has_parent(int index) {
    return index > 0;
}

bool has_left_child(int index, min_heap heap) {
    return get_left_child_index(index) < heap.size;
}

bool has_right_child(int index, min_heap heap) {
    return get_right_child_index(index) < heap.size;
}

void extra_capacity(min_heap *heap) {
    if (heap->size == heap->capacity) {
        heap->capacity <<= 1;
        heap->data = realloc(heap->data, heap->capacity * sizeof(node));
    }
}

void swap(min_heap *heap, int idx1, int idx2) {
    node temp = heap->data[idx1];
    heap->data[idx1] = heap->data[idx2];
    heap->data[idx2] = temp;
}

void heapify_up(min_heap *heap) {
    int index = heap->size - 1;
    while (has_parent(index) && atoi(heap->data[get_parent_index(index)].value) > atoi(heap->data[index].value)) {
        swap(heap, index, get_parent_index(index));
        index = get_parent_index(index);
    }
}

void heapify_down(min_heap *heap) {
    int index = 0;
    while (has_left_child(index, *heap)) {
        int smallest = get_left_child_index(index);
        if (has_right_child(index, *heap) && get_right_child_value(*heap, index) < atoi(heap->data[smallest].value)) {
            smallest = get_right_child_index(index);
        } 
        if (atoi(heap->data[smallest].value) < atoi(heap->data[index].value)) {
            swap(heap, index, smallest);
        } else break;
        index = smallest;
    }
}

void add(min_heap *heap, char value[]) {
    extra_capacity(heap);
    heap->size++;
    strcpy(heap->data[heap->size - 1].value, value);
    heapify_up(heap);
}

int extract_min(min_heap *heap) {
    int min = atoi(heap->data->value);
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;
    heapify_down(heap);
    return min;
}

void show_heap(min_heap heap) {
    for (int i = 0; i < heap.size; i++) {
        printf("%c ", heap.data[i].value[0]);
    }
    printf("\n");
}

Vector2 calculate_center(Rectangle rec, char *text) {
    int font_size = 40;
    int text_width = MeasureText(text, font_size);
    return (Vector2){ 
        rec.x + (rec.width - text_width) / 2, 
        rec.y + (rec.height - font_size) / 2 
    };
}

void draw_array(int index, int x, Rectangle cell) {
    if (index >= 10) return;
    DrawRectangle(x, cell.y, cell.width, cell.height, LIGHTGRAY);
    DrawRectangleLines(x, cell.y, cell.width, cell.height, BLACK);
    draw_array(index + 1, x + cell.width, cell);
}

void output_nums_in_arr(min_heap heap, int index, int x, Rectangle cell) {
    if (index >= heap.size) return;
    Vector2 center = calculate_center(cell, &heap.data[index].value[0]);
    DrawText(&heap.data[index].value[0], x, center.y, 40, MAROON);
    output_nums_in_arr(heap, index + 1, x + cell.width, cell);
}

void update_node_positions(min_heap *heap, int index, Vector2 pos, float spacing) {
    if (index >= heap->size) return;

    heap->data[index].target_pos = pos;

    int left_idx = get_left_child_index(index);
    if (left_idx < heap->size) {
        Vector2 left_pos = { pos.x - spacing, pos.y + 60 };
        update_node_positions(heap, left_idx, left_pos, spacing * 0.5f);
    }

    int right_idx = get_right_child_index(index);
    if (right_idx < heap->size) {
        Vector2 right_pos = { pos.x + spacing, pos.y + 60 };
        update_node_positions(heap, right_idx, right_pos, spacing * 0.5f);
    }
}

void draw_heap_nodes(min_heap heap) {
    for (int i = 0; i < heap.size; i++) {
        Vector2 pos = heap.data[i].pos;

        if (has_left_child(i, heap)) {
            DrawLineV(pos, heap.data[get_left_child_index(i)].pos, BLACK);
        }

        if (has_right_child(i, heap)) {
            DrawLineV(pos, heap.data[get_right_child_index(i)].pos, BLACK);
        }

        DrawCircleV(pos, 20, BLUE);
        if (heap.data[i].in_use) DrawCircleLinesV(pos, 20, RED);

        int font_width = MeasureText(heap.data[i].value, 30);
        DrawText(heap.data[i].value, pos.x - font_width / 2.0f, pos.y - 15, 30, MAROON);
    }
}

void animate_node_positions(min_heap *heap) {
    const float speed = 5.0f; 
    for (int i = 0; i < heap->size; i++) {
        Vector2 *p = &heap->data[i].pos;
        Vector2 t = heap->data[i].target_pos;

        float dx = t.x - p->x;
        float dy = t.y - p->y;

        if (fabsf(dx) > 0.1f) p->x += dx / speed;
        else p->x = t.x;

        if (fabsf(dy) > 0.1f) p->y += dy / speed;
        else p->y = t.y;
    }
}


void heapify(min_heap *heap, int i) {
    int smallest = i; 
    int l = get_left_child_index(i); 
    int r = get_right_child_index(i); 

    if (l < heap->size && heap->data[l].value - '0' < heap->data[smallest].value - '0')
        smallest = l;

    if (r < heap->size && heap->data[r].value - '0' < heap->data[smallest].value - '0')
        smallest = r;

    if (smallest != i) {
        swap(heap, i, smallest);
        heapify(heap,smallest);
    }
}

void build_heap(min_heap *heap) {
    for (int i = heap->size/2 - 1; i >= 0; i--) {
        heapify(heap, i);
    }
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Heap Simulator");
    int digit_cnt = 0;
    int frames_cnt = 0;
    bool on_box = false;
    bool on_btn_add = false;
    bool on_btn_clear = false;
    Rectangle num_box = { 10.0f, 10.0f, 50, 50 };
    Rectangle add_btn = { 60.0f, 10.0f, 50, 50};
    Rectangle clear_btn = { 110.0f, 10.0f, 50, 50 };
    Rectangle arr_cell = { 200.0f, 10.0f, 50, 50 };
    char num[MAX_INPUT_CHARS] = "\0";

    int size = 0;
    int capacity = 4;
    min_heap heap = { size, capacity, malloc(sizeof(node) * capacity) };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();
        on_box = CheckCollisionPointRec(mouse, num_box);
        on_btn_add = CheckCollisionPointRec(mouse, add_btn);
        on_btn_clear = CheckCollisionPointRec(mouse, clear_btn);

        if (on_box) {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);

            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 48) && (key <= 57)) {
                    if (digit_cnt < MAX_INPUT_CHARS) {
                        num[digit_cnt] = (char)key;
                        digit_cnt++;
                        num[digit_cnt] = '\0';
                    }
                }

                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && digit_cnt > 0) {
                digit_cnt--;
                num[digit_cnt] = '\0';
            }
        } else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        if (on_btn_add && digit_cnt > 0) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                add(&heap, num);
                digit_cnt = 0;
                num[0] = '\0';
            }
            show_heap(heap);
        }

        if (on_btn_clear && heap.size > 0) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                memset(heap.data, 0, sizeof(node) * capacity);
                heap.size = 0;
            }
        }

        frames_cnt++;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRec(num_box, LIGHTGRAY);
        DrawRectangleRec(add_btn, LIGHTGRAY);
        DrawRectangleRec(clear_btn, LIGHTGRAY);
        if (on_box) DrawRectangleLines(num_box.x, num_box.y, num_box.width, num_box.height, RED);
        if (on_btn_add) DrawRectangleLines(add_btn.x, add_btn.y, add_btn.width, add_btn.height, BLACK);
        if (on_btn_clear) DrawRectangleLines(clear_btn.x, clear_btn.y, clear_btn.width, clear_btn.height, BLACK);
        Vector2 add_pos = calculate_center(add_btn, "A");
        Vector2 clear_pos = calculate_center(clear_btn, "C");
        Vector2 num_pos = calculate_center(num_box, num);
        DrawText(num, num_pos.x, num_pos.y, 40, MAROON);
        DrawText("A", add_pos.x, add_pos.y, 40, MAROON);
        DrawText("C", clear_pos.x, clear_pos.y, 40, MAROON);

        Vector2 start_pos = { WINDOW_WIDTH / 2.0f, 150 };
        update_node_positions(&heap, 0, start_pos, 150);
        animate_node_positions(&heap);
        draw_heap_nodes(heap);
        draw_array(0, arr_cell.x, arr_cell);
        output_nums_in_arr(heap, 0, arr_cell.x + 15, arr_cell);

        EndDrawing();
    }
    free(heap.data);
    CloseWindow();

    return 0;
}
