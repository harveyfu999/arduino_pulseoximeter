/*
 CircularBuffer.h - Circular buffer library for Arduino.
 Copyright (c) 2017 Roberto Lo Giacco.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as 
 published by the Free Software Foundation, either version 3 of the 
 License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __CIRCULAR_BUFFER__
#define __CIRCULAR_BUFFER__
#include <inttypes.h>

#ifndef CIRCULAR_BUFFER_XS
#define __CB_ST__ uint16_t
#else
#define __CB_ST__ uint8_t
#endif

#ifdef CIRCULAR_BUFFER_DEBUG
#include <Print.h>
#endif

template<typename T, __CB_ST__ S> class CircularBuffer {
public:

	CircularBuffer();

	~CircularBuffer();

	/**
	 * Adds an element to the beginning of buffer: the operation returns `false` if the addition caused overwriting an existing element.
	 */
	bool unshift(T value);

	/**
	 * Adds an element to the end of buffer: the operation returns `false` if the addition caused overwriting an existing element.
	 */
	bool push(T value);

	/**
	 * Removes an element from the beginning of the buffer.
	 */
	T shift();

	/**
	 * Removes an element from the end of the buffer.
	 */
	T pop();

	/**
	 * Returns the element at the beginning of the buffer.
	 */
	T inline first();

	/**
	 * Returns the element at the end of the buffer.
	 */
	T inline last();

	/**
	 * Array-like access to buffer
	 */
	T operator [] (__CB_ST__ index);

	/**
	 * Returns how many elements are actually stored in the buffer.
	 */
	__CB_ST__ inline size();

	/**
	 * Returns how many elements can be safely pushed into the buffer.
	 */
	__CB_ST__ inline available();

	/**
	 * Returns how many elements can be potentially stored into the buffer.
	 */
	__CB_ST__ inline capacity();

	/**
	 * Returns `true` if no elements can be removed from the buffer.
	 */
	bool inline isEmpty();

	/**
	 * Returns `true` if no elements can be added to the buffer without overwriting existing elements.
	 */
	bool inline isFull();

	/**
	 * Resets the buffer to a clean status, dropping any reference to current elements
	 * and making all buffer positions available again.
	 */
	void inline clear();

	#ifdef CIRCULAR_BUFFER_DEBUG
	void inline debug(Print* out);
	void inline debugFn(Print* out, void (*printFunction)(Print*, T));
	#endif

private:
	T buffer[S];
	T *head;
	T *tail;
	uint16_t count;
};

//#include "CircularBuffer.tpp"

/*
 CircularBuffer.tpp - Circular buffer library for Arduino.
 Copyright (c) 2017 Roberto Lo Giacco.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as 
 published by the Free Software Foundation, either version 3 of the 
 License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>

template<typename T, __CB_ST__ S> 
CircularBuffer<T,S>::CircularBuffer() :
		head(buffer), tail(buffer), count(0) {
}

template<typename T, __CB_ST__ S> 
CircularBuffer<T,S>::~CircularBuffer() {
}

template<typename T, __CB_ST__ S> 
bool CircularBuffer<T,S>::unshift(T value) {
	if (head == buffer) {
		head = buffer + S;
	}
	*--head = value;
	if (count == S) {
		if (tail-- == buffer) {
			tail = buffer + S - 1;
		}
		return false;
	} else {
		if (count++ == 0) {
			tail = head;
		}
		return true;
	}
}

template<typename T, __CB_ST__ S> 
bool CircularBuffer<T,S>::push(T value) {
	if (++tail == buffer + S) {
		tail = buffer;
	}
	*tail = value;
	if (count == S) {
		if (++head == buffer + S) {
			head = buffer;
		}
		return false;
	} else {
		if (count++ == 0) {
			head = tail;
		}
		return true;
	}
}

template<typename T, __CB_ST__ S> 
T CircularBuffer<T,S>::shift() {
	void(* crash) (void) = 0;
	if (count <= 0) crash();
	T result = *head++;
	if (head >= buffer + S) {
		head = buffer;
	}
	count--;
	return result;
}

template<typename T, __CB_ST__ S> 
T CircularBuffer<T,S>::pop() {
	void(* crash) (void) = 0;
	if (count <= 0) crash();
	T result = *tail--;
	if (tail < buffer) {
		tail = buffer + S - 1;
	}
	count--;
	return result;
}

template<typename T, __CB_ST__ S> 
T inline CircularBuffer<T,S>::first() {
	return *head;
}

template<typename T, __CB_ST__ S> 
T inline CircularBuffer<T,S>::last() {
	return *tail;
}

template<typename T, __CB_ST__ S> 
T CircularBuffer<T,S>::operator [](__CB_ST__ index) {
	return *(buffer + ((head - buffer + index) % S));
}

template<typename T, __CB_ST__ S> 
__CB_ST__ inline CircularBuffer<T,S>::size() {
	return count;
}

template<typename T, __CB_ST__ S> 
__CB_ST__ inline CircularBuffer<T,S>::available() {
	return S - count;
}

template<typename T, __CB_ST__ S> 
__CB_ST__ inline CircularBuffer<T,S>::capacity() {
	return S;
}

template<typename T, __CB_ST__ S> 
bool inline CircularBuffer<T,S>::isEmpty() {
	return count == 0;
}

template<typename T, __CB_ST__ S> 
bool inline CircularBuffer<T,S>::isFull() {
	return count == S;
}

template<typename T, __CB_ST__ S> 
void inline CircularBuffer<T,S>::clear() {
	memset(buffer, 0, sizeof(buffer));
	head = tail = buffer;
	count = 0;
}

#ifdef CIRCULAR_BUFFER_DEBUG
template<typename T, __CB_ST__ S> 
void inline CircularBuffer<T,S>::debug(Print* out) {
	for (__CB_ST__ i = 0; i < S; i++) {
		int hex = (int)buffer + i;
		out->print(hex, HEX);
		out->print("  ");
		out->print(*(buffer + i));
		if (head == buffer + i) {
			out->print(" head");
		} 
		if (tail == buffer + i) {
			out->print(" tail");
		}
		out->println();
	}
}

template<typename T, __CB_ST__ S> 
void inline CircularBuffer<T,S>::debugFn(Print* out, void (*printFunction)(Print*, T)) {
	for (__CB_ST__ i = 0; i < S; i++) {
		int hex = (int)buffer + i;
		out->print(hex, HEX);
		out->print("  ");
		printFunction(out, *(buffer + i));
		if (head == buffer + i) {
			out->print(" head");
		} 
		if (tail == buffer + i) {
			out->print(" tail");
		}
		out->println();
	}
}
#endif
#endif
