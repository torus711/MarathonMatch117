#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
#include <utility>
#include <type_traits>
#include <cmath>
#include <cassert>
#include <cstdio>

using namespace std;
using namespace placeholders;

using LL = long long;
using ULL = unsigned long long;
using VI = vector< int >;
using VVI = vector< vector< int > >;
using VS = vector< string >;
using ISS = istringstream;
using OSS = ostringstream;
using PII = pair< int, int >;
using VPII = vector< pair< int, int > >;
template < typename T = int > using VT = vector< T >;
template < typename T = int > using VVT = vector< vector< T > >;
template < typename T = int > using LIM = numeric_limits< T >;

template < typename T > inline istream& operator>>( istream &s, vector< T > &v ){ for ( T &t : v ) { s >> t; } return s; }
template < typename T > inline ostream& operator<<( ostream &s, const vector< T > &v ){ for ( int i = 0; i < int( v.size() ); ++i ){ s << ( " " + !i ) << v[i]; } return s; }

void in_impl(){};
template < typename T, typename... TS > void in_impl( T &head, TS &... tail ){ cin >> head; in_impl( tail ... ); }
#define IN( T, ... ) T __VA_ARGS__; in_impl( __VA_ARGS__ );

template < typename T > inline T fromString( const string &s ) { T res; istringstream iss( s ); iss >> res; return res; }
template < typename T > inline string toString( const T &a ) { ostringstream oss; oss << a; return oss.str(); }

#define NUMBERED( name, number ) NUMBERED2( name, number )
#define NUMBERED2( name, number ) name ## _ ## number
#define REP1( n ) REP2( NUMBERED( REP_COUNTER, __LINE__ ), n )
#define REP2( i, n ) REP3( i, 0, n )
#define REP3( i, m, n ) for ( int i = ( int )( m ); i < ( int )( n ); ++i )
#define GET_REP( a, b, c, F, ... ) F
#define REP( ... ) GET_REP( __VA_ARGS__, REP3, REP2, REP1 )( __VA_ARGS__ )
#define FOR( e, c ) for ( auto &&e : c )
#define ALL( c ) begin( c ), end( c )
#define AALL( a ) ( remove_all_extents< decltype( a ) >::type * )a, ( remove_all_extents< decltype( a ) >::type * )a + sizeof( a ) / sizeof( remove_all_extents< decltype( a ) >::type )

#define SZ( v ) ( (int)( v ).size() )
#define EXISTS( c, e ) ( ( c ).find( e ) != ( c ).end() )

template < typename T > inline bool chmin( T &a, const T &b ){ if ( b < a ) { a = b; return true; } return false; }
template < typename T > inline bool chmax( T &a, const T &b ){ if ( a < b ) { a = b; return true; } return false; }

#define PB push_back
#define EM emplace
#define EB emplace_back
#define BI back_inserter

#define MP make_pair
#define fst first
#define snd second

#define DUMP( x ) cerr << #x << " = " << ( x ) << endl

#include <memory>
#include <random>

constexpr int dy[] = { 0, 1, 0, -1 };
constexpr int dx[] = { 1, 0, -1, 0 };

struct Global
{
	const int N = -1;
	const int P = -1;
	const VVI initial_board;

	Global()
	{
		cin >> const_cast< int& >( N );
		cin >> const_cast< int& >( P );

		VVI board( N, VI ( N ) );
		cin >> board;
		for_each( ALL( board ), []( VI &row ){ transform( ALL( row ), begin( row ), bind( minus< int >(), _1, 1 ) ); } );
		const_cast< VVI& >( initial_board ) = board;
		return;
	}
};
Global global;

int get_penalty( const VVI &board )
{
	int res = 0;
	REP( i, global.N )
	{
		REP( j, global.N )
		{
			const int a = board[i][j];
			const int ti = a / global.N;
			const int tj = a % global.N;
			res += global.P * ( abs( i - ti ) + abs( j - tj ) );
		}
	}
	return res;
}

class Board
{
private:
	VVI board_;

	int score_;

	VS history_;

public:
	Board() : board_( global.initial_board ), score_( get_penalty( board_ ) )
	{
		return;
	}

// 	Board( const Board &rhs ) :
// 		board_( rhs.board_ ), score_( rhs.score_ ),
// 		history_( rhs.history_ )
// 	{
// 		return;
// 	}

	void rotate( const int r, const int c, const int s, const char dir, const bool rollback = false )
	{
		score_ += floor( pow( s - 1, 1.5 ) );

		const auto score_differential = [&]( const int sgn )
		{
			REP( i, r, r + s )
			{
				REP( j, c, c + s )
				{
					const int a = board_[i][j];
					const int ty = a / global.N;
					const int tx = a % global.N;

					score_ += sgn * global.P * ( abs( ty - i ) + abs( tx - j ) );
				}
			}
		};

		score_differential( -1 );
		rotate_board( r, c, s, dir );
		score_differential( 1 );

		if ( rollback )
		{
			score_ -= 2 * floor( pow( s - 1, 1.5 ) );
			history_.pop_back();
		}
		else
		{
			history_.EB( toString( r ) + " " + toString( c ) + " " + toString( s ) + " " + dir );
		}

		return;
	}

	void rotate_board( const int r, const int c, const int s, const char dir )
	{
		assert( 0 <= r && r + s <= global.N && 0 <= c && c + s <= global.N );

		VVI stamp( s, VI( s ) );
		REP( i, s )
		{
			REP( j, s )
			{
				if ( dir == 'R' )
				{
					stamp[j][ s - 1 - i ] = board_[ r + i ][ c + j ];
				}
				else
				{
					stamp[ s - 1 - j ][i] = board_[ r + i ][ c + j ];
				}
			}
		}

		REP( i, s )
		{
			REP( j, s )
			{
				board_[ r + i ][ c + j ] = stamp[i][j];
			}
		}

		return;
	}

	int score() const
	{
		return score_;
	}

	PII position_of( const int t )
	{
		REP( i, global.N )
		{
			REP( j, global.N )
			{
				if ( board_[i][j] == t )
				{
					return { i, j };
				}
			}
		}
		assert( false );
		return { -1, -1 };
	}

	void yose()
	{
// 		REP( i, global.N * global.N - 2 * global.N )
// 		{
// 			fix( i );
// 		}
// 		REP( i, global.N - 2 )
// 		{
// 			fix_2row( ( global.N - 2 ) * global.N + 1 + i );
// 		}

		VVI filled( global.N, VI( global.N ) );

		int ty = 0, tx = 0;
		for ( int d = 0, done = 0; done + 4 < global.N * global.N; ++done )
		{
// 			DUMP( done );
			const int t = ty * global.N + tx;
// 			DUMP( t );
			int y, x;
			tie( y, x ) = position_of( t );

			const int ny = ty + dy[d];
			const int nx = tx + dx[d];
			const bool corner = !( 0 <= ny && ny < global.N && 0 <= nx && nx < global.N ) ||
					filled[ ny ][ nx ];
			if ( corner )
			{
				if ( abs( ty - y ) == 1 && abs( tx - x ) == 1 )
				{
					int r = y;
					int c = x;
					if ( d == 0 )
					{
						--c;
					}
					if ( d == 1 )
					{
						--r;
						--c;
					}
					if ( d == 2 )
					{
						--r;
					}
					rotate( r, c, 2, 'R' );
					tie( y, x ) = position_of( t );
				}

				if ( d == 0 )
				{
					rotate( ty, tx - 1, 2, 'R' );
				}
				if ( d == 1 )
				{
					rotate( ty - 1, tx - 1, 2, 'R' );
				}
				if ( d == 2 )
				{
					rotate( ty - 1, tx, 2, 'R' );
				}
				if ( d == 3 )
				{
					rotate( ty, tx, 2, 'R' );
				}
			}
			tie( y, x ) = position_of( t );

			while ( d % 2 ? y != ty : x != tx )
			{
// 				cerr << "START" << endl;
				constexpr int dr[] = { 0,  0, -1, -1 };
				constexpr int dc[] = { 0, -1, -1,  0 };

				int r = y;
				int c = x;
// 				DUMP( y );
// 				DUMP( x );
				bool rev = false;
				if ( d == 0 )
				{
					if ( y < global.N / 2 )
					{
						// do nothing
					}
					else
					{
						--r;
						rev ^= true;
					}
					if ( tx < x )
					{
						--c;
						rev ^= true;
					}
				}
				if ( d == 1 )
				{
					--c;
					if ( x < global.N / 2 )
					{
						++c;
						rev ^= true;
					}
					else
					{
						// do nothing
					}
					if ( ty < y )
					{
						--r;
						rev ^= true;
					}
				}
				if ( d == 2 )
				{
					--r;
					--c;
					if ( y < global.N / 2 )
					{
						++r;
						rev ^= true;
					}
					else
					{
						// do nothing
					}
					if ( x < tx )
					{
						++c;
						rev ^= true;
					}
				}
				if ( d == 3 )
				{
					--r;
					if ( x < global.N / 2 )
					{
						// do nothing
					}
					else
					{
						--c;
						rev = true;
					}
					if ( y < ty )
					{
						++r;
						rev ^= true;
					}
				}

				rotate( r, c, 2, "LR"[ 1 - rev ] );
				tie( y, x ) = position_of( t );
			}
// 				DUMP( t );
			while ( d % 2 ? x != tx : y != ty )
			{
				constexpr int dr[] = { -1, 0,  0, -1 };
				constexpr int dc[] = {  0, 0, -1, -1 };

// 				cerr << "START" << endl;
				int r = y;
				int c = x;
				bool rev = false;
				if ( d == 0 )
				{
					--r;
					if ( corner )
					{
						--c;
						rev ^= true;
					}
				}
				if ( d == 1 )
				{
					if ( corner )
					{
						--r;
						rev -= true;
					}
				}
				if ( d == 2 )
				{
					--c;
					if ( corner )
					{
						++c;	
						rev ^= true;
					}
				}
				if ( d == 3 )
				{
					--r;
					--c;
					if ( corner )
					{
						++r;
						rev ^= true;
					}
				}


// 				cerr << "START" << endl;
// 				DUMP( r );
// 				DUMP( c );
// 				DUMP( t );
// 				DUMP( y );
// 				DUMP( x );
// 				DUMP( ty );
// 				DUMP( tx );
// 				DUMP( r );
// 				DUMP( c );
				rotate( r, c,  2, "LR"[ 1 - rev ]);
// 				cerr << "END" << endl;
				tie( y, x ) = position_of( t );
// 				cerr << "END" << endl;
			}

			filled[ ty ][ tx ] = true;
			if ( corner )
			{
				++d %= 4;
			}
			ty += dy[d];
			tx += dx[d];
		}

		return;
	}

	void fix( const int t )
	{
		const int ty = t / global.N;
		const int tx = t % global.N;

// 		DUMP( t );
// 		DUMP( ty );
// 		DUMP( tx );

		int y, x;
		tie( y, x ) = position_of( t );
// 		DUMP( y );
// 		DUMP( x );

		if ( tie( y, x ) == tie( ty, tx ) )
		{
			return;
		}

		if ( tx == global.N - 1 )
		{
			if ( board_[ ty + 1 ][ tx - 1 ] == t )
			{
				rotate( ty + 1, tx - 1, 2, 'L' );
			}
			rotate( ty, tx - 1, 2, 'R' );
		}

		tie( y, x ) = position_of( t );

		// move into right below cell
// 		cerr << "Step 1" << endl;
		while ( tx != x )
		{
			if ( x < tx )
			{
				if ( y + 1 < global.N )
				{
					rotate( y, x, 2, 'R' );
				}
				else
				{
					rotate( y - 1, x, 2, 'L' );
				}
				++x;
			}
			else
			{
				if ( y + 1 < global.N )
				{
					rotate( y, x - 1, 2, 'L' );
				}
				else
				{
					rotate( y - 1, x - 1, 2, 'R' );
				}
				--x;
			}
		}

		// move target into a row of right below
// 		cerr << "Step 2" << endl;
		while ( ty < y )
		{
			if ( x + 1 < global.N )
			{
				rotate( y - 1, x, 2, 'R' );
			}
			else
			{
				rotate( y - 1, x - 1, 2, 'L' );
			}
			--y;
		}


		// move into target cell
// 		cerr << "Step 3" << endl;
// 		if ( x == global.N - 1 )
// 		{
// 			rotate( ty, tx - 1, 2, 'L' );
// 		}
// 		else
// 		{
// 			rotate( ty, tx, 2, 'R' );
// 		}

		return;
	}

	void fix_2row( const int t1 )
	{
		const int t2 = t1 + global.N;
		const int ty = t1 / global.N;
		const int tx = t1 % global.N;

		if ( position_of( t1 ).snd == position_of( t2 ).snd &&
				position_of( t1 ).snd == tx )
		{
			return;
		}

		while ( position_of( t1 ).snd == tx )
		{
			rotate( ty, tx, 2, 'L' );
		}

		{ // move t2
			int y, x;
			tie( y, x ) = position_of( t2 );

			if ( y + 1 == global.N )
			{
				if ( x + 1 < global.N )
				{
					rotate( y - 1, x, 2, 'R' );
				}
				else
				{
					rotate( y - 1, x - 1, 2, 'L' );
				}
				--y;
			}

			while ( tx < x )
			{
				rotate( y, x - 1, 2, 'L' );
				--x;
			}
		}
		{ // move t1
			int y, x;
			tie( y, x ) = position_of( t1 );
			if ( !( tx <= x ) )
			{
				return;
			}
			assert( tx <= x ); // TODO: it fired
// 			assert( tx < x );

			if ( y + 1 == global.N )
			{
				if ( x + 1 < global.N )
				{
					rotate( y - 1, x, 2, 'R' );
				}
				else
				{
					rotate( y - 1, x - 1, 2, 'L' );
				}
				--y;
			}
			while ( tx < x )
			{
				rotate( y, x - 1, 2, 'L' );
				--x;
			}
		}

		return;
	}

	VS history() const
	{
		return history_;
	}

	bool operator<( const Board &rhs ) const
	{
		if ( score_ != rhs.score_ )
		{
			return score_ < rhs.score_;
		}
		return board_ < rhs.board_;
	}
};

// constexpr int MAX_MOVES = 128;
//
// mt19937 rng;
//
// VS beam_search()
// {
// 	constexpr double EXEC_TIME = 9.5;
// // 	constexpr double EXEC_TIME = 1.0;
// 	constexpr int BEAM_WIDTH = 256;
//
// 	const clock_t clock_start = clock();
//
// 	VT< set< Board > > queues( MAX_MOVES + 1 );
// 	queues[0].EM();
//
// 	Board best_board;
// 	int best_score = best_board.score();
//
// // 	uniform_int_distribution< int > rng_2N( 2, global.N );
//
// 	int turn = 0, iteration = 0;
// 	clock_t clock_now;
// 	while ( 1. * ( ( clock_now = clock() ) - clock_start ) / CLOCKS_PER_SEC <= EXEC_TIME )
// 	{
// 		const double t = 1 - 1. * ( ( clock_now - clock_start ) / CLOCKS_PER_SEC ) / EXEC_TIME;
// 		const double c = 4 + t * max( global.N, global.N / 8 );
// 		normal_distribution< double > rng_norm( c, sqrt( global.N ) );
//
// 		const int i = turn++ % MAX_MOVES;
// 		auto &currents = queues[i];
// 		auto &nexts = queues[ i + 1 ];
//
// // 		REP( 128 )
// 		{
// 			if ( currents.empty() )
// 			{
// 				continue;
// 			}
//
// 			Board board = *begin( currents );
// 			currents.erase( begin( currents ) );
//
// 			REP( min( 64, global.N * global.N * global.N ) )
// // 			REP( s, 3, global.N + 1 ) REP( r, global.N - s + 1 ) REP( c, global.N - s + 1 ) REP( d, 2 )
// 			{
// // 				const int s = rng_2N( rng );
// 				const int s = min( global.N, max( 2, int( rng_norm( rng ) ) ) );
// 				uniform_int_distribution< int > rng_pos( 0, global.N - s );
// 				const int r = rng_pos( rng );
// 				const int c = rng_pos( rng );
// 				const int d = rng() % 2;
//
// 				board.rotate( r, c, s, "LR"[d] );
//
// 				if ( chmin( best_score, board.score() ) )
// 				{
// 					best_board = board;
// 					// 					DUMP( best_score );
// 				}
//
// 				nexts.insert( board );
//
// 				board.rotate( r, c, s, "RL"[d], true );
// 			}
//
// 			while ( BEAM_WIDTH < SZ( nexts ) )
// 			{
// 				nexts.erase( ++nexts.rbegin().base() );
// 			}
//
// 			++iteration;
// 		}
// 	}
//
// 	DUMP( iteration );
// 	DUMP( SZ( best_board.history() ) );
//
// 	best_board.yose();
// 	best_score = best_board.score();
//
// // 	Board res( best_board );
// // 	REP( 4 )
// // 	{
// // 		best_board.rotate( global.N / 2, global.N / 2, 2, 'R' );
// // 		if ( chmin( best_score, best_board.score() ) )
// // 		{
// // 			res = best_board;
// // 		}
// // 	}
//
// 	cerr << ( 1. * ( clock() - clock_start ) / CLOCKS_PER_SEC ) << endl;
// 	return best_board.history();
// }

// constexpr int MAX_MOVES = 100000;
constexpr int MAX_MOVES = 256;

mt19937 rng;

VS beam_search()
{
	constexpr double EXEC_TIME = 9.0;
	constexpr int BEAM_WIDTH = 128;

	const clock_t clock_start = clock();

	VVT< Board > queues( MAX_MOVES + 1 );
	queues[0].EB();

	Board best_board;
	int best_score = best_board.score();

	uniform_int_distribution< int > rng_2N( 2, global.N / 2 );

	while ( 1. * ( clock() - clock_start ) / CLOCKS_PER_SEC <= EXEC_TIME )
	{
		REP( i, MAX_MOVES )
		{
			auto &currents = queues[i];
			auto &nexts = queues[ i + 1 ];
			if ( currents.empty() )
			{
				continue;
			}

			iter_swap( begin( currents ), min_element( ALL( currents ) ) );
			Board &board = currents[0];

			REP( 128 )
			{
				const int s = rng_2N( rng );
				uniform_int_distribution< int > rng_pos( 0, global.N - s );
				const int r = rng_pos( rng );
				const int c = rng_pos( rng );
				const int d = rng() % 2;

				board.rotate( r, c, s, "LR"[d] );

				if ( chmin( best_score, board.score() ) )
				{
					best_board = board;
// 					DUMP( best_score );
				}

				nexts.PB( board );

				board.rotate( r, c, s, "RL"[d], true );
			}

			const auto cutting_pos = begin( nexts ) + min( SZ( nexts ), BEAM_WIDTH );
			nth_element( begin( nexts ), cutting_pos, end( nexts ) );
			nexts.erase( cutting_pos, end( nexts ) );
		}
	}

	best_board.yose();
	return best_board.history();
}


int main()
{
	cin.tie( 0 );
	ios::sync_with_stdio( false );
	cout << setprecision( 12 ) << fixed;

// 	constexpr double EXEC_TIME = 9.5;
//
// 	const clock_t clock_start = clock();
//
// 	cerr << "TEST" << endl;
// 	MonteCarloSearchTree mcst;
//
// 	while ( 1. * ( clock() - clock_start ) / CLOCKS_PER_SEC <= EXEC_TIME )
// 	{
// 		mcst.explore();
// 	}
// 	mcst.output_bestmove();
//
// 	DUMP( MonteCarloSearchTree::total_playout_ );
	
	auto res = beam_search();
	cerr << "ENDED" << endl;
	DUMP( SZ( res ) );

	res.resize( min( SZ( res ), 100000 ) );
	cout << SZ( res ) << endl;
	copy( ALL( res ), ostream_iterator< string >( cout, "\n" ) );
	cout << flush;

	return 0;
}
