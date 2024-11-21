#include "utils.h"

#define NoArg 0x00

/* Error codes */
#define NoErr 0x00
#define ErrArg 0x01
#define ErrMem 0x02
#define ErrSegf 0x03
#define ErrOp 0x04
#define SysHlt 0xff

/* CPU register aliases */
#define $ax cpu.regs.ax
#define $bx cpu.regs.bx
#define $cx cpu.regs.cx
#define $dx cpu.regs.dx
#define $ip cpu.regs.ip
#define $sp cpu.regs.sp

/* Other defines */
#define IMs (sizeof(inst_sizemap) / sizeof(IM))
#define segfault(x) error((x), ErrSegf)

/*
  16-bit VM
  General Purpose Registers:
  AX, BX, CX, DX
  SP - Stack Pointer
  IP - Instruction Pointer or Program Counter
*/

/* Type definitions */
typedef int8 Args;
typedef int16 Reg;
typedef int8 Errorcode;
// typedef int8 Memory[(int16)(-1)];
typedef int16 Memory[40];
typedef int8 Opcode;
typedef int16 Program;

/* Enumerations */
enum
{
    hlt = 0xFF,
    mov = 0x01,
    nop = 0x02
};

/* Structures */
typedef struct s_registers
{
    Reg ax;
    Reg bx;
    Reg cx;
    Reg dx;
    Reg *ip;
    Reg *sp;
} Registers;

typedef struct s_cpu
{
    Registers regs;
} CPU;

typedef struct s_instruction
{
    Opcode opc;
    Args args[];
} Instruction;

typedef struct s_instrmap
{
    Opcode opc;
    int8 size;
} IM;

typedef struct s_programpointer
{
    Program *prog;
    int8 size;
} ProgramPointer;

typedef struct s_vm
{
    CPU cpu;
    int16 *brk; /* Break line for program and data */
    Memory memory;
    // Program *prog;
} VM;

/* Instruction mapping */
static IM inst_sizemap[] = {
    {hlt, 0x01},
    {mov, 0x03},
    {nop, 0x01}};

/* Function declarations */

/* Functions returning int */
int main(int, const char **);

/* Functions returning int8 */
int8 inst_size(Opcode);

/* Functions returning VM* */
VM *virtualmachine();

/* Functions returning void */
void _mov(VM *, Args, Args);
void error(VM *, Errorcode);
void executeprogram(VM *);
void loadprogram(VM *, ProgramPointer);
void printvm(VM *);
void runinst(VM *, Opcode, Args, Args);

ProgramPointer exampleprogram(VM *vm);