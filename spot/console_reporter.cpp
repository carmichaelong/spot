#include "console_reporter.h"

#include "xo/container/container_tools.h"

namespace spot
{
	
	console_reporter::console_reporter( int individual_precision, int summary_precision ) : individual_precision_( individual_precision ), summary_precision_( summary_precision )
	{

	}

	void console_reporter::evaluate( const optimizer& opt, const search_point& point, fitness_t fitness )
	{
		printf( "%.*f ", individual_precision_, fitness );
	}

	void console_reporter::evaluate( const optimizer& opt, const search_point_vec& pop, const fitness_vec_t& fitnesses, index_t best_idx, bool new_best )
	{
		auto avg = xo::average( fitnesses.begin(), fitnesses.end() );
		auto med = xo::median( fitnesses.begin(), fitnesses.end() );
		printf( " A=%.*f M=%.*f", summary_precision_, avg, summary_precision_, med );
		if ( new_best )
			printf( " B=%.*f\n", summary_precision_, opt.best_fitness() );
		else printf( "\r" );
	}

	void console_reporter::finish( const optimizer& opt )
	{
		printf( "Optimization finished\n" );
	}

	void console_reporter::next_step( const optimizer& opt, size_t gen )
	{
		printf( "%04d: ", ( int )gen );
	}

	void console_reporter::start( const optimizer& opt )
	{
		printf( "Starting optimization, dim=%d\n", ( int )opt.info().dim() );
	}

}
