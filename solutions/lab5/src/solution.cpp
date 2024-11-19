#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <memory_resource>
#include <vector>

class DynamicMemoryResource : public std::pmr::memory_resource {
public:
  void *do_allocate(std::size_t bytes, std::size_t alignment) override {
    void *ptr = std::malloc(bytes);
    if (!ptr) {
      throw std::bad_alloc();
    }
    allocatedBlocks[ptr] = bytes;
    return ptr;
  }

  void do_deallocate(void *p, std::size_t bytes,
                     std::size_t alignment) override {
    auto it = allocatedBlocks.find(p);
    if (it != allocatedBlocks.end()) {
      assert(it->second == bytes);
      allocatedBlocks.erase(it);
      std::free(p);
    }
  }

  bool
  do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
    return this == &other;
  }

  ~DynamicMemoryResource() override {
    for (auto const &[ptr, bytes] : allocatedBlocks) {
      std::free(ptr);
    }
  }

private:
  std::map<void *, std::size_t> allocatedBlocks;
};

template <class T, class allocator_type = std::pmr::polymorphic_allocator<T>>
requires std::is_default_constructible_v<T> &&std::is_same_v<
    allocator_type, std::pmr::polymorphic_allocator<T>> class DynamicArray {
public:
  explicit DynamicArray(
      std::pmr::memory_resource *mr = std::pmr::get_default_resource())
      : alloc_(mr), data_(nullptr), size_(0), capacity_(0) {}

  DynamicArray(const DynamicArray &other)
      : alloc_(other.alloc_), size_(other.size_), capacity_(other.capacity_) {
    data_ = alloc_.allocate(capacity_);
    for (size_t i = 0; i < size_; ++i) {
      alloc_.construct(data_ + i, other.data_[i]);
    }
  }

  DynamicArray(DynamicArray &&other) noexcept
      : alloc_(other.alloc_), data_(other.data_), size_(other.size_),
        capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
  }

  ~DynamicArray() {
    this->clear();
    alloc_.deallocate(data_, capacity_);
  }

  void push_back(const T &value) {
    if (size_ == capacity_) {
      reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    alloc_.construct(data_ + size_, value);
    ++size_;
  }

  void reserve(size_t newCapacity) {
    if (newCapacity > capacity_) {
      T *newData = alloc_.allocate(newCapacity);
      for (size_t i = 0; i < size_; ++i) {
        std::allocator_traits<allocator_type>::construct(alloc_, newData + i,
                                                         std::move(data_[i]));
        std::allocator_traits<allocator_type>::destroy(alloc_, data_ + i);
      }
      alloc_.deallocate(data_, capacity_);
      data_ = newData;
      capacity_ = newCapacity;
    }
  }

  void clear() {
    for (size_t i = 0; i < size_; ++i) {
      std::allocator_traits<allocator_type>::destroy(alloc_, data_ + i);
    }
    size_ = 0;
  }

  size_t size() const { return size_; }

  T &operator[](size_t index) { return data_[index]; }
  const T &operator[](size_t index) const { return data_[index]; }

  class iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

    iterator(T *ptr) : ptr_(ptr) {}

    T &operator*() const { return *ptr_; }
    T *operator->() const { return ptr_; }
    iterator &operator++() {
      ++ptr_;
      return *this;
    }
    iterator operator++(int) {
      iterator tmp = *this;
      ++ptr_;
      return tmp;
    }
    bool operator==(const iterator &other) const { return ptr_ == other.ptr_; }
    bool operator!=(const iterator &other) const { return ptr_ != other.ptr_; }
    T *get_ptr() const { return ptr_; }

  private:
    T *ptr_;
  };

  iterator begin() { return iterator(data_); }
  iterator end() { return iterator(data_ + size_); }

private:
  allocator_type alloc_;
  T *data_;
  size_t size_;
  size_t capacity_;
};
