/*
 * Copyright 2014-2015 Matthew McGowan.
 *
 * This file is part of Nice Firmware.
 *
 * Controlbox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Controlbox.  If not, see <http://www.gnu.org/licenses/>.
 */



#pragma once

#include <stdint.h>
#include <algorithm>

namespace cbox {

typedef uint16_t stream_size_t;

/**
 * An output stream that supports writing data. Optionally. annotations may also be written
 * to the stream, although these are entirely optional and should provide only
 * supplemental information.
 * @param data
 * @return
 */
class DataOut
{
public:
	DataOut() = default;
	virtual ~DataOut() = default;

    virtual void writeAnnotation(const char* /*data*/) {}
	virtual void writeResponseSeparator() {}
	virtual void writeListSeparator() {}
	virtual void endMessage() {}

	/**
	 * Writes a byte to the stream.
	 * @return {@code true} if the byte was successfully written, false otherwise.
	 */
	virtual bool write(uint8_t data)=0;

	bool write(int8_t data) { return write(uint8_t(data)); }
	bool write(char data) { return write(uint8_t(data)); }
	bool write(int data) { return write(uint8_t(data)); }

	template<typename T>
    bool put(const T & t){
        return writeBuffer(std::addressof(t), sizeof(T));
    }

	/**
	 * Writes a number of bytes to the stream.
	 * @param data	The address of the data to write.
	 * @param len	The number of bytes to write.
	 * @return {@code true} if the byte was successfully written, false otherwise.
	 */
	virtual bool writeBuffer(const void* data, stream_size_t len) {
		const uint8_t* d = (const uint8_t*)data;
		while (len-->0) {
			if (!write(*d++))
				return false;
		}
		return true;
	}

	virtual void flush() {}
};

/**
 * An output stream that buffers data before writing.
 */
class BufferDataOut : public DataOut {
private:
	uint8_t* buffer;
	stream_size_t size;
	stream_size_t pos;
public:

	BufferDataOut(uint8_t* _buffer, stream_size_t _size)
		: buffer(_buffer), size(_size), pos(0)
	{
	}

	void reset() {
		pos = 0;
	}

	virtual bool write(uint8_t data) override {
	    if (pos<size) {
	        buffer[pos++] = data;
	        return true;
	    }
	    return false;
	}

	stream_size_t bytesWritten() { return pos; }

	const uint8_t* data() {
		return buffer;
	}

};

/**
 * A DataOut implementation that discards all data.
 */
class BlackholeDataOut : public DataOut {
public:
    BlackholeDataOut() = default;
    virtual ~BlackholeDataOut() = default;
	virtual bool write(uint8_t /*data*/) override final { return true; }
};

/**
 * A DataOut implementation that discards all data, but counts each byte;
 */
class CountingBlackholeDataOut : public DataOut {
private:
	stream_size_t counted;
public:    
	CountingBlackholeDataOut() : counted(0) {};
    virtual ~CountingBlackholeDataOut() = default;
    virtual bool write(uint8_t /*data*/) override final {
        ++counted;
        return true;
    }

    stream_size_t count(){
        return counted;
    }
};

/**
 * A data input stream. The stream contents may be determined asynchronously.
 * hasNext() returns true if the stream may eventually produce a new item, false if the stream is closed.
 * next() fetches the next item from the stream. return value is undefined if available()==0.
 * peek() retrieves the next data in the stream without removing it. Result is undefined if
 * available() returns 0.
 * available() the number of times read can be called to retrieve valid data.
 */
class DataIn
{
public:
	/*
	 * Determines if there is potentially more data in this stream.
	 * Note that this is not dependent upon time and asynchronous delivery of data, but if the stream is still open.
	 */
	virtual bool hasNext() =0;

	/**
	 * Retrieves the next byte of data. The return value is only valid when `hasNext()` returns true.
	 */
	virtual uint8_t next() =0;

	/**
	 * Retrieves the next byte of data without removing it from the stream. The result is only valid if `available`
	 * previously returned a non-zero value.
	 */
	virtual uint8_t peek() =0;

	/**
	 * Determines how many bytes are available for reading from the stream without blocking.
	 */
	virtual stream_size_t available() =0;

	virtual ~DataIn() {}

	/*
	 * Unconditional read of {@code length} bytes.
	 */
	bool read(void* t, stream_size_t length) {
		uint8_t* target = (uint8_t*)t;
		while (length-->0) {
            if(!hasNext()){
                return false;
            }
			*target++ = next();
		}
		return true;
	}

    template<typename T>
    bool get(T& t){
        return read(&t, sizeof(T));
    }

    /**
     * Writes the contents of this stream to an output stream.
     * @param out
     * @param length
     * @return length was written
     */
	bool push(DataOut& out, stream_size_t length) {
		while (length > 0 && hasNext()) {
			out.write(next());
			--length;
		}
		return length == 0;
	}

	/**
     * Writes the contents of this stream to an output stream, until input stream is empty
     * @param out
     */
	bool push(DataOut& out) {
		bool success = true;
		while (hasNext()) {
			success &= out.write(next());
		}
		return success;
	}

    /**
     * Discards a number of bytes.
     * Can be overridden by child classes to skip actual reads for performance if the underlying structure can be accessed by index
     *
     * @param length
     * @return length was skipped (didn't encounter end of stream)
     */
	virtual bool spool(stream_size_t skip_length){
	    stream_size_t to_skip = skip_length;
	    while (to_skip > 0 && hasNext()){
	        next();
	        --to_skip;
	    }
        return to_skip == skip_length;
    }

	virtual void spool(){
	    while (hasNext()){
	        next();
		}
    }
};


/**
 * A DataIn that provides no data.
 */
class EmptyDataIn : public DataIn
{
public:
	virtual bool hasNext() override { return false; }
	virtual uint8_t next() override { return 0; }
	virtual uint8_t peek() override { return 0; }
	virtual stream_size_t available() override { return 0; }
};

/*
 * Reads data from a DataIn, and also writes the fetched bytes (if any) to a DataOut.
 */
class TeeDataIn : public DataIn
{
	DataIn& in;
	DataOut& out;
	bool success;

public:
	TeeDataIn(DataIn& _in, DataOut& _out)
		: in(_in), out(_out), success(true)
	{
	}

	bool teeOk() { return success; }

	virtual uint8_t next() override {
		uint8_t val = in.next();
		bool result = out.write(val);
		success = success && result;
		return val;
	}

	virtual bool hasNext() override { return in.hasNext(); }
	virtual void spool() override { return in.spool(); }
	virtual uint8_t peek() override { return in.peek(); }
	virtual stream_size_t available() override { return in.available(); }

};

/*
 * A DataOut that writes to two other DataOut streams.
 */
class TeeDataOut : public DataOut
{
public:
    TeeDataOut(DataOut& _out1, DataOut& _out2) : out1(_out1), out2(_out2){};
    virtual ~TeeDataOut() = default;

    virtual bool write(uint8_t data) override {
        bool res1 = out1.write(data);
        bool res2 = out2.write(data);
        return res1 || res2;
    }
private:
    DataOut & out1;
    DataOut & out2;
};


/**
 * Provides a DataIn stream from a static buffer of data.
 */
class BufferDataIn : public DataIn {
	const uint8_t* data;
	stream_size_t size;
    stream_size_t pos;

public:
	BufferDataIn(const uint8_t * buf, stream_size_t len) : data(buf), size(len), pos(0) {}

	virtual uint8_t next() override { return data[pos++]; }
	virtual bool hasNext() override { return pos < size; }
	virtual uint8_t peek() override { return data[pos]; }
	virtual stream_size_t available() override { return size - pos; }
	void reset() { pos = 0; };
	stream_size_t bytes_read() { return pos; };
};


/**
 * Limits reading from the stream to the given number of bytes.
 */
class RegionDataIn : public DataIn {
	DataIn& in;
	stream_size_t len;
public:
	RegionDataIn(DataIn &_in, stream_size_t _len)
	: in(_in), len(_len) {}

	bool hasNext() override {
	    return len && in.hasNext();
	}

	uint8_t next() override {
	    return hasNext() ? --len, in.next() : 0;
	}

	uint8_t peek() override {
	    return in.peek();
	}

	stream_size_t available() override {
	    return std::min(len, in.available());
	}

	void setLength(stream_size_t len_){
	    len = len_;
	}
};

/**
 * Limits writing to the stream to the given number of bytes.
 */
class RegionDataOut : public DataOut {
    DataOut* out; // use pointer to have assignment operator
    stream_size_t len;
public:
    RegionDataOut(DataOut& _out, stream_size_t _len)
    : out(&_out), len(_len) {}
    virtual ~RegionDataOut() = default;

    virtual bool write(uint8_t data) override {
        if(len > 0){
            --len;
            return out->write(data);
        }
        return false;
    }
    void setLength(stream_size_t len_){
        len = len_;
    }
    stream_size_t availableForWrite(){
        return len;
    }
};

}
