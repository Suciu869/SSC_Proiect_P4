#include "common/aligned_buffer.h"

#include <new>
#include <stdexcept>

AlignedBuffer::AlignedBuffer(std::size_t bytes, std::size_t alignment, std::size_t offset)
    : raw_(nullptr), data_(nullptr), alignment_(alignment) {
    if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
        throw std::invalid_argument("alinierea trebuie sa fie putere a lui 2");
    }

    raw_ = static_cast<unsigned char*>(
        ::operator new(bytes + offset, std::align_val_t(alignment)));
    data_ = raw_ + offset;
}

AlignedBuffer::~AlignedBuffer() {
    ::operator delete(raw_, std::align_val_t(alignment_));
}
