;; ===============================
;; Function Definitions
;; ===============================

; takes a list as a parameter and returns the length of the list
; uses recursion to count elements in list
; base case is an empty list represented by NULL
; at each recursive set add one and recursively call MY-Length with CDR of L
(DEFUN MY-LENGTH (L)
    (COND 
        ; base_case
        ((NULL L) 
            0
        ) 
        ; recursive step
        (T
            (1+ (MY-LENGTH (CDR L)))
        )
    )
)

;
(DEFUN MY-MEMQ (A L)
    (COND
        ; base case: empty list
        ((NULL L) 
            NIL
        )

        ; return list if first element equals A
        ((EQ A (CAR L)) 
            L
        )

        ; recursive step call MY-MEMQ on CDR L
        (T
            (MY-MEMQ A (CDR L))
        )
    )
)

(DEFUN MY-APPEND (L1 L2)
    (COND
        ((NULL L1)
            L2
        )
        (T 
            (CONS 
                (CAR L1) 
                (MY-APPEND (CDR L1) L2)
            )
        )
    )
)

(DEFUN MY-REVERSE (L)
    (COND
        ; base case
        ((Null L) 
            NIL
        )
        ; recursive step
        (T
            (MY-APPEND 
                (MY-REVERSE (CDR L)) (LIST(CAR L))
            )
        )
    )
)

(DEFUN MY-MAPCAR (F L)
    (COND
        ((NULL L) 
            NIL
        )
        
        (T 
            (CONS 
                (APPLY F (LIST (CAR L)))
                (MY-MAPCAR F (CDR L))
            )
        )
    )
)

(DEFUN MY-COPY (L)
    (COND
        ((ATOM L) L)
        (T 
            (CONS 
                (MY-COPY (CAR L)) 
                (MY-COPY (CDR L))
            )
        )
    )
)

(DEFUN MY-NTH (L N)
    (COND
        ((EQ N 0) (CONS NIL L))
        ((NULL L) NIL)
        ((EQ N 1) L)
        (T (MY-NTH (CDR L) (1- N)))
    )
)

(DEFUN MY-REMOVE (X L)
    (COND
        ((NULL L) 
            NIL
        )
        ((EQUAL X (CAR L)) 
            (MY-REMOVE X (CDR L))
        )
        (T 
            (CONS 
                (CAR L) (MY-REMOVE X (CDR L))
            )
        )
    )
)

(DEFUN MY-SUBST (X Y Z)
    (COND
        ((NULL Z) 
            NIL
        )
        ((LISTP (CAR Z)) 
            (CONS 
                (MY-SUBST X Y (CAR Z)) 
                (MY-SUBST X Y (CDR Z))
            )
        )
        ((EQUAL (CAR Z) X) 
            (CONS 
                Y 
                (MY-SUBST X Y (CDR Z))
            )
        )
        (T 
            (CONS 
                (CAR Z) 
                (MY-SUBST X Y (CDR Z))
            )
        )
    )
)

(DEFUN MY-SUBSET (FN L)
    (COND 
        ((NULL L) 
            NIL
        )
        ((APPLY FN (CONS (CAR L) NIL)) 
            (CONS 
                (CAR L) (MY-SUBSET FN (CDR L))
            )
        )
        (T 
            (MY-SUBSET FN (CDR L))
        )
    )
)

(DEFUN MY-CARRY-ADD (N1 N2 CARRY)
    (COND
        ;; Base case: both lists are empty and no carry left
        ((AND (NULL N1) (NULL N2) (= CARRY 0))
            NIL
        )

        ;; Base case: both lists empty but still a carry (like adding (9) + (9))
        ((AND (NULL N1) (NULL N2))
            (CONS CARRY NIL)
        )

        ;; When one list runs out, treat its digit as 0
        ((NULL N1)
            (MY-CARRY-ADD (CONS 0 NIL) N2 CARRY)
        )
        ((NULL N2)
            (MY-CARRY-ADD N1 (CONS 0 NIL) CARRY)
        )

        ;; Recursive step
        (T
            (CONS
                (MOD (+ (CAR N1) (CAR N2) CARRY) 10)    ; digit
                (MY-CARRY-ADD
                    (CDR N1)
                    (CDR N2)
                    (FLOOR (+ (CAR N1) (CAR N2) CARRY) 10)  ; new carry
                )
            )
        )
    )
)

(DEFUN MY-ADD (N1 N2)
    (MY-CARRY-ADD N1 N2 0)
)

(DEFUN MY-MERGE (L1 L2)
    (COND
        ;; Both list are empty
        ((AND (NULL L1) (NULL L2)) 
            NIL
        )

        ;; one list is empty
        ((NULL L1)
            (CONS
                (CAR L2)
                (MY-MERGE NIL (CDR L2))
            )
        )
        ((NULL L2)
            (CONS
                (CAR L1)
                (MY-MERGE (CDR L1) NIL)
            )
        )

        ;; recursive step
        (T
            (COND
                ((< (CAR L1) (CAR L2))
                    (CONS
                        (CAR L1)
                        (MY-MERGE (CDR L1) L2)
                    )
                )
                ((> (CAR L1) (CAR L2))
                    (CONS
                        (CAR L2)
                        (MY-MERGE L1 (CDR L2))
                    )
                )
            )
        )
    )

)

(DEFUN MY-SUBLIST-HELPER (L1 L2 PARTIALSUB)
    (COND
        ;; Base Cases
        ;; Both list are empty
        ((AND (NULL L1) (NULL L2))
            T
        )

        ((NULL L1)
            T
        )
        
        ((NULL L2)
            NIL
        )

        ;; Recusive step
        ((EQ (CAR L1) (CAR L2))
            (MY-SUBLIST-HELPER (CDR L1) (CDR L2) T)
        )
        (T
            (COND
                (PARTIALSUB 
                    NIL
                )
                (T
                    (MY-SUBLIST L1 (CDR L2)) 
                )
            )
        )
    )
)
(DEFUN MY-SUBLIST (L1 L2)
    (MY-SUBLIST-HELPER L1 L2 NIL)
)

(DEFUN MY-ASSOC (A ALIST)
    (COND
        ;; Base case
        ((NULL ALIST)
            NIL
        )

        ;;  Recusive step
        ;; the first ATOM of the first key/value pair matches A
        ((EQ A (CAR (CAR ALIST)))
            (CAR ALIST)
        )
        ;; Mismatch A and ALIST
        (T
            (MY-ASSOC A (CDR ALIST))
        )
    )
)

(DEFUN MY-REPLACE (ALIST L)
    (COND
        ;; BASE CASE
        ((NULL L)
            NIL
        )

        ;; RECURSIVE STEP
        (T
            (COND
                ;; FOUND A MATCH IN FOR THE FITST ATOM OF L IN ALIST
                ;; REPLACE FIRST ATOM L WITH CDR OF OF ALIST ENTRY
                ((MY-ASSOC (CAR L) ALIST)
                    (CONS
                        (CDR (MY-ASSOC (CAR L) ALIST))
                        (MY-REPLACE ALIST (CDR L))
                    )
                )
                ;; NO MATCH WAS FOUND 
                (T
                    (CONS 
                        (CAR L)
                        (MY-REPLACE ALIST (CDR L))
                    )
                )
            )
        )
    )
)

;; ===============================
;; Test Cases
;; ===============================

(format t "~%--- Testing MY-LENGTH ---~%")
(format t "(MY-LENGTH NIL)          = ~A~%" (MY-LENGTH NIL))
(format t "(MY-LENGTH '(A B C))     = ~A~%" (MY-LENGTH '(A B C)))
(format t "(MY-LENGTH '(A (B C) D)) = ~A~%" (MY-LENGTH '(A (B C) D)))

(format t "~%--- Testing MY-MEMQ ---~%")
(format t "(MY-MEMQ 'A NIL)                 = ~A~%" (MY-MEMQ 'A NIL))
(format t "(MY-MEMQ 'B '(A B C))            = ~A~%" (MY-MEMQ 'B '(A B C)))
(format t "(MY-MEMQ 'D '(A B C D E F G))    = ~A~%" (MY-MEMQ 'D '(A B C D E F G)))
(format t "(MY-MEMQ 'D '(A B C D))          = ~A~%" (MY-MEMQ 'D '(A B C D)))
(format t "(MY-MEMQ 'D '(A B C))            = ~A~%" (MY-MEMQ 'D '(A B C)))

(format t "~%--- Testing MY-APPEND ---~%")
(format t "(MY-APPEND '(A) '(B))                 = ~A~%" (MY-APPEND '(A) '(B)))

(format t "~%--- Testing MY-NTH ---~%")
(format t "(MY-NTH '(A B C D E) 1)  = ~A~%" (MY-NTH '(A B C D E) 1))
(format t "(MY-NTH '(A B C D E) 3)  = ~A~%" (MY-NTH '(A B C D E) 3))
(format t "(MY-NTH '(A B C D E) 30) = ~A~%" (MY-NTH '(A B C D E) 30))
(format t "(MY-NTH '(A B C D E) 0)  = ~A~%" (MY-NTH '(A B C D E) 0))

(format t "~%--- Testing MY-REMOVE---~%")
(format t "(MY-REMOVE '(A B) '(A B (A B) A A B (A B)))  = ~A~%" (MY-REMOVE '(A B) '(A B (A B) A A B (A B))))
(format t "(MY-REMOVE 'A '(A B (A B) A B))              = ~A~%" (MY-REMOVE 'A '(A B (A B) A B)))

(format t "~%--- Testing MY-SUBST---~%")
(format t "(MY-SUBST 'B 'A '(A B (A B) (C (A B)))) = ~A~%" (MY-SUBST 'B 'A '(A B (A B) (C (A B)))))

(format t "~%--- Testing MY-SUBSET---~%")
(format t "(MY-SUBSET 'ATOM '(A (B) (C D) E F G)) = ~A~%" (MY-SUBSET 'ATOM '(A (B) (C D) E F G)))
(format t "(MY-SUBSET 'LISTP '(A (B) (C D) E F G)) = ~A~%" (MY-SUBSET 'LISTP '(A (B) (C D) E F G)))

(format t "~%--- Testing MY-ADD---~%")
(format t "(MY-ADD '(0) '(0)) = ~A~%" (MY-ADD '(0) '(0)))
(format t "(MY-ADD '(1) '(1)) = ~A~%" (MY-ADD '(1) '(1)))
(format t "(MY-ADD '(9) '(9)) = ~A~%" (MY-ADD '(9) '(9)))
(format t "(MY-ADD '(1 1 1 1 1 1 1 1 1 1) '(9 9 9 9 9 9 9 9 9 9)) = ~A~%" (MY-ADD '(1 1 1 1 1 1 1 1 1 1) '(9 9 9 9 9 9 9 9 9 9)))
(format t "(MY-ADD '(1) '(9 9 9 9 9 9 9 9 9 9)) = ~A~%" (MY-ADD '(1) '(9 9 9 9 9 9 9 9 9 9)))

(format t "~%--- Testing MY-MERGE---~%")
(format t "(MY-MERGE '(1 3 5 7 9) '(2 4 6 8 10))    = ~A~%" (MY-MERGE '(1 3 5 7 9) '(2 4 6 8 10)))
(format t "(MY-MERGE '(1 2 3 7 8 9) '(4 5 6 10))    = ~A~%" (MY-MERGE '(1 2 3 7 8 9) '(4 5 6 10)))
(format t "(MY-MERGE '(1 2 3) '(4 5 6 7 8 9 10))    = ~A~%" (MY-MERGE '(1 2 3) '(4 5 6 7 8 9 10)))
(format t "(MY-MERGE '(1 3 5 6 7 8 9 10) '(2 4))    = ~A~%" (MY-MERGE '(1 3 5 6 7 8 9 10) '(2 4)))
(format t "(MY-MERGE NIL '(1 2 3 4 5 6 7 8 9 10))   = ~A~%" (MY-MERGE NIL '(1 2 3 4 5 6 7 8 9 10)))

(format t "~%--- Testing MY-MERGE---~%")
(format t "(MY-SUBLIST '(1 2 3) '(1 2 3 4 5))   = ~A~%" (MY-SUBLIST '(1 2 3) '(1 2 3 4 5)))
(format t "(MY-SUBLIST '(3 4 5) '(1 2 3 4 5))   = ~A~%" (MY-SUBLIST '(3 4 5) '(1 2 3 4 5)))
(format t "(MY-SUBLIST '(C D) '(A B C D E))     = ~A~%" (MY-SUBLIST '(C D) '(A B C D E)))
(format t "(MY-SUBLIST '(3 4) '(1 2 3 5 6))     = ~A~%" (MY-SUBLIST '(3 4) '(1 2 3 5 6)))
(format t "(MY-SUBLIST '(1 2 3 4 5) '(3 4 5))   = ~A~%" (MY-SUBLIST '(1 2 3 4 5) '(3 4 5)))
(format t "(MY-SUBLIST '(2 4) '(1 2 3 4 5))     = ~A~%" (MY-SUBLIST '(2 4) '(1 2 3 4 5)))
(format t "(MY-SUBLIST '(1 3 5) '(1 2 3 4 5))   = ~A~%" (MY-SUBLIST '(1 3 5) '(1 2 3 4 5)))

(format t "~%--- Testing MY-MERGE---~%")
(format t "(MY-ASSOC 'A NIL)                    = ~A~%" (MY-ASSOC 'A NIL))
(format t "(MY-ASSOC 'A '((A . B)(C E F)(B)))   = ~A~%" (MY-ASSOC 'A '((A . B)(C E F)(B))))
(format t "(MY-ASSOC 'C '((A . B)(C E F)(B)))   = ~A~%" (MY-ASSOC 'C '((A . B)(C E F)(B))))
(format t "(MY-ASSOC 'B '((A . B)(C E F)(B)))   = ~A~%" (MY-ASSOC 'B '((A . B)(C E F)(B))))
(format t "(MY-ASSOC 'F '((A . B)(C E F)(B)))   = ~A~%" (MY-ASSOC 'F '((A . B)(C E F)(B))))

(format t "~%--- Testing MY-Replace---~%")
(format t "(MY-REPLACE 
                '((G . C)(C . G)(T . A)(A . U)) 
                '(G A T C C T C C A T A T A C A A C G G T)
            ) 
            = ~A~%" 
            (MY-REPLACE 
                '((G . C)(C . G)(T . A)(A . U)) 
                '(G A T C C T C C A T A T A C A A C G G T)
            )
)
(format t "(MY-REPLACE '((UCB . UCLA)(UCSD . UCI)(BASKETBALL . TENNIS)) '(UCSD IS PLAYING BASKETBALL AGAINST UCB)) = ~A~%" (MY-REPLACE '((UCB . UCLA)(UCSD . UCI)(BASKETBALL . TENNIS)) '(UCSD IS PLAYING BASKETBALL AGAINST UCB)))

(format t "~%--- All tests completed. ---~%")