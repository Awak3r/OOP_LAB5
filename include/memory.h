#pragma once
#include <memory_resource>
#include <list>
#include <stdexcept>

class CustomMemoryResource : public std::pmr::memory_resource {
private:
    struct BlockInfo {
        void* ptr;
        size_t size;
        bool is_free;
    };
    std::list<BlockInfo> blocks;

protected:
    void* do_allocate(size_t bytes, size_t alignment) override {
        for (auto& block : blocks) {
            if (block.is_free && block.size >= bytes) {
                block.is_free = false;
                return block.ptr;
            }
        }
        void* ptr = ::operator new(bytes, std::align_val_t(alignment));
        blocks.push_back({ptr, bytes, false});
        return ptr;
    }

    void do_deallocate(void* p, size_t bytes, size_t alignment) override {
        for (auto& block : blocks) {
            if (block.ptr == p && block.size == bytes) {
                block.is_free = true;
                return;
            }
        }
        throw std::logic_error("Error: can't find object to deallocate");
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

public:
    ~CustomMemoryResource() {
        for (const auto& block : blocks) {
            ::operator delete(block.ptr);
        }
        blocks.clear();
    }
};