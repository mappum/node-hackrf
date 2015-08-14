#include <semaphore.h>
#include <dispatch/dispatch.h>

typedef dispatch_semaphore_t bindings_sem_t;

NAN_INLINE static int semaphore_init (dispatch_semaphore_t *sem) {
  *sem = dispatch_semaphore_create(0);
  return *sem == NULL ? -1 : 0;
}

NAN_INLINE static void semaphore_wait (dispatch_semaphore_t *sem) {
  dispatch_semaphore_wait(*sem, DISPATCH_TIME_FOREVER);
}

NAN_INLINE static void semaphore_signal (dispatch_semaphore_t *sem) {
  dispatch_semaphore_signal(*sem);
}
