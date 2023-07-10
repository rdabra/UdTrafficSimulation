#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <mutex>

// forward declarations to avoid include cycle
class Vehicle;

enum class TrafficLightPhase { RED, GREEN };

// FP.3 Define a class „MessageQueue“ which has the public methods send and
// receive. Send should take an rvalue reference of type TrafficLightPhase
// whereas receive should return this type. Also, the class should define an
// std::dequeue called _queue, which stores objects of type TrafficLightPhase.
// Also, there should be an std::condition_variable as well as an std::mutex as
// private members.

class MessageQueue {
   private:
      std::deque<TrafficLightPhase> _queue;
      std::mutex _mtxMQ;
      std::condition_variable _cndMQ;

   public:
      TrafficLightPhase receive();
      void send(TrafficLightPhase &&msg);
};

// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject.
// The class shall have the public methods „void waitForGreen()“ and „void
// simulate()“ as well as „TrafficLightPhase getCurrentPhase()“, where
// TrafficLightPhase is an enum that can be either „red“ or „green“. Also, add
// the private method „void cycleThroughPhases()“. Furthermore, there shall be
// the private member _currentPhase which can take „red“ or „green“ as its
// value.

class TrafficLight : public TrafficObject {
   private:
      // typical behaviour methods

      // FP.4b : create a private member of type MessageQueue for messages of type
      // TrafficLightPhase and use it within the infinite loop to push each new
      // TrafficLightPhase into it by calling send in conjunction with move
      // semantics.
      MessageQueue _trafficLights;

      //      std::condition_variable _condition;
      //      std::mutex _mutex;

      TrafficLightPhase _currentPhase{TrafficLightPhase::RED};
      void cycleThroughPhases();

   public:
      // constructor / desctructor

      // getters / setters

      // typical behaviour methods
      void waitForGreen();
      void simulate() override;
      TrafficLightPhase getCurrentPhase() { return _currentPhase; };
};

#endif