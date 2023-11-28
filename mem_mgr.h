#ifndef MEM_MGR_H_
#define MEM_MGR_H_

#include <stddef.h>

void *mem_mgr_malloc(size_t size);
void *mem_mgr_calloc (size_t num, size_t size);
void *mem_mgr_realloc(void *ptr, size_t size);
void mem_mgr_free(void *ptr);
size_t mem_mgr_size_get(void);
void mem_mgr_list_print(void);

#endif /* MEM_MGR_H_ */
