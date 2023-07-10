#include "TrafficLight.h"
#include <algorithm>
#include <iostream>
#include <mutex>
#include <random>

using namespace std::chrono;

long generateRandomNumber(long lowerBound, long upperBound) {
   std::uniform_int_distribution<long> dist(lowerBound, upperBound);
   std::mt19937 rng(std::random_device{}());
   return dist(rng);
}

TrafficLightPhase MessageQueue::receive() {
   // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait()
   // to wait for and receive new messages and pull them from the queue using move semantics.
   // The received object should then be returned by the receive function.

   std::unique_lock<std::mutex> ul{_mtxMQ};
   _cndMQ.wait(ul, [this]() -> bool { return !this->_queue.empty(); });
   TrafficLightPhase firstInLph{std::move(_queue.back())};
   _queue.pop_back();

   return firstInLph; // relying on Return Value Optimization (RVO) for copy elision
}

void MessageQueue::send(TrafficLightPhase &&msg) {
   // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex>
   // as well as _condition.notify_one() to add a new message to the queue and afterwards send a
   // notification.
   std::lock_guard<std::mutex> lg{_mtxMQ};
   _queue.push_back(std::move(msg));
   _cndMQ.notify_one();
}

/* Implementation of class "TrafficLight" */

void TrafficLight::waitForGreen() {
   // FP.5b : add the implementation of the method waitForGreen, in which an infinite while - loop
   // runs and repeatedly calls the receive function on the message queue.
   // Once it receives TrafficLightPhase::green, the method returns.
   while (true) {
      if (_trafficLights.receive() == TrafficLightPhase::GREEN)
         return;
   }
}

void TrafficLight::simulate() {
   // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when
   // the public method „simulate“ is called. To do this, use the thread queue in the base class.
   this->threads.emplace_back(&TrafficLight::cycleThroughPhases, this);
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {
   // FP.2a : Implement the function with an infinite loop that measures the time
   // between two loop cycles and toggles the current phase of the traffic light
   // between red and green and sends an update method to the message queue using
   // move semantics. The cycle duration should be a random value between 4 and 6
   // seconds. Also, the while-loop should use std::this_thread::sleep_for to
   // wait 1ms between two cycles.

   long durationLimit = generateRandomNumber(4000, 6000);
   time_point<system_clock> initTime{system_clock::now()};
   while (true) {
      if (duration_cast<milliseconds>(system_clock::now() - initTime).count() > durationLimit) {
         if (_currentPhase == TrafficLightPhase::GREEN)
            _currentPhase = TrafficLightPhase::RED;
         else
            _currentPhase = TrafficLightPhase::GREEN;
         TrafficLightPhase lph{_currentPhase};
         _trafficLights.send(std::move(lph));
         std::this_thread::sleep_for(milliseconds(1));
         durationLimit = generateRandomNumber(4000, 6000);
         initTime = system_clock::now();
      }
   }
}
