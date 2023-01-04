#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void PrintBits(void* bytes, uint64_t num_bytes) {
    printf("Bytes: ");
    for(uint64_t i = 0; i < num_bytes; ++i) {
        for(uint64_t j = 0; j < 8; ++j) {
            uint8_t on = ((uint8_t*)bytes)[i] & ((uint8_t)1 << ((uint8_t)7 - j));
            if(on) {
                printf("1");
            }
            else {
                printf("0");
            }

            if(j == 3) {
                printf(".");
            }
        }
        printf(" ");
    }
    printf("\n");
}

typedef struct {
    int* data;
} Page;

typedef struct {
    Page* pages;

    uint8_t page_mask;
    uint8_t page_shift;
} PageTable;

PageTable PageTable_Create() {
    PageTable page_table = {0};
    page_table.page_shift = 4;
    page_table.page_mask = (1 << page_table.page_shift) - 1;

    uint8_t num_pages = (uint8_t)(~page_table.page_mask) >> page_table.page_shift;
    page_table.pages = calloc(num_pages, sizeof(Page));
    page_table.pages = 0;
    if(!page_table.pages) {
        // If calloc fails try to malloc and zero out manually
        page_table.pages = malloc(num_pages * sizeof(Page));
        if(!page_table.pages) {
            printf("Could not calloc %u bytes for page table initialization. Struct is zeroed out...\n", num_pages);
            page_table.page_shift = 0;
            page_table.page_mask = 0;
            return page_table;
        }
        else {
            for(uint32_t i = 0; i < num_pages; ++i) {
                page_table.pages[i].data = 0;
            }
        }
    }

    return page_table;
}

void PageTable_Destroy(PageTable* page_table) {
    uint8_t num_pages = (uint8_t)(~page_table->page_mask) >> page_table->page_shift;
    // uint8_t page_size = page_table->page_mask + 1;
    for(uint8_t i = 0; i < num_pages; ++i) {
        int* page_data_ptr = page_table->pages[i].data;
        if(page_data_ptr) {
            free(page_data_ptr);
        }
    }
    free(page_table->pages);
    page_table->pages = 0;
    page_table->page_mask = 0;
    page_table->page_shift = 0;

    printf("Page table freed and struct zeroed out...\n");
}

uint32_t PageTable_ReadValue(PageTable* page_table, uint8_t address) {
    uint8_t page_index = address >> page_table->page_shift;
    uint8_t page_offset = address & page_table->page_mask;
    Page page = page_table->pages[page_index];
    if(!page.data) {
        printf("Reading uninitialized memory at address %u\n", address);
        return 0;
    }
    else {
        return page.data[page_offset];
    }
}

uint32_t PageTable_WriteValue(PageTable* page_table, uint8_t address, int value) {
    uint8_t page_index = address >> page_table->page_shift;
    uint8_t page_offset = address & page_table->page_mask;
    Page* page = page_table->pages + page_index;
    if(!page->data) {
        uint8_t page_size = page_table->page_mask + 1;
        page->data = malloc(page_size * sizeof(int));
        if(!page->data) {
            printf("Could not malloc %u bytes for page at index %u...\n", page_size, page_index);
            return 0;
        }
    }

    page->data[page_offset] = value;
    return 1;
}

int main() {
    PageTable page_table = PageTable_Create();

    uint8_t address = 33;
    PageTable_WriteValue(&page_table, address, -103);
    printf("value: %d\n", PageTable_ReadValue(&page_table, address));
    PageTable_Destroy(&page_table);

    return 0;
}