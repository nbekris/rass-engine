// File Name:    MemoryPool.h
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Memory pool allocator for efficient memory management.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

namespace RassEngine {

class MemoryPool {
private:
	struct GenericNode {
		GenericNode* next;
	};

	// Points to first free block
	GenericNode* freeList;
	// Linked list of pages
	GenericNode* pages;
	// Size of each block
	size_t blockSize;
	// Blocks per page
	size_t blockCount;
	// Padding bytes before/after each block
	size_t padding;
	// Counts how many nodes there are in a linked list
	static size_t countNodes(const GenericNode* node);
	// Allocates a new page
	void addPage();

public:
	MemoryPool(size_t blockSize, size_t blockCount, size_t padding = 0);
	~MemoryPool();

	// Auto-methods to remove
	MemoryPool(const MemoryPool&) = delete;
	MemoryPool& operator=(const MemoryPool&) = delete;

	void* allocate();
	void deallocate(void* ptr);
	// Checks whether the padding has been overwritten
	bool checkPadding() const;
	// returns number of pages in use
	inline size_t pageCount() const { return countNodes(pages); }
	// returns number of blocks in the freelist
	inline size_t freeBlockCount() const { return countNodes(freeList); }
	// checks if ptr is a valid block pointer
	bool isBlockPointer(void* ptr) const;
	// returns index of ptr in freelist or -1 if not found
	int freeListIndex(void* ptr) const;
};

}
