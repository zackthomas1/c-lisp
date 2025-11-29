;; A simple LISP interpreter written by Dr Klefstad for ICS 141 at UC Irvine.
;; I deleted key portions of it to allow you to learn more about evaluation.

;; I inserted NIL in places where I deleted code to allow them to load and run,
;; but nil is the wrong value to return. You must think about what to do instead.

;; Note the new top-level function, my-top, is defined at the bottom of this file
;; defines a new REPL read-eval-print loop that you can use to test your interpreter.

;; To run this, load this file into the lisp interpreter, then call
;; (my-top)
;; then type in expressions to be evaluated.

;; The homework assignment gives a list of good test cases which are
;; listed in order that you should probably implement thim in to ensure you
;; enough working to go on to the next test case.

;; We will use association lists, alists, for the stack of variables and
;; their values to implement dynamic scoping.  An alist is a list of this form:
;; ((var1 . val1) (var2 . val2) ... (varN . valN))
;; where each vari is a symbol representing a variable (or parameter) name
;; and each vali is the value of the variable.  assoc returns the association
;; of a given symbol, e.g,
;; (assoc 'myvar '((a . 10)(b a b c)(myvar d e f)))
;; returns (myvar d e f) and you take the cdr of that to get myvar's value
;; (d e f)

;; my-assoc returns the association (binding) of a variable in the association
;; list. An association list may contain multiple definitions of a variable
;; with the same name, for example with parameters to a recursive function.
;; Assoc always finds the first association of a variable, and this is how we
;; implement dynamic scoping. 

;; As evaluation proceeds deeper into recursion, new variables are added onto
;; the front of the current association list.  New defintions of a variable 
;; will hide previously made definitions effectively hiding them from access.
;; The previously made definitions will come back into scope when
;; recursive evaluation unwinds.

;; We us a special global variable, called global-alist, for saving top-level
;; definitions made using defun or setq. Note the global-alist is passed in
;; to my-eval only in the call made by my-top defined below.


;; You need to write this one.
(defun my-assoc (v alist)
    (COND
        ;; Base case
        ((null alist)
            nil
        )

        ;;  Recusive step
        ;; the first ATOM of the first key/value pair matches v
        ((eq v (car (car alist)))
            (car alist)
        )

        ;; Mismatch V and alist
        (T
            (my-assoc v (cdr alist))
        )
    )
)

;; This one is done. Note the simple, clean code
(defun my-eval (e alist)
    (cond ((atom e) (my-eval-atom e alist))
          (t (my-apply (car e) (cdr e) alist))
    )
)

;; You need to write this one.
(defun my-eval-atom (e alist)
;; how do you evaluate an atom???
;; Remember there are special cases: T, NIL, MY-SYMBOL, 10, "Hello"
    (cond
        ;; case 1: nil evaluates to itself
        ((null e) nil)

        ;; case 2: T evaluates to itself
        ((eq e 'T) T)

        ;; case 3: number literal
        ((numberp e) e)

        ;; case 4: string literal
        ((stringp e) e)

        ;; case 5: symbol - look up in alist
        ((symbolp e)
            (cond
                ((my-assoc e alist)
                    (cdr (my-assoc e alist))   ; returns value bound to symbol
                )
                (T nil) ; unbound symbol -> nil
            )
        )

        ;; case 6: anything else - just return it
        (T e)
    )
)

;; my-apply applies a function to arguments. The function may be the
;; name of a function, or it may be a lambda.

;; This one is done, but you must write the functions it calls

(defun my-apply (fn args alist)
    (cond ((atom fn) (my-apply-atom fn args alist))
          ( t (my-apply-lambda fn args alist)))
)

;; You need to write this one.
;; Utility function for eval-cond and apply-lambda.  Evaluates each expression
;; in l and returns the value of the last expression

(defun my-eval-list (l alist)
  (cond 
    ;; no expressions
    ((null l) nil)

    ;; one expression left - evaluate and return result
    ((null (cdr l)) 
        (my-eval (car l) alist)
    )

    ;; (recursive step) more than one expression - evaluate first, discard result, continue
    (T 
        (my-eval (car l) alist) 
        (my-eval-list (cdr l) alist)
    )
  )
)

;; You need to write this one.

(defun my-apply-lambda (fn args alist)
;; bind the formals to the evaluated actuals then evaluate the body in that
;; new scoping context (i.e., that becomes the new alist for recursive
;; evaluation of the function body.  Return the value of the last 
;; expression in the body (using eval-list)).
    
    (my-eval-list
        (cddr fn) ;; body of the lambda
        (my-bind-formals
            (cadr fn)   ;; formals
            args
            alist
        )
    )
)

;; You need to write this one.

(defun my-bind-formals (formals actuals alist)
;; This takes a list of formals and unevaluated actuals.  It should evaluate
;; each actual and bind it to its corresponding formal placing them all on
;; the front of the alist.  It should return the alist with the new bindings
;; on the front.  This will be used to evaluate calls to functions defined
;; via defun.
;; e.g., (my-bind-formals '(a) '((add 1 b)) '((b . 10)))
;; will return ((a . 11) (b . 10))
;; Note there will be one actual parameter for each formal parameter.
    (cond
        ;; base case - no more formals to bind
        ((null formals) alist)
        ((null actuals) nil)  ; error

        ;; recursive step
        (T
            (cons
                (cons
                    (car formals) 
                    (my-eval (car actuals) alist) ; evaluate one actual
                )
                (my-bind-formals (cdr formals) (cdr actuals) alist)
            )
        )
    )
)

;; You need to write this one.  Handle the primitives as special cases, then
;; handle user defined functions (defined via defun) in the default case.
;; Handle car, cdr, cons, eq, quote, cond, defun, setq, eval, print, atom, null,
;; listp, apply, equal, +, -, mod, floor and user defined functions (defined via defun).
;; This should allow you to interpret your functions from HW4.

(defun my-apply-atom (fn args alist)
    (cond
        ;; ---- Primitive functions ----
        ((eq fn 'eq)
            (eq (my-eval (car args) alist) (my-eval (cadr args) alist))
        )
        ;; I wrote the first one, eq, for you, you write the rest
        
        ((eq fn 'car)
            (car (my-eval (car args) alist))
        )
        ((eq fn 'cdr)
            (cdr (my-eval (car args) alist))
        )
        ((eq fn 'cons)
            (cons 
                (my-eval (car args) alist)
                (my-eval (cadr args) alist)
            )
        )
        ((eq fn 'quote)
            (car args) ;; quote returns arugment unevaluated
        )
        ((eq fn 'atom)
            (atom (my-eval (car args) alist))
        )
        ((eq fn 'null)
            (null (my-eval (car args) alist))
        )
        ((eq fn 'listp)
            (listp (my-eval (car args) alist))
        )
        ;; ---- Conditionals and evaluation ----
        ((eq fn 'cond) 
            (my-eval-cond args alist)
        )
        ((eq fn 'eval) 
            (my-eval (my-eval (car args) alist) alist)
        )

        ;; ---- printing ----
        ((eq fn 'print)
            (print (my-eval (car args) alist))
        )

        ;; ---- arithmetic opertions ----
        ((eq fn '+)
            (+  (my-eval (car args) alist)
                (my-eval (cadr args) alist))
        )
        ((eq fn '-)
            (-  (my-eval (car args) alist)
                (my-eval (cadr args) alist))
        )
        ((eq fn 'mod)
            (mod (my-eval (car args) alist)
                (my-eval (cadr args) alist))
        )
        ((eq fn 'floor)
            (floor (my-eval (car args) alist)
                (my-eval (cadr args) alist))
        )
        ((eq fn 'equal)
            (equal (my-eval (car args) alist)
                (my-eval (cadr args) alist))
        )
        
        ;; ---- setq & defun ----
        ((eq fn 'setq)
            ;; you'll have to figure out the arguments to pass
            (my-eval-setq (car args) (cadr args))
        )
        ((eq fn 'defun)
            (my-eval-defun args alist)
        )

        ;; ---- Default:user-defined function ----
        ;; in this last case, function must be a variable bound to a function
        ;; so get the lambda from the alist and then apply that to the arguments
        (T 
            ;; this nil must be replaced by the correct expression
            ;; get the lambda body from alist
            (cond
                ((my-assoc fn alist)
                    (my-apply (cdr (my-assoc fn alist)) args alist)
                )
                (T
                    (print (cons 'undefined-function fn))   ;; error message
                    nil
                )
            )
        )
    )
)

;; setq and defun will push a new association on the global-alist.
;; whenever we apply a function, we will bind the formals to the evaluated
;; actuals pushing these new bindings onto the local alist and then
;; evaluate the body of the function in that new scoping context.

;; You need to write this one.

(defun my-eval-setq (var val)
;; just push a new association of the var and its evaluated val onto the
;; global alist

    ;; Updates the global variable in place.
    (setq global-alist
        (cons
            (cons var (my-eval val global-alist))
            global-alist
        )
    )
    ;; return the value for compliance with Lisp semantics
    (cdr (car global-alist))
)

;; You need to write this one.  You should know how cond works at this point.
;; Remember, cond clauses have one or more expressions in each clause.

(defun my-eval-cond (clauses alist)
    (cond
        ;; (base case) no more clauses
        ((null clauses) nil)

        ;; if the test part of the first clause is true
        ((my-eval (car (car clauses)) alist)
            (my-eval-list (cdar clauses) alist)
        )

        ;; recursive step 
        ;; otherwise, continue wwith remaining clauses
        (T
            (my-eval-cond (cdr clauses) alist)
        )
    )
)

;; You need to write this one.
;; Hint: just push the function body onto the global alist.  It is already an
;; association, e.g., (equal (L1 L2) (cond (...))) and (assoc 'equal in
;; the global alist will return this.  You can then take the cdr and you
;; have a list containing the formal parameters and the expressions in
;; the function body. defun returns the name of the function.

(defun my-eval-defun (body alist)
    (setq global-alist
        (cons
            (cons
                (car body)  ;; function name
                (cons 'lambda (cdr body))   ;; build (lambda (params) body...)
            )
            global-alist
        )
    )
    (car body)
)

;; This one is done, it just initializes the global alist where global
;; settings, like those defined via setq and defun, go.

(defvar global-alist nil)

;; to push a new value, (setq global-alist (cons (cons 'newvar 'newval) global-alist))

;; This one is done, it will become the new top-level for LISP.  After you
;; load this file, call (my-top) and then you can type in expressions and
;; define and call functions to test your my-eval. Note it uses the prog which
;; allows defining local variables, labels and goto looping similar to features
;; found in imperative languages.

(defun my-top ()
    (prog ()
        top
            ;; read an s-expression, evaluate it using my-eval passing in the global-alist,
            ;; then print the result, functions and global variables will be on global-alist

            (print (my-eval (read) global-alist))
            (terpri) ;; prints a newline
            (go top) ;; loops forever
    )
)

;; ----- Zack's Test Cases -----
;; -----------------------------
(defun my-tests()
    (format t "~%--- Testing my-assoc---~%")
    (format t "(my-assoc 'b '((a . 1)(b . 2)(c . 3)(b . z))) = ~A~%" (my-assoc 'b '((a . 1)(b . 2)(c . 3)(b . z))))   ; --> (b . 2)
    (format t "(my-assoc 'z '((a . 1)(b . 2)(c . 3)(b . z))) = ~A~%" (my-assoc 'z '((a . 1)(b . 2)(c . 3)(b . z))))   ; --> nil

    (format t "~%--- Testing my-eval---~%")
    (format t "(my-eval 'a '((a . 10))) = ~A~%" (my-eval 'a '((a . 10))))   ; --> 10

    (format t "~%--- Testing my-eval-atom---~%")
    (format t "(my-eval-atom nil nil) = ~A~%" (my-eval-atom nil nil))   ; --> nil
    (format t "(my-eval-atom 'T nil) = ~A~%" (my-eval-atom T' nil))   ; --> T
    (format t "(my-eval-atom 42 nil) = ~A~%" (my-eval-atom 42 nil))   ; --> 42
;;    (format t "(my-eval-atom "hello" nil) = ~A~%" (my-eval-atom "hello" nil))   ; --> "hello"
    (format t "(my-eval-atom 'A '((a . 100) (b . 200))) = ~A~%" (my-eval-atom 'A '((a . 100) (b . 200))))   ; --> 100
    (format t "(my-eval-atom 'C '((a . 100) (b . 200))) = ~A~%" (my-eval-atom 'C '((a . 100) (b . 200))))   ; --> nil

    (format t "~%--- Testing my-eval---~%")

    (format t "~%--- Testing my-eval-list---~%")

    (format t "~%--- Testing my-apply-lambda ---~%")
    (format t "(my-apply-lambda '(lambda (x y) (cons x y)) '((quote a) (quote (b c))) nil) = ~A~%" (my-apply-lambda '(lambda (x y) (cons x y)) '((quote a) (quote (b c))) nil))   ; --> (A B C)

    (format t "~%--- Testing my-bind-formals---~%")
    (format t "(my-bind-formals '(a) '((+ 1 b)) '((b . 10))) = ~A~%" (my-bind-formals '(a) '((+ 1 b)) '((b . 10))))   ; --> ((a . 11) (b . 10))
    (format t "(my-bind-formals '(x y) '((+ 2 3) (cons 'a '(b c))) nil) = ~A~%" (my-bind-formals '(x y) '((+ 2 3) (cons 'a '(b c))) nil))   ; --> ((X . 5) (Y A B C))

    (format t "~%--- Testing my-apply-atom---~%")
    (format t "(my-apply-atom 'car '((quote (a b c))) nil) = ~A~%" (my-apply-atom 'car '((quote (a b c))) nil))   ; --> A
    (format t "(my-apply-atom 'cons '((quote a) (quote (b c))) nil) = ~A~%" (my-apply-atom 'cons '((quote a) (quote (b c))) nil))   ; --> (A B C)
    (format t "(my-apply-atom '+ '((quote 1) (quote 2)) nil) = ~A~%" (my-apply-atom '+ '((quote 1) (quote 2)) nil))   ; --> 3
    (format t "(my-apply-atom 'eq '((quote a) (quote b)) nil) = ~A~%" (my-apply-atom 'eq '((quote a) (quote b)) nil))   ; --> nil
    (format t "(my-apply-atom 'equal '((quote (a b)) (quote (a b))) nil) = ~A~%" (my-apply-atom 'equal '((quote (a b)) (quote (a b))) nil))   ; --> T

    (format t "~%--- Testing my-eval-setq ---~%")
    (format t "(my-eval-setq 'a '(+ 2 5)) = ~A~%" (my-eval-setq 'a '(+ 2 5)))   ; --> 7
    (format t "global-alist = ~A~%" global-alist)   ; --> ((A . 7))
    (format t "(my-eval-setq 'b '(+ a 5)) = ~A~%" (my-eval-setq 'b '(+ a 5)))   ; --> 12
    (format t "global-alist = ~A~%" global-alist)   ; --> ((B . 12) (A . 7))

    (format t "~%--- Testing my-eval-cond ---~%")
    (format t "(my-eval-cond '(((eq 'a 'b) (quote first)) ((eq 'a 'a) (quote second)) (T (quote last))) nil) = ~A~%" (my-eval-cond '(((eq 'a 'b) (quote first)) ((eq 'a 'a) (quote second)) (T (quote last))) nil))   ; --> SECOND

    (format t "~%--- Testing my-eval-defun ---~%")
    (format t "(my-eval-defun '(rev (l r) (cond ((null l) r) (T (rev (cdr l) (cons (car l) r))))) nil) = ~A~%" (my-eval-defun '(rev (l r) (cond ((null l) r) (T (rev (cdr l) (cons (car l) r))))) nil))   ; --> rev
    (format t "global-alist = ~A~%" global-alist)   ; --> ((REV LAMBDA (L R) (COND ((NULL L) R) (T (REV (CDR L) (CONS (CAR L) R))))) ...)
)

;; ----- Professor Klefstad test-----
;; ----------------------------------
(defun my-error (msg)
    (princ "Error: ")
    (princ msg)
    (terpri)
    nil
)

;; (trace my-eval my-apply-lambda my-eval-cond my-apply my-eval-list my-bind-formals)

(defun my-test (exp)
    (print exp )
    (print (my-eval exp global-alist))
    (terpri)
    (terpri)
)

(defun testallhw5 ()
    (my-test t)
    (my-test nil)
    (my-test "hello")
    (my-test 10)
    (my-test '(eq t t))
    (my-test '(eq nil nil))
    (my-test '(eq t nil))
    (my-test '(null nil))
    (my-test '(null t))
    (my-test '(quote (a b c)))
    (my-test '(eq 'a 'a))
    (my-test '(eq '(a b) '(a b)))
    (my-test '(car '(a b c)))
    (my-test '(cdr '(a b c)))
    (my-test '(cons 'foo '(a b c)))
    (my-test '(setq a '(a b c)))
    (my-test '(print '(a b c)))
    (my-test 'a)
    (my-test '(cond (nil 1)(t 2) (t 3)))
    (my-test '(cond ((eq t nil) (print "in case 1") 1)((eq t t) (print "in case 2") 2)(t (print "in case 3") 3)))
    (my-test '(defun rev (L R) (cond ((null L) R) (t (rev (cdr L) (cons (car L) R))))))
    (my-test '(rev a nil))
    (my-test '(rev (rev  a nil) nil))
    (my-test '(defun app (L R)(cond ((null L) R)(t (cons (car L) (app (cdr L) R))))))
    (my-test '(app (app a a) (app a a)))
)
