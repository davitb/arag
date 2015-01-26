arag is a "redis" compatible server completely written in C++ 11. 

There are several reasons why arag project has been started:

1) Benchmark performance of c++ implementation vs c implementation. Currently arag is around 30% slower than redis which is a great result given that optimization hasn't been a priority yet in arag's implementation.
2) Experiment how less code c++ implementation will require. Currently arag is ~10K and Redis is ~60K.

Currently arag implements over 130 commands and passes most of the tests by redis-py (most popular redis python client).

What it currently supports:

1) All data structures supported by Redis (String, Set, Hashe, List, Sorted Set, Hyperloglog).
2) Publish/Subcribe
3) Transactions
4) Scripting
5) Pipelining
6) Keys Commands and some Server and Connections commands

What is doesn't support yet:

1) Some commands (DUMP, RESTORE, SCAN*, MIGRATE, monitoring and configuration related commands)
2) Persistance
3) Replication
4) Clustering

Join in the development, if you are interested.