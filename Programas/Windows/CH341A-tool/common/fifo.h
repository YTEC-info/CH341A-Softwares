#ifndef FifoH
#define FifoH

template<class T, int Size>
class Fifo {
private:
	volatile int m_Read;
	volatile int m_Write;
	volatile T m_Data[Size];

public:
	Fifo()
	{
		m_Read = 0;
		m_Write = 0;
	}

	T* getWriteable(void) {
		int nextElement = (m_Write + 1) % Size;
		if(nextElement != m_Read) {
			return const_cast<T*>(&m_Data[m_Write]);
		} else {
			return NULL;
		}		
	}

	bool push(void) {
		int nextElement = (m_Write + 1) % Size;
		if(nextElement != m_Read) 
		{
			m_Write = nextElement;
			return true;
		}
		else
			return false;
	}

	T* getReadable(void) {
		if(m_Read == m_Write)
			return NULL;
		return const_cast<T*>(&m_Data[m_Read]);
	}

	bool pop(void) {
		if(m_Read == m_Write)
			return false;
		int nextElement = (m_Read + 1) % Size;
		m_Read = nextElement;
		return true;
	}
};

#endif

