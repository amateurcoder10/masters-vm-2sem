'basictests
0
234
t
nil
'logictests
t
(= t t)
(= 3 3)
(= (= 3 4) nil)
(atom nil)
(atom t)
(atom 23)
'arithtests
(= 3 3)
(= (+ 3 4) 7)
(= (+ 10 20 30) 60)
(= (- 100 10 20) 70)
(= (* 1 2 3 4) 24)
(= (/ 100 20) 5)
'symtests
(= 'hello 'hello)
(= 'hello (quote hello))
'definetests
(define equal (lambda (x y)
	(if (atom x)
		(if (atom y)
			(= x y)
			nil)
		(if (equal (car x) (car y))
			(equal (cdr x) (cdr y))
			nil))))
'listtests
(define x '(1 2 3))
(define y '(2 3 3))
(= (car '(1 2 3)) 1)
(equal (cdr '(1 2 3)) '(2 3))
(= (car (cons 2 3)) 2)
(= (cdr (cons 2 3)) 3)
(equal (cdr '(1 2 3 4)) '(2 3 4))
(= x x)
(= (= x y) nil)
(= (= '(1 2) '(1 2)) nil)
(equal '(1 2) '(1 2))
'lambdatests
(define add (lambda (x y) (+ x y)))
(= (add 23 34) 57)
(car add)
(car (cdr add))
'complextests
(define max (lambda (x y) (if (> x y) x y)))
(= (max 2 3) 3)
(= (max 5 3) 5)
'testequal
(equal '(1 2 3) '(1 2 3))
(= (equal '(1 2) '(1 2 3 4)) nil)
(= (equal '(1 2) nil) nil)
(= (equal '(1 2 3 4) '((1 2) 3 4)) nil)
'testdefine
(define ntimes (lambda (mult) (lambda (n) (* n mult))))
(define dozen (ntimes 12))
(= (dozen 2) 24)
(define score (ntimes 20))
(= (score 4) 80)
(define kibi (ntimes 1024))
(= (kibi 4) 4096)
(define lessby (lambda (decr) (lambda (n) (- n decr))))
(define less10 (lessby 10))
(define less2 (lessby 2))
(= (less10 23) 13)
(= (less2 29) 27)
(define member (lambda (a x)
	(if (= x nil)
		nil
		(if (equal a (car x)) t (member a (cdr x))))))
'testmember
(member 3 '(1 2 3 4 5))
(= (member 10 '(1 2 3 4 5)) nil)
(define union (lambda (x y)
	(if (= x nil)
		y
		(if (member (car x) y)
			(union (cdr x) y)
			(cons (car x) (union (cdr x) y))))))
'testunion
(define intersection (lambda (x y)
	(if (= x nil)
		y
		(if (member (car x) y)
			(cons (car x) (intersection (cdr x) y))
			(union (cdr x) y)))))
(union '(1 2 3 4) '(3 4 5 6))
(equal (union '(1 2 3 4) '(3 4 5 6)) '(1 2 3 4 5 6))
(union '(dog pig cow) '(cow horse donkey))
(equal (union '(dog pig cow) '(cow horse donkey)) '(dog pig cow horse donkey))
'tests-complete
