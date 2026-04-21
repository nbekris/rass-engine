// File Name:    MemoryPool.cpp
// Author(s):    main Niko Bekris
// Course:       GAM541
// Project:      RASS
// Purpose:      Memory pool allocator for efficient memory management.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#include "Precompiled.h"
#include "MemoryPool.h"

#include <cstddef>  // for size_t and std::byte

namespace RassEngine {

static constexpr std::byte PADDING_BYTE = std::byte{0xAA};

MemoryPool::MemoryPool(size_t blockSize, size_t blockCount, size_t padding) :
	freeList{nullptr}, pages{nullptr}, blockSize{blockSize}, blockCount{blockCount}, padding{padding} {
	// Setup the first page
	addPage();
}

MemoryPool::~MemoryPool() {
	GenericNode *currentNode = pages;
	void *toDelete = nullptr;
	while(currentNode != nullptr) {
		// Cache the pointer to delete
		toDelete = reinterpret_cast<void *>(currentNode);
		currentNode = currentNode->next;

		// Delete the current node
		free(toDelete);
	}
}

size_t MemoryPool::countNodes(const GenericNode *node) {
	size_t toReturn = 0;
	while(node != nullptr) {
		node = node->next;
		++toReturn;
	}
	return toReturn;
}

void MemoryPool::addPage() {
	void *const newStorage = malloc(
		// Allocate for next page pointer
		sizeof(GenericNode)
		// and for each block...
		+ blockCount * (
			// Allocate for padding,
			2 * padding
			// and finally the block itself
			+ blockSize
			)
	);
	if(newStorage == NULL) {
		throw "Cannot allocate more memory";
	}

	// Use placement new to construct a new page
	GenericNode *const newPage = new (newStorage) GenericNode;

	// Insert the new page at the start of the LinkedList
	newPage->next = pages;
	pages = newPage;

	// Initialize loop variables (minor optimization)
	size_t j = 0;
	GenericNode *newBlock = nullptr;
	GenericNode *lastBlock = nullptr;

	// Go through each storage block
	std::byte *blockStorage = reinterpret_cast<std::byte *>(newStorage) + sizeof(MemoryPool::GenericNode *);
	for(size_t i = 0; i < blockCount; ++i) {
		// First, fill the left padding
		for(j = 0; j < padding; ++j) {
			blockStorage[j] = PADDING_BYTE;
		}
		blockStorage += padding;

		// Construct a block in this storage space
		newBlock = new (blockStorage) GenericNode;
		newBlock->next = nullptr;

		// Update freeList pointer
		if(freeList == nullptr) {
			freeList = newBlock;
		}

		// Update the lastBlock to point to this block
		if(lastBlock != nullptr) {
			lastBlock->next = newBlock;
		}

		// Move the lastBlock to the new one
		lastBlock = newBlock;

		// Move the pointer past the block
		blockStorage += blockSize;

		// Lastly, fill the right padding
		for(j = 0; j < padding; ++j) {
			blockStorage[j] = PADDING_BYTE;
		}
		blockStorage += padding;
	}
}

void *MemoryPool::allocate() {
	// Make sure we have space to allocate a new block
	if(freeList == nullptr) {
		addPage();
	}

	// Pop the first node in the list
	GenericNode *node = freeList;
	freeList = freeList->next;

	// Destruct the popped node, then return it
	node->~GenericNode();
	return reinterpret_cast<void *>(node);
}

void MemoryPool::deallocate(void *ptr) {
	// Use placement new to construct a new free block
	GenericNode *const newBlock = new (ptr) GenericNode;

	// Insert the new block into the freelist as the first item
	newBlock->next = freeList;
	freeList = newBlock;
}

bool MemoryPool::checkPadding() const {
	bool toReturn = true;
	size_t stride = blockSize + 2 * padding;

	// Go through each page
	for(MemoryPool::GenericNode *page = pages; page != nullptr; page = page->next) {
		// Calculate index of the first block on the page
		std::byte *pageStart = reinterpret_cast<std::byte *>(page) + sizeof(MemoryPool::GenericNode *);

		// Go through each block
		for(size_t i = 0; i < blockCount; ++i) {
			// Calculate the first padding section
			std::byte *rangeStart = pageStart + i * stride;

			// Check left padding
			for(size_t j = 0; j < padding; ++j) {
				if(rangeStart[j] != PADDING_BYTE) {
					toReturn = false;
				}
			}

			// Calculate the right padding location
			rangeStart += padding + blockSize;

			// Check right padding
			for(size_t j = 0; j < padding; ++j) {
				if(rangeStart[j] != PADDING_BYTE) {
					toReturn = false;
				}
			}
		}
	}
	return toReturn;
}

bool MemoryPool::isBlockPointer(void *ptr) const {
	// Calculate the range of each page
	size_t startOffset = sizeof(GenericNode *) + padding;
	size_t nextBlockOffset = blockSize + 2 * padding;
	size_t endOffset = sizeof(GenericNode *) + nextBlockOffset * blockCount;

	// Go through each page
	size_t comparePtr = reinterpret_cast<size_t>(ptr);
	for(GenericNode *currentPage = pages; currentPage != nullptr; currentPage = currentPage->next) {
		// First check if pointer is in-range
		size_t currentPageFirstBlock = reinterpret_cast<size_t>(currentPage) + startOffset;
		size_t currentPageLastBound = reinterpret_cast<size_t>(currentPage) + endOffset;
		if((comparePtr < currentPageFirstBlock) || (comparePtr > currentPageLastBound)) {
			// If not, move to the next page
			continue;
		}

		// If so, check pointer index, make sure it's within the first index of a block
		size_t byteOffset = (comparePtr - currentPageFirstBlock) % nextBlockOffset;
		return (byteOffset == 0);
	}

	// Otherwise, return false
	return false;
}

int MemoryPool::freeListIndex(void *ptr) const {
	// Go through all nodes in the free list
	int toReturn = 0;
	for(GenericNode *currentNode = freeList; currentNode != nullptr; currentNode = currentNode->next, ++toReturn) {
		// Check if pointer matches
		if(ptr == currentNode) {
			// Indicate which index this pointer lies in
			return toReturn;
		}
	}
	return -1;
}

}
