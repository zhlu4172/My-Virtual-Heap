#include "virtual_alloc.h"
#include "virtual_sbrk.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>



#define DATA_START 2
//status 0 means free, status 1 means allocated
struct node
{
    uint8_t status;
    uint8_t size;
    struct node* next;
};



void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size) {
    int num = pow(2, initial_size);
    virtual_sbrk((heapstart - virtual_sbrk(0)) + sizeof(struct node) + DATA_START + num);
    uint8_t* i = (uint8_t*)(heapstart);
    *i = initial_size;
    uint8_t* j = (uint8_t*)(heapstart + 1);
    *j = min_size;
    struct node *head = (struct node*)(heapstart + num + DATA_START);
    head->status = 0;
    head->size = initial_size;
    head->next = 0;
    //Your code here
}

void * virtual_malloc(void * heapstart, uint32_t size) {
    
    int init_size = *(uint8_t*)(heapstart);
    int mini_size = *(uint8_t*)(heapstart + 1);
    int num = pow(2, init_size);
    struct node *blk = (struct node*)(heapstart + num + DATA_START);
    void* address = heapstart + DATA_START;
    //number is the block size we want
    int number = 0;
    int count = 0;
    for (int i = init_size; i >= 0; i--)
    { 
        number = i;      
        if (pow(2, i - 1) < size)
        {
            break;
        }
        count += 1;             
    }
    //edge case
    if (size > num)
    {
        return NULL;
    }
    //edge case
    if (size == 0)
    {
        return NULL;
    }


    ////////////////////////
    struct node *blk_test = (struct node*)(heapstart + num + DATA_START);
    void* address_test = heapstart + DATA_START;
    int min_in_large = 0;
    while ((struct node*)blk_test->next != NULL || ((struct node*)blk_test->next == NULL && (struct node*)blk_test != NULL))
    {
        if (blk_test->status == 0 && blk_test->size > number)
        {
            min_in_large = blk_test->size;
            break;
        }
        if (blk_test->next != NULL)
        {
            blk_test = blk_test->next;
        }
        else
        {
            break;
        }
    }
    
    blk_test = (struct node*)(heapstart + num + DATA_START);
    while ((struct node*)blk_test->next != NULL || ((struct node*)blk_test->next == NULL && (struct node*)blk_test != NULL))
    {
        if (blk_test->size >= number && blk_test->size <= min_in_large && blk_test->status == 0)
        {
            min_in_large = blk_test->size;           
        }
        int blk_size = pow(2, blk_test->size);
        address_test = address_test + blk_size;
        if (blk_test->next != NULL)
        {
            blk_test = blk_test->next;
        }
        else
        {
            break;
        }
    }
    
    ///////////////////////

    int cnt = 0;
    if ((struct node*)blk == NULL)
    {
        return NULL;
    }
    int cnnt = 0;
    while ((struct node*)blk->next != NULL || ((struct node*)blk->next == NULL && (struct node*)blk != NULL))
    {
        cnnt += 1;
        if (blk->status == 0 && blk->size == min_in_large)
        {
            cnt += 1;
            if (cnt == 1)
            {
                int cmp = fmax(mini_size,blk->size);
                for (int i = 0; i < cmp - number; i++)
                {
                    if (blk->size > mini_size)
                    {
                        blk->size = blk->size - 1;
                        virtual_sbrk(sizeof(struct node));
                        struct node *pre_next = blk->next;
                        blk->next = (struct node*)virtual_sbrk(sizeof(struct node));
                        blk->next->size = blk->size;
                        blk->next->status = 0;
                        blk->next->next = pre_next;
                    }                         
                }
                blk->status = 1;
                return address;                                       
            }
        }
        if (blk->status == 0 && blk->size == number)
        {
            blk->status = 1;
            return address;
        }
        int blk_size = pow(2, blk->size);
        address = address + blk_size;
        if (blk->next != NULL)
        {
            blk = blk->next;
        }
        else
        {
            break;
        }
        
    }

    return NULL;

}

int virtual_free(void * heapstart, void * ptr) {
    int init_size = *(uint8_t*)(heapstart);
    int num = pow(2, init_size);
    struct node *blk = (struct node*)(heapstart + num + DATA_START);
    void* address = heapstart + DATA_START;
    bool match = false;


    //traverse the block
    while ((struct node*)blk->next != NULL || ((struct node*)blk->next == NULL && (struct node*)blk != NULL))
    {
        //if we can find the block whose pointer is equal to ptr, free the block
        if (address == ptr)
        {
            match = true;
            blk->status = 0; 
        }
             
        int blk_size = pow(2, blk->size);
        address = address + blk_size;
        if (blk->next != NULL)
        {
            blk = blk->next;
        }
        else
        {
            break;
        }
        
    }
    if (match == false)
    {
        return 1;
    }
    struct node *blk_2 = (struct node*)(heapstart + num + DATA_START);
    bool again = true;
    while (again)
    {
        again = false;
        blk_2 = (struct node*)(heapstart + num + DATA_START);
        while ((struct node*)blk_2->next != NULL)
        {
            struct node* blk_3;    
            if (blk_2->status == 0 && blk_2->next->status == 0 && blk_2->size == blk_2->next->size)
            {
                again = true;
                blk_3 = blk_2->next;
                blk_2->next = NULL;
                blk_2->next = blk_3->next;
                blk_2->size = blk_2->size + 1;
                break;             
            }
            else
            {
                blk_2 = blk_2->next;
            }
        }
    }
    
    return 0;
}

void * virtual_realloc(void * heapstart, void * ptr, uint32_t size) {
    void* add;
    void* pre_add;
    int init_size = *(uint8_t*)(heapstart);
    int num = pow(2, init_size);
    struct node *blk = (struct node*)(heapstart + num + DATA_START);
    void* address = heapstart + DATA_START;

    //edge case
    if (size > num)
    {
        return NULL;
    }
    if (ptr == NULL)
    {
        virtual_malloc(heapstart, size);
    }
    if (size == 0)
    {
        virtual_free(heapstart, ptr);
    }
    bool has_sth = false;

    while ((struct node*)blk->next != NULL || ((struct node*)blk->next == NULL && (struct node*)blk != NULL))
    {
        if (address == ptr)
        {
            has_sth = true;
            pre_add = ptr;
            virtual_free(heapstart, address);
            int dec_blk_size = pow(2, blk->size);
            int number = 0;
            for (int i = size; i >= 0; i--)
            { 
                number = i;      
                if (pow(2, i - 1) < size)
                {
                    break;
                }
            }
            int number_dec = pow(2, number);
            if (size <= dec_blk_size)
            {
                add = virtual_malloc(heapstart, size);
                memcpy(add, pre_add, number_dec);
                break;
            }
            else
            {
                for (int i = dec_blk_size; i >= 0; i--)
                { 
                    number = i;      
                    if (pow(2, i - 1) < dec_blk_size)
                    {
                        break;
                    }
                }
                add = virtual_malloc(heapstart, size);
                number_dec = pow(2, number);
                memcpy(add, pre_add, number_dec);
                break;
            }
            break;
        }
        int blk_size = pow(2, blk->size);
        address = address + blk_size;
        if (blk->next != NULL)
        {
            blk = blk->next;
        }
        else
        {
            break;
        }
        
    }
    if (has_sth == false)
    {
        return NULL;
    }
    else
    {
        return add;
    }
    // Your code here
    
}

void virtual_info(void * heapstart) {
    int process = *(uint8_t*)(heapstart);
    int result = pow(2, (process));
    struct node *blk = (struct node*)(heapstart + result + DATA_START);
    while (blk->next != NULL)
    {
        int _pow = pow(2, blk->size);
        if (blk->status == 1)
        {
            printf("allocated %i\n", _pow);
        }
        else
        {
            printf("free %i\n", _pow);
        }
        blk = blk->next;
    }
    int _pow = pow(2, blk->size);
    if (blk->status == 1)
    {
        printf("allocated %i\n", _pow);
    }
    else
    {
        printf("free %i\n", _pow);
    }
}