arag is a "redis" compatible server completely written in C++ 11. 
<br>
There are several reasons why arag project has been started:
<br>
1) Benchmark performance of c++ implementation vs c implementation. Currently arag is around 30% slower than redis which is a great result given that optimization hasn't been a priority yet in arag's implementation.
<br>
2) Experiment how less code c++ implementation will require. Currently arag is ~10K and Redis is ~60K.
<br>
<br>
Currently arag implements over 130 commands and passes most of the tests by redis-py (most popular redis python client).
<br>
<br>
What it currently supports:
<br>

1) All data structures supported by Redis (String, Set, Hashe, List, Sorted Set, Hyperloglog).
<br>
2) Publish/Subcribe
<br>
3) Transactions
<br>
4) Scripting
<br>
5) Pipelining
<br>
6) Keys Commands and some Server and Connections commands
<br><br>

What is doesn't support yet:
<br>

1) Some commands (DUMP, RESTORE, SCAN*, MIGRATE, monitoring and configuration related commands)
<br>
2) Persistance
<br>
3) Replication
<br>
4) Clustering
<br>
<br>

