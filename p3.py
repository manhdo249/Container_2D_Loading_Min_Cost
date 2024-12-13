Constraint Satisfaction Problem consist of a set of
variables, each variable has a set of discrete integer
values and a set of constraints over these variables.
The constraints can be of 3 categories:
- Leq a X b Y c: a*X <= b*Y + c
- Neq a X b Y c: a*X != b*Y + c
- Eq a X b Y c: a*X = b*Y + c

Input:
Each line contains the information about a constraint or variable
The line contains a variable has the form: <name> <min> <max> : variable with name <name> and the domain is {<min> , <max>}
The line contains a constraint has the form:
<constraint_category><a><variable_1><b><variable_2><c>
- If <constraint_category> is Leq, then the meaning of the constraint is:
<a> * <variable_1> <= <b> * <variable_2> + <c>
- If <constraint category> is Neq, then the meaning of the constraint is:
<a> * <variable_1> != <b> * <variable_2> + <c>
- If <constraint category> is Eq, then the meaning of the constraint is:
<a> * <variable_1> = <b> * <variable_2> + <c>
The input is terminated with a line containing #

Output:
If the problem does not have any solutions, then write -1.
Otherwise, write the solution under the form:
- Line 1: contains a positive integer N which is the number of decision variables of the problem.
- Each line (of N subsequent lines) contains <variable><value> in which variable <variable> has value <value>

Example
Input
Var X 1 20
Var Y 1 20
Var Z 1 20
Eq 1 X 1 Y 2
Leq 2 Z 1 X -2
Eq 3 Z 1 Y 1
#

Output
3
X 4
Y 2
Z 1
