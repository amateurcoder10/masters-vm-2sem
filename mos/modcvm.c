
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct PU {
	unsigned pc; 	// counter
	unsigned icode; // current instruction
	unsigned fr; 	// flags register
	unsigned ef; 	// error flag
	unsigned r[8];  // registers

	unsigned char *pmem; 		  // points to RAM
	unsigned pmemsize;		  // RAM size
	void (*op[32])(struct PU *pcore); // execution functions
};

enum        Ops         { SUS,   MOV ,  ADD ,  SUB ,  JIF ,  JMR,   MPC,   IN,   OUT};
const char *ops_str[] = {"SUS", "MOV", "ADD", "SUB", "JIF", "JMR", "MPC", "IN", "OUT"};

#define FLG_ZERO	0x1
#define FLG_CARRY	0x2

#define isflag(pu, f)		(((pu)->fr) & (f))
#define setflag(pu, e, f)	((e) ? ((pu)->fr |= (f)) : ((pu)->fr &= ~(f)))

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
	assert(!isnum(addr));
	addr /= 2;
	if (addr < 16)
		pcore->r[addr] = obj_read(pcore, exp);
	else
		pcore->pmem[addr] = obj_read(pcore, exp);
}

static unsigned getinp(struct PU *pcore, unsigned src)
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

static void op_SUS(struct PU *pcore)
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
	// JIF flag, addr - set PC to addr if flag is true
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
	unsigned addr = pcore->pmem[pcore->pc++];
	unsigned in = getinp(pcore, from);

	obj_write(pcore, addr, num_(in));
}

static void op_OUT(struct PU *pcore)
{
	// OUT port, addr - output data to port
	unsigned to = pcore->pmem[pcore->pc++];
	unsigned addr = pcore->pmem[pcore->pc++];

	putout(pcore, to, obj_read(pcore, addr));
}

struct PU Core1 = {
	.op[SUS] = op_SUS,
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
	IN,   reg_(0),
	MOV,  reg_(0),	  addr_(a_v),	// a = in[0] - '0'
	IN,   reg_(2),
	MOV,  reg_(2),	  addr_(b_v),	// b = in[0] - '0'
	
	CMP,  reg_(0),   reg_(2),//reg2-reg1;carry flag is set if reg2<reg1
	JNF,num_(2),   num_(22),
	MOV,  reg_(0),   addr_(c_v),	// reg0 is the gcd which is moved to a61
	OUT, reg_(0),
	SUS,		
	
	JNF,num_(1),num_(27) ,
	JMP,num_(32),	
	SUB,   reg_(2),reg_(0),
	JMP,    num_(10),

	SUB, reg_(0),reg_(2),
	JMP,	num_(10),
		
	
	// data stack
	[d_v] = 255,
	[c_v] = 255,
	[b_v] = 4,
	[a_v] = 3,
};
#define array_size(a)	(sizeof(a)/sizeof(a[0]))

extern void dump_state(struct PU *);

static void resume_core(struct PU *pcore)
{
	dump_state(pcore);
	do {
		pcore->icode = pcore->pmem[pcore->pc++];
		if (pcore->op[pcore->icode])
			(pcore->op[pcore->icode])(pcore);
		else
			break;
		dump_state(pcore);
	} while (pcore->icode != SUS);
}

struct imageheader {
	unsigned magic;
	unsigned size;		// size of image excluding header
	unsigned memsize;	// pool memory size
	unsigned pc;		// PC status saved here
};

int debug = 0;

#define MAGIC 	0xBAABDDBA

static void save_image(struct PU *pcore, char *filename)
{
	int f;
	struct imageheader h;
	int n, len;

	if (!filename) return;//nowhere to save

	printf("saving image into new file %s\n", filename);
	if ((f = open(filename, O_RDWR|O_CREAT|O_EXCL,S_IRWXU)) < 0) {
		perror(filename);
		return;
	}
	h.magic = MAGIC;
	h.pc = pcore->pc;
	h.memsize = pcore->pmemsize;
	h.size = sizeof(h)+h.memsize;
	n = write(f, &h, sizeof(h));
	for (len = h.memsize; len > 0; len -= n) {
		n = write(f, pcore->pmem, h.memsize);
		if (n <= 0) {
			perror("write");
			break;
		}
	}
	close(f);
}

static int load_image(struct PU *pcore, char *filename)
{
	int f;
	struct imageheader h;
	int n, len;

	if (!filename) {
		// use built-in bootstrap
		pcore->pc = 0;
		pcore->pmem = MEM;
		pcore->pmemsize = array_size(MEM);
		return 0;
	}
	printf("loading image from %s\n", filename);
	if ((f = open(filename, O_RDONLY)) < 0) {
		perror(filename);
		return -1;
	}
	if (read(f, &h, sizeof(h)) < sizeof(h)) {
		close(f);
		perror(filename);
		return -1;
	}
	if (h.magic != MAGIC) {
		printf("error: bad magic %x\n", h.magic);
		close(f);
		return -1;
	}
	assert(0 < h.memsize);
	if (h.memsize > 1024) {
		printf("error: memory too big %d\n", h.memsize);
		close(f);
		return -1;
	}
	if ((pcore->pmem = malloc(h.memsize)) == 0) {
		perror("no memory");
		close(f);
		return -1;
	}
	pcore->pmemsize = h.memsize;
	pcore->pc = h.pc;
	if (h.pc >= pcore->pmemsize) {
		printf("error: pc %d beyond memory %d\n", h.pc, h.memsize);
		close(f);
		return -1;
	}
	for (len = pcore->pmemsize; len > 0 ; len -= n) {
		n = read(f, pcore->pmem, len);
		if (n <= 0) {
			close(f);
			perror(filename);
			return -1;
		}
	}
	close(f);
	return 0;
}

int main(int argc, char *argv[])
{
	char *arg;
	char *loadfile = 0;
	char *savefile = 0;
	/* simple board with just one PU and one MEM */

	while (--argc > 0) {
		arg = *++argv;
		if (!strcmp(arg, "-s")) {//save image
			--argc;
			savefile = *++argv;
		}
		else if (!strcmp(arg, "-l")) {//load image
			--argc;
			loadfile = *++argv;
		}
		else if (!strcmp(arg, "-d")) {//dump state
			debug = 1;
		}
		else
			loadfile = arg;
	}

	printf("%c\n",*arg);
	if (load_image(&Core1, loadfile) < 0) {
		exit(1);
	}
	printf("resuming core from %d\n", Core1.pc);
	resume_core(&Core1);
	save_image(&Core1, savefile);
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
	printf(" %02hhx %02hhx %02hhx", pcore->pc, pcore->fr, pcore->icode);
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
