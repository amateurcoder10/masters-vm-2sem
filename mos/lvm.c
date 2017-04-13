// Simple virtual machine for M-expressions
// compile with the following line to suppress warnings about casts
// gcc -g -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -o lvm lvm.c
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

enum obj_t {O_NUM, O_SYM, O_PAIR, O_PRIM, O_PROC};

typedef struct list_ {
	enum obj_t   type;
	struct list_ *slots[1]; // slots begin here
} list_s;

typedef list_s *list;
typedef list (*prim)(list);

list all_syms;	// global syms
list all_env;	// global environment

// predefined atoms
list nil;
list a_true;
list a_quote;
list a_if;
list a_lambda;
list a_define;
list a_set;

#define a_false		nil

static void error(char *s)
{
	printf("error: %s\n", s);
	exit(1);
}


static list mklist(enum obj_t type, int count, ...)
{
	list l;
	va_list ap;

	l = (list)calloc(sizeof(*l)+(count-1)*sizeof(l), 1);
	assert(l != 0);
	l->type = type;
	va_start(ap, count);
	for (int i =0; i < count; i++)
		l->slots[i] = va_arg(ap, list);
	va_end(ap);
	return l;
}

#define mknum(n)	mklist(O_NUM, 1, (list)(n))
#define	valof(l)	((int)(l)->slots[0])

#define mksym(s)	mklist(O_SYM, 1, (list)(s))
#define	symof(l)	((char *)(l)->slots[0])

#define mkpair(x,y)	mklist(O_PAIR, 2, (x), (y))
#define cons(x,y)	mkpair(x,y)
#define	car(x)		((x)->slots[0])
#define	cdr(x)		((x)->slots[1])
#define	setcar(l, x)	((l)->slots[0] = (x))
#define	setcdr(l, y)	((l)->slots[1] = (y))

#define mkproc(a, b, e)	mklist(O_PROC, 3, (a), (b), (e))
#define procpars(l)	((l)->slots[0])
#define proccode(l)	((l)->slots[1])
#define procenv(l)	((l)->slots[2])

#define mkprim(n)	mklist(O_PRIM, 1, (list)(n))
#define	primof(l)	((prim)(l)->slots[0])

#define isnum(l)	((l)->type == O_NUM)
#define issym(l)	((l)->type == O_SYM)
#define ispair(l)	((l)->type == O_PAIR)
#define isproc(l)	((l)->type == O_PROC)
#define isprim(l)	((l)->type == O_PRIM)
#define isnil(l)	((l) == nil)
#define sametype(x,y)	((x)->type == (y)->type)

#define notnil(l)	((l) != nil)
#define isatom(l)	(issym(l) || isnum(l))

static list lookupsym(char  *name)
{
	list l;

	for (list l = all_syms; !isnil(l); l = cdr(l)) {
		if (strcmp(name, symof(car(l))) == 0)
			return l;
	}
	return nil;
}

static list intern(char *name)
{
	list l = lookupsym(name);

	if (isnil(l)) {
		l = mksym(name);
		all_syms = cons(l, all_syms);
	}
	else
		l = car(l);
	return l;
}

#define caar(l)		car(car(l))
#define cadr(l)		car(cdr(l))
#define cdar(l)		cdr(car(l))
#define cddr(l)		cdr(cdr(l))
#define caddr(l)	car(cddr(l))

// Environment

#define extend(env, sym, val)	(cons(cons((sym), (val)), (env)))

static void writelist(list l);
static list readlist();
static void dump_(char *s, list l);

static list equal(list x, list y)
{
	if (!sametype(x,y)) return a_false;
	if (isnum(x)) return valof(x) == valof(y) ? a_true : a_false;
	if (issym(x)) return symof(x) == symof(y) ? a_true : a_false;
	return equal(car(x), car(y)) == a_true ? equal(cdr(x), cdr(y)) : a_false;
}

static list extend_env(list sym, list val)
{
	setcdr(all_env, cons(cons(sym, val), cdr(all_env)));
	return val;
}

static list append(list x, list y)
{
	if (isnil(x)) return y;
	return cons(car(x), append(cdr(x), y));
}

static list pairlis(list k, list v, list a)
{
	if (isnil(k)) return a;
	return cons(cons(car(k), car(v)), pairlis(cdr(k), cdr(v), a));
}

static list assoc(list k, list env)
{
	if (isnil(env)) return nil;
	return (equal(caar(env),k) == a_true) ? car(env) : assoc(k, cdr(env));
}

static list apply(list fn, list x, list a);
static list eval(list e, list a);
static list callproc(list fn, list env);
static list evlis(list m, list a);

static list apply(list fn, list args, list env)
{
	if (isprim(fn))
		return (*primof(fn))(args);
	if (isproc(fn))
		return callproc(proccode(fn), pairlis(procpars(fn), args, procenv(fn)));
	error("bad expression in apply");
	return fn;
}

static list eval(list exp, list env)
{
	list op, args;

	assert(exp != 0);
	if (isnum(exp)) return exp;
	if (issym(exp)) {
		list tmp = assoc(exp, env);
		if (tmp == nil) error("unknown symbol");
		return cdr(tmp);
	}
	op = car(exp);
	args = cdr(exp);
	if (ispair(exp)) {
		if (op == a_quote) return car(args);
		if (op == a_if) {
			return (eval(car(args), env) != nil)
				? eval(cadr(args),  env)
				: eval(caddr(args), env);
		}
		if (op == a_lambda) return mkproc(car(args), cdr(args), env);
		if (op == a_define) return extend_env(car(args), eval(cadr(args), env));
		if (op == a_set) {
			list name = assoc(car(args), env);
			list newval = eval(cadr(args), env);
			setcdr(name, newval);
			return newval;
		}
		return apply(eval(op, env), evlis(args, env), env);
	}
	return exp;
}

static list callproc(list code, list env)
{
	list val = nil;

	for(val = nil; !isnil(code); code = cdr(code)) {
		val = eval(car(code), env);
		if (cdr(code) == nil) return val;
	}
	return val;
}

static list evlis(list m, list env)
{
	if (isnil(m)) return nil;
	return cons(eval(car(m), env), evlis(cdr(m), env));
}

// Primitive procedures

static list prim_add(list args)
{
	int sum = valof(car(args));
	list p = cdr(args);

	while (!isnil(p)) {
		sum += valof(car(p));
		p = cdr(p);
	}
	return mknum(sum);
}

static list prim_sub(list args)
{
	int sum = valof(car(args));
	list p = cdr(args);

	while (!isnil(p)) {
		sum -= valof(car(p));
		p = cdr(p);
	}
	return mknum(sum);
}

static list prim_prod(list args)
{
	int prod = valof(car(args));
	list p = cdr(args);

	while (!isnil(p)) {
		prod *= valof(car(p));
		p = cdr(p);
	}
	return mknum(prod);
}

static list prim_div(list args)
{
	int div = valof(car(args));
	list p = cdr(args);

	while (!isnil(p)) {
		div /= valof(car(p));
		p = cdr(p);
	}
	return mknum(div);
}

static list prim_eq(list args)   { return valof(car(args)) == valof(cadr(args)) ? a_true : nil; }
static list prim_gt(list args)   { return valof(car(args)) > valof(cadr(args)) ? a_true : nil; }
static list prim_lt(list args)   { return valof(car(args)) < valof(cadr(args)) ? a_true : nil; }
static list prim_cons(list args) { return cons(car(args), cadr(args)); }
static list prim_car(list args)  { return caar(args); }
static list prim_cdr(list args)  { return cdar(args); }
static list prim_atom(list args) { return isatom(car(args)) ? a_true : nil; }


#define dumplist(l)	dump_(#l, l)

static void init_machine()
{
	nil = mksym("nil");
	all_syms = cons(nil, nil);
	all_env = cons(cons(nil, nil), nil);

	a_true   = intern("t");
	extend_env(a_true, a_true);

	a_quote  = intern("quote");
	a_lambda = intern("lambda");
	a_if     = intern("if");
	a_define = intern("define");
	a_set    = intern("setq");

	extend_env(intern("+"), mkprim(prim_add));
	extend_env(intern("-"), mkprim(prim_sub));
	extend_env(intern("*"), mkprim(prim_prod));
	extend_env(intern("/"), mkprim(prim_div));
	extend_env(intern("="), mkprim(prim_eq));
	extend_env(intern("<"), mkprim(prim_lt));
	extend_env(intern(">"), mkprim(prim_gt));
	extend_env(intern("cons"), mkprim(prim_cons));
	extend_env(intern("car"), mkprim(prim_car));
	extend_env(intern("cdr"), mkprim(prim_cdr));
	extend_env(intern("atom"), mkprim(prim_atom));
}

// input/output 

#define MAXLEN	32	// max token length
static char word[MAXLEN];
static int cursor;
static char *token_old;

static char delims[] = "()\'";
static char *err_incomplete = "incomplete expression";

static void append_word(char c) { if (cursor < MAXLEN - 1) word[cursor++] = c; }
static void ungettoken(char *s) { token_old = s; }
static char *maketoken() { word[cursor++] = 0; return strdup(word);}

static char *gettoken()
{
	char c;
	char *t;

	if (token_old) {
		t = token_old; token_old = 0;
		return t;
	}
	cursor = 0;
	do {
		if ((c = getchar()) == EOF) exit(0);
	} while (isspace(c));

	append_word(c);
	if (strchr(delims, c)) return maketoken();
	for (;;) {
		if ((c = getchar()) == EOF) error(err_incomplete);
		if (strchr(delims, c) || isspace(c)) {
			ungetc(c, stdin);
			return maketoken();
		}
		append_word(c);
	}
}

static list readexp()
{
	char *token = gettoken();

	if (strcmp("(", token) == 0) return readlist();
	if (strcmp("\'", token) == 0) return cons(a_quote, cons(readexp(), nil));
	if (token[strspn(token, "0123456789")] == 0) return mknum(atoi(token));
	return intern(token);
}

static list readlist()
{
	char *token = gettoken();
	list tmp;

	if (strcmp(")", token) == 0) return nil;
	if (strcmp(".", token) == 0) {
		tmp = readexp();
		if (strcmp(gettoken(), ")") != 0) error("unbalanced parentheses");
		return tmp;
	}
	// must be a symbol
	ungettoken(token);
	tmp = readexp();
	return cons(tmp, readlist());
}

static void writelist(list x)
{
	list p = x;

	switch(p->type) {
		case O_NUM: printf("%d", valof(p)); break;
		case O_SYM: printf("%s", isnil(p)? "nil" : symof(p)); break;
		case O_PAIR:
			printf("(");
			for (;;) {
				writelist(car(p));
				if (isnil(cdr(p))) {
					// like (A)
					printf(")");
					break;
				}
				p = cdr(p);
				assert(p != 0);
				if (p->type != O_PAIR) {
					// like (A . B)
					printf(" . ");
					writelist(p);
					printf(")");
					break;
				}
				// like (A B C ...)
				printf(" ");
			}
			break;
		case O_PROC:
			printf("(#PROC ");
			writelist(procpars(p));
			printf(" ");
			writelist(proccode(p));
			printf(")");
			break;
		case O_PRIM: printf("#PRIM"); break;
		default: printf("?"); break;
	}
}

static void dump_(char *s, list l)
{
	if (s) printf("%s ", s);
	writelist(l);
	putchar('\n');
}

int main(int argc, char *argv[])
{
	list in;

	init_machine();
	dumplist(all_env);
	for(;;) {
		printf("> ");
		fflush(stdout);
		in = readexp();
		dump_(0, in);
		in = eval(in, all_env);
		dump_(0, in);
	}
	return 0;
}
