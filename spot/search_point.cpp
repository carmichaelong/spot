#include "search_point.h"

#include "xo/container/container_tools.h"
#include <fstream>

namespace spot
{
	search_point::search_point( const objective_info& inf ) : info_( inf ), values_( inf.size() )
	{
		// init with mean values
		for ( index_t i = 0; i < info_.size(); ++i )
			values_[ i ] = info_[ i ].mean;
		round_values();
	}

	search_point::search_point( const objective_info& inf, const par_vec& values ) : info_( inf ), values_( values )
	{
		xo_assert( info_.size() == values_.size() );
		round_values();
	}

	search_point::search_point( const objective_info& inf, par_vec&& values ) : info_( inf ), values_( std::move( values ) )
	{
		xo_assert( info_.size() == values_.size() );
		round_values();
	}

	search_point::search_point( const objective_info& inf, const path& filename ) : search_point( inf )
	{
		import_values( filename );
	}

	optional_par_value search_point::try_get( const string& full_name ) const
	{
		// see if this is a parameter
		auto idx = info_.find_index( full_name );
		if ( idx != no_index )
			return values_[ idx ];
		else return info_.try_get_locked( full_name );
	}

	par_value search_point::add( const string& full_name, par_value mean, par_value std, par_value min, par_value max )
	{
		xo_error( "Previously undefined parameter: " + full_name );
	}

	size_t search_point::import_values( const path& filename )
	{
		size_t params_read = 0;
		std::ifstream str( filename.str() );
		xo_error_if( !str.good(), "Could not open " + filename.string() );
		while ( str.good() )
		{
			string name;
			par_value value, mean, stdev;
			str >> name >> value >> mean >> stdev;
			if ( str.good() )
			{
				index_t idx = info().find_index( name );
				if ( idx != no_index )
				{
					values_[ idx ] = value;
					++params_read;
				}
			}
		}
		return params_read;
	}

	void search_point::set_values( const par_vec& values )
	{
		xo_assert( values_.size() == values.size() );
		for ( size_t idx = 0; idx < values.size(); ++idx )
			values_[ idx ] = rounded( values[ idx ] );
	}

	void search_point::round_values()
	{
		for ( auto& v : values_ )
			v = rounded( v );
	}

	par_value search_point::rounded( par_value v )
	{
		std::stringstream str;
		str << std::setprecision( 8 ) << v;
		str >> v;
		return v;
	}

	std::ostream& operator<<( std::ostream& str, const search_point& ps )
	{
		size_t max_width = 0;
		for ( auto& inf : ps.info() )
			max_width = std::max( max_width, inf.name.size() );

		for ( index_t idx = 0; idx < ps.size(); ++idx )
		{
			auto& inf = ps.info()[ idx ];
			str << std::left << std::setw( max_width ) << inf.name << "\t";
			str << std::setprecision( 8 ) << ps[ idx ] << "\t" << inf.mean << "\t" << inf.std << "\t" << std::endl;
		}
		return str;
	}

	pair< par_vec, par_vec > SPOT_API compute_mean_std( const search_point_vec& pop )
	{
		const auto& info = pop.front().info();

		par_vec mean( info.dim() );
		for ( auto& ind : pop )
		{
			for ( index_t i = 0; i < info.dim(); ++i )
				mean[ i ] += ind[ i ] / pop.size();
		}

		par_vec stds( info.dim() );
		for ( index_t pop_idx = 0; pop_idx < pop.size(); ++pop_idx )
		{
			for ( index_t i = 0; i < info.dim(); ++i )
				stds[ i ] += squared( pop[ pop_idx ][ i ] - mean[ i ] ) / pop.size();
		}
		for ( index_t i = 0; i < info.dim(); ++i )
			stds[ i ] = sqrt( stds[ i ] );

		return std::make_pair( std::move( mean ), std::move( stds ) );
	}
}
