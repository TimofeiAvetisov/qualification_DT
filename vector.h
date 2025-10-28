#pragma once
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>

template<typename T, typename Allocator = std::allocator<T>>
class Vector {
public:
    // Member types
    using value_type             = T;
    using allocator_type         = Allocator;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer          = typename std::allocator_traits<Allocator>::const_pointer;
    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    pointer data_;
    size_type size_;
    size_type capacity_;
    Allocator alloc_;

public:
    // ctor and dtor
    Vector() noexcept(noexcept(Allocator()));
    explicit Vector(size_type count, const T& value = T(), const Allocator& alloc = Allocator());
    explicit Vector(const Allocator& alloc);
    Vector(std::initializer_list<T> init, const Allocator& alloc = Allocator());
    Vector(const Vector& other);
    Vector(const Vector& other, const Allocator& alloc);
    Vector(Vector&& other) noexcept;
    Vector(Vector&& other, const Allocator& alloc);
    ~Vector();

    // assign
    Vector& operator=(const Vector& other);
    Vector& operator=(Vector&& other) noexcept;
    Vector& operator=(std::initializer_list<T> ilist);

    void assign(size_type count, const T& value);
    template<class InputIt> void assign(InputIt first, InputIt last);
    void assign(std::initializer_list<T> ilist);

    allocator_type get_allocator() const noexcept;

    // acces
    reference at(size_type pos);
    const_reference at(size_type pos) const;
    reference operator[](size_type pos);
    const_reference operator[](size_type pos) const;
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;
    T* data() noexcept;
    const T* data() const noexcept;

    // iterators
    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;
    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crend() const noexcept;

    // size and cap
    bool empty() const noexcept;
    size_type size() const noexcept;
    size_type max_size() const noexcept;
    void reserve(size_type new_cap);
    size_type capacity() const noexcept;
    void shrink_to_fit();

    // modifiers
    void clear() noexcept;

    iterator insert(const_iterator pos, const T& value);
    iterator insert(const_iterator pos, T&& value);
    iterator insert(const_iterator pos, size_type count, const T& value);
    template<class InputIt> iterator insert(const_iterator pos, InputIt first, InputIt last) requires std::input_iterator<InputIt>;
    iterator insert(const_iterator pos, std::initializer_list<T> ilist);

    template<class... Args> reference emplace(const_iterator pos, Args&&... args);

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    void push_back(const T& value);
    void push_back(T&& value);
    template<class... Args> reference emplace_back(Args&&... args);

    void pop_back();

    void resize(size_type count);
    void resize(size_type count, const value_type& value);

    void swap(Vector& other) noexcept;

};

// ctor and dtor
template<typename T, typename Allocator>
Vector<T, Allocator>::Vector() noexcept(noexcept(Allocator()))
    : data_(nullptr), size_(0), capacity_(0), alloc_(Allocator()) {}


template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(const Allocator& alloc)
    : data_(nullptr), size_(0), capacity_(0), alloc_(alloc) {}


template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(size_type count, const T& value, const Allocator& alloc)
    : data_(nullptr), size_(0), capacity_(0), alloc_(alloc) {
    if (count > 0) {
        data_ = std::allocator_traits<Allocator>::allocate(alloc_, count);
        for (size_type i = 0; i < count; ++i) {
            std::allocator_traits<Allocator>::construct(alloc_, data_ + i, value);
        }
        size_ = capacity_ = count;
    }
}


template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(std::initializer_list<T> init, const Allocator& alloc)
    : data_(nullptr), size_(0), capacity_(0), alloc_(alloc) {
    if (init.size() > 0) {
        data_ = std::allocator_traits<Allocator>::allocate(alloc_, init.size());
        size_ = capacity_ = init.size();
        size_type i = 0;
        for (const auto& elem : init) {
            std::allocator_traits<Allocator>::construct(alloc_, data_ + i, elem);
            ++i;
        }
    }
}


template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(const Vector& other)
    : data_(nullptr), size_(0), capacity_(0), alloc_(std::allocator_traits<Allocator>::
                                                   select_on_container_copy_construction(other.alloc_)) {
    if (other.size_ > 0) {
        data_ = std::allocator_traits<Allocator>::allocate(alloc_, other.size_);
        for (size_type i = 0; i < other.size_; ++i) {
            std::allocator_traits<Allocator>::construct(alloc_, data_ + i, other.data_[i]);
        }
        size_ = capacity_ = other.size_;
    }
}


template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(const Vector& other, const Allocator& alloc)
    : data_(nullptr), size_(0), capacity_(0), alloc_(alloc) {
    if (other.size_ > 0) {
        data_ = std::allocator_traits<Allocator>::allocate(alloc_, other.size_);
        for (size_type i = 0; i < other.size_; ++i) {
            std::allocator_traits<Allocator>::construct(alloc_, data_ + i, other.data_[i]);
        }
        size_ = capacity_ = other.size_;
    }
}


template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(Vector&& other) noexcept
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_),
      alloc_(std::move(other.alloc_)) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}


template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(Vector&& other, const Allocator& alloc)
    : data_(nullptr), size_(0), capacity_(0), alloc_(alloc) {
    if (alloc_ == other.alloc_) {
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    } else {
        if (other.size_ > 0) {
            data_ = std::allocator_traits<Allocator>::allocate(alloc_, other.size_);
            for (size_type i = 0; i < other.size_; ++i) {
                std::allocator_traits<Allocator>::construct(alloc_, data_ + i, std::move(other.data_[i]));
            }
            size_ = capacity_ = other.size_;
        }
    }
}


template<typename T, typename Allocator>
Vector<T, Allocator>::~Vector() {
    clear();
    if (data_) {
        std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
    }
}

template<typename T, typename Allocator>
Vector<T, Allocator>& Vector<T, Allocator>::operator=(const Vector& other) {
    if (this == &other) {
        return *this;
    }

    if constexpr (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
        if (alloc_ != other.alloc_) {
            clear();
            std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
            data_ = nullptr;
            size_ = capacity_ = 0;
            alloc_ = other.alloc_;
        }
    }
    // now same allocators

    if (other.size_ > capacity_) {
        clear();
        std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
        data_ = std::allocator_traits<Allocator>::allocate(alloc_, other.size_);
        capacity_ = other.size_;
    }

    size_type i = 0;
    for (; i < other.size_ && i < size_; ++i) {
        data_[i] = other.data_[i];
    }

    for (; i < other.size_; ++i) {
        std::allocator_traits<Allocator>::construct(alloc_, data_ + i, other.data_[i]);
    }

    for (; i < size_; ++i) {
        std::allocator_traits<Allocator>::destroy(alloc_, data_ + i);
    }

    size_ = other.size_;
    return *this;

}

template<typename T, typename Allocator>
Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    clear();
    if constexpr (std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value) {
        std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
        alloc_ = std::move(other.alloc_);
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = other.capacity_ = 0;
    } else {
        if (alloc_ == other.alloc_) {
            std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = other.capacity_ = 0;
        } else {
            assign(other.begin(), other.end());
        }
    }
    return *this;
}

template<typename T, typename Allocator>
Vector<T, Allocator>& Vector<T, Allocator>::operator=(std::initializer_list<T> ilist) {
    assign(ilist.begin(), ilist.end());
    return *this;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::assign(size_type count, const T& value) {
    clear();
    if (count > capacity_) {
        std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
        data_ = std::allocator_traits<Allocator>::allocate(alloc_, count);
        capacity_ = count;
    }
    for (size_type i = 0; i < count; ++i) {
        std::allocator_traits<Allocator>::construct(alloc_, data_ + i, value);
    }
    size_ = count;
}

template<typename T, typename Allocator>
template<class InputIt>
void Vector<T, Allocator>::assign(InputIt first, InputIt last) {
    clear();
    size_type count = std::distance(first, last);
    if (count > capacity_) {
        std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
        data_ = std::allocator_traits<Allocator>::allocate(alloc_, count);
        capacity_ = count;
    }
    size_type i = 0;
    for (; first != last; ++first, ++i)
        std::allocator_traits<Allocator>::construct(alloc_, data_ + i, *first);
    size_ = i;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::assign(std::initializer_list<T> ilist) {
    assign(ilist.begin(), ilist.end());
}
template<typename T, typename Allocator>
typename Vector<T, Allocator>::allocator_type
Vector<T, Allocator>::get_allocator() const noexcept {
    return alloc_;
}

// observers
template<typename T, typename Allocator>
typename Vector<T, Allocator>::reference
Vector<T, Allocator>::at(size_type pos) {
    if (pos >= size_) {
        throw std::out_of_range("Vector::at index out of range");
    }

    return data_[pos];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::const_reference
Vector<T, Allocator>::at(size_type pos) const {
    if (pos >= size_) {
        throw std::out_of_range("Vector::at index out of range");
    }

    return data_[pos];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::reference
Vector<T, Allocator>::operator[](size_type pos) {
    return data_[pos];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::const_reference
Vector<T, Allocator>::operator[](size_type pos) const {
    return data_[pos];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::reference
Vector<T, Allocator>::front() {
    return data_[0];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::const_reference
Vector<T, Allocator>::front() const {
    return data_[0];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::reference
Vector<T, Allocator>::back() {
    return data_[size_ - 1];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::const_reference
Vector<T, Allocator>::back() const {
    return data_[size_ - 1];
}

template<typename T, typename Allocator>
T* Vector<T, Allocator>::data() noexcept {
    return data_;
}

template<typename T, typename Allocator>
const T* Vector<T, Allocator>::data() const noexcept {
    return data_;
}

// iterators

template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator
Vector<T, Allocator>::begin() noexcept{
    return data_;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::const_iterator
Vector<T, Allocator>::begin() const noexcept{
    return data_;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::const_iterator
Vector<T, Allocator>::cbegin() const noexcept{
    return data_;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator
Vector<T, Allocator>::end() noexcept{
    return data_ + size_;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::const_iterator
Vector<T, Allocator>::end() const noexcept{
    return data_ + size_;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::const_iterator
Vector<T, Allocator>::cend() const noexcept{
    return data_ + size_;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::reverse_iterator
Vector<T, Allocator>::rbegin() noexcept{
    return reverse_iterator(end());
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::const_reverse_iterator
Vector<T, Allocator>::rbegin() const noexcept{
    return reverse_iterator(end());;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::const_reverse_iterator
Vector<T, Allocator>::crbegin() const noexcept{
    return reverse_iterator(cend());;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::reverse_iterator
Vector<T, Allocator>::rend() noexcept{
    return reverse_iterator(begin());
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::const_reverse_iterator
Vector<T, Allocator>::rend() const noexcept{
    return reverse_iterator(begin());;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::const_reverse_iterator
Vector<T, Allocator>::crend() const noexcept{
    return reverse_iterator(cbegin());;
}


// size and cap
template<typename T, typename Allocator>
bool Vector<T, Allocator>::empty() const noexcept {
    return size_ == 0;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::size_type
Vector<T, Allocator>::size() const noexcept {
    return size_;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::size_type
Vector<T, Allocator>::max_size() const noexcept {
    return std::allocator_traits<Allocator>::max_size(alloc_);
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::size_type
Vector<T, Allocator>::capacity() const noexcept {
    return capacity_;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::reserve(size_type new_cap) {
    if (new_cap <= capacity_) {
        return;
    }

    pointer new_data = std::allocator_traits<Allocator>::allocate(alloc_, new_cap);

    for (size_type i = 0; i < size_; ++i) {
        std::allocator_traits<Allocator>::construct(alloc_, new_data + i, std::move_if_noexcept(data_[i]));
        std::allocator_traits<Allocator>::destroy(alloc_, data_ + i);
    }

    if (data_ != nullptr) {
        std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
    }

    data_ = new_data;
    capacity_ = new_cap;
}


template<typename T, typename Allocator>
void Vector<T, Allocator>::shrink_to_fit() {
    if (capacity_ == size_) {
        return;
    }

    if (size_ == 0) {
        if (data_) {
            std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
            data_ = nullptr;
        }
        capacity_ = 0;
        return;
    }

    pointer new_data = std::allocator_traits<Allocator>::allocate(alloc_, size_);
    for (size_type i = 0; i < size_; ++i) {
        std::allocator_traits<Allocator>::construct(alloc_, new_data + i, std::move_if_noexcept(data_[i]));
        std::allocator_traits<Allocator>::destroy(alloc_, data_ + i);
    }

    std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
    data_ = new_data;
    capacity_ = size_;
}

// modifiers

template<typename T, typename Allocator>
void Vector<T, Allocator>::clear() noexcept {
    for (size_type i = 0; i < size_; ++i) {
        std::allocator_traits<Allocator>::destroy(alloc_, data_ + i);
    }
    size_ = 0;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::push_back(const T& value) {
    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    std::allocator_traits<Allocator>::construct(alloc_, data_ + size_, value);
    ++size_;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::push_back(T&& value) {
    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    std::allocator_traits<Allocator>::construct(alloc_, data_ + size_, std::move(value));
    ++size_;
}


template<typename T, typename Allocator>
template<class... Args>
typename Vector<T, Allocator>::reference
Vector<T, Allocator>::emplace_back(Args&&... args) {
    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    std::allocator_traits<Allocator>::construct(alloc_, data_ + size_, std::forward<Args>(args)...);
    ++size_;
    return back();
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::pop_back() {
    if (size_ == 0) {
        return;
    }

    --size_;
    std::allocator_traits<Allocator>::destroy(alloc_, data_ + size_);
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::resize(size_type count) {
    if (count < size_) {
        for (size_type i = count; i < size_; ++i) {
            std::allocator_traits<Allocator>::destroy(alloc_, data_ + i);
        }
    } else if (count > size_) {
        reserve(count);
        for (size_type i = size_; i < count; ++i) {
            std::allocator_traits<Allocator>::construct(alloc_, data_ + i);
        }
    }
    size_ = count;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::resize(size_type count, const value_type& value) {
    if (count < size_) {
        for (size_type i = count; i < size_; ++i) {
            std::allocator_traits<Allocator>::destroy(alloc_, data_ + i);
        }
    } else if (count > size_) {
        reserve(count);
        for (size_type i = size_; i < count; ++i) {
            std::allocator_traits<Allocator>::construct(alloc_, data_ + i, value);
        }
    }
    size_ = count;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator
Vector<T, Allocator>::insert(const_iterator pos, const T& value) {
    size_type index = pos - data_;
    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    for (size_type i = size_; i > index; --i) {
        std::allocator_traits<Allocator>::construct(alloc_, data_ + i, std::move(data_[i - 1]));
        std::allocator_traits<Allocator>::destroy(alloc_, data_ + i - 1);
    }
    std::allocator_traits<Allocator>::construct(alloc_, data_ + index, value);
    ++size_;
    return data_ + index;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator
Vector<T, Allocator>::insert(const_iterator pos, T&& value) {
    size_type index = pos - data_;
    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    for (size_type i = size_; i > index; --i) {
        std::allocator_traits<Allocator>::construct(alloc_, data_ + i, std::move(data_[i - 1]));
        std::allocator_traits<Allocator>::destroy(alloc_, data_ + i - 1);
    }
    std::allocator_traits<Allocator>::construct(alloc_, data_ + index, std::move(value));
    ++size_;
    return data_ + index;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator
Vector<T, Allocator>::insert(const_iterator pos, size_type count, const T& value) {
    size_type index = pos - data_;
    if (count == 0) {
        return data_ + index;
    }
    if (size_ + count > capacity_)
        reserve(std::max(capacity_ * 2, size_ + count));

    for (size_type i = size_; i > index; --i) {
        std::allocator_traits<Allocator>::construct(alloc_, data_ + i + count - 1, std::move(data_[i - 1]));
        std::allocator_traits<Allocator>::destroy(alloc_, data_ + i - 1);
    }

    for (size_type i = 0; i < count; ++i) {
        std::allocator_traits<Allocator>::construct(alloc_, data_ + index + i, value);
    }

    size_ += count;
    return data_ + index;
}

template<typename T, typename Allocator>
template<class InputIt>
typename Vector<T, Allocator>::iterator
Vector<T, Allocator>::insert(const_iterator pos, InputIt first, InputIt last) requires std::input_iterator<InputIt>{
    size_type index = pos - data_;
    size_type count = std::distance(first, last);
    if (count == 0) {
        return data_ + index;
    }
    if (size_ + count > capacity_) {
        reserve(std::max(capacity_ * 2, size_ + count));
    }

    for (size_type i = size_; i > index; --i) {
        std::allocator_traits<Allocator>::construct(alloc_, data_ + i + count - 1, std::move(data_[i - 1]));
        std::allocator_traits<Allocator>::destroy(alloc_, data_ + i - 1);
    }

    for (size_type i = 0; first != last; ++first, ++i) {
        std::allocator_traits<Allocator>::construct(alloc_, data_ + index + i, *first);
    }

    size_ += count;
    return data_ + index;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator
Vector<T, Allocator>::insert(const_iterator pos, std::initializer_list<T> ilist) {
    return insert(pos, ilist.begin(), ilist.end());
}

template<typename T, typename Allocator>
template<class... Args>
typename Vector<T, Allocator>::reference
Vector<T, Allocator>::emplace(const_iterator pos, Args&&... args) {
    size_type index = pos - data_;
    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }

    for (size_type i = size_; i > index; --i) {
        std::allocator_traits<Allocator>::construct(alloc_, data_ + i, std::move(data_[i - 1]));
        std::allocator_traits<Allocator>::destroy(alloc_, data_ + i - 1);
    }

    std::allocator_traits<Allocator>::construct(alloc_, data_ + index, std::forward<Args>(args)...);
    ++size_;
    return data_[index];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator
Vector<T, Allocator>::erase(const_iterator pos) {
    return erase(pos, pos + 1);
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::iterator
Vector<T, Allocator>::erase(const_iterator first, const_iterator last) {
    size_type index_first = first - data_;
    size_type index_last = last - data_;
    size_type count = index_last - index_first;
    if (count == 0) {
        return data_ + index_first;
    }

    for (size_type i = index_first; i < index_last; ++i) {
        std::allocator_traits<Allocator>::destroy(alloc_, data_ + i);
    }

    for (size_type i = index_last; i < size_; ++i) {
        std::allocator_traits<Allocator>::construct(alloc_, data_ + i - count, std::move(data_[i]));
        std::allocator_traits<Allocator>::destroy(alloc_, data_ + i);
    }
    size_ -= count;
    return data_ + index_first;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::swap(Vector& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    if constexpr (std::allocator_traits<Allocator>::propagate_on_container_swap::value) {
        std::swap(alloc_, other.alloc_);
    }
}