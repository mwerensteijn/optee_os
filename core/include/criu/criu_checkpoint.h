#ifndef __CRIU_CHECKPOINT_H
#define __CRIU_CHECKPOINT_H

#include <stdint.h>
#include <stdbool.h>

typedef uintptr_t vaddr_t;

struct criu_vm_area {
	vaddr_t vm_start;
	vaddr_t vm_end;
	void * original_data;
	unsigned long offset;
	uint32_t protection;
	uint8_t status;
	bool dirty;
};

struct criu_pagemap_entry {
	vaddr_t vaddr_start;
	unsigned long file_page_index;
	unsigned long nr_pages;
	uint8_t flags;
};

struct criu_pagemap_entry_tracker{
	struct criu_pagemap_entry entry;
	bool dirty;
	void * buffer;
	TAILQ_ENTRY(criu_pagemap_entry_tracker) link;
};

TAILQ_HEAD(criu_pagemap_entries, criu_pagemap_entry_tracker);

struct criu_checkpoint_regs {
	uint64_t vregs[64];
	uint64_t regs[31];
	uint64_t entry_addr;
	uint64_t stack_addr;
	uint64_t tpidr_el0_addr;
	uint32_t pstate;
	uint32_t fpsr;
	uint32_t fpcr;
};

struct criu_checkpoint_dirty_pages {
	uint32_t dirty_page_count;
	uint32_t offset;
};

struct criu_checkpoint {
	struct criu_vm_area * vm_areas;
	uint32_t vm_area_count;
	struct criu_pagemap_entries pagemap_entries;
	struct criu_checkpoint_regs regs;
	uint8_t l2_tables_index;
};

enum criu_status_bits {
	VMA_AREA_REGULAR  = 1 << 0,
	VMA_FILE_PRIVATE  = 1 << 1
};

enum criu_pte_flags {
	PE_PRESENT  = 1 << 0,
	PE_LAZY     = 1 << 1
};

enum checkpoint_file_types { 
	CORE_FILE = 0,				// core-*.img
	MM_FILE,				// mm-*.img
	PAGEMAP_FILE,			// pagemap-*.img
	PAGES_BINARY_FILE,		// pages-*.img
	EXECUTABLE_BINARY_FILE	// The binary itself that is checkpointed
};

// Subtract the last enum from the first to determine the number of 
// elements in the enum. By doing this we can use the enum values as indexes
// to the checkpoint_files array. Example checkpoint_files[CORE_FILE].
static const int CHECKPOINT_FILES = EXECUTABLE_BINARY_FILE - CORE_FILE + 1; 

struct checkpoint_file {
	enum checkpoint_file_types file_type;
	uint64_t buffer_index;
	uint64_t file_size;
};
struct checkpoint_file_data {
	struct checkpoint_file file;
	char * filename;
	char * buffer;
};

#endif /*__CRIU_CHECKPOINT_H*/