// From https://gist.github.com/jvanwinden/93a9b4b4433e1f20de97
#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdexcept>

template <class T>
class Ringbuffer {
public:
	explicit Ringbuffer(const int size);
	~Ringbuffer();
	T & head();
	T & head_next();
    T & tail();
    T & tail_next();
    
	int size();
	bool full();
	bool empty();
    
	int start;
	int end;
private:
	int n_elements;
	T * arr;
	const int arr_size;

};

template <class T>
Ringbuffer<T>::Ringbuffer(const int size) : 
	arr_size(size),
	arr(new T[size]),
	start(0),
	end(0),
	n_elements(0) {}

template <class T>
Ringbuffer<T>::~Ringbuffer() {
	delete[] arr;
}

// Return a reference to the tail and advance
template <class T>
T & Ringbuffer<T>::tail_next() {
    int orig_start = start;
    if (n_elements > 0){
        n_elements--;
        start = (++start % arr_size);
    }
	return arr[orig_start];
}

template <class T>
T &  Ringbuffer<T>::tail() {
	arr[start];
}

// Returna reference to the head and advance
template <class T>
T &  Ringbuffer<T>::head_next() {
    int orig_end = end;
    if(n_elements < arr_size){
        n_elements++;
        end = (++end % arr_size);
    }
	return arr[orig_end];
}

template <class T>
T &  Ringbuffer<T>::head() {
	arr[end];
}


template <class T>
int Ringbuffer<T>::size() {
	return n_elements;
}

template <class T>
bool Ringbuffer<T>::full() {
	return size() == arr_size;
}

template <class T>
bool Ringbuffer<T>::empty() {
	return size() == 0;
}

#endif