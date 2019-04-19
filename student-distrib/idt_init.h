//IDT_INIT HEADER FILE

#define PIT_VAL 0x20
#define SYSCALL_VAL 0x80

void idt_init();
void divide_error();
void debug();
void nmi();
void breakpoint();
void overflow();
void bound_range();
void invalid_op();
void device_na();
void double_fault();
void seg_overrun();
void invalid_tss();
void seg_np();
void seg_fault();
void gen_prot();
void page_fault();
void blank();
void fpe();
void align();
void machine();
void simd();
void general();
