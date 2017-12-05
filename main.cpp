#include <iostream>
#include "Event.hpp"


int hallo()
{
    std::cout << "From Function" << std::endl;
    return 2;
}
struct test
{
    int hallo2()
    {
        std::cout << "From Method" << std::endl;
        return 3;
    }
};

struct something_sayer
{
    void something()
    {
        std::cout << "Hello from the struct" << std::endl;
    }
};

void say_something()
{
    std::cout << "Hello1 from say_something" << std::endl;
}

void say_something_else()
{
    std::cout << "Hello2 From say_something_else" << std::endl;
}

void print_num(int num)
{
    std::cout << num;
}

int main()
{

    something_sayer ss;

    doo::Event<> event;
    doo::DelegateID id1 = event.Subscribe( FROM_FUNC(void(), say_something ));
    doo::DelegateID id2 = event.Subscribe( FROM_FUNC(void(), say_something_else ));
    doo::DelegateID id3 = event.Subscribe( FROM_METHOD(void(),something,ss));
    doo::DelegateID id4 = event.Subscribe( FROM_FUNCTOR(void(), [](){std::cout << "Hello from Lambda" << std::endl; } ) );

    std::cout << std::endl;
    event();
    
    event.Unsubscribe(id1);

    std::cout << std::endl;
    event();

    std::cout << "Test of EventGuard; Remove id2" << std::endl << std::endl;


    // If this guard gets out of scope the the delegate will automatically be removed from the Event. 
    // Example: Make this a member. When the object is destroyed the EventGuard will automatically care for the unsubscription of the delegate.
    // If you don't unsubscribe from the associated event then there will be nullptr exceptions etc.
    {
        doo::EventGuard<> guard = event.CreateGuard(id2); 
    }

    event();

    // Example of making an event that receives parameters
    doo::Event<int> event2;
    event2.Subscribe(FROM_FUNC(void(int), print_num));
    event2(2);

    return 0;
}