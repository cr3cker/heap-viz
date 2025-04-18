#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <raylib.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 400
#define MAX_INPUT_CHARS 2

typedef struct {
    int size;
    int capacity;
    int *data;
} min_heap;

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
    return heap.data[get_left_child_index(index)];
}

int get_right_child_value(min_heap heap, int index) {
    return heap.data[get_right_child_index(index)];
}

int get_parent_value(min_heap heap, int index) {
    return heap.data[get_parent_index(index)];
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
        heap->data = realloc(heap->data, heap->capacity * sizeof(int));
    }
}

void swap(min_heap *heap, int idx1, int idx2) {
    int temp = heap->data[idx1];
    heap->data[idx1] = heap->data[idx2];
    heap->data[idx2] = temp;
}

void heapify_up(min_heap *heap) {
    int index = heap->size - 1;
    while (has_parent(index) && heap->data[get_parent_index(index)] > heap->data[index]) {
        swap(heap, index, get_parent_index(index));
        index = get_parent_index(index);
    }
}

void heapify_down(min_heap *heap) {
    int index = 0;
    while (has_left_child(index, *heap)) {
        int smallest = get_left_child_index(index);
        if (has_right_child(index, *heap) && get_right_child_value(*heap, index) < heap->data[smallest]) {
            smallest = get_right_child_index(index);
        } 
        if (heap->data[smallest] < heap->data[index]) swap(heap, index, smallest);
        else break;
        index = smallest;
    }
}

void add(min_heap *heap, int value) {
    extra_capacity(heap);
    heap->size++;
    heap->data[heap->size - 1] = value;
    heapify_up(heap);
}

int extract_min(min_heap *heap) {
    int min = heap->data[0];
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;
    heapify_down(heap);
    return min;
}

void show_heap(min_heap heap) {
    for (int i = 0; i < heap.size; i++) {
        printf("%d ", heap.data[i]);
    }
    printf("\n");
}

void draw_node(Vector2 pos, int value) {
    DrawCircleV(pos, 20, BLUE);
    char buffer[10];
    sprintf(buffer, "%d", value);
    Vector2 text_size = MeasureTextEx(GetFontDefault(), buffer, 20, 1);
    DrawText(buffer, pos.x - text_size.x / 2, pos.y - text_size.y / 2, 20, BLACK);
}

void draw_recursive(min_heap heap, int index, Vector2 pos, float spacing) {
    if (index >= heap.size) return;
    draw_node(pos, heap.data[index]);
    
    int left_idx = get_left_child_index(index);
    if (left_idx < heap.size) {
        Vector2 left_pos = { pos.x - spacing, pos.y + 60 };
        DrawLineV(pos, left_pos, BLACK);
        draw_recursive(heap, left_idx, left_pos, spacing * 0.5f);
    }

    int right_idx = get_right_child_index(index);
    if (right_idx < heap.size) {
        Vector2 right_pos = { pos.x + spacing, pos.y + 60 };
        DrawLineV(pos, right_pos, BLACK);
        draw_recursive(heap, right_idx, right_pos, spacing * 0.5f);
    }
}

Vector2 calculate_center(Rectangle rec, char *text) {
    int font_size = 40;
    int text_width = MeasureText(text, font_size);
    return (Vector2){ 
        rec.x + (rec.width - text_width) / 2, 
        rec.y + (rec.height - font_size) / 2 
    };
}

int main(int argc, char *argv[]) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Heap Simulator");
    char *num = malloc((MAX_INPUT_CHARS + 1) * sizeof(char));
    num[0] = '\0';
    int digit_cnt = 0;
    bool on_box = false;
    bool on_btn = false;
    Rectangle num_box = { 10.0f, 10.0f, 50, 50 };
    Rectangle add_btn = { 60.0f, 10.0f, 50, 50};

    min_heap heap = { 0, 4, malloc(sizeof(int) * 4) };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();
        on_box = CheckCollisionPointRec(mouse, num_box);
        on_btn = CheckCollisionPointRec(mouse, add_btn);

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

        if (on_btn && digit_cnt > 0) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                add(&heap, atoi(num));
                digit_cnt = 0;
                num[0] = '\0';
            }
            show_heap(heap);
        }

        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRec(num_box, LIGHTGRAY);
        DrawRectangleRec(add_btn, LIGHTGRAY);
        if (on_box) DrawRectangleLines(num_box.x, num_box.y, num_box.width, num_box.height, RED);
        if (on_btn) DrawRectangleLines(add_btn.x, add_btn.y, add_btn.width, add_btn.height, BLACK);
        Vector2 num_pos = calculate_center(num_box, num);
        Vector2 add_pos = calculate_center(add_btn, "A");
        DrawText(num, num_pos.x, num_pos.y, 40, MAROON);
        DrawText("A", add_pos.x, add_pos.y, 40, MAROON);

        Vector2 start_pos = { WINDOW_WIDTH / 2, 80 };
        draw_recursive(heap, 0, start_pos, 150.0f);

        EndDrawing();
    }

    free(heap.data);
    CloseWindow();

    return 0;
}
