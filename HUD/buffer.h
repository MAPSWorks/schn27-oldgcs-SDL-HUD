// buffer.h

#ifndef BUFFER_H
#define BUFFER_H

#include <cstdint>
#include <cassert>

class Buffer
{
public:
	Buffer(std::uint8_t *baseAddr, int size)
		: baseAddr_(baseAddr)
		, size_(size)
		, indexGet_(0)
		, indexPut_(0)
	{
	}

	void SetIndex(int value) {indexPut_ = indexGet_ = value;}
	void SetIndexForGet(int value) {indexGet_ = value;}
	void SetIndexForPut(int value) {indexPut_ = value;}
	int GetIndexForPut(void) {return indexPut_;}
	

// get methods ////////////////////////////////////////////////////
	
	std::uint8_t getUint8(void)
	{
		assert(indexGet_ < size_);
		return (indexGet_ < size_) ? baseAddr_[indexGet_++] : 0;
	}
	
	std::uint16_t getUint16(void)
	{
		std::uint16_t res = getUint8();
		res += 256 * getUint8();
		return res;
	}
	
	std::uint32_t getUint32(void)
	{
		std::uint32_t res = getUint16();
		res += 65536UL * getUint16();
		return res;
	}
	
	float getFloat32(void)
	{
		float f;
		*reinterpret_cast<std::uint32_t *>(&f) = getUint32();
		return f;
	}
		
	double getFloat64(void)
	{
		double df;
		*reinterpret_cast<std::uint32_t *>(&df) = getUint32();
		*(reinterpret_cast<std::uint32_t *>(&df) + 1) = getUint32();
		return df;
	}

	std::uint8_t getUint8(int index)
	{
		indexGet_ = index;
		return getUint8();
	}
	
	std::uint16_t getUint16(int index)
	{
		indexGet_ = index;
		return getUint16();
	}
	
	std::uint32_t getUint32(int index)
	{
		indexGet_ = index;
		return getUint32();
	}
	
	float getFloat32(int index)
	{
		indexGet_ = index;
		return getFloat32();
	}
		
	double getFloat64(int index)
	{
		indexGet_ = index;
		return getFloat64();
	}

	
// put methods ////////////////////////////////////////////////////
	
	void putUint8(std::uint8_t val)
	{
		assert(indexPut_ < size_);
		if (indexPut_ < size_)
			baseAddr_[indexPut_++] = val;
	}
	
	void putUint16(std::uint16_t val)
	{
		putUint8(val & 0xFF);
		putUint8(val >> 8);
	}
	
	void putUint32(std::uint32_t val)
	{
		putUint16(val & 0xFFFF);
		putUint16(val >> 16);
	}
	
	void putFloat32(float val)
	{
		putUint32(*reinterpret_cast<std::uint32_t *>(&val));
	}
		
	void putFloat64(double val)
	{
		putUint32(*reinterpret_cast<std::uint32_t *>(&val));
		putUint32(*(reinterpret_cast<std::uint32_t *>(&val) + 1));
	}
	
	void putUint8(int index, std::uint8_t val)
	{
		indexPut_ = index;
		putUint8(val);
	}
	
	void putUint16(int index, std::uint16_t val)
	{
		indexPut_ = index;
		putUint16(val);
	}
	
	void putUint32(int index, std::uint32_t val)
	{
		indexPut_ = index;
		putUint32(val);
	}
	
	void putFloat32(int index, float val)
	{
		indexPut_ = index;
		putFloat32(val);
	}
		
	void putFloat64(int index, double val)
	{
		indexPut_ = index;
		putFloat64(val);
	}
	

private:
	Buffer(void);
	std::uint8_t *baseAddr_;
	int size_;
	int indexGet_;
	int indexPut_;
	
};


#endif
