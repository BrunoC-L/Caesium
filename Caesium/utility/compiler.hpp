#pragma once

#ifdef __GNUC__
	#ifdef __clang__
		#define clang
	#else
		#define gcc
	#endif
#else
#define msvc
#endif
