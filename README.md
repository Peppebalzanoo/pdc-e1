# Parallel and Distributed Computing
Exercise 1
### The following exercise aims to solve the "Sum of N Numbers" problem in a parallel computing environment with MIMD-DM architecture
The parallel computing environment used for solving the problem employs MIMD-DM architecture (Multiple Instruction Multiple Data - Distributed Memory). Specifically, this type of architecture includes multiple distinct processing units (processors) that simultaneously execute separate computations on different data streams. Each processing unit has its own local memory and can execute its instructions independently of the other units. If a unit needs to access data stored in another unit, it must request access through techniques such as Message Passing Interface.

Specifically, given $a_0, ..., a_{n−1}$ ∈ R, the sum S of the $a_i$ is defined as: <br>
<p align="center"> $S = a_0+,...,+a_{n-1}\ =\ \sum\limits_{i=0}^{n-1}a_i$</p>

Solving the problem in a parallel computing environment is based on the idea of decomposing the problem into smaller-sized sub-problems and solving them simultaneously on multiple computing units/processors. 

Let $X$ be our problem with a size of $n$. The concept involves dividing problem $X$ into $p$ sub-problems (given that we have $p$ processors) of size $n/p$ and solving them concurrently on the $p$ processors. Once the results of the $p$ sub-problems are obtained, they need to be combined in a suitable manner to obtain the total sum.

<b>Example:</b> <br>
$n$ = 16 and $p$ = 4, we should:
  1.  Divide the problem $X$ into $p = 4$ sub-problems
  2.  Each of the $p$ sub-problems should have a size of $n/p$, meaning it will solve the summation of $4$ numbers
  3.  Each of the $p$ processors will execute the resolution of a single sub-problem
  4.  Solving the $p = 4$ sub-problems will yield $s_0, ..., s_3$ partial sums
  5.  To sum $s_0, ..., s_3$ partial sums, we will reapply step $1$, considering $n = 4$, and using only $p = n/2$ processors, leaving the others inactive
  6.  Solving the $p = 2$ sub-problems will yield ($s_0 + s_1$) and ($s_2 + s_3$)
  7.  To sum the two results ($s_0 + s_1$) and ($s_2 + s_3$), we will reapply step $1$, considering $n = 2$, and using only $p = n/2$ processors, leaving the others inactive
  8.  Solving the $p = 1$ sub-problem will return the sum of ($s_0 + s_1 + s_2 + s_3$), which is the total sum

<p align="center">
  <img src="https://github.com/Peppebalzanoo/pdc-e1/assets/59487799/c580ac51-26f2-4238-a737-df1614297334" align="center" height="200" width="350"/>
</p>
<br>

## Data Reading and Generation
In the implementation, data is expected to be either read from input sources (if $n <= 20$) or generated (if $n > 20$) using a pseudo-random number generator. It's important to note that data generation does not reflect reality because, in real parallel computing environments, data is not generated but is already available for reading and utilization.

## Data Distribution Among Processors
This phase involves the fair and balanced distribution of data across various processors, labeled as $P_1, P_2, ..., P_n$. There are different techniques for achieving load balance, and the one used in this work is data partitioning, which allows for dividing the dataset in a way that each processor receives a similar amount of data for processing. This is particularly useful when the workload is known in advance, and the data can be divided fairly. An equitable and balanced load distribution will ensure:
  1. Optimal performance
  2. Increased efficiency in resource utilization
  3. Reduction in waiting times
  4. Scalability
  5. Enhanced reliability

## Strategy 1
Once each processor $P_i$ has computed the partial sums $S_i$, each partial sum will be sent to a specific process (in our case, $P_0$) that will sequentially sum these partial sums to obtain the total sum. In this case, the total sum is stored on a single processor, $P_0$.
## Strategy 2
The second strategy employs a hierarchical approach based on a tree structure to perform the cumulative sum of partial values. This tree is constructed using a "bottom-up" approach, starting from the nodes with partial sums and moving up to the root with the total sum.

1. In the first time step, each processor calculates its partial sum
2. Subsequently, the set of processors, each with its own partial sum, is virtually divided into pairs <br>

Each pair consists of two processors:
  - The processor that transmits its partial sum, which is the processor with the larger ID
  - The processor that receives the transmitted partial sum and adds it to its own partial sum, which is the processor with the smaller ID

This process is iteratively repeated until the total sum is contained in a single processor.

## Strategy 3
The third strategy is very similar to the second one, but at each communication step, all processors are involved. This means that every processor in each pair both sends and receives partial sums. The purpose of this approach is to ensure that, at the end of the algorithm, the total sum is distributed across all processors. However, this strategy can be computationally expensive due to a high number of communications.

## Performance Analysis
In order to understand how the problem size, the number of processors used, and the chosen strategy influence the output times as they vary, tables and graphs will be presented below for Execution Time, Speed-Up, and Efficiency.

For each of the three strategies, the number of values to be summed is 10.000, 100.000, 1.000.000, and 10.000.000, while the number of processors used for the various analyses is 1, 2, 4, and 8. This is because only with a number of processors that is a power of two can all three strategies be applied.

## Strategy 1
<p align="center">
  <img src="https://github.com/Peppebalzanoo/pdc-e1/assets/59487799/d5a19528-be66-4e6a-bcbc-a3cf48fc8ba0" align="center" height="300" width="650"/>
</p>
<br>
<p align="center">
  <img src="https://github.com/Peppebalzanoo/pdc-e1/assets/59487799/4584c41f-cf29-4d6d-aacf-2a919ed30ae2" align="center" height="280" width="450"/>
</p>

## Strategy 2
<p align="center">
  <img src="https://github.com/Peppebalzanoo/pdc-e1/assets/59487799/6a02700d-1549-4fb4-bb26-61a1afa47489" align="center" height="300" width="650"/>
</p>
<br>
<p align="center">
  <img src="https://github.com/Peppebalzanoo/pdc-e1/assets/59487799/3c700885-0914-4ec7-ab77-1e1030371f55" align="center" height="280" width="450"/>
</p>

## Strategy 3
<p align="center">
  <img src="https://github.com/Peppebalzanoo/pdc-e1/assets/59487799/fb029352-c507-47a7-a887-09a15b34a295" align="center" height="300" width="650"/>
</p>
<br>
<p align="center">
  <img src="https://github.com/Peppebalzanoo/pdc-e1/assets/59487799/6c6f8319-c885-46a6-a7dd-307f794e0d71" align="center" height="280" width="450"/>
</p>
