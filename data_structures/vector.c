#include "vector.h"

#include <stdlib.h>
#include <string.h>

static void *_vector_at(const struct vector *v, const size_t at) {
    return (char *)v->data + (at * v->item_size);
}

struct vector *vector_allocate(const size_t item_size, const vec_it_dealloc_type deallocator) {
    struct vector *new;

    if (item_size == 0) {
        return NULL;
    }

    new = malloc(sizeof(*new));
    if (!new) {
        return NULL;
    }

    if (!vector_init(new, item_size, deallocator)) {
        free(new);
        return NULL;
    }

    return new;
}

int vector_init(struct vector *v, const size_t item_size, const vec_it_dealloc_type deallocator) {
    if (!v || item_size == 0)
        return 0;

    v->count = 0;
    v->capacity = 0;
    v->item_size = item_size;
    v->data = NULL;
    v->deallocator = deallocator;

    if (!vector_realloc(v, VECTOR_INIT_SIZE))
        return 0;

    return 1;
}

void vector_deinit(struct vector *v) {
    size_t i;

    if (!v)
        return;

    if (v->deallocator) {
        for (i = 0; i < vector_count(v); ++i)
            v->deallocator(_vector_at(v, i));
    }

    if (v->data)
        free(v->data);
}

void vector_deallocate(struct vector **v) {
    if (!v || !*v)
        return;

    vector_deinit(*v);

    free(*v);
    *v = NULL;
}

size_t vector_capacity(const struct vector *v) {
    return v ? v->capacity : 0; 
}

size_t vector_count(const struct vector *v) {
    return v ? v->count : 0;
}

int vector_isempty(const struct vector *v)  {
    return vector_count(v) == 0;
}

int vector_realloc(struct vector *v, const size_t capacity) {
    void *data_temp;

    if (!v || capacity < vector_count(v)) {
        return 0;
    }

    data_temp = realloc(v->data, v->item_size * capacity);
    if (!data_temp) {
        return 0;
    }

    v->capacity = capacity;
    v->data = data_temp;

    return 1;
}

void *vector_at(const struct vector *v, const size_t at) {

    if (at > vector_count(v) - 1)
        return NULL;

    return _vector_at(v, at);
}

int vector_push_back(struct vector *v, const void *item) {
    if (!v || !item)
        return 0;

    if (vector_count(v) >= vector_capacity(v)) {
        if (!vector_realloc(v, v->capacity * VECTOR_SIZE_MULT))
            return 0;
    }

    memcpy(_vector_at(v, v->count), item, v->item_size);
    v->count++;

    return 1;
}

void *vector_give_up(struct vector *v) {
    void *data;

    if (vector_isempty(v))
        return NULL;

    data = v->data;

    vector_init(v, v->item_size, v->deallocator);

    return data;
}

void vector_remove(struct vector *v, const size_t at) {
    void *item;

    if (at >= vector_count(v))
        return;

    item = _vector_at(v, at);

    memmove(item, item + v->item_size, (vector_count(v) - at - 1) * v->item_size);
    v->count--;
}
