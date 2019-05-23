#ifndef AL_THREAD_H
#define AL_THREAD_H

#include <thread>
#include <mutex>

#include "al_time.h"

/*
    void call_from_thread();
    
    std::thread t1(call_from_thread);
    t1.join();

    std::mutex mutex;
    mutex.lock(); // waits until mutex is available
    mutex.try_lock(); // does not block; returns true if acquired, false if not acquired
    mutex.unlock(); // releases mutex for others to acquire

    std::lock_guard<std::mutex> guard(mutex); // locks mutex until scope exits
*/

/*
    Simplify the process of running a function in a separate thread at a limited rate
*/
struct MetroThread {
    FPS fps;
    std::thread thread;
    std::function<void(double dt)> thefunction;
    bool isRunning = 0;

    MetroThread(double ticks_per_second) : fps(ticks_per_second) {}

    void begin(std::function<void(double dt)> thread_function) {
        if (isRunning) {
            //console.error("already running");
            return;
        }
        isRunning = 1;
		thefunction = thread_function;
        thread = std::thread([this](){
            while(isRunning) {
				thefunction(fps.dt);
                fps.sleep();
                fps.measure();
            }
        });
    }

    void end() {
        isRunning = 0;
        if (thread.joinable()) thread.join();
    }

    // returns the FPS that would be possible if the thread didn't sleep at all
    double potentialFPS() {
        return fps.fps / fps.performance;
    }

};

#endif //AL_THREAD_H
