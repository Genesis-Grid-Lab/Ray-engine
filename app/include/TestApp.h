#pragma once
#include "Core/Application.h"
#include "TestLayer.h"

class TestApp : public RE::Application {
public:
    TestApp()
        :RE::Application("TestApp", {1280, 900})
        {
            PushLayer(new TestLayer());
        }

    ~TestApp(){}
};