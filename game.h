#ifndef GAME_H
#define GAME_H
#include "model.h"
#include "geom.h"

class actor
{
public:
    Model *mdl;
	v3df pos;
	QRgb color = qRgb(0,0,0);
	actor(Model *carmdl);
	~actor();
	v3df mincb, maxcb;
	v3df getmincollbox();
	v3df getmaxcollbox();
	bool collisioncheck(actor *rival);
};


#endif // GAME_H
