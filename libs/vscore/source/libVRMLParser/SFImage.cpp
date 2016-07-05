#include "stdafx.h"
#include "SFImage.h"

SFImage::SFImage(int width, int height, int depth, const int *pixels)
{
	_width = width;
	_height = height;
	_depth = depth;

	int size = _width * _height;
	for (int i = 0; i < size; i++) _pixels.append(pixels[i]);
}

void SFImage::write(FILE *f, int indent) const
{
	fprintf(f, "%d %d %d ", _width, _height, _depth);

	int size = _width * _height;
	for (int i = 0; i < size; i++)
		fprintf(f, "%d ", (unsigned) _pixels[i]);
	fprintf(f, "\n");
}

bool SFImage::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	SFImage *v = (SFImage *) value;
	if (_width != v->_width) return false;
	if (_height != v->_height) return false;
	if (_depth != v->_depth) return false;

	int size = _width * _height;
	for (int i = 0; i < size; i++)
		if (_pixels[i] != v->_pixels[i]) return false;
	return true;
}
