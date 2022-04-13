/*
 * alocador_de_memoria.h
 *
 *  Created on: 6 de abr de 2021
 *      Author: rgnagel
 */

#ifndef MEMORIA_ALOCADOR_DE_MEMORIA_H_
#define MEMORIA_ALOCADOR_DE_MEMORIA_H_

#define TAMANHO_MAX_RING_BUFFER 10*1024

#include <iostream>

class ring_allocator_base {
public:
	typedef std::size_t size_type;

protected:
	ring_allocator_base()
	{
	}
	static constexpr size_type buffer_size = TAMANHO_MAX_RING_BUFFER;
	static void* do_allocate(const size_type size)
	{
		static std::uint8_t buffer[buffer_size];
		static std::uint8_t *get_ptr = buffer;

		std::uint8_t *p = get_ptr;

		get_ptr += size;

		const bool is_wrap = (get_ptr > (buffer + buffer_size));

		if (is_wrap) {
			p = buffer;
			get_ptr = buffer + size;
		}

		return static_cast<void*>(p);
	}
};

template<typename T>
class AlocadorRing: public ring_allocator_base {
public:
	typedef T value_type;
	typedef value_type *pointer;
	typedef const value_type *const_pointer;

	size_type max_size() const noexcept
	{
		return buffer_size / sizeof(value_type);
	}
	pointer allocate(size_type count, const_pointer =
			nullptr)
	{
		void *p = do_allocate(count * sizeof(value_type));

		return static_cast<pointer>(p);
	}

	void deallocate(pointer, size_type)
	{
	}
};


#endif /* MEMORIA_ALOCADOR_DE_MEMORIA_H_ */
