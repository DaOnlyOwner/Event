#include <iostream>
#include "Event.h"

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
    std::cout << "Hello1" << std::endl;
}

void say_something_else()
{
    std::cout << "Hello2" << std::endl;
}


int main()
{

    /*test test1;
    int to_capture = 2;

    auto func_delegate = FROM_FUNC(int() , hallo);
    auto method_delegate = FROM_METHOD(int(), test, hallo2, test1);
    doo::Delegate<void()> capturing_lambda =  [to_capture](){std::cout << "From Capturing Lambda: " << to_capture << std::endl; }; // All classes that overload operator();

    func_delegate();
    method_delegate();
    capturing_lambda();*/

    something_sayer ss;

    doo::Event<> event;
    doo::DelegateID id1 = event.Subscribe( FROM_FUNC(void(), say_something ));
    doo::DelegateID id2 = event.Subscribe( FROM_FUNC(void(), say_something_else ));
    doo::DelegateID id3 = event.Subscribe( FROM_METHOD(void(),something_sayer,something,ss));
    doo::DelegateID id4 = event.Subscribe( FROM_FUNCTOR(void(), [](){std::cout << "Hello from Lambda" << std::endl; } ) );

    std::cout << std::endl;
    event();

    event.Unsubscribe(id1);

    std::cout << std::endl;

    event();
}