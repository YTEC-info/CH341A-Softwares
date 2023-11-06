#ifndef ObservableH
#define ObservableH

#include "Observer.h"
#include <set>

class Observable
{
    bool changed;
    std::set<Observer*> observers;
protected:
    virtual void setChanged()
    {
        changed = true;
    }
    virtual void clearChanged()
    {
        changed = false;
    }
public:
    virtual void addObserver(Observer& o)
    {
        observers.insert(&o);
    }
    virtual void deleteObserver(Observer& o)
    {
        observers.erase(&o);
    }
    virtual void deleteObservers()
    {
        observers.clear();
    }
    virtual int countObservers()
    {
        return observers.size();
    }
    virtual bool hasChanged()
    {
        return changed;
    }
    // If this object has changed, notify all of its observers:
	virtual void notifyObserversIfChanged(Argument* arg = NULL)
	{
		if(!hasChanged()) return;
		notifyObservers(arg);
		clearChanged(); // Not "changed" anymore
	}
	/** \brief Notify observers regardless of changed flag
	*/
	virtual void notifyObservers(Argument* arg = NULL)
	{
		std::set<Observer*>::iterator it;
		for(it = observers.begin();
				it != observers.end(); ++it)
			(*it)->obsUpdate(this, arg);
	}
};
#endif

