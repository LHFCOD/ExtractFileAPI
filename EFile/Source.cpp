#define JCONFIG_INCLUDED


#include "boost\gil\extension\io\jpeg_io.hpp"
#include "boost\gil\gil_all.hpp"
using namespace boost::gil;
void main()
{
	bgr16_image_t img;
	jpeg_read_image("temp.jpg", img);
	int i = 0;
	i++;
}