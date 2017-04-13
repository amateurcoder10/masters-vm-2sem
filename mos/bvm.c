/* Virtual Machine with I/O */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

struct PU {
	unsigned pc; 	// counter
	unsigned icode; // current instruction
	unsigned fr; 	// flags register
	unsigned ef; 	// error flag
	unsigned r[8];  // registers

	unsigned char *pmem; 		  // points to RAM
	void (*op[32])(struct PU *pcore); // execution functions
};

enum        Ops         { HLT,   MOV ,  ADD ,  SUB ,  JIF ,  JMR,   MPC,   IN,   OUT};
const char *ops_str[] = {"HLT", "MOV", "ADD", "SUB", "JIF", "JMR", "MPC", "IN", "OUT"};

#define FLG_ZERO	0x1
#define FLG_CARRY	0x2

#define isflag(pu, f)		(((pu)->fr) & (f))  //check if a flag is set
#define setflag(pu, e, f)	((e) ? ((pu)->fr |= (f)) : ((pu)->fr &= ~(f)))//check if the error flag is set,if yes bitwise or to set either carry or zero flags;alt is to clear

#define setzf(pu, e)	setflag(pu, e, FLG_ZERO)
#define setcf(pu, e)	setflag(pu, e, FLG_CARRY)

#define num_(n)		((n)*2 + 1)
#define addr_(p)	((p)*2)
#define reg_(p)		addr_(p)
#define tonum(e)	((e)>>1)
#define isnum(e)	((e)&1)

static unsigned obj_read(struct PU *pcore, unsigned addr)
{
	if (isnum(addr))
		return tonum(addr);
	addr /= 2;
	return addr < 16 ? pcore->r[addr] : pcore->pmem[addr];
}

static void obj_write(struct PU *pcore, unsigned addr, unsigned exp)
{
	assert(!isnum(addr));//to ensure that it is an address
	addr /= 2;
	if (addr < 16)
		pcore->r[addr] = obj_read(pcore, exp);
	else
		pcore->pmem[addr] = obj_read(pcore, exp);
}

static unsigned getinp(struct PU *pcore, unsigned src)//returns whatever is entered on screen
                                         /*NOTUSED*/
{
	int n;

	putchar('?'); // added for clarity
	scanf("%d", &n);
	return n;
}

static void putout(struct PU *pcore, unsigned dst, unsigned n)
                                      /*NOTUSED*/
{
	printf(">%d\n", n);
}

static void op_HLT(struct PU *pcore)
{
}

static void op_MOV(struct PU *pcore)
{
	// MOV expr addr
	unsigned expr = pcore->pmem[pcore->pc++];
	unsigned addr = pcore->pmem[pcore->pc++];

	obj_write(pcore, addr, expr);
}

static void op_ADD(struct PU *pcore)
{
	// ADD expr addr
	unsigned expr = pcore->pmem[pcore->pc++];
	unsigned addr = pcore->pmem[pcore->pc++];
	unsigned old, new;

	old = obj_read(pcore, addr);
	new = old + obj_read(pcore, expr);
	setcf(pcore, (new < old));
	setzf(pcore, (new == 0));
	obj_write(pcore, addr, num_(new));
}

static void op_SUB(struct PU *pcore)
{
	// SUB expr addr
	unsigned expr = pcore->pmem[pcore->pc++];
	unsigned addr = pcore->pmem[pcore->pc++];
	unsigned old, new;

	old = obj_read(pcore, addr);
	new = old - obj_read(pcore, expr);
	setcf(pcore, (new > old));
	setzf(pcore, (new == 0));
	obj_write(pcore, addr, num_(new));
}

static void op_JIF(struct PU *pcore)
{
	// JIF flag, addr - set PC to addr if flag is not true
	unsigned flag = pcore->pmem[pcore->pc++];
	unsigned addr = pcore->pmem[pcore->pc++];

	if (!isflag(pcore, flag))
		pcore->pc = obj_read(pcore, addr);
}

static void op_JMR(struct PU *pcore)
{
	// JMR reg - jump to address in a register
	unsigned addr = pcore->pmem[pcore->pc++];

	pcore->pc = obj_read(pcore, addr);
}

static void op_MPC(struct PU *pcore)
{
	// MPC reg - save pc contents in a register
	unsigned addr = pcore->pmem[pcore->pc++];

	obj_write(pcore, addr, num_(pcore->pc));
}

static void op_IN(struct PU *pcore)
{
	// IN port, addr - input data from port into register
	unsigned from = pcore->pmem[pcore->pc++];
	unsigned addr = pcore->pmem[pcore->pc++];//register
	unsigned in = getinp(pcore, from);

	obj_write(pcore, addr, num_(in));
}

static void op_OUT(struct PU *pcore)
{
	// OUT port, addr - output data to port
	unsigned to = pcore->pmem[pcore->pc++];//irrelevant
	unsigned addr = pcore->pmem[pcore->pc++];//register

	putout(pcore, to, obj_read(pcore, addr));
}

struct PU Core1 = {
	.op[HLT] = op_HLT,
	.op[MOV] = op_MOV,
	.op[ADD] = op_ADD,
	.op[SUB] = op_SUB,
	.op[JIF] = op_JIF,
	.op[JMR] = op_JMR,
	.op[MPC] = op_MPC,
	.op[IN]  = op_IN,
	.op[OUT] = op_OUT,
};

#define MEM_SIZE	128

#define a_v	(MEM_SIZE-1)
#define b_v	(MEM_SIZE-2)
#define c_v	(MEM_SIZE-3)
#define d_v	(MEM_SIZE-4)

unsigned char MEM[MEM_SIZE] = {
	// instructions section
	IN,   num_(0),    reg_(0),
	MOV,  reg_(0),	  addr_(a_v),	// a = in[0] - '0'
	IN,   num_(0),    reg_(2),
	MOV,  reg_(2),	  addr_(b_v),	// b = in[0] - '0'
	ADD,  reg_(2),   reg_(0),
	MOV,  reg_(0),   addr_(c_v),	// c = a + b
	OUT,  num_(1),  reg_(0),	// out[1] = c

	MOV,  num_(0),   reg_(0),
	MOV,  addr_(a_v), reg_(1),
	MPC,  reg_(3),			// save PC in reg3
	ADD,  reg_(1),   reg_(0),
	SUB,  num_(1),   reg_(2),
	JIF,  num_(1),   reg_(3),	// if zf resume from reg3
	MOV,  reg_(0),   addr_(d_v),	// d = a * b
	OUT,  num_(1),  reg_(0),	// out[1] = d
	HLT,

	// data stack
	[d_v] = 255,
	[c_v] = 255,
	[b_v] = 4,
	[a_v] = 3,
};

#define array_size(a)	(sizeof(a)/sizeof(a[0]))

extern void dump_state(struct PU *);

static void start_core(struct PU *pcore, unsigned char pmem[])
{
	pcore->pc = 0;
	pcore->pmem = pmem;
	dump_state(pcore);
	do {
		pcore->icode = pmem[pcore->pc++];
		if (pcore->op[pcore->icode])
			(pcore->op[pcore->icode])(pcore);
		else
			break;
		dump_state(pcore);
	} while (pcore->icode != HLT);
}

int debug = 0;
int main(int argc, char *argv[])
{
	char *arg;
	/* simple board with just one PU and one MEM */

	while (--argc > 0) {
		arg = *++argv;
		if (strcmp(arg, "-d") == 0) {
			debug=1;//flag to activate dump state
		}
	}
	start_core(&Core1, MEM);
	return 0;
}

void dump_state(struct PU *pcore)
{
	int i;

	if (debug == 0) return;

	//printf("\033[2J\033[H");	// clear screen, home cursor
	printf(" pc fr ic");
	for (i = 0; i < array_size(pcore->r); i++) printf(" r%d", i);
	putchar('\n');
	printf(" %02hhx %02hhx %02hhx", pcore->pc, pcore->fr, pcore->icode);//printing hex values %x
	for (i = 0; i < array_size(pcore->r); i++) printf(" %02hhx", pcore->r[i]);
	printf("\n------------------------- MEM ------------------------\n");
	for(i = 0; i < array_size(MEM);) {
		printf("%04x  ", i);
		for (int col = 0; col < 16; col++, i++) {
			if (col == 8) putchar(' ');
			printf("%02hhx%c", pcore->pmem[i], i == pcore->pc ? '<': ' ');
		}
		putchar('\n');
	}
	printf("Press enter to continue...\n"); getchar();
}
