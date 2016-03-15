#ifndef _MMAP_H_
#define __MMAP_H_

struct multiboot_info {
       /* Multiboot info version number */
       uint32_t flags;
     
       /* Available memory from BIOS */
       uint32_t mem_lower;
       uint32_t mem_upper;
     
       /* "root" partition */
       uint32_t boot_device;
     
       /* Kernel command line */
       uint32_t cmdline;
     
       /* Boot-Module list */
       uint32_t mods_count;
       uint32_t mods_addr;
     
       union {
         multiboot_aout_symbol_table_t aout_sym;
         multiboot_elf_section_header_table_t elf_sec;
       } u;
     
       /* Memory Mapping buffer */
       uint32_t mmap_length;
       uint32_t mmap_addr;
     
       /* Drive Info buffer */
       uint32_t drives_length;
       uint32_t drives_addr;
     
       /* ROM configuration table */
       uint32_t config_table;
     
       /* Boot Loader Name */
       uint32_t boot_loader_name;
     
       /* APM table */
       uint32_t apm_table;
     
       /* Video */
       uint32_t vbe_control_info;
       uint32_t vbe_mode_info;
       uint16_t vbe_mode;
       uint16_t vbe_interface_seg;
       uint16_t vbe_interface_off;
       uint16_t vbe_interface_len;
};
typedef struct multiboot_info multiboot_info_t;

struct multiboot_mmap_entry {
       multiboot_uint32_t size;
       multiboot_uint64_t addr;
       multiboot_uint64_t len;
     #define MULTIBOOT_MEMORY_KERNEL                 0
     #define MULTIBOOT_MEMORY_AVAILABLE              1
     #define MULTIBOOT_MEMORY_RESERVED               2
       multiboot_uint32_t type;
} __attribute__((packed));
typedef struct multiboot_mmap_entry multiboot_memory_map_t;

/*
struct mmap_entry {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
};
*/

void get_mmap();
void print_mmap();
extern struct mmap_entry* mmap;
extern uint64_t mmap_length;

#endif
