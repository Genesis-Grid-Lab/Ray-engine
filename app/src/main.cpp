#include "TestApp.h"

int main(int argc, char** argv) {
    auto app = new TestApp();

    app->Run();

    delete app;

    return 0;  
}
