#pragma once
#include<string>

#define ADD 110
#define SUB 111
#define MUL 112
#define DIV 113
#define FSET_START 110
#define FSET_END 113
class TGP_MemPool {
public:
	TGP_MemPool() {}
	~TGP_MemPool() { deallocate(); }
	void allocate(const int max_len, const int popsize);
	void deallocate() { delete[] pool; }
	void reallocate(const int max_len, const int popsize);
	char* getNewIndiv();
	void freeIndiv(char* ind);
private:
	char* pool;
	int max_len;
	int popsize;
	char* free_slot;
	char* last_slot;
};

class TinyGP
{
public:
						TinyGP(std::string fname, long seed);
						~TinyGP();
	void                evolve();
private:
	/*
		load fitness cases from file
	*/
	void				setupFitness(std::string fname);
	/*
		Fills pop with POPSIZE random programs.
		Also calls fitnessFunction on each and gathers result in 'fitness'
	*/
	void				createRandomPop();
	/*
		return a ptr to a newly allocated random individual.
		First calls grow to grow an individual into a buffer
		of size MAX_LEN then allocates a new array that fits it exactly
		using the return value of grow, and copies it into this, returning the ptr.
	*/
	char*				createRandomIndividual(int depth);
	/*
		grow a random program and return its size
	*/
	int					grow(char* buffer, int pos, int max, int depth); 
	/*
		print an individual program in standard mathematical
		notation, the same kind used in C++, excel, ect. 
		Starting at the index buffercounter.
		Prints variables as "X1", "X2",... ect.
	*/
	int                 printIndividual(const char* buffer, int buffercounter);
	/*
		interpret the program with all fitness cases and return
		the sum of fitnesses
	*/
	double              fitnessFunction(char* prog);
	/*
		return length of the program pointed to by 'buffer' from node buffercount
	*/
	int                 traverse(const char* buffer, const int buffercount);         
	/*
		interpret the program pointed to by 'program' and return result
	*/
	double              run();                                           
	/*
		print best individual, best fitness, average fitness and average size of pop
	*/
	void                stats(int generation);       
	/*
		compare tsize random individuals and return the
		index of the one with the best (highest) fitness.
		used to select fit individuals to cross over or mutate
	*/
	int                 tournament(double* fitness, int tsize);    
	/*
		compare tsize random individuals and return the
		index of the one with the worst (lowest) fitness.
		Used to select an unfit individual to be deallocated and replaced with a new program,
		produced by either crossover or mutation.
	*/
	int                 negativeTournament(double* fitness, int tsize);
	/*
		cross over the two trees pointed to by parent1 and 2
		at a random point on each one, at which they exchange sub trees.
		Returns a ptr to the newly allocated "offspring", leaves parents unchanged.
	*/
	char*				crossover(const char* parent1, const char* parent2);
	/*
		iterates over a copy of the parent tree, with PMUT_PER_NODE probability of exchanging
		each node for a random new node of the same type (terminal or function).
		Returns ptr to the new copy, leaves parent unchanged.
	*/
	char*				mutation(const char* parent1, const double pmut);
	/*
		prints run parameters
	*/
	void                printParams();
private:
	char**				pop;
	double				fitness[100000];
	
	double				x[ADD]; // the set of terminal nodes - first varnumber of inputs then randomnumber random consts
							    // values in the program less than 110 are indexes into this array
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
	static const int	MAX_LEN = 10000,//10000
						POPSIZE = 100000,
						DEPTH = 5,
						GENERATIONS = 100,
						TSIZE = 2;
	const double		PMUT_PER_NODE	= 0.05,
						CROSSOVER_PROB	= 0.9;
	double**			targets;
	char				buffer[MAX_LEN];
	TGP_MemPool         mempool;
};

