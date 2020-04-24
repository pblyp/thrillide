#include "game.h"

actor::actor(Model *carmdl)
{
    mdl = carmdl;
	mincb = getmincollbox();
	maxcb = getmaxcollbox();
}

v3df actor::getmincollbox()
{
	v3df result;
	result = mdl->vert(0);
	for (int i=1; i<mdl->nverts(); i++)
	{
		v3df vert = mdl->vert(i);
		if (result.x < vert.x)
			result.x = vert.x;
		if (result.z < vert.z)
			result.z = vert.z;
	}
	return result;
}

v3df actor::getmaxcollbox()
{
	v3df result;
	result = mdl->vert(0);
	for (int i=1; i<mdl->nverts(); i++)
	{
		v3df vert = mdl->vert(i);
		if (result.x > vert.x)
			result.x = vert.x;
		if (result.z > vert.z)
			result.z = vert.z;
	}
	return result;
}

bool actor::collisioncheck(actor *rival)
{
	v3df distance = v3df(fabs(pos.x-rival->pos.x), 0, fabs(pos.z-rival->pos.z));
	if (distance.x < mincb.x-maxcb.x && distance.z < mincb.z-maxcb.z)
	{
		std::cout<<"Collision!\n";
		return true;
	}
	return false;
}
