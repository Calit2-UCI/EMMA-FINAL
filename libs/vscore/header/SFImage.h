#ifndef _SFIMAGE_H
#define _SFIMAGE_H

#ifndef _FIELDVALUE_H
#include "FieldValue.h"
#endif
#ifndef _ARRAY_H
#include "Array.h"
#endif

/**
* SFImage field type.
* \ingroup VRMLParser
*/
class SFImage : public FieldValue {
public:
			SFImage(int width, int height, int depth, const int *pixels);

    virtual int         getType() const { return SFIMAGE; }
    virtual const char *getTypeName() const { return "SFImage"; }
    virtual void        write(FILE *f, int indent) const;
    virtual bool	equals(const FieldValue *value) const;
    virtual FieldValue *copy() { return new SFImage(*this); }

    int			getWidth() const { return _width; }
    int			getHeight() const { return _height; }
    int			getDepth() const { return _depth; }
    const int	       *getPixels() const { return _pixels.getData(); }

private:
    int			_width;
    int			_height;
    int			_depth;
    Array<int>		_pixels;
};

#endif // _SFIMAGE_H
