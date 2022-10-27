/*
 * buildstars.cpp - building star database file
 *
 *  Created on: May 11, 2020
 *      Author: Tim Stark
 */

#include "main/core.h"
#include "universe/astro.h"
#include "universe/xhipdata.h"

using namespace ofs::astro;

struct starEntry
{
	int hipNumber = -1;
	int hdNumber = -1;

	double ascension = 0.0;
	double declination = 0.0;
	double parallax = 0.0;
};

int main(int argc, char **argv)
{

	return 0;
}
