//Max Hofmeyer & Ahmed Malik | EGRE 591 | 02/23/2024

#pragma once
#include "subscriber.h"
#include <list>

//https://www.youtube.com/watch?v=Th1A2szPctI&ab_channel=Code%2CTech%2CandTutorials
// A pattern to allow notifications to be sent to anyone who subscribes.
// Overall the point is to not have users manually check something,
// but rather be sent a notification whenever a certain event happens 
// to a subject. 
class SubjectInterface {
public:
    virtual ~SubjectInterface() = default;
    virtual void Add(SubscriberInterface* sub) = 0;
    virtual void Remove(SubscriberInterface* sub) = 0;
    virtual void Notify(const token& tok) = 0;
};

class Subject : public SubjectInterface {
public:
    void Add(SubscriberInterface* sub) override { _subs.push_back(sub); }
    void Remove(SubscriberInterface* sub) override { _subs.remove(sub); }
    void Notify(const token& tok){
        for (auto* sub : _subs) {
            sub->Update(tok);
        }
    }

private:
    std::list<SubscriberInterface*> _subs;
};