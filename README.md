# Parallel and Distributed Computing
Exercise 1
### The following exercise aims to solve the "Sum of N Numbers" problem in a parallel computing environment with MIMD-DM architecture
The parallel computing environment used for solving the problem employs MIMD-DM architecture (Multiple Instruction Multiple Data - Distributed Memory). Specifically, this type of architecture includes multiple distinct processing units (processors) that simultaneously execute separate computations on different data streams. Each processing unit has its own local memory and can execute its instructions independently of the other units. If a unit needs to access data stored in another unit, it must request access through techniques such as Message Passing Interface.

Specifically, given $a_0, ..., a_{n−1}$ ∈ R, the sum S of the $a_i$ is defined as: <br>
<p align="center"> $S = a_0+,...,+a_{n-1}\ =\ \sum\limits_{i=0}^{n-1}a_i$</p>

Solving the problem in a parallel computing environment is based on the idea of decomposing the problem into smaller-sized sub-problems and solving them simultaneously on multiple computing units/processors. 

Let $X$ be our problem with a size of $n$. The concept involves dividing problem $X$ into $p$ sub-problems (given that we have $p$ processors) of size $n/p$ and solving them concurrently on the $p$ processors. Once the results of the $p$ sub-problems are obtained, they need to be combined in a suitable manner to obtain the total sum.

Example: <br>
$n$ = 16 and $p$ = 4, we should:
  1.  Divide the problem $X$ into $p = 4$ sub-problems.
  2.  Each of the $p$ sub-problems should have a size of $n/p$, meaning it will solve the summation of $4$ numbers.
  3.  Each of the $p$ processors will execute the resolution of a single sub-problem.
  4.  Solving the $p = 4$ sub-problems will yield $s_0, ..., s_3$ partial sums.
  5.  To sum $s_0, ..., s_3$ partial sums, we will reapply step $1$, considering $n = 4$, and using only $p = n/2$ processors, leaving the others inactive.
  6.  Solving the $p = 2$ sub-problems will yield ($s_0 + s_1$) and ($s_2 + s_3$).
  7.  To sum the two results ($s_0 + s_1$) and ($s_2 + s_3$), we will reapply step $1$, considering $n = 2$, and using only $p = n/2$ processors, leaving the others inactive.
  8.  Solving the $p = 1$ sub-problem will return the sum of ($s_0 + s_1 + s_2 + s_3$), which is the total sum.

![esempio1(1)](https://github.com/Peppebalzanoo/pdc-e1/assets/59487799/46a7792f-b35a-4d78-9385-f45d8328fdc2)

