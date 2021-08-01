

#include <iostream>
#include "TinyGP.h"

#include <sstream>
#include <string>
#include <fstream>

#include <regex>
#include <vector>
#include <assert.h>
#include "timer.h"
#define USE_POOL
inline double randZeroToOne()
{
	return rand() / (RAND_MAX + 1.);
}
TinyGP::TinyGP(std::string fname, long s)
	:seed(s)
{
#ifdef USE_POOL
	mempool.allocate(MAX_LEN, POPSIZE);
#endif
	srand(seed);
	setupFitness(fname);
	createRandomPop();
	for (size_t i = 0; i < FSET_START; i++) {
		x[i] = (maxrandom - minrandom) * randZeroToOne() + minrandom;
	}
	/*
	for (size_t i = 0; i < 200; i++) {
		std::cout << std::endl;
		printIndividual(pop[i], 0);
		std::cout << std::endl;
	}
	*/
}

TinyGP::~TinyGP()
{
	for (int i = 0; i < fitnesscases; i++) {
		delete[] targets[i];
	}
	delete[] targets;
#ifndef USE_POOL
	for (int i = 0; i < POPSIZE; i++) {
		delete[] pop[i];
	}
#endif 

	
	delete[] pop;
}

void TinyGP::evolve()
{
	char* newind;  // a new individual, points to heap memory allocated by crossover or mutation
	printParams();
	stats(0);
	for (int gen = 0; gen < GENERATIONS; gen++) {
		if (fbestpop > -1e-5) {
			std::cout << "Problem solved!" << std::endl;
			return;
		}
		for (int indivs = 0; indivs < POPSIZE; indivs++) {
			if (randZeroToOne() > CROSSOVER_PROB) {
				int parent1 = tournament(fitness, TSIZE);
				int parent2 = tournament(fitness, TSIZE);
				newind = crossover(pop[parent1], pop[parent2]);
			}
			else {
				int parent = tournament(fitness, TSIZE);
				newind = mutation(pop[parent], PMUT_PER_NODE);
			}
			double newfit = fitnessFunction(newind);
			int offspring = negativeTournament(fitness, TSIZE); 
			TIMER_START("free")
#ifdef USE_POOL
			mempool.freeIndiv(pop[offspring]);
#endif // USE_POOL
#ifndef USE_POOL
			delete[] pop[offspring];
#endif // !USE_POOL
			TIMER_STOP
			
			pop[offspring] = newind;
			fitness[offspring] = newfit;
		}
		stats(gen+1);
	}
}

void TinyGP::setupFitness(std::string fname)
{
	using namespace std;
	ifstream infile(fname);
	string   line;
	regex    reg("\\s+");
	getline(infile, line);
	sregex_token_iterator iter(line.begin(), line.end(), reg, -1);
	sregex_token_iterator end;
	vector<string> vec(iter, end);
	
	/* 
	FILE HEADER FORMAT:
	varnumber randomnumber minrandom maxrandom fitnesscases
	^         ^            ^         ^         ^
	|         |            |         |         number of fitness cases in file
	|         |            |         max random const value
	|         |            min random const value 
	|         no of random consts in terminal set
	no of inputs
	*/
	varnumber    = stoi(vec[0]);
	randomnumber = stoi(vec[1]);
	minrandom    = stod(vec[2]);
	maxrandom    = stod(vec[3]);
	fitnesscases = stoi(vec[4]);

	targets = new double*[fitnesscases];
	for (int i = 0; i < fitnesscases; i++) {
		targets[i] = new double[varnumber + 1];
	}

	int oncase = 0;
	while (getline(infile, line)) {
		//cout << line << endl;
		iter = sregex_token_iterator(line.begin(), line.end(), reg, -1);
		vector<string> vec(iter, end);
		if (vec.size() != varnumber + 1)
			cerr << "not the right number of values on line "<< oncase << ". Header specified " << varnumber << " inputs and 1 output, got "<< vec.size() << endl;
		for (int i = 0; i < vec.size(); i++) {
			double val = stod(vec[i]);
			targets[oncase][i] = val;
			//cout << targets[oncase][i] << endl;
		}
		//cout << endl;
		oncase++;
	}
	infile.close();
}

void TinyGP::createRandomPop()
{
	pop = new char* [POPSIZE];
	for (size_t i = 0; i < POPSIZE; i++) {
		pop[i] = createRandomIndividual(DEPTH);
		fitness[i] = fitnessFunction(pop[i]);
	}
	
}

char* TinyGP::createRandomIndividual(int depth)
{
	int len = grow(buffer, 0, MAX_LEN, depth);
	while (len < 0)
		len = grow(buffer, 0, MAX_LEN, depth);
#ifdef USE_POOL
	char* ind = mempool.getNewIndiv();
#endif // USE_POOL
#ifndef USE_POOL
	char* ind = new char[len];
#endif 

	
	memcpy(ind, buffer, len);
	return ind;
}

int TinyGP::grow(char* buffer, int pos, int max, int depth)
{
	char prim = (char)rand() % 2;
	if (pos >= max)
		return -1;
	if (pos == 0)
		prim = 1;
	if (prim == 0 || depth == 0) {
		prim = (char)(rand() % (varnumber + randomnumber));
		buffer[pos] = prim;

		return pos + 1;
	}
	else {
		prim = (char)(rand() % (FSET_END - FSET_START + 1)) + FSET_START;
		
		switch (prim) {
		case ADD:
		case SUB:
		case MUL:
		case DIV:
			buffer[pos] = prim;
			return grow(buffer, grow(buffer, pos + 1, max, depth - 1), max, depth - 1);
		}
	}
	return 0; // should never get here
}

int TinyGP::printIndividual(const char* buf, const int buffercounter)
{
	int a1 = 0, a2;
	if (buf[buffercounter] < FSET_START) {
		if (buf[buffercounter] < varnumber) {
			std::cout << " X" << (int)buf[buffercounter]+1 << " ";
		}
		else {
			std::cout << " " << x[buf[buffercounter]]<<" ";
			
		}
		return 1 + buffercounter;
	}
	switch (buf[buffercounter]) {
	case ADD:
		std::cout << "(";
		a1 = printIndividual(buf, 1 + buffercounter);
		std::cout << "+";
		break;
	case SUB:
		std::cout << "(";
		a1 = printIndividual(buf, 1 + buffercounter);
		std::cout << "-";
		break;
	case MUL:
		std::cout << "(";
		a1 = printIndividual(buf, 1 + buffercounter);
		std::cout << "*";
		break;
	case DIV:
		std::cout << "(";
		a1 = printIndividual(buf, 1 + buffercounter);
		std::cout << "/";
		break;
	}
	a2 = printIndividual(buf, a1);
	std::cout << ")";
	return a2;
}

double TinyGP::fitnessFunction(char* prog)
{
	int len;
	double result, fit = 0.0;

	len = traverse(prog, 0);
	for (int i = 0; i < fitnesscases; i++) {
		for (int j = 0; j < varnumber; j++) {
			x[j] = targets[i][j];
		}
		program = prog;
		PC = 0;
		result = run();
		fit += abs(result - targets[i][varnumber]);
	}
	return -fit;
}

int TinyGP::traverse(const char* buffer, int buffercount)
{
	if (buffer[buffercount] < FSET_START)
		return buffercount + 1;
	switch (buffer[buffercount]) {
	case ADD:
	case SUB:
	case MUL:
	case DIV:
		return traverse(buffer, traverse(buffer, buffercount + 1));
	}
	return 0; // should never get here
}

double TinyGP::run()
{
	// interpret program loaded into the class variable "program"
	// set PC to 0 before calling
	char primitive = program[PC++];
	if (primitive < FSET_START)
		return x[primitive];
	switch (primitive) {
	case ADD: return (run() + run());
	case SUB: return (run() - run());
	case MUL: return (run() * run());
	case DIV: 
		double num = run(), den = run();
		if (abs(den) <= 0.001) {
			return num;
		}
		else {
			return num / den;
		}
	}
	return 0.0; // should never get here
}

void TinyGP::stats(int generation)
{
	std::cout << std::endl;
	std::cout << "-------------------------------> Generation " << generation << " <-------------------------------" << std::endl;
	int best = rand() % POPSIZE;
	int node_count = 0;
	fbestpop = fitness[best];
	favgpop = 0.0;
	for (int i = 0; i < POPSIZE; i++) {
		node_count += traverse(pop[i], 0);
		favgpop += fitness[i];
		if (fitness[i] > fbestpop) {
			best = i;
			fbestpop = fitness[i];
		}
	}
	avg_len = (double)node_count / (double)POPSIZE;
	favgpop /= (double)POPSIZE;
	std::cout << "average fitness: " << favgpop << "\n";
	std::cout << "best fitness: " << fbestpop << "\n";
	std::cout << "average size: " << avg_len << "\n";
	std::cout << "best individual: " << "\n";
	printIndividual(pop[best], 0);
	std::cout << std::endl;
}

int TinyGP::negativeTournament(double* fitness, int tsize)
{
	int worst = rand() % POPSIZE, competitor;
	double fworst = -1.0e34;
	for (int i = 0; i < tsize; i++) {
		competitor = rand() % POPSIZE;
		if (fitness[competitor] < fworst) {
			fworst = fitness[competitor];
			worst = competitor;
		}
	}
	return worst;
}

char* TinyGP::crossover(const char* parent1, const char* parent2)
{
	int len1 = traverse(parent1, 0);
	int len2 = traverse(parent2, 0);

	int xo1start = rand() % len1;
	int xo1end   = traverse(parent1, xo1start);

	int xo2start = rand() % len2;
	int xo2end   = traverse(parent2, xo2start);

	int lenoff = xo1start + (xo2end - xo2start) + (len1 - xo1end);
#ifdef USE_POOL
	char* offspring = mempool.getNewIndiv();
#endif // USE_POOL
#ifndef USE_POOL
	char* offspring = new char[lenoff];
#endif
	

	memcpy(offspring, parent1, xo1start);
	memcpy(offspring + xo1start, parent2 + xo2start, (xo2end - xo2start));
	memcpy(offspring + (xo1start + xo2end - xo2start), parent1 + xo1end, len1 - xo1end);
	return offspring;
}

char* TinyGP::mutation(const char* parent, const double pmut)
{
	//TIMER_START("mutation")
	int len = traverse(parent, 0);
	int mutsite;
#ifdef USE_POOL
	char* parentcopy = mempool.getNewIndiv();
#endif
#ifndef USE_POOL
	char* parentcopy = new char[len];
#endif 

	memcpy(parentcopy, parent, len);
	for (int i = 0; i < len; i++) {
		if (randZeroToOne() < pmut) {
			mutsite = i;
			if (parentcopy[mutsite] < FSET_START) {
				parentcopy[mutsite] = (char)rand() % varnumber;
			}
			else {
				switch (parentcopy[mutsite]) {
				case ADD:
				case SUB:
				case MUL:
				case DIV:
					parentcopy[mutsite] = (char)(rand() % (FSET_END - FSET_START + 1) + FSET_START);
				}
			}
		}
	}
	//TIMER_STOP
	return parentcopy;
}

void TinyGP::printParams()
{
	using namespace std;
	cout << endl;
	cout << "Tiny GP " << endl;
	cout << "SEED: " << seed << endl;
	cout << "MAX_LEN: " << MAX_LEN << endl;
	cout << "POPSIZE: " << POPSIZE << endl;
	cout << "DEPTH: " << DEPTH << endl;
	cout << "CROSSOVER_PROB: " << CROSSOVER_PROB << endl;
	cout << "PMUT_PER_NODE: " << PMUT_PER_NODE << endl;
	cout << "MIN_RANDOM: " << minrandom << endl;
	cout << "MAX_RANDOM: " << maxrandom << endl;
	cout << "GENERATIONS: " << GENERATIONS << endl;
	cout << "TSIZE: " << TSIZE << endl;
	cout << endl;
}

int TinyGP::tournament(double* fitness, int tsize)
{
	int best = rand() % POPSIZE, competitor;
	double fbest = -1.0e34;
	for (int i = 0; i < tsize; i++) {
		competitor = rand() % POPSIZE;
		if (fitness[competitor] > fbest) {
			fbest = fitness[competitor];
			best = competitor;
		}
	}
	return best;
}

void TGP_MemPool::allocate(const int ml, const int ps)
{
	max_len = ml;
	popsize = ps;
	pool = new char[max_len * popsize + max_len];
	//memset(pool, 0xff, max_len * popsize + max_len);
	last_slot = pool + max_len * popsize;
	free_slot = pool;
}

void TGP_MemPool::reallocate(const int max_len, const int popsize)
{
	deallocate();
	allocate(max_len, popsize);
}

char* TGP_MemPool::getNewIndiv()
{
	char* f = free_slot;
	free_slot += max_len;
	return f;
}

void TGP_MemPool::freeIndiv(char* ind)
{
	assert(ind <= (last_slot) && ind >= pool);
	//memset(ind, 0xff, max_len);
	free_slot = ind;
}
