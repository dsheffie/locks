#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <vector>

#include "lock.hh"

mcslock *mcs = nullptr;
size_t ready = 0;

void *worker(void *arg) {
  size_t tid = reinterpret_cast<size_t>(arg);
  __sync_fetch_and_sub(&ready, 1);
  volatile size_t *r = reinterpret_cast<volatile size_t*>(&ready);
  while(*r != 0) {}
  
  for(int i = 0; i < 100; i++) {
    mcs->lock(tid);
    printf("tid %zu has the lock\n", tid);
    mcs->unlock(tid);
  }
  
  return nullptr;
}


int main(int argc, char *argv[]) {
  std::vector<pthread_t> threads;
  size_t n_thr = 4;
  
  ready = n_thr;
  threads.resize(n_thr);  
  mcs = new mcslock(n_thr);

  for(size_t i = 0; i < n_thr; i++) {
    pthread_create(&threads.at(i), nullptr, worker, reinterpret_cast<void*>(i));
  }
  for(size_t i = 0; i < n_thr; i++) {
    pthread_join(threads.at(i), nullptr);
  }

  
  delete mcs;
  return 0;
}
