#ifndef HEAP_ALLOC_CAPS_H
#define HEAP_ALLOC_CAPS_H

#define MALLOC_CAP_EXEC				(1<<0)	//Memory must be able to run executable code
#define MALLOC_CAP_32BIT			(1<<1)	//Memory must allow for aligned 32-bit data accesses
#define MALLOC_CAP_8BIT				(1<<2)	//Memory must allow for 8/16/...-bit data accesses
#define MALLOC_CAP_DMA				(1<<3)	//Memory must be able to accessed by DMA
#define MALLOC_CAP_PID2				(1<<4)	//Memory must be mapped to PID2 memory space
#define MALLOC_CAP_PID3				(1<<5)	//Memory must be mapped to PID3 memory space
#define MALLOC_CAP_PID4				(1<<6)	//Memory must be mapped to PID4 memory space
#define MALLOC_CAP_PID5				(1<<7)	//Memory must be mapped to PID5 memory space
#define MALLOC_CAP_PID6				(1<<8)	//Memory must be mapped to PID6 memory space
#define MALLOC_CAP_PID7				(1<<9)	//Memory must be mapped to PID7 memory space
#define MALLOC_CAP_SPISRAM			(1<<10)	//Memory must be in SPI SRAM
#define MALLOC_CAP_INVALID			(1<<31)	//Memory can't be used / list end marker


void heap_alloc_caps_init();
void *pvPortMallocCaps(size_t xWantedSize, uint32_t caps);

#endif