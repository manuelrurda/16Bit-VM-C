#include "manuelvm.h"

void _mov(VM *vm, Args arg1, Args arg2)
{
  printf("MOV\n");
  vm->$ax = (Reg)arg1;
  return;
}

int8 size_inst(Opcode opc)
{
  int16 n;
  int8 size;
  IM *map;

  for (n = IMs, map = inst_sizemap; n > 0; n--, map++)
  {
    if (map->opc == opc)
    {
      size = map->size;
      break;
    }
  }
  return size;
}

void error(VM *vm, Errorcode e)
{
  if (vm)
    free(vm);

  switch (e)
  {
  case ErrSegf:
    fprintf(stderr, "%s\n", "VM Segmentation fault");
    break;
  case SysHlt:
    fprintf(stderr, "%s\n", "VM System halted");
  default:
    break;
  }

  exit(-1);
}
void runinst(VM *vm, Opcode opc, Args arg1, Args arg2)
{
  switch (opc)
  {
  case mov:
    _mov(vm, arg1, arg2);
    break;

  case nop:
    break;

  case hlt:
    error(vm, SysHlt);
    break;
  default:
    error(vm, ErrOp);
    break;
  }
}

void executeprogram(VM *vm)
{
  assert(vm);
  Instruction *ip = (Instruction *)vm->$ip;

  printf("----- EXECUTE -----\n");
  while ((ip->opc != (Opcode)hlt) && ((int16 *)ip <= vm->brk))
  {
    // printhex((int8 *)ip, 1);
    Args arg1, arg2;
    Opcode opc = ip->opc;
    int8 size = size_inst(ip->opc);
    ip = (Instruction *)((int8 *)ip + 1); // same as (int8 *)ip++ without warning
    switch (size)
    {
    case 1:
      break;
    case 2:
      arg1 = *((int8 *)ip++);
      break;
    case 3:
      arg1 = *((int8 *)ip++);
      arg2 = *((int8 *)ip++);
      break;
    default:
      errno = ErrArg;
      segfault(vm);
      break;
    }
    runinst(vm, opc, arg1, arg2);
  }
  if ((int16 *)ip > vm->brk)
  {
    segfault(vm);
  }
}

void loadprogram(VM *vm, ProgramPointer pp)
{
  assert(vm && pp.prog);
  int8 *sp = (int8 *)(vm->$sp);
  int8 *prog = (int8 *)(pp.prog);

  for (int8 n = pp.size; n > 0; n--, sp++, prog++)
  {
    *sp = *prog;
  }
  vm->$sp = vm->brk = (int16 *)sp;
}

VM *virtualmachine()
{
  size_t size_vm = sizeof(VM);
  VM *vm = (VM *)malloc(size_vm);
  if (!vm)
  {
    errno = ErrMem;
    return NULL;
  }

  memset(vm, 0, size_vm);
  vm->$ip = &(vm->memory[0]);
  vm->$sp = &(vm->memory[0]);
  vm->brk = &(vm->memory[0]);
  return vm;
}

void printvm(VM *vm)
{
  printhex(vm, sizeof(VM));
}

ProgramPointer exampleprogram(VM *vm)
{
  ProgramPointer pp;
  Program *p;
  int16 size_prog = 0;
  int16 s1, s2, s3;
  int16 offset = 0;
  int8 *prog_p;

  s1 = size_inst(mov);
  s2 = size_inst(nop);
  s3 = size_inst(hlt);

  size_prog = s1 + s2 + s3;
  p = (Program *)malloc(size_prog);
  if (!p)
  {
    errno = ErrMem;
    exit(EXIT_FAILURE);
  }

  prog_p = (int8 *)p;

  Instruction *i = (Instruction *)(prog_p);
  i->opc = mov;
  i->args[0] = 0xe1;
  i->args[1] = 0x05;
  offset += s1;

  i = (Instruction *)(prog_p + offset);
  i->opc = nop;
  offset += s2;

  i = (Instruction *)(prog_p + offset);
  i->opc = hlt;
  offset += s3;

  pp.prog = p;
  pp.size = size_prog;
  return pp;
}

int main(int argc, char const *argv[])
{
  ProgramPointer pp;
  VM *vm;

  vm = virtualmachine();
  printf("------ VM INIT ------\n");
  printf("vm = %p\n", vm);
  printf("ax = %.02x\n", vm->$ax);
  printf("bx = %.02x\n", vm->$bx);
  printf("ip = %p\n", vm->$ip);
  printf("sp = %p\n", vm->$sp);

  pp = exampleprogram(vm);
  printhex(pp.prog, pp.size);
  loadprogram(vm, pp);
  free(pp.prog);

  printvm(vm);
  executeprogram(vm);
  printvm(vm);

  free(vm);
  return 0;
}
