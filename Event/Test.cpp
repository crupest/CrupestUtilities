#include <iostream>

#include "Event.h"

using namespace cru;
using namespace std;

class TestClass
{
public:
    Event test_event;
};

class TestHandlerClass : EnableEventHandler
{
public:
    void fun() { cout << "Handled!" << endl; }
};

int main()
{
    TestClass test;
    test.test_event += [] { cout << "Hello world!!!" << endl; };
    SendEvent(test.test_event);

    auto handler = new TestHandlerClass;
    
    test.test_event += EventHandler(handler, &TestHandlerClass::fun);

    SendEvent(test.test_event);

    delete handler;

    SendEvent(test.test_event);

    getchar();

    return 0;
}
