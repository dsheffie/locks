#ifndef __lockshh__
#define __lockshh__

#include <cstdint>

class baselock {
private:
  int nthr;
public:
  baselock(int nthr = 1) : nthr(nthr) {}
  virtual ~baselock() {}
  virtual void lock(int tid ) = 0;
  virtual void unlock(int tidx) = 0;
};


class spinlock : public baselock {
private:
  __attribute__((__aligned__(64))) int lck;
public:
  spinlock() : baselock(1), lck(0) {}
  void lock(int tid = 0) override;
  void unlock(int tid = 0) override;
};

class ticketlock : public baselock {
private:
  __attribute__((__aligned__(64)))   uint64_t curr_serving;
  __attribute__((__aligned__(64)))  uint64_t counter;
public:
  ticketlock() : baselock(1), curr_serving(0), counter(0) {}
  void lock(int tid = 0) override;
  void unlock(int tid = 0) override;
};

class mcslock : public baselock {
private:
  struct node {
    volatile node *next;
    __attribute__((__aligned__(64))) int ready;
    node() : next(nullptr), ready(0) {}
  };
  node* head;
  node *nodes;
public:
  mcslock(int nthr) : baselock(nthr), head(nullptr) {
    nodes = new node[nthr];
  }
  ~mcslock() override {
    delete [] nodes;
  }
  void lock(int tid ) override;
  void unlock(int tid ) override;  
};

#endif
