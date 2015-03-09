/*
Copyright(C) thecodeway.com
*/

#ifndef _CYCLONE_CORE_RING_BUF_H_
#define _CYCLONE_CORE_RING_BUF_H_

#include <cyclone_config.h>

///
/// A byte-addressable ring buffer FIFO implementation.
///
///    +-------------------+------------------+------------------+
///    |  writable bytes   |  readable bytes  |  writable bytes  |
///    |                   |     (CONTENT)    |                  |
///    +-------------------+------------------+------------------+
///    |                   |                  |                  |
/// m_beginPoint <=     m_readIndex   <=   m_writeIndex    <=  m_endIndex
///
///  wrap
///    +-------------------+------------------+------------------+
///    |  readable bytes   |  writable bytes  |  readable bytes  |
///    |  (CONTENT PART2)  |                  | (CONTENT PART1)  |
///    +-------------------+------------------+------------------+
///    |                   |                  |                  |
/// m_beginPoint <=     m_writeIndex   <=   m_readIndex   <=  m_endIndex
///

namespace cyclone
{

class RingBuf
{
public:
	enum { kDefaultCapacity = 1024 - 1 };

	/// return the size of the internal buffer, in bytes.
	size_t size(void) const {
		return (m_write >= m_read) ? (m_write - m_read) : (m_end - m_read + m_write);
	}

	/// reset a ring buffer to its initial state (empty).
	void reset(void) {
		m_write = m_read = 0;
	}

	/// return the usable capacity of the ring buffer, in bytes.
	size_t capacity(void) const {
		return m_end-1;
	}

	//// return the number of free/available bytes in the ring buffer.
	size_t get_free_size(void) const {
		return (m_write >= m_read) ? (m_end - m_write + m_read - 1) : (m_read - m_write - 1);
	}

	//// return is empty
	bool empty(void) const {
		return (m_write == m_read);
	}

	/// return is full
	bool full(void) const {
		return get_free_size() == 0;
	}

	////  copy n bytes from a contiguous memory area src into the ring buffer
	void memcpy_into(const void *src, size_t count);

	//// copy n bytes from the ring buffer src into a contiguous memory area dst
	size_t memcpy_out(void *dst, size_t count);

	//// call read on the socket descriptor(fd), using the ring buffer rb as the 
	//// destination buffer for the read, and read as more data as impossible data.
	ssize_t read_socket(socket_t fd);

	//// call write on the socket descriptor(fd), using the ring buffer rb as the 
	//// source buffer for writing, In Linux platform, it will only call writev
	//// once, and may return a short count.
	ssize_t write_socket(socket_t fd);

public:
	RingBuf(size_t capacity = kDefaultCapacity);
	~RingBuf();

private:
	char* m_buf;
	size_t m_end;
	size_t m_read;
	size_t m_write;

private:
	void _auto_resize(size_t need_size);
};

}
#endif