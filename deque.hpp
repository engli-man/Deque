#pragma once
#include <cmath>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <vector>

/* class Deque */

template <typename T, typename Allocator = std::allocator<T>>
class Deque {
 public:
  using allocator_type = Allocator;
  using allocator_type_traits = std::allocator_traits<Allocator>;

  Deque() {
    deque_.resize(1);
    alloc_ = Allocator();
    deque_[0] = allocator_type_traits::allocate(alloc_, kMagicNumber);
  }

  explicit Deque(const Allocator& alloc) : alloc_(alloc) {
    deque_.resize(1);
    deque_[0] = allocator_type_traits::allocate(alloc_, kMagicNumber);
  }

  Deque(size_t count, const T& value, const Allocator& alloc = Allocator())
      : count_(count), alloc_(alloc) {
    try {
      int if_it_need = static_cast<int>((count % kMagicNumber) != 0);
      arrays_ = count / kMagicNumber + if_it_need;
      deque_.resize(2 * arrays_);
      for (size_t i = 0; i < static_cast<size_t>(2 * arrays_); ++i) {
        deque_[i] = allocator_type_traits::allocate(alloc_, kMagicNumber);
      }
      end_ = count % kMagicNumber;
      for (size_t i = arrays_; i < static_cast<size_t>(2 * arrays_); ++i) {
        for (int j = 0; j < kMagicNumber; ++j) {
          allocator_type_traits::construct(alloc_, deque_[i] + j, value);
        }
      }
      front_arrays_ = arrays_;
      front_ = 0;
    } catch (...) {
      for (size_t i = 0; i < deque_.size(); ++i) {
        for (size_t j = 0; j < static_cast<size_t>(kMagicNumber); ++j) {
          allocator_type_traits::destroy(alloc_, deque_[i] + j);
        }
        allocator_type_traits::deallocate(alloc_, deque_[i], kMagicNumber);
      }

      throw;
    }
  }

  Deque(size_t count, const Allocator& alloc = Allocator())  //one extra copy
      : Deque(count, T(), alloc) {
    count_ = count;
    alloc_ = alloc;
  }

  Deque(const Deque& other)
      : count_(other.count_),
        arrays_(other.arrays_),
        front_arrays_(other.front_arrays_),
        front_(other.front_),
        end_(other.end_) {
    alloc_ = allocator_type_traits::select_on_container_copy_construction(
        other.alloc_);
    size_t flag = 0;
    try {
      size_t size = other.deque_.size();
      deque_.resize(size);
      for (size_t i = 0; i < size; ++i) {
        deque_[i] = allocator_type_traits::allocate(alloc_, kMagicNumber);
      }
      if (static_cast<size_t>(arrays_) == size) {
        for (size_t j = static_cast<size_t>(front_);
             j < static_cast<size_t>(end_); ++j) {
          allocator_type_traits::construct(alloc_, deque_[arrays_ - 1] + j,
                                           other.deque_[arrays_ - 1][j]);
          flag++;
        }
      }
      for (size_t i = arrays_; i < size; ++i) {
        if (count_ >= kMagicNumber) {
          for (size_t j = 0; j < static_cast<size_t>(kMagicNumber); ++j) {
            allocator_type_traits::construct(alloc_, deque_[i] + j,
                                             other.deque_[i][j]);
            flag++;
          }
        } else {
          for (size_t j = 0; j < static_cast<size_t>(count_); ++j) {
            allocator_type_traits::construct(alloc_, deque_[i] + j,
                                             other.deque_[i][j]);
            flag++;
          }
        }
      }
    } catch (...) {
      for (size_t itt = 0; itt < flag; ++itt) {
        allocator_type_traits::destroy(alloc_, &(*this)[itt]);
      }

      for (size_t i = 0; i < deque_.size(); ++i) {
        allocator_type_traits::deallocate(alloc_, deque_[i], kMagicNumber);
      }
      throw;
    }
  }

  void destruct_helper(size_t point, size_t dq_sz) {
    if (arrays_ == 1) {
      for (size_t j = static_cast<size_t>(front_);
           j < static_cast<size_t>(end_); ++j) {
        allocator_type_traits::destroy(alloc_, deque_[point] + j);
      }
      return;
    }
    if (point == 0) {
      for (size_t j = static_cast<size_t>(front_);
           j < static_cast<size_t>(kMagicNumber); ++j) {
        allocator_type_traits::destroy(alloc_, deque_[point] + j);
      }
      return;
    }
    if (point == dq_sz - 1) {
      for (size_t j = 0; j < static_cast<size_t>(end_); ++j) {
        allocator_type_traits::destroy(alloc_, deque_[point] + j);
      }
      return;
    }
    int check = 1;
    if (check == 1) {
      for (size_t j = 0; j < static_cast<size_t>(kMagicNumber); ++j) {
        allocator_type_traits::destroy(alloc_, deque_[point] + j);
      }
      return;
    }
  }

  ~Deque() {
    size_t dq_sz = deque_.size();
    for (size_t i = 0; i < static_cast<size_t>(front_arrays_); ++i) {
      allocator_type_traits::deallocate(alloc_, deque_[i], kMagicNumber);
    }

    for (size_t i = static_cast<size_t>(front_arrays_); i < dq_sz; ++i) {
      destruct_helper(i, dq_sz);
      allocator_type_traits::deallocate(alloc_, deque_[i], kMagicNumber);
    }
  }

  Deque& operator=(const Deque& other) {
    if (allocator_type_traits::propagate_on_container_copy_assignment::value &&
        alloc_ != other.alloc_) {
      alloc_ = other.alloc_;
    }
    std::vector<T*> temp(1, nullptr);
    size_t flag = 0;
    try {
      size_t size = other.deque_.size();
      temp.resize(size);
      for (size_t i = 0; i < size; ++i) {
        temp[i] = allocator_type_traits::allocate(alloc_, kMagicNumber);
      }
      for (size_t i = other.arrays_; i < size; ++i) {
        if (other.count_ >= kMagicNumber) {
          for (size_t j = 0; j < kMagicNumber; ++j) {
            allocator_type_traits::construct(alloc_, temp[i] + j,
                                             other.deque_[i][j]);
            flag++;
          }
        } else {
          for (size_t j = 0; j < static_cast<size_t>(other.count_); ++j) {
            allocator_type_traits::construct(alloc_, temp[i] + j,
                                             other.deque_[i][j]);
            flag++;
          }
        }
      }
    } catch (...) {
      for (size_t itt = 0; itt < flag; ++itt) {
        allocator_type_traits::destroy(alloc_, &(*this)[itt]);
      }

      for (size_t i = 0; i < deque_.size(); ++i) {
        allocator_type_traits::deallocate(alloc_, deque_[i], kMagicNumber);
      }
      throw;
    }

    for (size_t itt = 0; itt < static_cast<size_t>(count_); ++itt) {
      allocator_type_traits::destroy(alloc_, &(*this)[itt]);
    }

    for (size_t i = 0; i < deque_.size(); ++i) {
      allocator_type_traits::deallocate(alloc_, deque_[i], kMagicNumber);
    }
    deque_ = temp;
    count_ = other.count_;
    arrays_ = other.arrays_;
    front_arrays_ = other.front_arrays_;
    front_ = other.front_;
    end_ = other.end_;
    return *this;
  }

  Deque& operator=(Deque&& other) {
    if (allocator_type_traits::propagate_on_container_copy_assignment::value &&
        alloc_ != other.alloc_) {
      alloc_ = other.alloc_;
    }
    std::vector<T*> temp(1, nullptr);
    try {
      size_t size = other.deque_.size();
      temp.resize(size);
      for (size_t i = 0; i < size; ++i) {
        temp[i] = allocator_type_traits::allocate(alloc_, kMagicNumber);
      }
      for (size_t i = other.arrays_; i < size; ++i) {
        for (size_t j = 0; j < kMagicNumber; ++j) {
          allocator_type_traits::construct(alloc_, temp[i] + j,
                                           std::move(other.deque_[i][j]));
        }
      }
    } catch (...) {
      for (size_t i = 0; i < deque_.size(); ++i) {
        for (size_t j = 0; j < static_cast<size_t>(kMagicNumber); ++j) {
          allocator_type_traits::destroy(alloc_, temp[i] + j);
        }
        allocator_type_traits::deallocate(alloc_, temp[i], kMagicNumber);
      }
      throw;
    }
    for (size_t i = 0; i < deque_.size(); ++i) {
      if (count_ >= kMagicNumber) {
        for (size_t j = 0; j < static_cast<size_t>(kMagicNumber); ++j) {
          allocator_type_traits::destroy(alloc_, deque_[i] + j);
        }
      } else {
        for (size_t j = 0; j < static_cast<size_t>(count_); ++j) {
          allocator_type_traits::destroy(alloc_, deque_[i] + j);
        }
      }
      allocator_type_traits::deallocate(alloc_, deque_[i], kMagicNumber);
    }
    deque_ = temp;
    count_ = other.count_;
    arrays_ = other.arrays_;
    front_arrays_ = other.front_arrays_;
    front_ = other.front_;
    end_ = other.end_;
    return *this;
  }

  Deque(std::initializer_list<T> init, const Allocator& alloc = Allocator()) {
    deque_.resize(1);
    alloc_ = alloc;
    deque_[0] = allocator_type_traits::allocate(alloc_, kMagicNumber);
    for (auto itt = init.begin(); itt != init.end(); ++itt) {
      push_back(*itt);
    }
  }

  Deque(Deque&& other) {
    alloc_ = allocator_type_traits::select_on_container_copy_construction(
        other.alloc_);
    try {
      std::swap(count_, other.count_);
      std::swap(arrays_, other.arrays_);
      std::swap(front_arrays_, other.front_arrays_);
      std::swap(front_, other.front_);
      std::swap(end_, other.end_);
      for (size_t itt = 0; itt < other.deque_.size(); ++itt) {
        deque_.push_back(other.deque_[itt]);
      }
      while (other.deque_.size() != 0) {
        other.deque_.pop_back();
      }
    } catch (...) {
      for (size_t i = 0; i < deque_.size(); ++i) {
        for (size_t j = 0; j < static_cast<size_t>(kMagicNumber); ++j) {
          allocator_type_traits::destroy(alloc_, deque_[i] + j);
        }
        allocator_type_traits::deallocate(alloc_, deque_[i], kMagicNumber);
      }
      throw;
    }
  }

  size_t size() const noexcept { return count_; }
  bool empty() const noexcept { return (count_ == 0); }

  T& operator[](size_t pos_elem) {
    int pos_first = front_arrays_ + static_cast<int>(pos_elem / kMagicNumber);
    int pos_second = (front_ + static_cast<int>(pos_elem)) % kMagicNumber;
    return deque_[pos_first][pos_second];
  }

  const T& operator[](size_t pos_elem) const {
    int pos_first = front_arrays_ + static_cast<int>(pos_elem / kMagicNumber);
    int pos_second = (front_ + static_cast<int>(pos_elem)) % kMagicNumber;
    return deque_[pos_first][pos_second];
  }

  T& at(size_t pos_elem) {
    if (static_cast<int>(pos_elem) >= count_ ||
        static_cast<int>(pos_elem) < 0) {
      throw std::out_of_range("out of range!");
    }
    int pos_first = front_arrays_ + static_cast<int>(pos_elem / kMagicNumber);
    int pos_second = (front_ + static_cast<int>(pos_elem)) % kMagicNumber;
    return deque_[pos_first][pos_second];
  }

  const T& at(size_t pos_elem) const {
    if (static_cast<int>(pos_elem) >= count_ ||
        static_cast<int>(pos_elem) < 0) {
      throw std::out_of_range("out of range!");
    }
    int pos_first = front_arrays_ + static_cast<int>(pos_elem / kMagicNumber);
    int pos_second = (front_ + static_cast<int>(pos_elem)) % kMagicNumber;
    return deque_[pos_first][pos_second];
  }

  void resize_for_push_back() {
    std::vector<T*> temp(front_arrays_ + 2 * arrays_);

    for (int i = front_arrays_ + arrays_; i < front_arrays_ + 2 * arrays_;
         ++i) {
      temp[i] = allocator_type_traits::allocate(alloc_, kMagicNumber);
    }
    for (size_t i = 0; i < deque_.size(); ++i) {
      temp[i] = deque_[i];
    }
    deque_ = temp;
  }

  void push_back(const T& character) {
    if (end_ >= kMagicNumber) {
      end_ = 0;
    }
    if (front_arrays_ + arrays_ == static_cast<int>(deque_.size()) &&
        end_ == 0) {
      resize_for_push_back();
    }
    if (end_ == 0) {
      ++arrays_;
    }
    size_t first_pos = front_arrays_ + arrays_ - 1;
    if (count_ != 0) {
      first_pos = front_arrays_ + arrays_ - 1;
    }
    try {
      allocator_type_traits::construct(alloc_, deque_[first_pos] + end_,
                                       character);
      ++end_;
      ++count_;
    } catch (...) {
      throw;
    }
  }

  void push_back(T&& character) {
    if (end_ >= kMagicNumber) {
      end_ = 0;
    }
    if (front_arrays_ + arrays_ == static_cast<int>(deque_.size()) &&
        end_ == 0) {
      resize_for_push_back();
    }
    if (end_ == 0) {
      ++arrays_;
    }
    size_t first_pos = front_arrays_ + arrays_ - 1;
    if (count_ != 0) {
      first_pos = front_arrays_ + arrays_ - 1;
    }
    try {
      allocator_type_traits::construct(alloc_, deque_[first_pos] + end_,
                                       std::move(character));
      ++end_;
      ++count_;
    } catch (...) {
      throw;
    }
  }

  template <class... Args>
  void emplace_back(Args&&... args) {
    if (end_ >= kMagicNumber) {
      end_ = 0;
    }
    if (front_arrays_ + arrays_ == static_cast<int>(deque_.size()) &&
        end_ == 0) {
      resize_for_push_back();
    }
    if (end_ == 0) {
      ++arrays_;
    }
    size_t first_pos = front_arrays_ + arrays_ - 1;
    if (count_ != 0) {
      first_pos = front_arrays_ + arrays_ - 1;
    }
    try {
      allocator_type_traits::construct(alloc_, deque_[first_pos] + end_,
                                       std::forward<Args>(args)...);
      ++end_;
      ++count_;
    } catch (...) {
      throw;
    }
  }

  void pop_back() {
    if (count_ == 0) {
      return;
    }
    size_t first_pos = 0;
    if ((front_arrays_ + arrays_ - 1) >= 0) {
      first_pos = front_arrays_ + arrays_ - 1;
    }
    if (end_ != 0) {
      --end_;
      allocator_type_traits::destroy(alloc_, deque_[first_pos] + end_);
    } else {
      end_ = kMagicNumber - 1;
      allocator_type_traits::destroy(alloc_, deque_[first_pos] + end_);
      if (static_cast<size_t>(front_arrays_ + arrays_) != deque_.size()) {
        arrays_--;
      }
    }
    --count_;
  }

  void resize_for_push_front() {
    std::vector<T*> temp(2 * deque_.size());
    front_arrays_ += static_cast<int>(deque_.size());
    for (int i = 0; i < front_arrays_; ++i) {
      temp[i] = allocator_type_traits::allocate(alloc_, kMagicNumber);
    }
    for (size_t i = 0; i < deque_.size(); ++i) {
      temp[i + front_arrays_] = deque_[i];
    }
    deque_ = temp;
  }

  void push_front(const T& character) {
    if (front_arrays_ == 0 && front_ == 0) {
      resize_for_push_front();
      try {
        front_ = kMagicNumber - 1;
        ++arrays_;
        --front_arrays_;
        allocator_type_traits::construct(alloc_, deque_[front_arrays_] + front_,
                                         character);
      } catch (...) {
        --arrays_;
        ++front_arrays_;
        front_ = 0;
        throw;
      }
    } else if (front_ > 0) {
      try {
        --front_;
        allocator_type_traits::construct(alloc_, deque_[front_arrays_] + front_,
                                         character);
      } catch (...) {
        ++front_;
        throw;
      }
    } else {
      try {
        front_ = kMagicNumber - 1;
        --front_arrays_;
        allocator_type_traits::construct(alloc_, deque_[front_arrays_] + front_,
                                         character);

      } catch (...) {
        ++front_arrays_;
        front_ = 0;
        throw;
      }
    }
    ++count_;
  }

  void push_front(T&& character) {
    if (front_arrays_ == 0 && front_ == 0) {
      resize_for_push_front();
      try {
        front_ = kMagicNumber - 1;
        ++arrays_;
        --front_arrays_;
        allocator_type_traits::construct(alloc_, deque_[front_arrays_] + front_,
                                         std::move(character));
      } catch (...) {
        --arrays_;
        ++front_arrays_;
        front_ = 0;
        throw;
      }
    } else if (front_ > 0) {
      try {
        --front_;
        allocator_type_traits::construct(alloc_, deque_[front_arrays_] + front_,
                                         std::move(character));
      } catch (...) {
        ++front_;
        throw;
      }
    } else {
      try {
        front_ = kMagicNumber - 1;
        --front_arrays_;
        allocator_type_traits::construct(alloc_, deque_[front_arrays_] + front_,
                                         std::move(character));
      } catch (...) {
        ++front_arrays_;
        front_ = 0;
        throw;
      }
    }
    ++count_;
  }

  template <class... Args>
  void emplace_front(Args&&... args) {
    if (front_arrays_ == 0 && front_ == 0) {
      resize_for_push_front();
      try {
        front_ = kMagicNumber - 1;
        ++arrays_;
        --front_arrays_;
        allocator_type_traits::construct(alloc_, deque_[front_arrays_] + front_,
                                         std::forward<Args>(args)...);
      } catch (...) {
        --arrays_;
        ++front_arrays_;
        front_ = 0;
        throw;
      }
    } else if (front_ > 0) {
      try {
        --front_;
        allocator_type_traits::construct(alloc_, deque_[front_arrays_] + front_,
                                         std::forward<Args>(args)...);
      } catch (...) {
        ++front_;
        throw;
      }
    } else {
      try {
        front_ = kMagicNumber - 1;
        --front_arrays_;
        allocator_type_traits::construct(alloc_, deque_[front_arrays_] + front_,
                                         std::forward<Args>(args)...);
      } catch (...) {
        ++front_arrays_;
        front_ = 0;
        throw;
      }
    }
    ++count_;
  }

  void pop_front() {
    if (count_ == 0) {
      return;
    }
    allocator_type_traits::destroy(alloc_, deque_[0] + front_);
    if (front_ < kMagicNumber - 1) {
      front_++;
    } else {
      front_ = 0;
      arrays_--;
      front_arrays_++;
    }
    --count_;
  }

  allocator_type get_allocator() { return alloc_; }

  template <bool IsConst, bool IsReversed>
  class Iterator;

  using iterator = Iterator<false, false>;
  using const_iterator = Iterator<true, false>;

  using reverse_iterator = Iterator<false, true>;
  using const_reverse_iterator = Iterator<true, true>;

  using value_type = std::remove_cv_t<T>;

  iterator end() {
    int offset = static_cast<int>(end_ != 0);
    return iterator(&(deque_[front_arrays_]) + arrays_ - offset, end_);
  }

  iterator begin() { return iterator(&(deque_[front_arrays_]), front_); }

  const_iterator cend() {
    int offset = static_cast<int>(end_ != 0);
    return const_iterator(&(deque_[front_arrays_]) + arrays_ - offset, end_);
  }

  const_iterator cbegin() {
    return const_iterator(&(deque_[front_arrays_]), front_);
  }

  reverse_iterator rbegin() {
    if (count_ == 0) {
      return {&(deque_[front_arrays_]), end_};
    }
    if (end_ > 0) {
      return {&(deque_[front_arrays_ + arrays_]) - 1, end_ - 1};
    }
    return {&(deque_[front_arrays_ + arrays_]) - 1, kMagicNumber - 1};
  }

  reverse_iterator rend() {
    if (count_ == 0) {
      return {&(deque_[front_arrays_]), front_};
    }
    if (front_ > 0) {
      return {&(deque_[front_arrays_]), front_ - 1};
    }
    reverse_iterator itt(&(deque_[front_arrays_]) - 1, kMagicNumber - 1);
    return itt;
  }

  const_reverse_iterator crbegin() {
    if (count_ == 0) {
      return {&(deque_[front_arrays_]), end_};
    }
    if (end_ > 0) {
      return {&(deque_[front_arrays_ + arrays_]), end_ - 1};
    }
    return {&(deque_[front_arrays_ + arrays_]) - 1, 0};
  }

  const_reverse_iterator crend() {
    if (count_ == 0) {
      return {&(deque_[front_arrays_]), front_};
    }
    if (front_ > 0) {
      return {&(deque_[front_arrays_]), front_};
    }
    reverse_iterator itt(&(deque_[front_arrays_]) + 1, kMagicNumber - 1);
    return itt;
  }

  void insert(iterator itt, const T& other) {
    Deque<T> temp = *this;
    try {
      if (itt > end() || itt < begin()) {
        throw std::out_of_range("out of range!");
      }
      auto end_of_my_deque = end();
      T temp1 = other;
      T temp2 = other;
      while (itt < end_of_my_deque) {
        temp2 = *itt;
        *itt = temp1;
        temp1 = temp2;
        itt++;
      }
      push_back(temp1);
    } catch (...) {
      *this = temp;
      throw;
    }
  }

  void erase(iterator itt) {
    if (itt >= end() || itt < begin()) {
      throw std::out_of_range("out of range!");
    }
    auto end_of_my_deque = end() - 1;
    while (itt < end_of_my_deque) {
      *itt = *(itt + 1);
      itt++;
    }
    if (itt.get_pos() == 0) {
      arrays_--;
    }
    --count_;
    --end_;
  }

  template <class... Args>
  void emplace(iterator itt, Args&&... args) {
    Deque<T> temp = *this;
    try {
      if (itt > end() || itt < begin()) {
        throw std::out_of_range("out of range!");
      }
      auto end_of_my_deque = end();

      T temp1(std::forward<Args>(args)...);
      T temp2 = T();

      while (itt < end_of_my_deque) {
        temp2 = *itt;
        *itt = temp1;
        temp1 = temp2;
        itt++;
      }
      push_back(temp1);
    } catch (...) {
      *this = temp;
      throw;
    }
  }

  /* class Iterator */

  template <bool IsConst, bool IsReversed>
  class Iterator {
   public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::remove_cv_t<T>;
    using m_type = std::conditional_t<IsConst, const T, T>;
    using pointer = m_type*;
    using iterator_category = std::random_access_iterator_tag;
    using reference = m_type&;

    Iterator(T** vector, int pos_elem_in_deque)
        : pos_(pos_elem_in_deque), ptr_vector_(vector) {}

    Iterator(const Iterator& other)
        : pos_(other.pos_), ptr_vector_(other.ptr_vector_) {}

    operator Iterator<true, IsReversed>() const {
      return Iterator<true, IsReversed>(ptr_vector_, pos_);
    }

    operator Iterator<false, IsReversed>() const {
      if (IsConst) {
        throw std::bad_cast();
      }
      return Iterator<false, IsReversed>(ptr_vector_, pos_);
    }

    Iterator& operator=(const Iterator& other) {
      pos_ = other.pos_;
      ptr_vector_ = other.ptr_vector_;
      return *this;
    }
    reference operator*() const { return (*ptr_vector_)[pos_]; }

    pointer operator->() const { return *ptr_vector_ + pos_; }

    Iterator operator+(int n) const {
      if (IsReversed) {
        Iterator<IsConst, IsReversed> temp = *this;
        if (pos_ - n >= 0) {
          temp.pos_ -= n;
        } else {
          int it_need = n - pos_ - 1;
          if (it_need != 0) {
            temp.pos_ =
                (kMagicNumber - (it_need + 1 - (it_need - 1) * kMagicNumber)) %
                kMagicNumber;
          } else {
            temp.pos_ = (kMagicNumber - (it_need + 1)) % kMagicNumber;
          }
          temp.ptr_vector_ -= (it_need / kMagicNumber + 1);
        }
        return temp;
      }
      Iterator<IsConst, IsReversed> temp = *this;
      if (pos_ + n < kMagicNumber) {
        temp.pos_ += n;
      } else {
        temp.pos_ = (pos_ + n) % kMagicNumber;
        temp.ptr_vector_ += (pos_ + n) / kMagicNumber;
      }
      return temp;
    }

    Iterator<IsConst, IsReversed>& operator+=(int num) {
      *this = (*this + num);
      return *this;
    }
    Iterator<IsConst, IsReversed>& operator++() {
      *this += 1;
      return *this;
    }

    Iterator<IsConst, IsReversed> operator++(int) {
      Iterator<IsConst, IsReversed> temp = *this;
      *this += 1;
      return temp;
    }

    Iterator<IsConst, IsReversed> operator-(int n) const {
      if (IsReversed) {
        Iterator<IsConst, IsReversed> temp = *this;
        if (pos_ + n < kMagicNumber) {
          temp.pos_ += n;
        } else {
          temp.pos_ = (pos_ + n) % kMagicNumber;
          temp.ptr_vector_ += (pos_ + n) / kMagicNumber;
        }
        return temp;
      }
      Iterator<IsConst, IsReversed> temp = *this;
      if (pos_ - n >= 0) {
        temp.pos_ -= n;
      } else {
        int it_need = n - pos_ - 1;
        if (it_need != 0) {
          temp.pos_ =
              (kMagicNumber - (it_need + 1 - (it_need - 1) * kMagicNumber)) %
              kMagicNumber;
        } else {
          temp.pos_ = (kMagicNumber - (it_need + 1)) % kMagicNumber;
        }
        temp.ptr_vector_ -= (it_need / kMagicNumber + 1);
      }
      return temp;
    }

    Iterator<IsConst, IsReversed>& operator-=(int num) {
      *this = (*this - num);
      return *this;
    }
    Iterator<IsConst, IsReversed>& operator--() {
      *this -= 1;
      return *this;
    }

    Iterator<IsConst, IsReversed> operator--(int) {
      Iterator<IsConst, IsReversed> temp = *this;
      *this -= 1;
      return temp;
    }

    bool operator<(const Iterator<IsConst, IsReversed>& second) const {
      if (IsReversed) {
        if (ptr_vector_ == second.ptr_vector_) {
          return pos_ > second.pos_;
        }
        return ptr_vector_ > second.ptr_vector_;
      }
      if (ptr_vector_ == second.ptr_vector_) {
        return pos_ < second.pos_;
      }
      return ptr_vector_ < second.ptr_vector_;
    }

    bool operator==(const Iterator<IsConst, IsReversed>& second) const {
      return ptr_vector_ == second.ptr_vector_ && pos_ == second.pos_;
    }

    bool operator!=(const Iterator<IsConst, IsReversed>& second) const {
      return ptr_vector_ != second.ptr_vector_ || pos_ != second.pos_;
    }

    bool operator>(const Iterator<IsConst, IsReversed>& second) const {
      return second < *this;
    }

    bool operator<=(const Iterator<IsConst, IsReversed>& second) const {
      return !(*this > second);
    }

    bool operator>=(const Iterator<IsConst, IsReversed>& second) const {
      return !(*this < second);
    }

    difference_type operator-(
        const Iterator<IsConst, IsReversed>& second) const {
      difference_type temp =
          static_cast<int>(ptr_vector_ - second.ptr_vector_) * kMagicNumber +
          (pos_ - second.pos_);
      if (IsReversed) {
        temp =
            static_cast<int>(second.ptr_vector_ - ptr_vector_) * kMagicNumber +
            (second.pos_ - pos_);
      }
      return temp;
    }

    int get_pos() { return pos_; }

    T** get_ptr() { return ptr_vector_; }

   private:
    T** ptr_vector_ = nullptr;
    int pos_;
  };

 private:
  static const int kMagicNumber = 10000;
  int count_ = 0;
  int arrays_ = 0;
  int front_ = 0;
  int end_ = 0;
  int front_arrays_ = 0;
  std::vector<T*> deque_;
  allocator_type alloc_;
};