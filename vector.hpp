#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>
// Implementation of std::vector from the C++ STL.

template<typename T>
class vector
{
private:
	std::unique_ptr<T[]> m_buffer;
	std::size_t m_vec_size;
	std::size_t m_vec_capacity;

	//~vector() = default;
private:
	// inline function, avoids having to make direct std::make_unique calls.
	inline void realloc_vector(const std::size_t n) noexcept
	{
		m_buffer = std::make_unique<T[]>(n);
	}
	inline void swap(vector<T>& vec) noexcept { std::swap(m_buffer, vec.m_buffer); }
public:
	explicit vector(const std::size_t vec_capacity) noexcept : m_vec_capacity(vec_capacity), m_vec_size(0)
	{
		m_buffer = std::make_unique<T[]>(vec_capacity);
	}
	// copy constructor, copys x elements into another vector.
	const explicit vector(const vector<T>& vec) noexcept
	{
		// copy vector size and capacity.
		this->m_vec_size = vec.m_vec_size;
		this->m_vec_capacity = vec.m_vec_capacity;

		// allocate new unique_ptr buffer.
		realloc_vector(m_vec_capacity);

		// copy elements into new vector.
		//std::copy(this->m_buffer.get(), vec.m_buffer.get(), this->size());
	}
	// move constructor, moves the members and then switches places with the old vector.
	const explicit vector(vector<T>&& vec) noexcept
	{
		// move basic members.
		m_vec_size = std::move(vec.m_vec_size);
		m_vec_capacity = std::move(vec.m_vec_capacity);

		// move buffer.
		m_buffer = std::move(vec.m_buffer);

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
	// size and capacity set to 0 by default constructor
	explicit vector() noexcept : m_vec_size(0), m_vec_capacity(0) {};
public:
	inline T& operator[](const std::size_t element_index) const noexcept { return this->m_buffer[element_index]; }
public:
	// capacity functions

	// Returns x amount of elements existing in the dynamic-array.
	inline std::size_t size() const noexcept { return m_vec_size; }

	// returns vector capacity
	constexpr std::size_t capacity() const noexcept { return m_vec_capacity; }

	inline bool empty() const noexcept { return m_vec_size == 0; }

	// reallocates vector to change it's capacity.
	void resize(const std::size_t n) noexcept
	{
		// reallocate memory
		realloc_vector(n);
		
		this->m_vec_capacity = n;
	}
	void push_back(const T& val) noexcept
	{
		
	}
	void push_back(const T&& val) noexcept {}

	// destroys the last element and decrements the size.
	void pop_back() noexcept
	{
		m_buffer[0] = 1;
	}
	void reserve(const std::size_t n) noexcept
	{
		if (n < this->m_vec_capacity)
			return;

		// this will hold the current buffer data.
		std::unique_ptr<T[]> temporary_buffer;

		// copy current buffer data into the new buffer.
		std::copy(this->m_buffer.get(), this->m_buffer.get() + this->size(), temporary_buffer.get());

		realloc_vector(n);

		this->m_vec_capacity = n;

		// vector has been reallocated, so we move the replacement_buffer data into the old buffer.
		std::copy(temporary_buffer.get(), temporary_buffer.get() + this->size(), this->m_buffer.get());

	}
	inline T* data() const noexcept { return this->m_buffer.get(); }
};

