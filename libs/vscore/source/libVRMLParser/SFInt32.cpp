#include "stdafx.h"
#include "SFInt32.h"
#include <math.h>
#include <float.h>
#include <limits.h>

SFInt32::SFInt32(int value)
{
	_value = value;
}

void SFInt32::write(FILE *f, int indent) const
{
	fprintf(f, "%d\n", _value);
}

bool SFInt32::equals(const FieldValue *value) const
{
	if (this == value) return true;
	if (value == NULL) return false;
	if (getType() != value->getType()) return false;
	SFInt32 *v = (SFInt32 *) value;
	if (_value != v->_value) return false;
	return true;
}

void SFInt32::clamp(float min, float max)
{
	int _max;
	if (max != FLT_MAX)
		_max = (int) (max < 0 ? ceil(max - 0.5): floor(max + 0.5));
	else
		_max = INT_MAX;

	int _min;
	if (min != -FLT_MAX)
		_min = (int) (min < 0 ? ceil(min - 0.5): floor(min + 0.5));
	else
		_min = -INT_MAX;

	_value = CLAMP(_value, _min, _max);
}
