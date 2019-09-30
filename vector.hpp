#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>
#include <math.h>
// Implementation of std::vector from the C++ STL.

template<typename T>
class vector
{
private:
	std::unique_ptr<T[]> m_buffer;
	std::size_t m_vec_size;
	std::size_t m_vec_capacity;

	using iterator = T*;

	//~vector() = default;
private:
	// inline function, avoids having to make direct std::make_unique calls.
	inline void realloc_vector(const std::size_t n) noexcept
	{
		m_buffer = std::make_unique<T[]>(n);
	}
	inline void swap(vector<T>& vec) noexcept { std::swap(m_buffer, vec.m_buffer); }
public:
	explicit vector(const std::size_t vec_capacity) noexcept : 
		m_vec_capacity(vec_capacity), 
		m_vec_size(0)
	{
		m_buffer = std::make_unique<T[]>(vec_capacity);
	}
	// size and capacity set to 0 by default constructor
	explicit constexpr vector() noexcept : 
		m_vec_size(0), 
		m_vec_capacity(0), 
		m_buffer(nullptr) {};

	vector(const std::initializer_list<T>& init) noexcept(false) : 
		m_vec_capacity(init.size()),
		m_vec_size(init.size())
	{
		m_buffer = std::make_unique<T[]>(init.size());

		std::copy(init.begin(), init.end(), m_buffer.get());
	}
	// copy constructor, copys x elements into another vector.
	const explicit vector(const vector<T>& vec) noexcept : 
		m_vec_size(vec.m_vec_size), // copy vector size
		m_vec_capacity(vec.m_vec_capacity) // copy vector capacity
	{
		// allocate new unique_ptr buffer.
		realloc_vector(m_vec_capacity);

		// copy elements into new vector.
		std::copy(vec.m_buffer.get(), vec.m_buffer.get() + vec.size(), this->m_buffer.get());
	}
	// move constructor, moves the members and then switches places with the old vector.
	explicit vector(vector<T>&& vec) noexcept : 
		m_vec_capacity(0), 
		m_vec_size(0)
	{
		// move basic members.
		m_vec_size = std::move(vec.m_vec_size);
		m_vec_capacity = std::move(vec.m_vec_capacity);

		// move buffer.
		//m_buffer = std::move(vec.m_buffer);

		// swap the moving vector.
		this->swap(*this);
	}
	// copy assignment operator, copys x elements into another vector.
	const vector& operator= (const vector<T>& vec) noexcept
	{
		this->m_vec_size = vec.m_vec_size;
		this->m_vec_capacity = vec.m_vec_capacity;

		// allocate new unique_ptr buffer.
		realloc_vector(m_vec_capacity);

		std::copy(vec.m_buffer.get(), vec.m_buffer.get() + vec.size(), this->m_buffer.get());
	}
	// move assignment operator, moves the members and then switches places with the old vector.
	const vector& operator= (vector<T>&& vec) noexcept
	{
		// move basic members.
		m_vec_size = std::move(vec.m_vec_size);
		m_vec_capacity = std::move(vec.m_vec_capacity);

		// move buffer.
		m_buffer = std::move(vec.m_buffer);

		// swap the moving vector.
		this->swap(*this);

		return *this;
	}
public:
	inline T& operator[](const std::size_t element_index) const noexcept { return this->m_buffer[element_index]; }
public:
	// capacity functions

	// Returns x amount of elements existing in the dynamic-array.
	constexpr std::size_t size() const noexcept { return m_vec_size; }

	constexpr std::size_t max_size() const noexcept { return std::pow(2, 32) - 1; }
	
	// returns vector capacity
	constexpr std::size_t capacity() const noexcept { return m_vec_capacity; }

	constexpr bool empty() const noexcept { return m_vec_size == 0; }

	iterator begin() const noexcept { return m_buffer.get(); }

	iterator end() const noexcept { return m_buffer.get() + this->size(); }

	//iterator erase() noexcept {}

	// reallocates vector to change it's capacity.
	void resize(const std::size_t n) noexcept
	{
		// reallocate memory
		reserve(n);
	}
	void push_back(const T& val) noexcept
	{
		// reserve vector if we exceed capacity (this will reallocate the buffer).
		if (this->size() >= this->capacity())
			reserve(this->capacity() + 1);

		m_buffer[m_vec_size++] = val;
	}
	void push_back(const T&& val) noexcept
	{
		if (this->size() >= this->capacity())
			reserve(this->capacity() + 1);

		m_buffer[m_vec_size++] = std::move(val);
	}

	// destroys the last element and decrements the size.
	void pop_back() noexcept
	{
		if (this->empty())
			return;

		std::fill(this->m_buffer.get(), this->m_buffer.get() + this->size(), 0);

		m_vec_size--;
	}
	// Inserts a new element before pos, args are forwarded to buffer pos.
	template<typename... Args>
	iterator emplace(const iterator pos, Args&&... args) noexcept
	{
		if (this->capacity() >= this->size())
			reserve(this->size() + 1);

		// acquire a pointer to the last position of the buffer.
		iterator pos = &m_buffer[pos - m_buffer];

		// forward arguments to T.
		*pos = T(std::forward<Args>(args)...);

		m_vec_size++;
		
		return pos;
	}
	// inserts a new element at the end of buffer, args are forwarded to T.
	template<typename... Args>
	void emplace_back(Args&&... args) noexcept
	{
		if (this->capacity() >= this->size())
			reserve(this->size() + 1);
		
		m_buffer[m_vec_size++] = T(std::forward<Args>(args)...);
	}
	template<typename... Args>
	T& emplace_back(Args&& ... args) noexcept
	{
		if (this->capacity() >= this->size())
			reserve(this->size() + 1);

		return m_buffer[m_vec_size++] = T(std::forward<Args>(args)...);
	}
	// reallocates vector to match the capacity to element size.
	void shrink_to_fit() noexcept
	{
		if (this->capacity() >= this->size())
			reserve(this->size());
	}
	T& at(const std::size_t n) const noexcept(false)
	{
		if (n > this->size())
			throw std::out_of_range("Vector element position is out of bounds");

		return m_buffer[n];
	}
	T& front() const noexcept { return m_buffer[0]; }

	T& back() const noexcept { return m_buffer[this->size() - 1]; }

	// inserts x at either the start or end of the buffer.
	void insert(const iterator pos, const T& val) noexcept
	{
		
		
		m_buffer[pos + 1] = val;
	}
	void insert(const iterator pos, const T&& val) noexcept
	{
		

		//m_buffer[n] = std::move(val);
	}
	
	void assign(const std::size_t n, const T& val) noexcept
	{
		realloc_vector(n);

		
	}
	void assign(const std::initializer_list<T>& il) noexcept(false)
	{
		realloc_vector(il.size());

		std::copy(il.begin(), il.end(), this->m_buffer.get());

		this->m_vec_size = il.size();

		this->m_vec_capacity = il.size();
	}
	void assign(iterator first, iterator last) noexcept
	{
		
	}

	void reserve(const std::size_t n) noexcept(false)
	{
		// this will hold the current buffer data.
		std::unique_ptr<T[]> temporary_buffer = std::make_unique<T[]>(n);

		// copy current buffer data into the new buffer.
		std::copy(this->m_buffer.get(), this->m_buffer.get() + this->size(), temporary_buffer.get());

		realloc_vector(n);

		this->m_vec_capacity = n;

		// vector has been reallocated, so we move the replacement_buffer data into the old buffer.
		std::copy(temporary_buffer.get(), temporary_buffer.get() + this->size(), this->m_buffer.get());

	}
	void erase(const std::size_t n) noexcept
	{

	}

	void clear() noexcept
	{
		std::fill_n(m_buffer.get(), this->size(), 0);
		
		this->m_vec_size = 0;
	}

	inline T* data() const noexcept { return this->m_buffer.get(); }
};

template<typename T>
bool operator== (const vector<T>& lhs, const vector<T>& rhs) noexcept
{
	return lhs == rhs;
}
template<typename T>
bool operator!= (const vector<T>& lhs, const vector<T>& rhs) noexcept
{
	return lhs != rhs;
}
template<typename T>
bool operator< (const vector<T>& lhs, const vector<T>& rhs) noexcept
{
	return lhs < rhs;
}
