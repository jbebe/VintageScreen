#pragma once

#include <vector>

#include <Misc\PixelData\Color.hpp>
#include <Misc\Debug\Assert.hpp>

class PixelData {

	class PixelDataRow {

	private:
		Color *row;
		int upperLimit;

	public:
		PixelDataRow(Color *row, int upper_limit)
			: row(row), upperLimit(upper_limit)
		{}

		// width
		Color &operator [] (int index) {
			if (index < 0) index = 0; 
			else if (index > upperLimit) index = upperLimit;
			
			return row[index]; 
		}

		const Color &operator [] (int index) const {
			return const_cast<PixelDataRow&>(*this).operator[](index);
		}
	};

private:
	Color *data;

public:
	unsigned width;
	unsigned height;

public:
	PixelData(){}

	PixelData(unsigned width, unsigned height)
		: data(nullptr), width(width), height(height)
	{
		data = new Color[width*height];
		RT_ASSERT(data != nullptr, "data is NULL");
	}

	PixelData(const PixelData &rhs)
		: data(nullptr), width(rhs.width), height(rhs.height)
	{
		data = new Color[width*height];
		RT_ASSERT(data != nullptr, "data is NULL");

		memcpy(data, rhs.data, width*height*sizeof(rhs.data[0]));
	}

	// height
	PixelDataRow operator [] (int index) {
		int upper_limit = height-1;
		
		if (index < 0) index = 0; 
		else if (index > upper_limit) index = upper_limit;

		return PixelDataRow(data + index*width, width-1);
	}

	const PixelDataRow operator [] (int index) const {
		return const_cast<PixelData&>(*this).operator[](index);
	}

	~PixelData() {
		delete[] data;
	}

};
