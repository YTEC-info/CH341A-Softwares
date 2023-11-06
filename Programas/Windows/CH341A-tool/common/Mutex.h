#ifndef MutexH
#define MutexH

class Mutex
{
public:
	Mutex () { InitializeCriticalSection(&cs); }
	~Mutex () { DeleteCriticalSection(&cs); }
	// Waits for ownership of the specified critical section object.
	// The function returns when the calling thread is granted ownership.
	inline void lock () { EnterCriticalSection(&cs); }
	inline void unlock () { LeaveCriticalSection(&cs);	}	
private:
	CRITICAL_SECTION cs;
	Mutex(const Mutex&);
	Mutex& operator = (const Mutex&);
};

#endif
