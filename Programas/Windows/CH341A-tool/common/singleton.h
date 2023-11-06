/** \file	singleton.h
	\brief	Singleton object creational pattern template
*/

/*
	Usage:

	class CSth : public CSingleton<CSth>
	{
		friend CSingleton<CSth>;
		private:
			CSth();
			~CSth();
		...
	}

*/

#ifndef SingletonH
#define SingletonH

template <typename T>
class CSingleton
{
public:

	static T* Instance()
    {
		if (instance == NULL) instance = new T;
		return instance;
	};
	static Destroy()
	{
		delete instance;
		instance = NULL;
	};
protected:
	CSingleton() {};
	virtual ~CSingleton() {};
private:
	CSingleton(const CSingleton& source) {
	};
	CSingleton& operator=(const CSingleton&);
	static T* instance;
};

// static initialization
template <typename T> T* CSingleton<T>::instance = NULL;


#endif 