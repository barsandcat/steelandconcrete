//-< SUPPORT.H >-----------------------------------------------------*--------*
// SAL                       Version 1.0         (c) 1997  GARRET    *     ?  *
// (System Abstraction Layer)                                        *   /\|  *
//                                                                   *  /  \  *
//                          Created:      7-Jan-97    K.A. Knizhnik  * / [] \ *
//                          Last update: 21-Nov-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Miscellaneuse support classes and functions: buffers, memory allocators... 
//-------------------------------------------------------------------*--------*

#ifndef __SUPPORT_H__
#define __SUPPORT_H__

class l2elem {
  public:
    l2elem* next;
    l2elem* prev;

    void link_after(l2elem* after) {
	(next = after->next)->prev = this;
	(prev = after)->next = this;
    }
    void link_before(l2elem* before) {
	(prev = before->prev)->next = this;
	(next = before)->prev = this;
    }
    void unlink() {
        prev->next = next;
        next->prev = prev;
    }
    void prune() {
	next = prev = this;
    }
    boolean empty() const {
	return  next == this;
    };

    l2elem() {
	next = prev = this;
    }
};

//
// Attention ! All memory allocators below are not thread safe, so
// access to them should be synchronized by mutexes. 
//

#define INIT_DNM_BUFFER_SIZE 4096

class dnm_buffer { 
  protected:
    char*  ptr;
    size_t used;
    size_t allocated;
  public:
    char*  operator& () { return ptr; }
 
    char*  put(size_t size) { 
	if (size > allocated) { 
	    delete[] ptr;
	    ptr = new char[size];
	    allocated = size;
	}
	used = size;
	return ptr;
    }
    char*  append(size_t size) { 
	size_t cur_size = used;
	used += size; 
	if (used > allocated) { 
	    size_t new_size = (used > allocated*2) ? used : allocated*2; 
	    char* new_buf = new char[new_size];
	    memcpy(new_buf, ptr, cur_size);
	    delete[] ptr;
	    ptr = new_buf;
	    allocated = new_size;
 	}
	return &ptr[cur_size];
    } 

    size_t size() const { 
	return used;
    }

    size_t allocated_size() const { 
	return allocated;
    }

    dnm_buffer(size_t init_size = INIT_DNM_BUFFER_SIZE) { 
	used = 0;
	allocated = init_size;
	ptr = new char[init_size];
    }
    ~dnm_buffer() { 
	delete[] ptr;
    }
};

#define DEFAULT_DNM_POOL_QUANT 1024

class dnm_object_pool { 
  protected:
    struct pool_item { 
	pool_item* next;
	char       data[1];
    }; 
    struct pool_block { 
	pool_block* next; 
    }; 
    size_t      obj_size;
    size_t      obj_alignment;
    size_t      alloc_quantum;
    pool_block* block_chain;
    pool_item*  free_chain; 
    int         n_allocated_blocks;

  public:
    void*       alloc() { 
	pool_item* ip = free_chain; 
	if (ip == NULL) { 
	    pool_block* block = (pool_block*)new char[sizeof(pool_block) 
						     + alloc_quantum*obj_size 
						     + obj_alignment-1];
	    block->next = block_chain; 
	    block_chain = block;
	    pool_item* item = (pool_item*)
		DOALIGN(long(block)+sizeof(pool_block), obj_alignment);
	    for (int i = alloc_quantum; -- i >= 0;) {
		item->next = ip;
		ip = item;
		item = (pool_item*)((char*)item + obj_size);
	    }
	    n_allocated_blocks += 1;
	}
	free_chain = ip->next;
	return ip;
    }
    void        free(void* addr) { 
	pool_item* item = (pool_item*)addr; 
	item->next = free_chain;
	free_chain = item;
    }
    dnm_object_pool(size_t object_size, 
		    size_t allocation_quantum = DEFAULT_DNM_POOL_QUANT,
		    size_t object_alignment = 8) 
    { 
	free_chain = NULL;
	block_chain = NULL;
	obj_size = object_size;
	obj_alignment = object_alignment;
	alloc_quantum = allocation_quantum;
	n_allocated_blocks = 0;
    }
    void reset() { 
	pool_block* bp = block_chain;
	while (bp != NULL) {
	    pool_block* next = bp->next;
	    delete bp;
	    bp = next;
	}
	block_chain = NULL;
	free_chain = NULL;
	n_allocated_blocks = 0;
    }
	
    size_t allocated_size() const { 
	return n_allocated_blocks
	    * (sizeof(pool_block) + alloc_quantum*obj_size + obj_alignment-1);
    }

    ~dnm_object_pool() { reset(); }
};

#define SMALL_ARRAY_SIZE 64

template<class T>
class dnm_array { 
  protected:
    T*     ptr; 
    size_t allocated;
    size_t used;
    T      buf[SMALL_ARRAY_SIZE];

    inline void reallocate(size_t new_size) {
	if (new_size > allocated) {
	    size_t new_allocated = (allocated*2 > new_size) 
		? allocated*2 : new_size;
	    T* new_ptr = new T[new_allocated];
	    memcpy(new_ptr, ptr, used*sizeof(T));
	    if (ptr != buf) { 
		delete[] ptr;
	    }
	    ptr = new_ptr;
	    allocated = new_allocated;
	}
    }

  public:
    T* operator &() { return ptr; }

    size_t size() const { 
	return used; 
    }
    size_t allocated_size() const { 
	return allocated;
    } 
    void  change_size(size_t size) { 
	reallocate(size);
	used = size;
    }
    T& operator[] (int i) { 
	reallocate(i+1);
	if (size_t(i) >= used) { 
	    memset(&ptr[used], 0, (i + 1 - used)*sizeof(T));	    
	    used = i + 1;
	}
	return ptr[i];
    }
    void push(T val) { 
	reallocate(used+1);
	ptr[used++] = val;
    }
    T pop() { 
	assert(used > 0); 
	return ptr[--used]; 
    }
    boolean empty() { 
	return used == 0;
    }
    dnm_array() { 
	used = 0;
	allocated = items(buf);
	ptr = buf;
    }
    ~dnm_array() { 
	if (ptr != buf) { 
	    delete[] ptr;
	}
    }
};


#define DEFAULT_STACK_BLOCK_SIZE 4096

template<class T>
class dnm_stack { 
  protected:
    struct stack_block { 
	stack_block* next;
	stack_block* prev;

	stack_block(stack_block* chain) : next(NULL), prev(chain) {}
	void* operator new(size_t hdr_size, size_t body_size) { 
	    return new char[hdr_size + body_size];
	}
    };
    const size_t block_size;
    stack_block* chain;
    stack_block* top;
    size_t       sp;
    T*           bp;
    int          n_blocks;

  public: 
    void push(T val) { 
	if (sp == block_size) { 
	    n_blocks += 1;
	    if (top->next != NULL) { 
		top = top->next;
	    } else {
		top = top->next = new (block_size*sizeof(T)) stack_block(top);
	    }
	    sp = 0;
	    bp = (T*)(top+1);
	}	    
	bp[sp++] = val;
    }
    T pop() { 
	if (sp == 0) { 
	    assert (top->prev != NULL); 
	    n_blocks -= 1;
	    top = top->prev;
	    bp = (T*)(top+1);
	    sp = block_size;
	}
	return bp[--sp];
    }
    boolean is_empty() const { return sp == 0 && top == chain; }

    void make_empty() { 
	top = chain;
	bp = (T*)(top+1);
	sp = 0;
	n_blocks = 0;
    }
    size_t size() const { 
	return n_blocks*block_size + sp;
    }
    dnm_stack(size_t stack_block_size = 4096) : block_size(stack_block_size) {
	chain = new (stack_block_size*sizeof(T)) stack_block(NULL);
	make_empty();
    }
    ~dnm_stack() { 
	stack_block* bp = chain;
	while (bp != NULL) { 
	    stack_block* next = bp->next;
	    delete bp;
	    bp = next;
	}
    }
};


#define DEFAULT_QUEUE_BLOCK_SIZE 4096

template<class T>
class dnm_queue { 
  protected:
    struct queue_block { 
	queue_block* next;

	queue_block() { next = NULL; }
	void* operator new(size_t hdr_size, size_t body_size) { 
	    return new char[hdr_size + body_size];
	}
    };
    const int    block_size;
    queue_block* tail;
    queue_block* head;
    int          tail_pos;
    int          head_pos;
    int          n_items;

  public: 
    void put(T val) { 
	n_items += 1;
	if (head_pos == block_size) { 
	    if (head->next != NULL) { 
		head = head->next;
	    } else {
		head = head->next = new (block_size*sizeof(T)) queue_block;
	    }
	    head_pos = 0;
	}	    
	((T*)(head+1))[head_pos++] = val;
    }

    T get() { 
	n_items -= 1;
	if (tail_pos == block_size) { 
	    assert (tail->next != NULL); 
	    queue_block* next = tail->next;
	    tail->next = head->next;
	    head->next = tail;
	    tail = next;
	    tail_pos = 0;
	}
	return ((T*)(tail+1))[tail_pos++];
    }

    boolean is_empty() const { return n_items == 0; }

    void make_empty() { 
	head = tail;
	head_pos = tail_pos = 0;
	n_items = 0;
    }

    size_t size() const { return n_items; }

    dnm_queue(size_t queue_block_size = 4096) : block_size(queue_block_size) {
	tail = new (queue_block_size*sizeof(T)) queue_block;
	make_empty();
    }
    ~dnm_queue() { 
	queue_block* bp = tail;
	while (bp != NULL) { 
	    queue_block* next = bp->next;
	    delete bp;
	    bp = next;
	}
    }
};


inline unsigned string_hash_function(const char* name)
{ 
    unsigned h = 0, g;
    while(*name) { 
	h = (h << 4) + *name++;
	if ((g = h & 0xF0000000) != 0) { 
	    h ^= g >> 24;
	}
	h &= ~g;
    }
    return h;
}

#endif
