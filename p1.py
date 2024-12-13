giúp tôi với 
input nhập từ bàn phím 

We need to make a planing to transport goods from n warehouses to m super markets.
Amount of goods at warehouse i is A[i] and the amount of goods requested at the super market j is B[j].
The cost of transporting one unit of goods from warehouse i to super market j is C[i][j].
Goal: Make a plan to transport goods from warehouses to super markets such that the amount of goods the super market j receives must be greater or equal to B[j] and the total cost is minimized.
Input:
Line 1: contains two integers n and m, the number of warehouses and the number of super markets.
Line 2: contains n positive integers A[1], A[2],..., A[n] (1 ≤ A[i] ≤ 1000), the capacities of the warehouses.
Line 3: contains m positive integers B[1], B[2],..., B[m] (1 ≤ B[i] ≤ 1000), the demands of the super markets.
Line i+1: contains m integers C[i][1], C[i][2],..., C[i][m] (1 ≤ C[i][j] ≤ 1000), the cost of transporting one unit of goods from warehouse i to super market j.
Output:
Line 1: contains a positive integer K
Line k+1 (k=1..K): contains two integers i and j and a real number g which is the amount of goods transported from warehouse i to super market j. 

Example
Input
2 3
30 20
12 17 20
2 3 1
1 2 5

Output
6
1 1 0.0
1 2 9.0
1 3 20.0
2 1 12.0
2 2 8.0
2 3 0.0

