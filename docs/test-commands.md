**Math Operator**
(+ 5 4)
(- 10 4)
(* 5 3)
(/ 15 5)

**Set/Setq**
(set 'A (+ 2 6))
(set 'B '(+ 3 9))
(set 'X 4)
(set 'Y 6)
(A)
(B)
(X)
(Y)
(+ X Y)

**CAR CDDR CONS**
(car '(a b))
(cdr '(a b))
(set 'C '(4 5 6 7))
(car '(1 2 3 4))
(cdr '(1 2 3 4))
(car C)
(cdr C)
(car (cdr '(1 2 3 4)))
(car (cdr (cdr '(1 2 3 4))))

(cons 'A 'B)
(cons '(1 2) '(3 4))

**EVAL**
(set 'E (* 10 4))
(eval '(+ 5 4))
(eval 'E)
(eval (car '('(+ 1 2) '(+ 10 20))))

**DEF**
(def 'add-mult (defun '(x y) '(+ x (* x y))))
(add-mult 10 20)
(def 'add-mult-ten (add-mult 10))
(add-mult-ten 50)