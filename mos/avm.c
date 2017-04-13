/* A Simple Virtual Machine */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct PU {
	unsigned pc;			// program counter
	unsigned icode;			// current instruction
	unsigned zf;			// zero flag 
	unsigned cf;			// carry flag
	unsigned ef;			// error flag
	unsigned r[4];			// registers
	unsigned char *pmem;  		 // points to RAM
	void (*op[32])(struct PU *pcore); // computing functions:array of function pointers;points to a function that takes ptr to structure as argument and returns nothing.each pointer points to a speciifc function
};

enum        Ops         { HLT,   MOV ,  ADD ,  SUB ,  JNZ ,  JMR,   MPC};
const char *ops_str[] = {"HLT", "MOV", "ADD", "SUB", "JNZ", "JMR", "MPC"};

// encode numbers and memory addresses into 8-bit
#define num_(n)		((n)*2 + 1)    //number implies odd lsb
#define addr_(p)	((p)*2)		//address or register implies even lsb
#define reg_(p)		addr_(p)	

static unsigned obj_read(struct PU *pcore, unsigned addr)//read from a memory address
{
	if (addr & 1) return addr >> 1; //immediate data;leave out the lsb and return
	addr /= 2; //else this is a memory address or register
	return addr < 16 ? pcore->r[addr] : pcore->pmem[addr]; //less than 16 implies a register:return register value of that index;else memory corresponding to that address
}

static void obj_set(struct PU *pcore, unsigned addr, unsigned exp)//set a particular address 
{
	assert(!(addr & 1));// if assert evaluates to true then it does nothing;else displays an error code on screen.This is to ensure that variable addr is not immediate data.
	addr /= 2;
	if (addr < 16)//register
		pcore->r[addr] = obj_read(pcore, exp);//set the value of that register to exp
	else
		pcore->pmem[addr] = obj_read(pcore, exp);//set memory to exp
}

static void op_HLT(struct PU *pcore)//halt--does nothing
{
}

static void op_MOV(struct PU *pcore)//usual l=r assignment--mov expr to addr
{
	// MOV expr addr
	unsigned expr = pcore->pmem[pcore->pc++];//read next byte which is operand 1
	unsigned addr = pcore->pmem[pcore->pc++];//operand 2

	obj_set(pcore, addr, expr);//set operand 2 to operand 1
}

static void op_ADD(struct PU *pcore)//addr=addr+exp
{
	// ADD expr addr
	unsigned expr = pcore->pmem[pcore->pc++];//operand 1
	unsigned addr = pcore->pmem[pcore->pc++];//operand 2
	unsigned old, new;//intermediate variables

	old = obj_read(pcore, addr);//store operand 1 here
	new = old + obj_read(pcore, expr);//operand2+operand1
	pcore->cf = (new < old);//set carry flag if there is an overflow
	pcore->zf = (new == 0);//set zero flag if sum is zero
	obj_set(pcore, addr, num_(new));//set the contents of address to sum;since sum is immediate data use num_ to indicate so
}

static void op_SUB(struct PU *pcore)//addr=addr-exp
{
	// SUB expr addr
	unsigned expr = pcore->pmem[pcore->pc++];//operand 1
	unsigned addr = pcore->pmem[pcore->pc++];//operand 2
	unsigned old, new;//intermediate variables

	old = obj_read(pcore, addr);//store operand 1 here
	new = old - obj_read(pcore, expr);//operand2+operand1
	pcore->cf = (new > old);//set carry flag if there is a borrow
	pcore->zf = (new == 0);//set zero flag if diff is zero
	obj_set(pcore, addr, num_(new));//set the contents of address to diff;since diff is immediate data use num_ to indicate so
}

static void op_JNZ(struct PU *pcore)//conditional jump--only if not zero
{
	// JNZ addr - set PC to given value
	unsigned addr = pcore->pmem[pcore->pc++];//address to be jumped to

	if (!pcore->zf) //check for zero flag cleared;if so can jump
	{
		pcore->pc = obj_read(pcore, addr);//read addr into program counter
	}
}

static void op_JMR(struct PU *pcore)//unconditional jump
{
	// JMR reg - jump to address in a register
	unsigned addr = pcore->pmem[pcore->pc++];//read the destination address

	pcore->pc = obj_read(pcore, addr);//set pc to this value
}

static void op_MPC(struct PU *pcore)
{
	// MPC reg - save pc contents in a register
	unsigned addr = pcore->pmem[pcore->pc++];//destination register

	obj_set(pcore, addr, num_(pcore->pc));//set the address to the value of pc which is immediate
}

static struct PU Core1 = {//intilisation of the array of function pointers;since it is a staic struct dot operator is used without any object reference
	.op[HLT] = op_HLT,
	.op[MOV] = op_MOV,
	.op[ADD] = op_ADD,
	.op[SUB] = op_SUB,
	.op[JNZ] = op_JNZ,
	.op[JMR] = op_JMR,
	.op[MPC] = op_MPC,
};

static unsigned char ROM[64] = {
	// instructions section
		
	MOV,  addr_(63), reg_(0),	// m61 = m63 + m62
	MOV,  addr_(62), reg_(2),
	ADD,  reg_(2),   reg_(0),
	MOV,  reg_(0),   addr_(61),
	MOV,  num_(0),   reg_(0),	// m60 = m63 * m62
	MOV,  addr_(63), reg_(1),
	MPC,  reg_(3),			// save PC in reg3
	ADD,  reg_(1),   reg_(0),
	SUB,  num_(1),   reg_(2),
	JNZ,  reg_(3),			// jump if 0 to addr in reg3
	MOV,  reg_(0),   addr_(60),
	HLT,

	// data stack
	[60] = 255, /* a*b */
	[61] = 255, /* a+b */
	[62] = 4,  /* b */
	[63] = 3,  /* a */
};

#define array_size(a)	(sizeof(a)/sizeof(a[0]))

static void dump_state(struct PU *pcore)
{
	printf("\033[H;\033[2J");//ansi control ascii codes--h:reset cursor to the home position:top left;j-clear screen
	printf("pc  zf  cf  ic");
	for (int i = 0; i < array_size(pcore->r); i++) printf("  r%d", i);//print indices of registers
	putchar('\n');
	printf("%3d %3d %3d %3d ", pcore->pc, pcore->zf, pcore->cf, pcore->icode);//print flags,pc and instruction code
	for (int i = 0; i < array_size(pcore->r); i++) printf("%3d ", pcore->r[i]);//print contents of registers
	printf("\n---------------- MEM ----------------\n");
	for(int i = 0; i < array_size(ROM);) {
		printf("%3d: ", i); //print memory indices
		for (int col = 0; col < 8; col++, i++) {
			printf("%3d%c", pcore->pmem[i], i == pcore->pc ? '<': ' ');//pc points to this character if <
		}
		putchar('\n');
	}
	printf("Press enter to continue...\n"); getchar();
}

static void start_core(struct PU *pcore, unsigned char pmem[])
{
	pcore->pc = 0;
	pcore->pmem = pmem;
	dump_state(pcore);
	do {
		pcore->icode = pmem[pcore->pc++];//instructions are enumerated so icode becomes a number
		if (pcore->op[pcore->icode])
			(pcore->op[pcore->icode])(pcore);//call the function pointed to by the fptr
		else
			break;
		dump_state(pcore);
	} while (pcore->icode != HLT);//till instr is to halt
}

int main(int argc, char *argv[])
{
	start_core(&Core1, ROM);
	return 0;
}
