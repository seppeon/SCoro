# SCoro

A stackless, resumable, resettable state machine style coroutine library for C++17.
This was made when I discovered that C++20 coroutines very frequently require heap allocation and their embodied types are erased.

This does not use C++20 coroutines. It is a managed state machine style coroutine library, a modern take on duffs device. This is suppose to work with C++17 compilers.

Why, because C++20 coroutines:

* are difficult to develop with.
* make nested co_await clumbsy and have large memory overhead to do it right.
* HALO optimization seems to regularly fail.
* require heap allocation to be avaliable in the general case, making them unusable on many systems.

And:

* This library allows saving of a SCoro state to a file for later resumption.
* It allows heap or stack saving of the state, making it easy to use locally or passed to a scheduler or thread.
* Its lightweight and can be evaluated in constexpr context.
* Trivially composable (I sometimes find this isn't the case for c++20 coroutines).
* This library makes it easier to use C++20 coroutines (does for me anyway)., simply use this library within your coroutines and construct a facade for C++20 coroutines. This way, the nesting of co_await is no longer an issue and polling for ready is also no longer an issue. std::noop_coroutine is no longer needed and from the user perspective the coroutine syntax can still be used.

Any feedback is welcome, I have a long todo here. I'm looking at improving the actual standard C++ coroutines eventually with a similar approach. If anybody sees advantages and disadvantages of my approach I'd love to hear about them!

The ultimate idea is to have the language be syntactic sugar around this sort of pattern.

**To boost the algorithms generality I'd love requests for examples, that I can put in the git. The examples would need to be cross platform, so that limits things a little (most async ops are connected with OS or hardware). But any ideas I'll add an example.**

**Examples in library:**

1. composition, with nested SCoro's and reusable SCoro blocks.
2. timing, an example that deals with polling of a scheduled event, something like a wait\_for.
3. continuing on a different thread than the initiating thread.
4. iota example.
5. C++20 coroutine integration example (very basic)
