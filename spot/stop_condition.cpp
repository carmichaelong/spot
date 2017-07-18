#include "stop_condition.h"
#include "optimizer.h"

namespace spot
{
	bool abort_condition::test( const optimizer& opt )
	{
		return opt.test_abort();
	}

	bool flat_fitness_condition::test( const optimizer& opt )
	{
		best_ = opt.current_step_best();
		median_ = opt.current_step_median();
		return flut::math::equals( best_, median_, epsilon_ );
	}

	bool max_steps_condition::test( const optimizer& opt )
	{
		return opt.current_step() >= max_steps_;
	}

	bool min_progress_condition::test( const optimizer& opt )
	{
		if ( progress_window_.full() )
			progress_window_.pop_front();
		progress_window_.push_back( float( opt.current_step_best() ) );
		if ( progress_window_.size() < 2 )
			return false;

		float window = static_cast< float >( progress_window_.size() );
		float start = static_cast< float >( opt.current_step() ) - window;

		progress_regression_ = linear_regression( start, 1.0f, progress_window_ );
		auto scale = progress_regression_( start + 0.5f * window );
		progress_ = opt.info().minimize() ? -progress_regression_.slope() / scale : progress_regression_.slope() / scale;

		if ( progress_window_.full() )
			return progress_ < min_progress_;
		else return false;
	}
}
