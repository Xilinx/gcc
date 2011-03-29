#ifndef THREAD_ANNOT_COMMON_H
#define THREAD_ANNOT_COMMON_H

#if defined(__GNUC__) && defined(__SUPPORT_TS_ANNOTATION__)

#define LOCKABLE             __attribute__ ((lockable))
#define SCOPED_LOCKABLE      __attribute__ ((scoped_lockable))
#define GUARDED_BY(x)        __attribute__ ((guarded_by(x))) 
#define GUARDED_VAR          __attribute__ ((guarded)) 
#define PT_GUARDED_BY(x)     __attribute__ ((point_to_guarded_by(x)))
#define PT_GUARDED_VAR       __attribute__ ((point_to_guarded))
#define ACQUIRED_AFTER(...)  __attribute__ ((acquired_after(__VA_ARGS__)))
#define ACQUIRED_BEFORE(...) __attribute__ ((acquired_before(__VA_ARGS__)))
#define EXCLUSIVE_LOCK_FUNCTION(...) \
  __attribute__ ((exclusive_lock(__VA_ARGS__)))
#define SHARED_LOCK_FUNCTION(...) \
  __attribute__ ((shared_lock(__VA_ARGS__)))
#define EXCLUSIVE_TRYLOCK_FUNCTION(...) \
  __attribute__ ((exclusive_trylock(__VA_ARGS__)))
#define SHARED_TRYLOCK_FUNCTION(...) \
  __attribute__ ((shared_trylock(__VA_ARGS__)))
#define UNLOCK_FUNCTION(...) __attribute__ ((unlock(__VA_ARGS__)))
#define LOCK_RETURNED(x)     __attribute__ ((lock_returned(x)))
#define LOCKS_EXCLUDED(...)  __attribute__ ((locks_excluded(__VA_ARGS__)))
#define EXCLUSIVE_LOCKS_REQUIRED(...) \
  __attribute__ ((exclusive_locks_required(__VA_ARGS__)))
#define SHARED_LOCKS_REQUIRED(...) \
  __attribute__ ((shared_locks_required(__VA_ARGS__)))
#define NO_THREAD_SAFETY_ANALYSIS  __attribute__ ((no_thread_safety_analysis))
#define IGNORE_READS_BEGIN   __attribute__ ((ignore_reads_begin))
#define IGNORE_READS_END     __attribute__ ((ignore_reads_end))
#define IGNORE_WRITES_BEGIN  __attribute__ ((ignore_writes_begin))
#define IGNORE_WRITES_END    __attribute__ ((ignore_writes_end))
#define UNPROTECTED_READ     __attribute__ ((unprotected_read))

#else

#define LOCKABLE
#define SCOPED_LOCKABLE
#define GUARDED_BY(x)
#define GUARDED_VAR
#define PT_GUARDED_BY(x)
#define PT_GUARDED_VAR
#define ACQUIRED_AFTER(...)
#define ACQUIRED_BEFORE(...)
#define EXCLUSIVE_LOCK_FUNCTION(...)
#define SHARED_LOCK_FUNCTION(...)
#define EXCLUSIVE_TRYLOCK_FUNCTION(...)
#define SHARED_TRYLOCK_FUNCTION(...)
#define UNLOCK_FUNCTION(...)
#define LOCK_RETURNED(x)
#define LOCKS_EXCLUDED(...)
#define EXCLUSIVE_LOCKS_REQUIRED(...)
#define SHARED_LOCKS_REQUIRED(...)
#define NO_THREAD_SAFETY_ANALYSIS
#define IGNORE_READS_BEGIN
#define IGNORE_READS_END
#define IGNORE_WRITES_BEGIN
#define IGNORE_WRITES_END
#define UNPROTECTED_READ

#endif // defined(__GNUC__) && defined(__SUPPORT_TS_ANNOTATION__)


class LOCKABLE Mutex {
 public:
  void Lock() EXCLUSIVE_LOCK_FUNCTION();
  void ReaderLock() SHARED_LOCK_FUNCTION();
  void Unlock() UNLOCK_FUNCTION();
  bool TryLock() EXCLUSIVE_TRYLOCK_FUNCTION(true);
  bool ReaderTryLock() SHARED_TRYLOCK_FUNCTION(true);
  void LockWhen(const int &cond) EXCLUSIVE_LOCK_FUNCTION();
};

class SCOPED_LOCKABLE MutexLock {
 public:
  MutexLock(Mutex *mu) EXCLUSIVE_LOCK_FUNCTION(mu);
  ~MutexLock() UNLOCK_FUNCTION();
};

class SCOPED_LOCKABLE ReaderMutexLock {
 public:
  ReaderMutexLock(Mutex *mu) SHARED_LOCK_FUNCTION(mu);
  ~ReaderMutexLock() UNLOCK_FUNCTION();
};

class SCOPED_LOCKABLE ReleasableMutexLock {
 public:
  explicit ReleasableMutexLock(Mutex *mu) EXCLUSIVE_LOCK_FUNCTION(mu);
  ~ReleasableMutexLock() UNLOCK_FUNCTION();
  void Release() UNLOCK_FUNCTION();
};

void AnnotateIgnoreReadsBegin(const char *file, int line) IGNORE_READS_BEGIN;
void AnnotateIgnoreReadsEnd(const char *file, int line) IGNORE_READS_END;
void AnnotateIgnoreWritesBegin(const char *file, int line) IGNORE_WRITES_BEGIN;
void AnnotateIgnoreWritesEnd(const char *file, int line) IGNORE_WRITES_END;

#define ANNOTATE_IGNORE_READS_BEGIN() \
  AnnotateIgnoreReadsBegin(__FILE__, __LINE__)

#define ANNOTATE_IGNORE_READS_END() \
  AnnotateIgnoreReadsEnd(__FILE__, __LINE__)

#define ANNOTATE_IGNORE_WRITES_BEGIN() \
  AnnotateIgnoreWritesBegin(__FILE__, __LINE__)

#define ANNOTATE_IGNORE_WRITES_END() \
  AnnotateIgnoreWritesEnd(__FILE__, __LINE__)

#define ANNOTATE_IGNORE_READS_AND_WRITES_BEGIN() \
  do {                                           \
    ANNOTATE_IGNORE_READS_BEGIN();               \
    ANNOTATE_IGNORE_WRITES_BEGIN();              \
  }while(0)                                      \

#define ANNOTATE_IGNORE_READS_AND_WRITES_END() \
  do {                                         \
    ANNOTATE_IGNORE_WRITES_END();              \
    ANNOTATE_IGNORE_READS_END();               \
  }while(0)                                    \

template <class T>
inline T ANNOTATE_UNPROTECTED_READ(const T &x) UNPROTECTED_READ {
  ANNOTATE_IGNORE_READS_BEGIN();
  T res = x;
  ANNOTATE_IGNORE_READS_END();
  return res;
}

#endif // THREAD_ANNOT_COMMON_H
