# CI_LISP
Edgar Ramirez
CS 232 L

TASK 1:
Task 1 simply takes and implements Cambridge Polish Notation (CPN) and allows the user to input function names within
parenthesis followed by one or two operands and evaluates them.
The value is then outputted into a NUM_NODE to be outputted by the program.

    Sample Output:
        > (add 1 3)
        <INT>: 4
        > (abs 3)
        <INT>: 3
        > (abs -3.0)
        <DOUBLE>: 3.000000
        > (sqrt 16)
        <INT>: 4
        > (sqrt 16.0)
        <DOUBLE>: 4.000000
        > (mult 6 4.3)
        <DOUBLE>: 25.800000
        > (div 36 6)
        <INT>: 6
        > (max 3 53)
        <INT>: 53
        > (min 54.64 34.64)
        <DOUBLE>: 34.640000

TASK 2:
Task 2 would further implement what was created in TASK 1 and allow the user to nest functions within more functions,
following the correct syntax, of evaluating the values input inside the functions. The values that were evaluated would
then be outputted into a RET_VAL of NUM_NODE.

    Sample Output:
        > (add 3 (sub 3 4))
        <INT>: 2
        > (mult 3 (min 3 4))
        <INT>: 9
        > (add ((let (a ((let (b 2)) (mult b (sqrt 10))))) (div a 2)) ((let (c 5)) (sqrt c)))
        <INT>: 5
        > (add 3 (mult 3 (cbrt 36)))
        <INT>: 12
        > (add 2 (add 45.3 (sub 4 (mult 35 (cbrt 16)))))
        <DOUBLE>: -18.700000
        > (add 1 (mult 3 (sub (exp 4) (min 2 3))))
        <INT>: 157
        > (abs (sub (min 2.3 5.6) (add (max 4.5 653.1) (remainder 36 6))))
        <DOUBLE>: 650.800000


TASK 3:
Task 3 allows the user to change the type of value, by casting when using a let statement. If the value is the same as
what is being cast then no changes should be done to the value. Although, if the value being cast is a double to an int,
the value will be floored, lose the decimal values, and will be rounded off. Nothing will be done if the value is an int
and is being cast to double. It will just gain additional decimal values.

    Sample Output:
        >((let (int a 1.25))(add a 1))
        WARNING: precision loss in the assignment for variable <a>
        <INT>: 2.000000
        > ((let (double a 2))(sub a 3))
        <DOUBLE>: -1
        > ((let (int abc 3.5))(mult abc 16))
        WARNING: precision loss in the assignment for variable <abc>
        <INT>: 48.000000

TASK 4:
Task 4 will simply print the s_expression.

    Sample Output:
        > (print 2)
        <INT>: 2
        > (print 2.3)
        <DOUBLE>: 2.300000
        > (print (add 23.1 3.4))
        <DOUBLE>: 26.500000
        > (print (mult 34 1))
        <INT>: 34
        > (print (mult 3 (add 3 4.5)))
        <DOUBLE>: 22.500000
        > (print (sub 34.5 (add 3 (cbrt 8))))
        <DOUBLE>: 29.500000

TASK 5: (INCOMPLETED)
TASK 5, creates an operand List that will take all the operands and place them into a linked list of nodes, where depending
on the function will evaluate what it needs and place it into a return value. ADD, MULTIPLY, MIN, MAX, and HYPOT should
allow for multiple operands. It will also generate an error message when there are too little or too many opearands in the
function. In the case of too many operands, it will take the first one and evaluate.

(Note: I am having issues with the print case within evalFuncNode, that does not print out the multiple operands, it only
prints out the first one.
There is also an issue where if no operands are entered with the function, it just doesn't mention the function name.)

    Sample Output:
        > (add 1 2 4.3 4 6 4.6)
        <DOUBLE>: 21.900000
        > (mult 2.3 5.3 2.1 1)
        <DOUBLE>: 25.599000
        > (min 3.4 1.3 3 550 3.21)
        <DOUBLE>: 1.300000
        > (max 2.3 4.2 421 356 34454)
        <DOUBLE>: 34454.000000
        > (hypot 23 213 64 223)
        <INT>: 315
        > (add 2 34 6 (cbrt 3))
        <INT>: 43
        > (add 2)
        ERROR: too few parameters for the function <add>
        <INT>: nan
        > (cbrt 2 354)
        WARNING: too many parameters for the function <cbrt>
        <INT>: 1
        > (add 2 3 5 3 45 57 678 789 56 34 23 65 76)
        <INT>: 1836
