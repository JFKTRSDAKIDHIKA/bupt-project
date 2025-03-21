#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>
static Context* (*user_handler)(Event, Context*) = NULL;

/*
static void print_context(struct Context *ctx) {
  printf("General Purpose Registers (GPR):\n");
  for (int i = 0; i < NR_REGS; i++) {
      printf("  x%d = 0x%x\n", i, ctx->gpr[i]);
  }

  printf("mcause: 0x%x\n", ctx->mcause);
  printf("mstatus: 0x%x\n", ctx->mstatus);
  printf("mepc: 0x%x\n", ctx->mepc);
  printf("pdir: %d\n", ctx->pdir);
}
*/

Context* __am_irq_handle(Context *c) {
  //print_context(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      // value in mcause is not standard 
      case 0x8: ev.event = EVENT_YIELD; break;
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
