
#ifndef MAIN_H
#define MAIN_H
#include<cstdio>
#include<cmath>
#include<cstdlib>
#include <iostream>
#include <fstream>
#include <random>
#include "o2.h"
#include "montecarlo.h"
#include "initialization.h"
#include <filesystem>
#include <mpi.h>
#include "rng.h"

namespace fs = std::filesystem;


/*Temporary defined as global variables here*/
#define Lx 8
#define Ly 8
#define Lz 8

#define N (Lx*Ly*Lz)


void mainloop(struct Node* Site, struct MC_parameters &MCp, struct H_parameters &Hp, double my_beta, struct PT_parameters PTp, struct PTroot_parameters PTroot, const fs::path & directory_write);
void parallel_temp(double my_E , double &my_beta, struct PT_parameters PTp, struct PTroot_parameters PTroot);
unsigned int nn(unsigned int i, unsigned int coord, int dir);
void myhelp(int argd, char** argu);
#endif
