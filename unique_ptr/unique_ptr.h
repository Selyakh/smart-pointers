// Copyright (c) 2024 Irina Selyakh
//
// Данное программное обеспечение распространяется на условиях лицензии MIT.
// Подробности смотрите в файле LICENSE

#ifndef UNIQUE_PTR_H
#define UNIQUE_PTR_H

template <class T>
class UniquePtr {
  T* ptr_;

 public:
  explicit UniquePtr(T* pointer = nullptr) : ptr_{pointer} {
  }

  UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_) {  // конструктор перемещения
    other.ptr_ = nullptr;
  }

  UniquePtr& operator=(
      UniquePtr&& other) noexcept {  // присваивающее перемещение, которое не будет передавать исключения
    if (this != &other) {
      delete ptr_;
      ptr_ = other.ptr_;
      other.ptr_ = nullptr;
    }
    return *this;
  }

  T* Get() {
    return ptr_;
  }

  T* Get() const {
    return ptr_;
  }

  T* Release() {  // отлучает класс от владения текущим ресурсом и возвращает указатель на него
    T* tmp = ptr_;
    ptr_ = nullptr;
    return tmp;
  }

  void Reset(T* ptr = nullptr) {  // меняет указатель, которым владеет объект (старый ресурс удаляется)
    T* old = ptr_;
    ptr_ = ptr;
    delete old;
  }

  explicit operator bool() const {
    return ptr_;
  }

  T& operator[](T idx) {
    return ptr_[idx];
  }

  void Swap(UniquePtr<T>& other) {
    auto tmp = std::move(ptr_);
    ptr_ = std::move(other.ptr_);
    other.ptr_ = std::move(tmp);
  }

  T& operator*() const {
    return *ptr_;
  }

  T* operator->() const {
    return ptr_;
  }

  ~UniquePtr() {
    delete ptr_;
  }
};
#endif