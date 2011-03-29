// Test handling of arguments passed to reference parameters.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include <string>
#include "thread_annot_common.h"

class Base {
 public:
  Base() {}
 protected:
  Mutex* mutex() const LOCK_RETURNED(mutex_) { return &mutex_; }
 private:
  mutable Mutex mutex_;
};

class Subclass : public Base {
 public:
  Subclass() {}

  void ClearValue() { SetValueLocked(0); }
  std::string GetValue() const;

 private:
  void SetValueLocked(std::string value) { value_ = value; }

  std::string value_ GUARDED_BY(mutex_);
};

std::string Subclass::GetValue() const {
  return value_; // { dg-warning "Reading variable 'value_' requires lock" }
}
