#ifndef _INTERPOLATOR_H
#define _INTERPOLATOR_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

class FieldValue;


class Interpolator : public Node {
public:
			Interpolator(Scene *scene, Proto *proto);

    /*int	    		getNumKeys() const;
    float		getKey(int index) const;
    float		getKeyValue(int channel, int index) const;
    void		setKey(int index, float value);
    virtual void	setKeyValue(int channel, int index, float value);
    void		backupKey(int index);
    void		backup(CommandList *list);
    void		receiveEvent(int eventIn, double timestamp, FieldValue *value);
    void		insertKey(int pos, float key, const float *values);
    void		deleteKeys(int start, int end);
    virtual void	interpolate(float k, float *values);
    void		sendInterpolatedValue(double timestamp, float k);

    virtual int		getNumChannels() const = 0;
    virtual FieldValue *createKey(const float *value) const = 0;
    virtual FieldValue *createKeys(const float *value, int numKeys) const = 0;

    int			findKey(float value) const;
    int			findKeyInclusive(float value) const;
    bool		getNearestKeys(float k, float *k1, float *k2,
				       int *pos1, int *pos2);

    float		getFraction() const { return _fraction; }
    int			getNodeClass() const { return NODE_INTERPOLATOR; }

    void		recordKey(FieldValue *value);*/

/*
private:
    
    float		_fraction;
*/
};

class ProtoInterpolator : public Proto {
public:
			ProtoInterpolator(Scene *scene, const char *name,
					    int keyType, int keysType, 
					    FieldValue *defaultValue);
    virtual Node       *create(Scene *scene) = 0;
};

#endif
