#ifndef AL_SIGNALS_H
#define AL_SIGNALS_H

/*
https://github.com/cpp11nullptr/lsignal - single header, thread-safe
https://github.com/fr00b0/nod - single header, thread-safe
https://github.com/vdksoft/signals -  single header, thread-safe, auto-disconnection, option for upfront allocation
*/

#include "vdk-signals/signals.h"
/*
Quick signals usage:

vdk::signal<void()> sig; 
vdk::signal<void(void)> sig; // same
vdk::signal<int(int, int)> sig2;
vdk::signal<void()> sig(2); // reserve 2 slots initially (default is 5)

sig.connect(&fun); // connect C function
sig.connect(&functor); // Connects function object
sig.connect(&SomeClass::static_method); // Connects static method
sig.connect(&object, &SomeClass::method); // Connects method
sig.connect(&sig2);

auto lambda = [](int arg)->void { ...} 
sig.connect(&lambda); // Connects lambda

sig.emit(<args>);

bool sig.connected(functor_ptr); // is it connected?
sig.size();
sig.empty();

sig.block(); // Equivalent to sig.block(true);
sig.block(false); 

sig.disconnect(&SomeClass::static_method);
sig.disconnect_all();


// auto-disconnect via shared_ptr:

std::shared_ptr<SomeClass> object_ptr(new SomeClass);
sig.connect(object_ptr, &SomeClass::method);
sig.emit(10);
object_ptr.reset();
sig.emit(10);
*/

#endif //AL_SIGNALS_H