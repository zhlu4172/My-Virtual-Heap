#include "virtual_alloc.h"
#include "virtual_sbrk.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <math.h>
#include "cmocka.h"

#define SIZE 30000000
#define INIT_SIZE 15
#define MINI_SIZE 12

void * virtual_heap = NULL;

void * virtual_sbrk(int32_t increment) {
    return sbrk(increment);
}

void _init_heap(int32_t size)
{
    virtual_heap = malloc(size);
}

static void test_virtual_malloc_bad()
{
    _init_heap(SIZE);
    init_allocator(virtual_heap, INIT_SIZE, MINI_SIZE);
    assert_null(virtual_malloc(virtual_heap, 33000));
    free(virtual_heap);
}

static void test_virtual_malloc_bad_2()
{
    _init_heap(SIZE);
    init_allocator(virtual_heap, INIT_SIZE, MINI_SIZE);
    assert_null(virtual_malloc(virtual_heap, 0));
    free(virtual_heap);
}

static void test_virtual_malloc()
{
    _init_heap(SIZE);
    init_allocator(virtual_heap, INIT_SIZE, MINI_SIZE);
    long res = (long)virtual_malloc(virtual_heap, 3000);
    assert_int_equal(res, virtual_heap + 2);
    free(virtual_heap);
}

static void test_virtual_malloc_2()
{
    _init_heap(SIZE);
    init_allocator(virtual_heap, INIT_SIZE, MINI_SIZE);
    virtual_malloc(virtual_heap, 3000);
    int num = pow(2, 12);
    long res = (long)virtual_malloc(virtual_heap, 8000);
    assert_int_equal(res, virtual_heap + 2 + num + num);
    free(virtual_heap);
}

static void test_virtual_realloc()
{
    _init_heap(SIZE);
    init_allocator(virtual_heap, INIT_SIZE, MINI_SIZE);
    void* ptr = virtual_malloc(virtual_heap, 3000);
    long res = (long)virtual_realloc(virtual_heap, ptr, 3600);
    assert_int_equal(res, virtual_heap + 2);
    free(virtual_heap);
}

static void test_virtual_realloc_bad()
{
    _init_heap(SIZE);
    init_allocator(virtual_heap, INIT_SIZE, MINI_SIZE);
    void* ptr = virtual_malloc(virtual_heap, 3000);
    assert_null(virtual_realloc(virtual_heap, ptr, 360000));
    free(virtual_heap);
}

static void test_virtual_free_nice()
{
    _init_heap(SIZE);
    init_allocator(virtual_heap, INIT_SIZE, MINI_SIZE);
    void* ptr = virtual_malloc(virtual_heap, 3000);
    int result = virtual_free(virtual_heap,ptr);
    assert_int_equal(result, 0);
    free(virtual_heap);
}

static void test_virtual_free_nice_or_not()
{
    _init_heap(SIZE);
    init_allocator(virtual_heap, INIT_SIZE, MINI_SIZE);
    void* ptr = (void*)((long)virtual_malloc(virtual_heap, 3000) + 1);
    int result = virtual_free(virtual_heap,ptr);
    assert_int_equal(result, 1);
    free(virtual_heap);
}

static void test_virtual_free_bad()
{
    _init_heap(SIZE);
    init_allocator(virtual_heap, INIT_SIZE, MINI_SIZE);
    int result = virtual_free(virtual_heap,(void*)12341283798);
    assert_int_equal(result, 1);
    free(virtual_heap);
}

static void test_virtual_free_bad_ptr()
{
    _init_heap(SIZE);
    init_allocator(virtual_heap, INIT_SIZE, MINI_SIZE);
    void* ptr = (void*)0x39428309;
    int result = virtual_free(virtual_heap,ptr);
    assert_int_equal(result, 1);
    free(virtual_heap);
}

static void test_virtual_info_init()
{
    _init_heap(SIZE);
    init_allocator(virtual_heap, INIT_SIZE, MINI_SIZE);
    FILE *file = freopen("virtual_info_10.txt", "w", stdout);
    virtual_info(virtual_heap);
    fclose(file);
    file = fopen("virtual_info_10.txt", "r");
    char array_1[10000];
    char array_2[10000];
    fscanf(file, "%s", array_1);
    fclose(file);
    FILE *file_2 = fopen("expected_virtual_info_10.txt", "r");
    fscanf(file_2, "%s", array_2);
    fclose(file_2);
    assert_string_equal(array_1, array_2);
    remove("virtual_info_10.txt");
    free(virtual_heap);
}

static void test_virtual_info()
{
    _init_heap(SIZE);
    init_allocator(virtual_heap, INIT_SIZE, MINI_SIZE);
    virtual_malloc(virtual_heap, 3000);
    FILE *file = freopen("virtual_info_11.txt", "w", stdout);
    virtual_info(virtual_heap);
    fclose(file);
    file = fopen("virtual_info_11.txt", "r");
    char array_1[10000];
    char array_2[10000];
    fscanf(file, "%s", array_1);
    fclose(file);
    FILE *file_2 = fopen("expected_virtual_info_11.txt", "r");
    fscanf(file_2, "%s", array_2);
    fclose(file_2);
    assert_string_equal(array_1, array_2);
    remove("virtual_info_11.txt");
    free(virtual_heap);
}

static void test_virtual_info_2()
{
    _init_heap(SIZE);
    init_allocator(virtual_heap, INIT_SIZE, MINI_SIZE);
    void * ptr = virtual_malloc(virtual_heap, 3000);
    virtual_free(virtual_heap, ptr);
    FILE *file = freopen("virtual_info_12.txt", "w", stdout);
    virtual_info(virtual_heap);
    fclose(file);
    file = fopen("virtual_info_12.txt", "r");
    char array_1[10000];
    char array_2[10000];
    fscanf(file, "%s", array_1);
    fclose(file);
    FILE *file_2 = fopen("expected_virtual_info_12.txt", "r");
    fscanf(file_2, "%s", array_2);
    fclose(file_2);
    assert_string_equal(array_1, array_2);
    remove("virtual_info_12.txt");
    free(virtual_heap);
}

static void test_virtual_info_3()
{
    _init_heap(SIZE);
    init_allocator(virtual_heap, INIT_SIZE, MINI_SIZE);
    void * ptr = virtual_malloc(virtual_heap, 3000);
    virtual_realloc(virtual_heap, ptr, 4000);
    FILE *file = freopen("virtual_info_13.txt", "w", stdout);
    virtual_info(virtual_heap);
    fclose(file);
    file = fopen("virtual_info_13.txt", "r");
    char array_1[10000];
    char array_2[10000];
    fscanf(file, "%s", array_1);
    fclose(file);
    FILE *file_2 = fopen("expected_virtual_info_13.txt", "r");
    fscanf(file_2, "%s", array_2);
    fclose(file_2);
    assert_string_equal(array_1, array_2);
    remove("virtual_info_13.txt");
    free(virtual_heap);
}


int main(void) 
{     

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_virtual_free_nice),
        cmocka_unit_test(test_virtual_free_bad),
        cmocka_unit_test(test_virtual_free_bad_ptr),
        cmocka_unit_test(test_virtual_free_nice_or_not),
        cmocka_unit_test(test_virtual_malloc_bad),
        cmocka_unit_test(test_virtual_malloc_bad_2),
        cmocka_unit_test(test_virtual_malloc),
        cmocka_unit_test(test_virtual_malloc_2),
        cmocka_unit_test(test_virtual_realloc),
        cmocka_unit_test(test_virtual_realloc_bad),
        cmocka_unit_test(test_virtual_info_init),
        cmocka_unit_test(test_virtual_info),
        cmocka_unit_test(test_virtual_info_2),
        cmocka_unit_test(test_virtual_info_3),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}