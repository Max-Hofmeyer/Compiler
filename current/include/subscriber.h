#pragma once
#include "tokens.h"
#include <iostream>

class SubscriberInterface {
public:
    virtual ~SubscriberInterface() = default;
    friend class Subject;
protected:
    virtual void Update(const token& tok) = 0;
};

class Subscriber : public SubscriberInterface {
public:
    Subscriber() {
        _unique_id = ++_unique_subscribers;
    }

    ~Subscriber() {}

protected:
    void Update(const token& tok) override {
        std::cout << "SUBID: \"" << _unique_id << "\" recieved message -> " << "sdfsdf" << std::endl;
    };

private:
    int _unique_id = -1;
    inline static int _unique_subscribers = 0;
};
