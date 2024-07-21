typedef struct {
  U8* data;
  I64 size;
  I64 capacity;
  I64 unit;
} List;

#define make_list(type) make_list_with_unit(sizeof(type))

static List make_list_with_unit(I64 unit) {
  List list = {};
  list.unit = unit;
  return list;
}

static void* list_get(List* list, I64 index) {
  return &list->data[index * list->unit];
}

static void* list_add(List* list) {
  if (list->capacity == 0) {
    list->data     = malloc(list->unit);
    list->capacity = 1;
  } else if (list->size == list->capacity) {
    list->capacity *= 2;
    list->data      = realloc(list->data, list->capacity * list->unit);
  }
  void* result = &list->data[list->size * list->unit];
  list->size++;
  return result;
}

