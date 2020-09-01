# SCoro

A stackless, resumable, resettable coroutine library for C++17.
This was made when I discovered that C++20 coroutines very frequently require heap allocation and their embodied types are erased. 

This library allows:
- Resettable coroutines
- Resumable coroutines
- Easy scheduling on other threads
- State stored on stack or heap

Upcomming:
- Allow arbitary return types
- Allow arbitary parameters
- Enable exceptions.

An example of how to use the code is here:

*main.cpp*