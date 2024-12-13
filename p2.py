giúp tôi với: input nhập từ bàn phím, không sử dụng thư viện 

Perform the domain consistency for the CSP with n variables
X1, X2, ..., Xn with a set of constraints under the form:
- LEQ i j D (it means: Xi <= Xj + D)
- EQ i j a b (it means: Xi = a*Xj + b)

Input:
- Line 1: contains a positive integer n (1<=n<=50)
- Line i+1(i = 1,2,...,n): contains a sequence of integers v(i,1), v(i,2) ... v(i,k) 
which are the values of the domain of Xi
- Subsequent lines are constraints: each line contains the information of a constraint described above.
The constraint lines is terminated with a line containing #

Output:
- Write FAIL if the domain  of some variables becomes empty.
- Otherwise, line i (i=1,2...,n): contains q (number of values of the domain of Xi)
and a sequence(increasing order) of values of the domain of Xi the DC propagation
(after each value, there is a SPACE character)

Example

Input
5
1 2 4 5 6
1 3 4 6
2 3 4 5
1 2 3 4
6 7 8 9 10
LEQ 1 2 -1
EQ 3 2 1 -2
#

Output
4 1 2 4 5
2 4 6
2 2 4
4 1 2 3 4
5 6 7 8 9 10

sử dụng ortools

Tôi cần một lời giải khác vẫn sử dụng ortools mà không bị lỗi này:
OpenBLAS blas_thread_init: pthread_create failed for thread 10 of 16: Resource temporarily unavailable
timeout: the monitored command dumped core

Kiểm tra lại lời giải trên:
Nếu làm thế thì sẽ in ra output sai như sau:
4 1 2 4 5
4 1 3 4 6
2 2 4
4 1 2 3 4
5 6 7 8 9 10
Trong khi output đúng là:
4 1 2 4 5
2 4 6 
2 2 4
4 1 2 3 4
5 6 7 8 9 10


dùng khử như bình thường 

