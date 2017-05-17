/*
 *  This file is part of WTlib.
 *
 *  WTlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WTlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with WTlib.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstdlib>
#include <algorithm>
#include <string>
#include <iostream>

#include "main.h"
#include "wt.h"

#define cimg_display 0
#include "CImg.h"


std::vector<WTlib::WTdata> load_obrazek (std::string cesta) {
	using namespace cimg_library;

	std::cerr << "Opening " << cesta << std::endl;

	CImg<int> image;
	unsigned int bits_per_pixel = 0;

	try {
		image.load_png(cesta.c_str(), &bits_per_pixel);
	} catch( CImgIOException ex ) {
		throw std::invalid_argument("CImg error:\n" + std::string(ex.what()) );
	}

	int height = image.height();
	int width = image.width();      
	int depth = image.depth();       // 1 => 2d img
	int spectrum = image.spectrum(); // obvykle 3 rgb / 4 rgba

	std::cerr << "  it is " << height << "x" <<  width << "x" << spectrum << "  image" << std::endl;

	if ( std::min(height, width) <= 0 )
		throw std::invalid_argument("Image dimensions is not supported");
	if ( depth != 1 )
		std::cerr << "Warning image depth is not coded in" << std::endl;

	if ( bits_per_pixel != 8 )
		std::cerr << 
		"Uncommon bits per pixel value (" << bits_per_pixel << ") "
		"it can result in wrong decompression (only 8bps supported)" 
		<< std::endl;

	std::vector<WTlib::WTdata> dest;
	for ( int c = 0; c < spectrum; ++c )
	{
		WTlib::WTdata data (width, height);
		// nacte data
		for ( int y = 0; y < height; ++y )
		for ( int x = 0; x < width;  ++x ) {
			data[x + y*width] = *image.data(x,y,c);
		}
		// ulozi data
		dest.push_back(data);
	}

	return dest;
}


void save_obrazek (std::string cesta, std::vector<WTlib::WTdata> src) {
	using namespace cimg_library;

	if ( src.size() <= 0 ) 
		throw std::invalid_argument("Cannot encode zero colorplanes");

	int width = src[0].rozmer_x,
		height = src[0].rozmer_y,
		spectrum = static_cast<int>(src.size());

	CImg<int> image (width, height, 1, spectrum);

	// ulozeni do jine podoby
	for ( int c = 0; c < spectrum; ++c ) {
		auto& data = src[c].data;

		if (src[c].rozmer_x != width || src[c].rozmer_y != height )
			throw std::invalid_argument("Input vector suggest that each colorspace is different in sizes");

		for ( int y = 0; y < height; ++y )
		for ( int x = 0; x < width; ++x  )
		{
			*image.data(x,y,c) = data[x + y*width];
		}
	}

	image.save_png(cesta.c_str(), 8);
}

bool check_obrazek (const Obrazek& a, const Obrazek& b) {
	if ( a.size() != b.size() ) return true;

	for ( unsigned ic = 0; ic < a.size(); ++ic )
	{
		const auto& ac = a[ic];
		const auto& bc = b[ic];

		if ( ac.rozmer_x != bc.rozmer_x ) return true;
		if ( ac.rozmer_y != bc.rozmer_y ) return true;
		if ( ac.data.size() != bc.data.size() ) return true;

		const auto &ad = ac.data, &bd = bc.data;

		for ( unsigned id = 0; id < ad.size(); ++id ) {
			if ( ad[id] != bd[id] ) return true;
		}
	}

	return false;
}

