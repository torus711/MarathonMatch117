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

VVI rotate_board( VVI board, const int r, const int c, const int s, const char dir )
{
	assert( 0 <= r && r + s <= global.N && 0 <= c && c + s <= global.N );

	REP( dir == 'R' ? 1 : 3 )
	{
		VVI stamp( s, VI( s ) );
		REP( i, s )
		{
			REP( j, s )
			{
				stamp[j][ s - 1 - i ] = board[ r + i ][ c + j ];
			}
		}
		REP( i, s )
		{
			REP( j, s )
			{
				board[ r + i ][ c + j ] = stamp[i][j];
			}
		}
	}

	return move( board );
}

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
			res += abs( i - ti ) + abs( j - tj );
		}
	}
	return res;
}

class Board
{
private:
	VVI board_;
	VS history_;

public:
	Board() : board_( global.initial_board )
	{
		return;
	}

	void rotate( const int r, const int c, const int s, const char dir )
	{
		board_ = rotate_board( board_, r, c, s, dir );
		history_.EB( toString( r ) + " " + toString( c ) + " " + toString( s ) + " " + dir );
		return;
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
			rotate( ty, tx - 1, 2, 'R' );
		}

		if ( ty == y )
		{
			if ( x + 1 < global.N )
			{
				rotate( y, x, 2, 'L' );
			}
			else
			{
				rotate( y, x - 1, 2, 'R' );
			}
		}

		tie( y, x ) = position_of( t );

		// move target into a row of right below
// 		cerr << "Step 1" << endl;
		while ( ty + 1 < y )
		{
			const int s = y - ty;
			assert( 2 <= s );
			if ( x + s <= global.N )
			{
				rotate( ty + 1, x, s, 'R' );
			}
			else
			{
// 				cerr << "!( x < s )" << endl;
// 				DUMP( y );
// 				DUMP( x );
// 				DUMP( s );
				const int ss = min( s, x + 1 );
				rotate( y - ss + 1, x - ss + 1, ss, 'L' );
			}
			tie( y, x ) = position_of( t );
		}

		// move into right below cell
// 		cerr << "Step 2" << endl;
		while ( tx != x )
		{
			const int s = min( abs( tx - x ) + 1, global.N - y );
// 			DUMP( y );
// 			DUMP( position_of( t ).fst );
// 			DUMP( x );
// 			DUMP( s );
			assert( 2 <= s );
			if ( x < tx )
			{
				rotate( y, x, s, 'R' );
				x += s - 1;
			}
			else
			{
				rotate( y, x - s + 1, s, 'L' );
				x -= s - 1;
			}
		}

		// move into target cell
// 		cerr << "Step 3" << endl;
		if ( x == global.N - 1 )
		{
			rotate( ty, tx - 1, 2, 'L' );
		}
		else
		{
			rotate( ty, tx, 2, 'R' );
		}

		return;
	}

	VS history() const
	{
		return history_;
	}
};

int main()
{
	cin.tie( 0 );
	ios::sync_with_stdio( false );
	cout << setprecision( 12 ) << fixed;


	Board board;
	REP( i, global.N * global.N - 2 * global.N )
	{
// 		DUMP( i );
		board.fix( i );
	}

	if ( global.P == 1 )
	{
		cout << 0 << endl;
		return 0;
	}

	auto res = board.history();
// 	cerr << "ENDED" << endl;
// 	DUMP( SZ( res ) );
	res.resize( min( SZ( res ), 100000 ) );
	cout << SZ( res ) << endl;
// 	DUMP( res[0] );
	copy( ALL( res ), ostream_iterator< string >( cout, "\n" ) );
	cout << flush;

	return 0;
}
