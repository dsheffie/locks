#include "lock.hh"
#include <cstdio>
#include <cassert>

void spinlock::lock(int tid) {
  while(__sync_bool_compare_and_swap(&lck, 0, 1) == false) {}
}
void spinlock::unlock(int tid) {
  lck = 0;
}


void ticketlock::lock(int tid) {
  volatile uint64_t *s = reinterpret_cast<volatile uint64_t*>(&curr_serving);
  uint64_t tkt = __sync_fetch_and_add(&counter, 1);
  while(*s != tkt) { /* spin */}
}


void ticketlock::unlock(int tid) {
  __sync_fetch_and_add(&curr_serving, 1);
}

void mcslock::lock(int tid) {
  node *n = &nodes[tid];
  node *old = __atomic_exchange_n (&head, n, __ATOMIC_SEQ_CST);
  if(old ==nullptr)
    return;
  old->next = n;
  __sync_synchronize();
  volatile uint64_t *s = reinterpret_cast<volatile uint64_t*>(&(n->ready));
  while(*s == 0) {}
  s = 0;
}

void mcslock::unlock(int tid) {
  node *n = &nodes[tid];
  if(__sync_bool_compare_and_swap(&head,n,nullptr)) {
    return;
  }
  assert(head != n);
  volatile node **next = reinterpret_cast<volatile node**>(&(n->next));
  while(*next == nullptr) {}
  n->next->ready = 1;
  n->next = nullptr;
}

