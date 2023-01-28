// vsop87.cpp - VSOP87 C++ data generator package
//
// VSOP87 package are available for downloading:
//
// ftp://ftp.imcce.fr/pub/ephem/planets/vsop87/
//
// Author:	Tim Stark
// Date:	Jan 7, 2023

//   = Elliptic at Epoch J2000
// A = Rectangular at Epoch J2000
// B = Spherical at Epoch J2000
// C = Rectangular at date
// D = Spherical at date
// E = Barycentric at Epoch J2000

#include <iostream>
#include <fstream>
#include <filesystem>
#include <fmt/format.h>

using namespace std;

namespace fs = std::filesystem;

enum fmtType { fmtRect, fmtSph };

void usage(char *name)
{
	cout << fmt::format("Usage: {} <path> [planet name] [data type]\n", name);
	exit(1);
}

void print(double data[3000][3], const char *planet, fmtType type, int set, int elm, int deg, int terms)
{
	const char *label = nullptr;
	if (type == fmtSph)
		label = "lbr";
	else if (type == fmtRect)
		label = "xyz";

	cout << fmt::format("static vsop87_t {}_{}{}[{}] = {{\n",
		planet, label[elm], deg, terms);

	for (int idx = 0; idx < terms; idx++) {
		cout << fmt::format("\t{{ {:18.12f}, {:18.12f}, {:20.12f} }}{}\n",
			data[idx][0], data[idx][1], data[idx][2],
			(idx < terms-1) ? ',' : ' ');
	}

	cout << fmt::format("}};\n\n");
}

void read(fs::path fname, const char *planet, fmtType type)
{
	ifstream vsopFile(fname, ios::in);
	if (!vsopFile.is_open()) {
		cerr << "File " << fname << ": " << strerror(errno) << endl;
		exit(1);
	}

	string line;
	string dtype;
	int lnum = 0;
	int dset = 0;
	int terms = 0;
	int elm, deg, emax;
	double data[3000][3];

	while (getline(vsopFile, line)) {
		lnum++;
		if (line.empty())
			continue;
		const char *cline = line.c_str();
		if (!strncmp(cline, " VSOP87", 7)) {
			if (terms > 0)
			{
				cout << "Set " << dset << " Element: " << elm << " Degree: " << deg << " Terms: " << terms << " Type: " << dtype << endl;
				print(data, planet, type, dset, elm, deg, terms);
			}
			dset++;
			terms = 0;

			dtype = line.substr(44, 3);
			elm = (int)cline[41] - (int)'1';
			deg = (int)cline[59] - (int)'0';
			emax = dtype == "ALK" ? 6 : 3;

			if (deg < 0 || deg > 5) {
				cout << "Bad degree (" << deg << ") in VSOP data file at line "
					 << lnum << endl;
				break;
			}

			if (elm < 0 || elm > emax) {
				cout << "Bad data type (" << elm << ") in VSOP data file at line "
					 << lnum << endl;
				break;
			}

		} else {

			double a, b, c;

			if (sscanf(cline+80, " %lf %lf %lf", &a, &b, &c) != 3) {
				cout << "Bad data in VSOP data at line " << lnum << endl;
				break;
			}

			data[terms][0] = a;
			data[terms][1] = b;
			data[terms][2] = c;
			terms++;
		}
	}

	if (terms > 0)
	{
		cout << "// Set " << dset << " Element: " << elm << " Degree: " << deg << " Terms: " << terms << " Type: " << dtype << endl;
		print(data, planet, type, dset, elm, deg, terms);
	}

	vsopFile.close();
}

int main(int argc, char **argv)
{
	const char *list = nullptr;
	const char *planet = "earth"; // default planet 
	const char *dtype = "B"; // default type (Spherical J2000)
	std::string fext;
	fmtType type;

	if (argc > 1)
	{
		list = argv[1];
		if (argc > 2)
			planet = argv[2];
		if (argc > 3)
			dtype = argv[3];

		fext = "." + std::string(planet, 0, 3);
	}
	else
		usage(argv[0]);

	if (dtype[0] == 'A' || dtype[0] == 'C' || dtype[0] == 'E')
		type = fmtRect;
	else if (dtype[0] == 'B' || dtype[0] == 'D')
		type = fmtSph;

	fs::path path(list);
	for(auto &file : fs::recursive_directory_iterator(list,
		fs::directory_options::skip_permission_denied))
	{
		fs::path infile = file;
		// cout << infile.string() << ": " << infile.extension() << endl;
		if (infile.extension() != fext)
			continue;
		cout << infile.filename() << endl;
		if (infile.filename().string()[6] != *dtype)
			continue;
		cout << infile.string() << endl;
		read(infile, planet, type);
	}

	exit(EXIT_SUCCESS);
}
