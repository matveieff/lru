# Users lru based cache

Simple c++/python implementation of read-through cache which is used to store users names requests.

##C++
Users cache based on generic LRU template class with pure virtual member function "DataSourceRequest" that should be overwritten for specific needs in the inherited class. User cache class introduces its own DataSourceRequest implementation.

##Python

Users cache implemented as a decorator for a data source request functions with an unique key parameter (see "data_source_request" test function) and may be extended to work with any other functions with an arbitary parameters number by serializing them using (\*args, \**kwargs) approach.
