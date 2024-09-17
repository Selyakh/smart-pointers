// Copyright (c) 2024 Irina Selyakh
//
// Данное программное обеспечение распространяется на условиях лицензии MIT.
// Подробности смотрите в файле LICENSE

#ifndef SHARED_PTR_H
#define SHARED_PTR_H
#include <stdexcept>
#include <utility>
#include <cstddef>

template <class T>
struct Node {
  T* ptr;
  size_t strong_counter;
};

template <class T>
class SharedPtr {
  Node<T>* node_;

 public:
  SharedPtr() : node_(nullptr) {
  }

  SharedPtr(std::nullptr_t) noexcept : SharedPtr() {  // NOLINT
  }

  explicit SharedPtr(T* ptr) : node_(new Node<T>{ptr, 1}) {  // конструктор от указателя
  }

  SharedPtr(SharedPtr&& other) noexcept : node_(other.node_) {  // конструктор перемещающий
    other.node_ = nullptr;
  }

  SharedPtr(const SharedPtr& other) : node_(other.node_) {  // конструктор копироваания
    if (node_ != nullptr) {
      ++(node_->strong_counter);
    }
  }

  T* Release() {  // я больше не владею текущим блоком
    if (!node_) {
      return nullptr;
    }
    auto ptr = node_->ptr;
    --(node_->strong_counter);
    if (node_->strong_counter == 0) {
      delete ptr;
      delete node_;
    }
    node_ = nullptr;
    return ptr;
  }

  SharedPtr& operator=(SharedPtr&& other) noexcept {  // перемещающее присваивание
    Release();
    node_ = other.node_;
    other.node_ = nullptr;
    return *this;
  }

  SharedPtr& operator=(const SharedPtr& other) {  // копирующее присваивание
    SharedPtr tmp = other;
    Swap(tmp);
    return *this;
  }

  void Reset() noexcept {  // меняет указатель, которым владеет объект, а тaкже ф-ция не будет передавать исключений
    SharedPtr().Swap(*this);
  }

  void Reset(T* ptr) {
    SharedPtr(ptr).Swap(*this);
  }

  T* Get() const {
    return (node_ == nullptr) ? nullptr : node_->ptr;
  }

  T& operator*() const {
    return *node_->ptr;
  }

  T* operator->() const {
    return node_->ptr;
  }

  explicit operator bool() const {
    return node_;
  }

  void Swap(SharedPtr<T>& other) noexcept {
    auto tmp = std::move(node_);
    node_ = std::move(other.node_);
    other.node_ = std::move(tmp);
  }

  [[nodiscard]] T UseCount() const {
    return node_ == nullptr ? 0 : node_->strong_counter;
  }

  ~SharedPtr() {
    Release();
  }
};
#endif