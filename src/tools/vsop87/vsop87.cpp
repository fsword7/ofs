/*
 * vsop87sph.cpp - VSOP87 C++ data generator package
 *
 * VSOP87 package are available for downloading:
 *
 * ftp://ftp.imcce.fr/pub/ephem/planets/vsop87/
 *
 *  Created on: Mar 10, 2020
 *      Author: Tim Stark
 */

#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/format.h>

using namespace std;

namespace fs = std::filesystem;

void usage(char *name)
{
	cout << fmt::format("Usage: {} <planet name>\n", name);
	exit(1);
}

void print(double data[2000][3], const char *planet, int lbr, int deg, int terms)
{
	cout << fmt::format("static vsop87_t {}_{}{}[{}] = {{\n",
		planet, "lbr"[lbr], deg, terms);

	for (int idx = 0; idx < terms; idx++) {
		cout << fmt::format("\t{{ {:18.12f}, {:18.12f}, {:20.12f} }}{}\n",
			data[idx][0], data[idx][1], data[idx][2],
			(idx < terms-1) ? ',' : ' ');
	}

	cout << fmt::format("}};\n\n");
}

int main(int argc, char **argv)
{
	const char *list = nullptr;

	if (argc > 1)
		list = argv[1];
	else
		usage(argv[0]);

	fs::path path(list);
	for(auto &file : fs::recursive_directory_iterator(list,
		fs::directory_options::skip_permission_denied))
	{
		fs::path infile = file;

		cout << infile.string() << endl;
	}

	exit(EXIT_SUCCESS);
}

int oldmain(int argc, char **argv)
{
	const char *planet = "earth"; // default planet name

	if (argc > 1)
		planet = argv[1];
	else
		usage(argv[0]);

	string fname = "VSOP87B." + string(planet, 0, 3);
//	cout << "Generating VSOP87 data terms from " << fname << " for " << planet << "..." << endl;

	ifstream vsopFile(fname, ios::in);
	if (!vsopFile.is_open()) {
		cerr << "File " << fname << ": " << strerror(errno) << endl;
		exit(1);
	}

	string line;
	int lnum = 0;
	int terms = 0;
	int lbr, deg;
	double data[2000][3];

	while (getline(vsopFile, line)) {
		lnum++;
		const char *cline = line.c_str();
		if (!strncmp(cline, " VSOP87", 7)) {
			if (terms > 0)
				print(data, planet, lbr, deg, terms);
			terms = 0;

			lbr = (int)cline[41] - (int)'1';
			deg = (int)cline[59] - (int)'0';

			if (deg < 0 || deg > 5) {
				cout << "Bad degree (" << deg << ") in VSOP data file at line "
					 << lnum << endl;
				exit(1);
			}

			if (lbr < 0 || lbr > 3) {
				cout << "Bad data type (" << lbr << ") in VSOP data file at line "
					 << lnum << endl;
				exit(1);
			}

		} else {

			double a, b, c;

			if (sscanf(cline+80, " %lf %lf %lf", &a, &b, &c) != 3) {
				cout << "Bad data in VSOP data at line " << lnum << endl;
				exit(1);
			}

			data[terms][0] = a;
			data[terms][1] = b;
			data[terms][2] = c;
			terms++;
		}
	}

	if (terms > 0)
		print(data, planet, lbr, deg, terms);

	vsopFile.close();
	exit(1);
}
