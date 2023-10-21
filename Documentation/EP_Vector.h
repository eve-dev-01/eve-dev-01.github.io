#pragma once


#include "EP_Numerics.h"
#include "EP_Memory.h"
#include "EP_Assert.h"
#include "EP_ScratchAllocator.h"

#include <cstring>

namespace ep
{
	////////////////////////////////////////////////////////////////////////////////
	// Class definition
	template <typename T, typename AllocatorT = EP_DefaultAllocator>
	class vector
	{
	public:
		////////////////////////////////////////////////////////////////////////////////
		// iterator functions
		// we can use T directly for a vector iterator
		typedef T* iterator;
		typedef const T* const_iterator;
		class reverse_iterator
		{
			friend class vector<T, AllocatorT>;
		public:
			reverse_iterator(const T*);
			reverse_iterator operator=(reverse_iterator other);
			bool operator==(reverse_iterator other);
			bool operator!=(reverse_iterator other);

			operator T* ();
			T& operator *();

			void operator ++();
			void operator --();
		private:
			T* curr_;
		};



	public:

		////////////////////////////////////////////////////////////////////////////////
		// main vector functions
		vector();
		vector(uint32 capacity);
		vector(const ep::vector<T, AllocatorT>& copy);
		vector(ep::vector<T, AllocatorT>&& move);
		ep::vector<T, AllocatorT>& operator=(const ep::vector<T, AllocatorT>& copy);
		~vector();

		uint32 capacity() const;
		uint32 size() const;

		T& operator[](uint32 idx) const;

		// Get pointer to start of contiguous data
		T* data() const;

		T& front() const;
		T& back() const;
		iterator begin() const;
		iterator end() const;

		iterator erase(iterator);

		void push_back(const T&);
		void pop_back();
		void clear();

		// create space for this many elements (minimum)
		void resize(uint32 elCount);
		// allocate space for this many elements (minimum) but do not increase size
		void reserve(uint32 elCount);

	private:
		AllocatorT allocator;
		uint64 dataSize_ = 0;						// number of elements used

		// This will check if we need to realloc memory such that later resizes will not require a realloc
		void internal_reserve(uint32 newSize);
		// This will map pages from pre-reserved memory if there is enough; if not enough
		// will make more
		void internal_resize(uint32 newSize);
		// This will free any allocated memory and unmap any mapped mem
		void internal_free();


		const static uint32 GROW_BY = 16;
	};

	///////////////////////////////////////////////////////////////////////////////////
	// Reverse iterator functions definitions 
#pragma region reverse_iterator
	template <typename T, typename AllocatorT>
	vector<T, AllocatorT>::reverse_iterator::reverse_iterator(const T* v)
	{
		curr_ = v;
	}
	template <typename T, typename AllocatorT>
	typename vector<T, AllocatorT>::reverse_iterator vector<T, AllocatorT>::reverse_iterator::operator=(reverse_iterator other)
	{
		curr_ = other.curr_;
	}
	template <typename T, typename AllocatorT>
	bool vector<T, AllocatorT>::reverse_iterator::operator==(reverse_iterator other)
	{
		return other.curr_ == curr_;
	}
	template <typename T, typename AllocatorT>
	bool vector<T, AllocatorT>::reverse_iterator::operator!=(reverse_iterator other)
	{
		return other.curr_ != curr_;
	}

	template <typename T, typename AllocatorT>
	vector<T, AllocatorT>::reverse_iterator::operator T* ()
	{
		return curr_;
	}
	template <typename T, typename AllocatorT>
	T& vector<T, AllocatorT>::reverse_iterator::operator *()
	{
		return *curr_;
	}

	template <typename T, typename AllocatorT>
	void vector<T, AllocatorT>::reverse_iterator::operator ++()
	{
		++curr_;
	}
	template <typename T, typename AllocatorT>
	void vector<T, AllocatorT>::reverse_iterator::operator --()
	{
		--curr_;
	}
#pragma endregion reverse_iterator


	///////////////////////////////////////////////////////////////////////////////////
	// Vector functions definitions 
	template <typename T, typename AllocatorT>
	vector<T, AllocatorT>::vector()
	{
	}
	template <typename T, typename AllocatorT>
	vector<T, AllocatorT>::vector(uint32 capacity)
	{
		internal_reserve(capacity);
	}
	template <typename T, typename AllocatorT>
	vector<T, AllocatorT>::vector(const ep::vector<T, AllocatorT>& copy)
	{
		operator=(copy);
	}
	template <typename T, typename AllocatorT>
	vector<T, AllocatorT>::vector(ep::vector<T, AllocatorT>&& move)
	{
		// clean our own house first
		if (data())
			internal_free();

		// steal their buffer !
		allocator = move.allocator;
		dataSize_ = move.dataSize_;

		move.allocator = AllocatorT();
		move.dataSize_ = 0;
	}
	template <typename T, typename AllocatorT>
	vector<T, AllocatorT>& vector<T, AllocatorT>::operator=(const ep::vector<T, AllocatorT>& copy)
	{
		if (allocator.data())
			internal_free();

		resize(copy.size());
		memcpy(allocator.data(), copy.data(), sizeof(T) * size());
		return *this;
	}
	template <typename T, typename AllocatorT>
	vector<T, AllocatorT>::~vector()
	{
		internal_free();
	}

	template <typename T, typename AllocatorT>
	uint32 vector<T, AllocatorT>::capacity() const
	{
		uint64 mapSize = allocator.size();
		return (uint32)(mapSize / sizeof(T));
	}
	template <typename T, typename AllocatorT>
	uint32 vector<T, AllocatorT>::size() const
	{
		return (uint32)dataSize_;
	}

	template <typename T, typename AllocatorT>
	T& vector<T, AllocatorT>::operator[](uint32 idx) const
	{
		EP_ASSERT(idx < size());
		return *((T*)(allocator.data()) + idx);
	}


	template <typename T, typename AllocatorT>
	T* vector<T, AllocatorT>::data() const
	{
		return (T*)allocator.data();
	}

	template <typename T, typename AllocatorT>
	T& vector<T, AllocatorT>::front() const
	{
		return *((T*)allocator.data());
	}
	template <typename T, typename AllocatorT>
	T& vector<T, AllocatorT>::back() const
	{
		return *((T*)allocator.data() + (dataSize_ - 1));
	}

	template <typename T, typename AllocatorT>
	typename vector<T, AllocatorT>::iterator vector<T, AllocatorT>::begin() const
	{
		return (T*)allocator.data();
	}
	template <typename T, typename AllocatorT>
	typename vector<T, AllocatorT>::iterator vector<T, AllocatorT>::end() const
	{
		return (T*)allocator.data() + (dataSize_);
	}

	template <typename T, typename AllocatorT>
	typename vector<T, AllocatorT>::iterator vector<T, AllocatorT>::erase(iterator it)
	{
		uint32 idx = (uint32)(it - begin());
		EP_ASSERT(idx <= dataSize_);
		if (it != end())
		{
			memcpy(
				(T*)it,
				(T*)it + 1,
				sizeof(T) * (end() - (it + 1))
			);
		}
		--dataSize_;
		return it;
	}

	template <typename T, typename AllocatorT>
	void vector<T, AllocatorT>::push_back(const T& v)
	{
		if (dataSize_ == capacity())
		{
			internal_reserve(capacity() + 1);
		}
		T* pLoc = ((T*)allocator.data()) + dataSize_;
		*pLoc = v;
		++dataSize_;
	}
	template <typename T, typename AllocatorT>
	void vector<T, AllocatorT>::pop_back()
	{
		--dataSize_;
	}
	template <typename T, typename AllocatorT>
	void vector<T, AllocatorT>::clear()
	{
		dataSize_ = 0;
	}

	template <typename T, typename AllocatorT>
	void vector<T, AllocatorT>::resize(uint32 elCount)
	{
		internal_resize(elCount);
	}
	template <typename T, typename AllocatorT>
	void vector<T, AllocatorT>::reserve(uint32 elCount)
	{
		internal_reserve(elCount);
	}


	template <typename T, typename AllocatorT>
	void vector<T, AllocatorT>::internal_reserve(uint32 newNumEls)
	{
		uint64 newSize = newNumEls * sizeof(T);
		allocator.Alloc(newSize);
	}
	template <typename T, typename AllocatorT>
	void vector<T, AllocatorT>::internal_resize(uint32 newNumEls)
	{
		// may already have allocated but unmapped space
		internal_reserve(newNumEls);
		dataSize_ = newNumEls;
	}
	template <typename T, typename AllocatorT>
	void vector<T, AllocatorT>::internal_free()
	{
		allocator.Dealloc();
		dataSize_ = 0;
	}

	// Make an easy use temp_vector
	template <typename T>
	using temp_vector = vector<T, EP_ScratchAllocator>;


}