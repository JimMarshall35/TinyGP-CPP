#pragma once
#include<string>

#define ADD 110
#define SUB 111
#define MUL 112
#define DIV 113
#define FSET_START 110
#define FSET_END 113
class TinyGP
{
public:
						TinyGP(std::string fname, long seed);
						~TinyGP();
	void                evolve();
private:
	void				setupFitness(std::string fname);
	void				createRandomPop();
	char*				createRandomIndividual(int depth);
	int					grow(char* buffer, int pos, int max, int depth);
	int                 printIndividual(char* buffer, int buffercounter);
	double              fitnessFunction(char* prog);
	int                 traverse(char* buffer, int buffercount);
	double              run();
	void                stats(int generation);
	int                 tournament(double* fitness, int tsize);
	int                 negativeTournament(double* fitness, int tsize);
	char*				crossover(char* parent1, char* parent2);
	char*				mutation(char* parent1, double pmut);
	void                printParams();
private:
	char**				pop;
	double				fitness[100000];
	
	double				x[ADD]; // the set of terminal nodes - first varnumber of inputs then randomnumber random consts
	double				minrandom,
						maxrandom;
	char*				program;
	int					PC;
	int					varnumber,
						fitnesscases,
						randomnumber;
	double				fbestpop		= 0.0,
						favgpop			= 0.0;
	long				seed;
	double				avg_len;
	static const int	MAX_LEN = 10000,
						POPSIZE = 100000,
						DEPTH = 5,
						GENERATIONS = 100,
						TSIZE = 3;//2;
	const double		PMUT_PER_NODE	= 0.05,
						CROSSOVER_PROB	= 0.9;
	double**			targets;
	char				buffer[MAX_LEN];

};

