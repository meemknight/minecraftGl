#pragma once
#include <Windows.h>
#include <assert.h>

// /////////////////
// Pinned vector
// Define PINNED_VECTOR_MEMORY_CHECK to check if acces deleted memory areas
// Define _DEBUG to check in bounds acces
// https://github.com/meemknight/pinnedVector
// licensed under MIT license, do not remove this notice https://github.com/meemknight/pinnedVector/blob/master/LICENSE
// /////////////////


// this check does not release memory, but rather ivalidates it so it would
// fail on acces. Can consume a lot of memory due to this reason
//#define PINNED_VECTOR_MEMORY_CHECK

#ifdef _DEBUG

#define PINNED_VECTOR_DEBUG(x) x
#define PINNED_VECTOR_ASSERT(x) assert(x)

#else

#define PINNED_VECTOR_DEBUG(x) 
#define PINNED_VECTOR_ASSERT(x) 

#endif // DEBUG


template <class T, unsigned int maxElCount = 12000>
struct PinnedVector
{
	T *beg = 0;
	unsigned int size = 0;

	typedef T* iterator;
	typedef const T* constIterator;

	iterator begin() { return &((T*)beg)[0]; }
	constIterator begin() const { return &((T*)beg)[0]; }
	iterator end() { return &((T*)beg)[size]; }
	constIterator end() const { return &((T*)beg)[size]; }

	static constexpr unsigned int maxSize = maxElCount;

	PinnedVector()
	{
		initializeArena();
	}

	PinnedVector(PinnedVector &&other)
	{
		this->beg = other.beg;
		this->size = other.size;

		other.size = 0;
		other.beg = nullptr;

		//?
		other.initializeArena();
	}

	PinnedVector(PinnedVector &other)
	{
		initializeArena();
		unsafeResize(other.size);

		memcpy(beg, other.beg, size * sizeof(T));
	}

	void initializeArena();

	void unsafeResize(unsigned int elementCount);

	void resize(unsigned int elementCount);

	void reserve(unsigned int elementCount);

	T *data();

	PinnedVector &operator= (const PinnedVector& other)
	{
		this->unsafeResize(other.size);
		memcpy(beg, other.beg, size * sizeof(T));
		return *this;
	}

	PinnedVector &operator= (const PinnedVector&& other)
	{
		free();
		this(std::move(other));
		return *this;
	}

	T &operator[] (unsigned int index)
	{
		PINNED_VECTOR_ASSERT(index < size);
		return static_cast<T*>(beg)[index];
	}

	T operator[] (unsigned int index) const
	{
		PINNED_VECTOR_ASSERT(index < size);
		return static_cast<T*>(beg)[index];
	}

	void push_back(const T& el);
	void pop_back();

	void clear();

	void free();

	~PinnedVector();
};

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::initializeArena()
{
	if (beg)
	{
		free();
	}

	beg = (T*)VirtualAlloc(0, sizeof(T) * maxElCount, MEM_RESERVE, PAGE_READWRITE);
	PINNED_VECTOR_ASSERT(beg != nullptr);

}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::unsafeResize(unsigned int elementCount)
{
	if (!beg)
	{
		initializeArena();
	}

	if (elementCount > size)
	{
		VirtualAlloc(beg, elementCount * sizeof(T), MEM_COMMIT, PAGE_READWRITE);
		size = elementCount;
	}
	else
	{
		size = elementCount;
	}

}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::resize(unsigned int elementCount)
{
	if (!beg)
	{
		initializeArena();
	}

	if (elementCount > size)
	{
		VirtualAlloc(beg, elementCount * sizeof(T), MEM_COMMIT, PAGE_READWRITE);
		for (int i = size; i < elementCount; i++)
		{
			beg[i] = T();
		}
		size = elementCount;
	}
	else
	{
		for (int i = elementCount; i < size; i++)
		{
			beg[i].~T();
		}
		elementCount = size;
	}
}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::reserve(unsigned int elementCount)
{
	VirtualAlloc(beg, elementCount * sizeof(T), MEM_COMMIT, PAGE_READWRITE);
}

template<class T, unsigned int maxElCount>
inline T * PinnedVector<T, maxElCount>::data()
{
	return beg;
}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::push_back(const T & el)
{
	unsafeResize(size + 1);
	beg[size - 1] = el;
}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::pop_back()
{
	///todo
	beg[size - 1].~T();
	unsafeResize(size - 1);
}

template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::clear()
{
	for (int i = 0; i < size; i++)
	{
		beg[i].~T();
	}
	size = 0;
}

//todo add commited size
template<class T, unsigned int maxElCount>
inline void PinnedVector<T, maxElCount>::free()
{

	for (int i = 0; i < size; i++)
	{
		beg[i].~T();
	}

#ifdef PINNED_VECTOR_MEMORY_CHECK

	if (beg)
	{
		//VirtualFree(beg, maxElCount * sizeof(T), MEM_DECOMMIT);
		DWORD last;
		VirtualProtect(beg, sizeof(T) * maxElCount, PAGE_NOACCESS, &last);
	}

#else

	if (beg)
	{
		VirtualFree(beg, 0, MEM_RELEASE);
	}

	size = 0;

#endif // PINNED_VECTOR_MEMORY_CHECK

}

template<class T, unsigned int maxElCount>
inline PinnedVector<T, maxElCount>::~PinnedVector()
{
	free();
}
