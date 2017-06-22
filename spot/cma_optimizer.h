#pragma once

#include "optimizer.h"

namespace spot
{
	enum class cma_weights { equal = 0, linear = 1, log = 2 };

	class SPOT_API cma_optimizer : public optimizer
	{
	public:
		cma_optimizer( const objective& obj, int lambda = 0, int seed = 123, cma_weights weights = cma_weights::log );
		virtual ~cma_optimizer();

		// optimization
		const search_point_vec& sample_population();
		void update_distribution( const fitness_vec_t& results );
		virtual void step() override;

		// analysis
		par_vec current_mean() const;
		par_vec current_std( bool use_covariance = true ) const;
		vector< par_vec > current_covariance() const;

		// actual parameters
		int dim() const;
		int lambda() const;
		int mu() const;
		int random_seed() const;
		double sigma() const;

	protected:
		struct pimpl_t* pimpl;
	};
}