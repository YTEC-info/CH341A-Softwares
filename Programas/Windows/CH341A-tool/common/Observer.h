#ifndef ObserverH
#define ObserverH

class Observable;

class Argument
{
	virtual void dummy(void) {}
};

class Observer
{
public:
	// Called by the observed object, whenever
	// the observed object is changed:
	virtual void obsUpdate(Observable* o, Argument * arg) = 0;
};
#endif
