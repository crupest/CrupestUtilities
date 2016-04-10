#include <iostream>

#include "Event.h"

using namespace cru;
using namespace std;

class TestClass
{

};

class TestEvent : EnableEventMap(TestClass, TestEvent)
{
public:
    using Event<TestClass>::Event;
};

int main()
{
    TestClass object;
    EventHandler<TestEvent> event_handler(&object,
        [](auto)
    {
        cout << "Event handler is invoked!" << endl;
    });

    SendEvent<TestEvent>(&object);

    getchar();

    return 0;
}
