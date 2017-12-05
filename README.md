# Event
Provides an interface for event handling in C++. 

Creating:

```C++
doo::Event<> event;
doo::DelegateID id1 = event.Subscribe( FROM_FUNC(void(), say_something ));
doo::DelegateID id2 = event.Subscribe( FROM_FUNC(void(), say_something_else ));
doo::DelegateID id3 = event.Subscribe( FROM_METHOD(void(),something,ss)); // First the function type, then the function itself, then the object that the function belongs to.
doo::DelegateID id4 = event.Subscribe( FROM_FUNCTOR(void(), [](){std::cout << "Hello from Lambda" << std::endl; } ) );

doo::Event<int> event2;
event2.Subscribe(FROM_FUNC(void(int), print_num));
```

Unsubscribing:

```C++
event.Unsubscribe(id1);
// etc.
```

Memory Management:
```C++
doo::EventGuard<> guard = event.CreateGuard(id2); 
```
If "guard" gets out of scope it will automatically unsubscribe from the associated event.
Note that there are no checks if event is already destroyed, so use this only if you know that lifetime(guard) < lifetime(event)!
I might change that in the future to provide full memory safety. 
