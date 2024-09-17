// Copyright (c) 2024 MIPT-school of applied mathematics and computer science
// Author: Bulat Ibragimov
//
// Данное программное обеспечение распространяется на условиях лицензии MIT.
// Подробности смотрите в файле LICENSE

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <vector>
#include <utility>
#include "shared_ptr.h"

TEST_CASE("Shared Constructors", "[SharedPtr]") {
  const SharedPtr<int> a;
  const SharedPtr<int> b(nullptr);

  auto ptr = new int(35);
  const SharedPtr<int> c(ptr);
  SharedPtr<int> d(c);

  REQUIRE(a.Get() == nullptr);
  REQUIRE(b.Get() == nullptr);
  REQUIRE(c.Get() == ptr);
  REQUIRE(d.Get() == ptr);

  const SharedPtr<int> e(std::move(d));

  REQUIRE(c.Get() == ptr);
  REQUIRE(d.Get() == nullptr);  // NOLINT check moved valid state
  REQUIRE(e.Get() == ptr);

  const auto copy = a;
  REQUIRE(copy.Get() == nullptr);

  REQUIRE(std::is_nothrow_move_constructible_v<SharedPtr<int>>);
}

TEST_CASE("Assignment", "[SharedPtr]") {
  SharedPtr<int> a;
  SharedPtr<int> b;

  auto ptr = new int(11);
  {  // value assignment
    a = SharedPtr<int>(ptr);
    REQUIRE(a.Get() == ptr);
  }

  {  // copy assignment
    b = a;
    REQUIRE(b.Get() == ptr);
    REQUIRE(a.Get() == ptr);
  }

  ptr = new int(13);
  {  // reassigning
    a = SharedPtr<int>(ptr);
    REQUIRE(a.Get() == ptr);
  }

  {  // copy assignment
    b = a;
    REQUIRE(b.Get() == ptr);
    REQUIRE(a.Get() == ptr);
  }

  {  // copy is independent
    b = SharedPtr<int>(nullptr);
    REQUIRE(a.Get() == ptr);
    REQUIRE(b.Get() == nullptr);
  }

  {  // move
    b = std::move(a);
    REQUIRE(a.Get() == nullptr);  // NOLINT check moved valid state
    REQUIRE(b.Get() == ptr);
  }

  {  // self-assignment
    b = b;
    REQUIRE(b.Get() == ptr);
  }

  {  // copy assignment
    b = a;
    REQUIRE(b.Get() == nullptr);
    REQUIRE(a.Get() == nullptr);
  }

  REQUIRE(std::is_nothrow_move_assignable_v<SharedPtr<int>>);
}

TEST_CASE("UseCount", "[SharedPtr]") {
  SharedPtr<int> a;
  const SharedPtr<int> b(new int(6));

  REQUIRE(a.UseCount() == 0);
  REQUIRE(b.UseCount() == 1);

  a = b;
  REQUIRE(a.UseCount() == 2);
  REQUIRE(b.UseCount() == 2);

  {  // copy/move
    SharedPtr<int> c(a);
    REQUIRE(a.UseCount() == 3);
    REQUIRE(b.UseCount() == 3);
    REQUIRE(c.UseCount() == 3);

    const SharedPtr<int> d(std::move(c));
    REQUIRE(a.UseCount() == 3);
    REQUIRE(b.UseCount() == 3);
    REQUIRE(c.UseCount() == 0);  // NOLINT check moved valid state
    REQUIRE(d.UseCount() == 3);
  }

  REQUIRE(a.UseCount() == 2);
  REQUIRE(b.UseCount() == 2);

  a = nullptr;
  REQUIRE(a.UseCount() == 0);
  REQUIRE(b.UseCount() == 1);
}

TEST_CASE("Swap", "[SharedPtr]") {
  auto ptr1 = new int;
  auto ptr2 = new int;
  SharedPtr<int> a;
  SharedPtr<int> b(ptr1);
  SharedPtr<int> c(ptr2);
  const SharedPtr<int> d(c);

  a.Swap(a);
  REQUIRE(!a);
  REQUIRE(a.Get() == nullptr);
  REQUIRE(a.UseCount() == 0);

  b.Swap(b);
  REQUIRE(b);
  REQUIRE(b.Get() == ptr1);
  REQUIRE(b.UseCount() == 1);

  a.Swap(b);
  REQUIRE(a);
  REQUIRE(a.Get() == ptr1);
  REQUIRE(a.UseCount() == 1);
  REQUIRE(!b);
  REQUIRE(b.Get() == nullptr);
  REQUIRE(b.UseCount() == 0);

  b.Swap(c);
  REQUIRE(b);
  REQUIRE(b.Get() == ptr2);
  REQUIRE(b.UseCount() == 2);
  REQUIRE(!c);
  REQUIRE(c.Get() == nullptr);
  REQUIRE(c.UseCount() == 0);

  a.Swap(b);
  REQUIRE(a);
  REQUIRE(a.Get() == ptr2);
  REQUIRE(a.UseCount() == 2);
  REQUIRE(b);
  REQUIRE(b.Get() == ptr1);
  REQUIRE(b.UseCount() == 1);

  REQUIRE(d);
  REQUIRE(d.Get() == ptr2);
  REQUIRE(d.UseCount() == 2);
}

TEST_CASE("Shared Reset", "[SharedPtr]") {
  {  // reset empty
    SharedPtr<int> a;
    a.Reset();
    REQUIRE(a.UseCount() == 0);

    a.Reset(new int);
    REQUIRE(a.UseCount() == 1);

    a.Reset();
    REQUIRE(a.UseCount() == 0);
  }

  {  // reset shared
    auto ptr1 = new int(5);
    SharedPtr<int> a(ptr1);
    SharedPtr<int> b = a;

    b.Reset();
    REQUIRE(a.UseCount() == 1);
    REQUIRE(a.Get() == ptr1);
    REQUIRE(b.UseCount() == 0);
    REQUIRE(b.Get() == nullptr);

    b = a;
    auto ptr2 = new int(7);
    a.Reset(ptr2);
    REQUIRE(a.UseCount() == 1);
    REQUIRE(a.Get() == ptr2);
    REQUIRE(b.UseCount() == 1);
    REQUIRE(b.Get() == ptr1);
  }
}

TEST_CASE("Operators", "[SharedPtr]") {
  {  // operator*
    const SharedPtr<int> a(new int(19));
    REQUIRE(*a == 19);

    *a = 11;
    REQUIRE(*a == 11);

    *a.Get() = -11;
    REQUIRE(*a == -11);
  }

  {  // operator->
    auto ptr = new int(11);
    const SharedPtr<SharedPtr<int>> a(new SharedPtr<int>(ptr));

    REQUIRE(a->UseCount() == 1);
    REQUIRE(a->Get() == ptr);

    a->Reset();
    REQUIRE(a->UseCount() == 0);
    REQUIRE(a->Get() == nullptr);
  }

  {  // operator bool
    const SharedPtr<int> a;
    if (a) {
      REQUIRE(false);
    }

    const SharedPtr<int> b = nullptr;
    if (b) {
      REQUIRE(false);
    }

    const SharedPtr<int> c(new int(6));
    if (c) {
      REQUIRE(true);
    }
  }
}