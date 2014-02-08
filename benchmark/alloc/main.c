/* main.c  -  Memory allocation tests  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform memory allocation library in C11 providing basic support data types and
 * functions to write applications and games in a platform-independent fashion. The latest source code is
 * always available at
 *
 * https://github.com/rampantpixels/memory_lib
 *
 * This library is built on top of the foundation library available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#include <foundation/foundation.h>

#include <memory/memory.h>
#include <memory/log.h>


static void** ptr_malloc[512];
static void** ptr_memory[512];


int main_initialize( void )
{
	int ret = 0;

	application_t app = {0};
	app.name = "Memory allocation benchmark";
	app.short_name = "benchmark_alloc";
	app.config_dir = "benchmark_alloc";
	app.flags = APPLICATION_UTILITY;

	log_enable_prefix( false );
	log_set_suppress( 0, ERRORLEVEL_INFO );
	log_set_suppress( HASH_MEMORY, ERRORLEVEL_INFO );
	log_set_suppress( HASH_BENCHMARK, ERRORLEVEL_DEBUG );

	if( ( ret = foundation_initialize( memory_system_malloc(), app ) ) < 0 )
		return ret;

	config_set_int( HASH_FOUNDATION, HASH_TEMPORARY_MEMORY, 64 * 1024 );

	return 0;
}


int main_run( void* main_arg )
{
	int iloop, ipass;
	tick_t time_start, time_end, time_elapsed;

	memory_system_t sys_malloc = memory_system_malloc();
	memory_system_t sys_memory = memory_system();

	sys_malloc.initialize();
	sys_memory.initialize();

	for( iloop = 0; iloop < 512; ++iloop )
	{
		ptr_malloc[iloop] = sys_malloc.allocate( 0, sizeof( void* ) * 8192, 0, MEMORY_PERSISTENT );
		ptr_memory[iloop] = sys_memory.allocate( 0, sizeof( void* ) * 8192, 0, MEMORY_PERSISTENT );
	}

	//Warmup phase
	for( iloop = 0; iloop < 64; ++iloop )
	{
		for( ipass = 0; ipass < 8192; ++ipass )
		{
			ptr_malloc[0][ipass] = sys_malloc.allocate( 0, ( ipass + iloop ), 0, MEMORY_PERSISTENT );
			ptr_memory[0][ipass] = sys_memory.allocate( 0, ( ipass + iloop ), 0, MEMORY_PERSISTENT );
		}

		for( ipass = 0; ipass < 8192; ++ipass )
		{
			sys_malloc.deallocate( ptr_malloc[0][ipass] );
			sys_memory.deallocate( ptr_memory[0][ipass] );
		}
	}

	log_info( HASH_BENCHMARK, "Single threaded small allocation");
	log_info( HASH_BENCHMARK, "================================");
	time_elapsed = 0;
	time_start = time_current();
	for( iloop = 0; iloop < 512; ++iloop )
	{
		for( ipass = 0; ipass < 8192; ++ipass )
		{
			ptr_malloc[iloop][ipass] = sys_malloc.allocate( 0, ( ipass + iloop ), 0, MEMORY_PERSISTENT );
		}
	}
	time_end = time_current();
	time_elapsed += time_diff( time_start, time_end );
	for( iloop = 0; iloop < 512; ++iloop )
	{
		for( ipass = 0; ipass < 8192; ++ipass )
		{
			sys_malloc.deallocate( ptr_malloc[iloop][ipass] );
		}
	}
	log_infof( HASH_BENCHMARK, "Malloc time: %.4" PRIREAL "s", time_ticks_to_seconds( time_elapsed ) );

	time_elapsed = 0;
	time_start = time_current();
	for( iloop = 0; iloop < 512; ++iloop )
	{
		for( ipass = 0; ipass < 8192; ++ipass )
		{
			ptr_memory[iloop][ipass] = sys_memory.allocate( 0, ( ipass + iloop ), 0, MEMORY_PERSISTENT );
		}
	}
	time_end = time_current();
	time_elapsed += time_diff( time_start, time_end );
	for( iloop = 0; iloop < 512; ++iloop )
	{
		for( ipass = 0; ipass < 8192; ++ipass )
		{
			sys_memory.deallocate( ptr_memory[iloop][ipass] );
		}
	}
	log_infof( HASH_BENCHMARK, "Memory time: %.4" PRIREAL "s", time_ticks_to_seconds( time_elapsed ) );

	log_info( HASH_BENCHMARK, "");
	log_info( HASH_BENCHMARK, "Single threaded reallocation");
	log_info( HASH_BENCHMARK, "============================");

	log_info( HASH_BENCHMARK, "");
	log_info( HASH_BENCHMARK, "Single threaded deallocation");
	log_info( HASH_BENCHMARK, "============================");

	log_info( HASH_BENCHMARK, "");
	log_info( HASH_BENCHMARK, "Single threaded mixed allocation/reallocation/&deallocation");
	log_info( HASH_BENCHMARK, "===========================================================");

	log_info( HASH_BENCHMARK, "");
	log_info( HASH_BENCHMARK, "Multi threaded allocation");
	log_info( HASH_BENCHMARK, "=========================");

	log_info( HASH_BENCHMARK, "");
	log_info( HASH_BENCHMARK, "Multi threaded reallocation");
	log_info( HASH_BENCHMARK, "===========================");

	log_info( HASH_BENCHMARK, "");
	log_info( HASH_BENCHMARK, "Multi threaded deallocation");
	log_info( HASH_BENCHMARK, "===========================");

	log_info( HASH_BENCHMARK, "");
	log_info( HASH_BENCHMARK, "Multi threaded mixed allocation/reallocation/&deallocation");
	log_info( HASH_BENCHMARK, "==========================================================");

	for( iloop = 0; iloop < 512; ++iloop )
	{
		sys_malloc.deallocate( ptr_malloc[iloop] );
		sys_memory.deallocate( ptr_memory[iloop] );
	}

	sys_malloc.shutdown();
	sys_memory.shutdown();

	return 0;
}


void main_shutdown( void )
{
	foundation_shutdown();
}




/*
DECLARE_TEST( alloc, alloc )
{
	unsigned int iloop = 0;
	unsigned int ipass = 0;
	unsigned int icheck = 0;
	unsigned int id = 0;
	void* addr[8142];
	char data[20000];
	unsigned int datasize[7] = { 473, 39, 195, 24, 73, 376, 245 };
	
	memory_system_t memsys = memory_system();
	memsys.initialize();

	for( id = 0; id < 20000; ++id )
		data[id] = id % 139 + id % 17;
	
	for( iloop = 0; iloop < 64; ++iloop )
	{
		for( ipass = 0; ipass < 8142; ++ipass )
		{
			addr[ipass] = memsys.allocate( 0, 500, 0, MEMORY_PERSISTENT );
			EXPECT_NE( addr[ipass], 0 );

			memcpy( addr[ipass], data, 500 );
			
			for( icheck = 0; icheck < ipass; ++icheck )
			{
				EXPECT_NE( addr[icheck], addr[ipass] );
				if( addr[icheck] < addr[ipass] )
					EXPECT_LT( pointer_offset( addr[icheck], 500 ), addr[ipass] ); //LT since we have some bookkeeping overhead in memory manager between blocks
				else if( addr[icheck] > addr[ipass] )
					EXPECT_LT( pointer_offset( addr[ipass], 500 ), addr[icheck] );
			}
		}

		for( ipass = 0; ipass < 8142; ++ipass )
			EXPECT_EQ( memcmp( addr[ipass], data, 500 ), 0 );
		
		for( ipass = 0; ipass < 8142; ++ipass )
			memsys.deallocate( addr[ipass] );
	}
	
	for( iloop = 0; iloop < 64; ++iloop )
	{
		for( ipass = 0; ipass < 1024; ++ipass )
		{
			unsigned int cursize = datasize[ipass%7] + ipass;
	
			addr[ipass] = memsys.allocate( 0, cursize, 0, MEMORY_PERSISTENT );
			EXPECT_NE( addr[ipass], 0 );

			memcpy( addr[ipass], data, cursize );
			
			for( icheck = 0; icheck < ipass; ++icheck )
			{
				EXPECT_NE( addr[icheck], addr[ipass] );
				//if( addr[icheck] < addr[ipass] )
				//	EXPECT_LT( pointer_offset( addr[icheck], cursize ), addr[ipass] ); //LT since we have some bookkeeping overhead in memory manager between blocks
				//else if( addr[icheck] > addr[ipass] )
				//	EXPECT_LT( pointer_offset( addr[ipass], cursize ), addr[icheck] );
			}
		}

		for( ipass = 0; ipass < 1024; ++ipass )
		{
			unsigned int cursize = datasize[ipass%7] + ipass;			
			EXPECT_EQ( memcmp( addr[ipass], data, cursize ), 0 );
		}
		
		for( ipass = 0; ipass < 1024; ++ipass )
			memsys.deallocate( addr[ipass] );
	}
	
	for( iloop = 0; iloop < 128; ++iloop )
	{
		for( ipass = 0; ipass < 1024; ++ipass )
		{
			addr[ipass] = memsys.allocate( 0, 500, 0, MEMORY_PERSISTENT );
			EXPECT_NE( addr[ipass], 0 );

			memcpy( addr[ipass], data, 500 );
			
			for( icheck = 0; icheck < ipass; ++icheck )
			{
				EXPECT_NE( addr[icheck], addr[ipass] );
				if( addr[icheck] < addr[ipass] )
					EXPECT_LT( pointer_offset( addr[icheck], 500 ), addr[ipass] ); //LT since we have some bookkeeping overhead in memory manager between blocks
				else if( addr[icheck] > addr[ipass] )
				EXPECT_LT( pointer_offset( addr[ipass], 500 ), addr[icheck] );
			}
		}

		for( ipass = 0; ipass < 1024; ++ipass )
		{
			EXPECT_EQ( memcmp( addr[ipass], data, 500 ), 0 );
		}
		
		for( ipass = 0; ipass < 1024; ++ipass )
			memsys.deallocate( addr[ipass] );
	}

	memsys.shutdown();
	
	return 0;
}


typedef struct _allocator_thread_arg
{
	memory_system_t     memory_system;
	unsigned int        loops;
	unsigned int        passes; //max 4096
	unsigned int        datasize[32];
	unsigned int        num_datasize; //max 32
} allocator_thread_arg_t;

void* allocator_thread( object_t thread, void* argp )
{
	allocator_thread_arg_t arg = *(allocator_thread_arg_t*)argp;
	memory_system_t memsys = arg.memory_system;
	unsigned int iloop = 0;
	unsigned int ipass = 0;
	unsigned int icheck = 0;
	unsigned int id = 0;
	void* addr[4096];
	char data[8192];
	unsigned int cursize;
	unsigned int iwait = 0;

	for( id = 0; id < 8192; ++id )
		data[id] = (unsigned char)id;

	iwait = random32_range( 0, 10 );
	thread_sleep( iwait );

	for( iloop = 0; iloop < arg.loops; ++iloop )
	{
		for( ipass = 0; ipass < arg.passes; ++ipass )
		{
			cursize = arg.datasize[ ( iloop + ipass + iwait ) % arg.num_datasize ] + ( iloop % 1024 );

			addr[ipass] = memsys.allocate( 0, cursize, 0, MEMORY_PERSISTENT );
			EXPECT_NE( addr[ipass], 0 );

			memcpy( addr[ipass], data, cursize );
			
			for( icheck = 0; icheck < ipass; ++icheck )
			{
				EXPECT_NE( addr[icheck], addr[ipass] );
				if( addr[icheck] < addr[ipass] )
					EXPECT_LT( pointer_offset( addr[icheck], cursize ), addr[ipass] ); //LT since we have some bookkeeping overhead in memory manager between blocks
				else if( addr[icheck] > addr[ipass] )
					EXPECT_LT( pointer_offset( addr[ipass], cursize ), addr[icheck] );
			}
		}

		for( ipass = 0; ipass < arg.passes; ++ipass )
		{
			cursize = arg.datasize[ ( iloop + ipass + iwait ) % arg.num_datasize ] + ( iloop % 1024 );

			EXPECT_EQ( memcmp( addr[ipass], data, cursize ), 0 );
			memsys.deallocate( addr[ipass] );
		}
	}

	return 0;
}


DECLARE_TEST( alloc, threaded )
{
	object_t thread[32];
	void* threadres[32];
	unsigned int i;
	bool running;
	unsigned int num_alloc_threads;
#if BUILD_ENABLE_MEMORY_STATISTICS
	volatile memory_statistics_t stat;
#endif
	allocator_thread_arg_t thread_arg;
	memory_system_t memsys = memory_system();
	memsys.initialize();

	num_alloc_threads = system_hardware_threads() + 1;
	if( num_alloc_threads < 3 )
		num_alloc_threads = 3;

#if BUILD_ENABLE_MEMORY_STATISTICS
	memory_statistics_reset();
	
	stat = memory_statistics();

	log_memory_info( "STATISTICS AFTER INITIALIZE" );
	log_memory_infof( "Raw current size: %llu", stat.allocated_current_raw );
	log_memory_infof( "Current size:     %llu", stat.allocated_current );
	log_memory_info( "" );
	log_memory_infof( "Raw total size:   %llu", stat.allocated_total_raw );
	log_memory_infof( "Total size:       %llu", stat.allocated_total );
	log_memory_info( "" );
	log_memory_infof( "Raw count:        %llu", stat.allocations_current_raw );
	log_memory_infof( "Count:            %llu", stat.allocations_current );
	log_memory_info( "" );
	log_memory_infof( "Raw total count:  %llu", stat.allocations_total_raw );
	log_memory_infof( "Total count:      %llu", stat.allocations_total );
#endif
	
	//Warm-up
	thread_arg.memory_system = memsys;
	thread_arg.loops = 100000;
	thread_arg.passes = 1024;
	thread_arg.datasize[0] = 19;
	thread_arg.datasize[1] = 249;
	thread_arg.datasize[2] = 797;
	thread_arg.datasize[3] = 3;
	thread_arg.datasize[4] = 79;
	thread_arg.datasize[5] = 34;
	thread_arg.datasize[6] = 389;
	thread_arg.num_datasize = 7;

	EXPECT_EQ( allocator_thread( 0, &thread_arg ), 0 );

	for( i = 0; i < 7; ++i )
		thread_arg.datasize[i] = 500;
	EXPECT_EQ( allocator_thread( 0, &thread_arg ), 0 );

	thread_arg.datasize[0] = 19;
	thread_arg.datasize[1] = 249;
	thread_arg.datasize[2] = 797;
	thread_arg.datasize[3] = 3;
	thread_arg.datasize[4] = 79;
	thread_arg.datasize[5] = 34;
	thread_arg.datasize[6] = 389;
	thread_arg.num_datasize = 7;

#if BUILD_ENABLE_MEMORY_STATISTICS
	memory_statistics_reset();
#endif

	for( i = 0; i < num_alloc_threads; ++i )
	{
		thread[i] = thread_create( allocator_thread, "allocator", THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[i], &thread_arg );
	}

	test_wait_for_threads_startup( thread, num_alloc_threads );

	do
	{
		running = false;

		for( i = 0; i < num_alloc_threads; ++i )
		{
			if( thread_is_running( thread[i] ) )
			{
				running = true;
				break;
			}
		}

		thread_yield();
	} while( running ); 
	
	for( i = 0; i < num_alloc_threads; ++i )
	{
		threadres[i] = thread_result( thread[i] );
		thread_destroy( thread[i] );
	}

	test_wait_for_threads_exit( thread, num_alloc_threads );

#if BUILD_ENABLE_MEMORY_STATISTICS
	stat = memory_statistics();

	log_memory_info( "STATISTICS AFTER TEST" );
	log_memory_infof( "Raw current size: %llu", stat.allocated_current_raw );
	log_memory_infof( "Current size:     %llu", stat.allocated_current );
	log_memory_info( "" );
	log_memory_infof( "Raw total size:   %llu", stat.allocated_total_raw );
	log_memory_infof( "Total size:       %llu", stat.allocated_total );
	log_memory_info( "" );
	log_memory_infof( "Raw count:        %llu", stat.allocations_current_raw );
	log_memory_infof( "Count:            %llu", stat.allocations_current );
	log_memory_info( "" );
	log_memory_infof( "Raw total count:  %llu", stat.allocations_total_raw );
	log_memory_infof( "Total count:      %llu", stat.allocations_total );
#if BUILD_ENABLE_MEMORY_STATISTICS > 1
	log_memory_info( "" );
	log_memory_infof( "Calls alloc oversize:           %llu", stat.allocations_calls_oversize );
	log_memory_infof( "Calls alloc heap:               %llu", stat.allocations_calls_heap );
	log_memory_infof( "Calls alloc heap loops:         %llu", stat.allocations_calls_heap_loops );
	log_memory_info( "" );
	for( i = 0; i < 32; ++i )
		log_memory_infof( "Calls alloc heap pool[%u]:  %llu", i, stat.allocations_calls_heap_pool[i] );
	log_memory_info( "" );	
	log_memory_infof( "New descriptor alloc:           %llu", stat.allocations_new_descriptor_superblock );
	log_memory_infof( "New descriptor dealloc:         %llu", stat.allocations_new_descriptor_superblock_deallocations );
	log_memory_info( "" );
	log_memory_infof( "Active block calls:             %llu", stat.allocations_calls_active );
	log_memory_infof( "Active block no active:         %llu", stat.allocations_calls_active_no_active );
	log_memory_infof( "Active block to partial:        %llu", stat.allocations_calls_active_to_partial );
	log_memory_infof( "Active block to active:         %llu", stat.allocations_calls_active_to_active );
	log_memory_infof( "Active block to full:           %llu", stat.allocations_calls_active_to_full );
	log_memory_infof( "Active block credits:           %llu", stat.allocations_calls_active_credits );
	log_memory_info( "" );
	log_memory_infof( "Partial block calls:            %llu", stat.allocations_calls_partial );
	log_memory_infof( "Partial block tries:            %llu", stat.allocations_calls_partial_tries );
	log_memory_infof( "Partial block no descriptor:    %llu", stat.allocations_calls_partial_no_descriptor );
	//log_memory_infof( "Partial block full descriptor:  %llu", stat.allocations_calls_partial_full_descriptor );
	log_memory_infof( "Partial block to retire:        %llu", stat.allocations_calls_partial_to_retire );
	log_memory_infof( "Partial block to active:        %llu", stat.allocations_calls_partial_to_active );
	log_memory_infof( "Partial block to full:          %llu", stat.allocations_calls_partial_to_full );
	log_memory_info( "" );
	log_memory_infof( "New block calls :               %llu", stat.allocations_calls_new_block );
	log_memory_infof( "New block early out:            %llu", stat.allocations_new_block_earlyouts );
	log_memory_infof( "New block alloc new:            %llu", stat.allocations_new_block_superblock );
	log_memory_infof( "New block hit pending:          %llu", stat.allocations_new_block_pending_hits );
	log_memory_infof( "New block new success:          %llu", stat.allocations_new_block_superblock_success );
	log_memory_infof( "New block pending success:      %llu", stat.allocations_new_block_pending_success );
	log_memory_infof( "New block new dealloc:          %llu", stat.allocations_new_block_superblock_deallocations );
	log_memory_infof( "New block pending dealloc:      %llu", stat.allocations_new_block_pending_deallocations );
	log_memory_infof( "New block new stored:           %llu", stat.allocations_new_block_superblock_stores );
	log_memory_infof( "New block pending store:        %llu", stat.allocations_new_block_pending_stores );
#endif
#endif
	
	memsys.shutdown();

#if BUILD_ENABLE_MEMORY_STATISTICS
	stat = memory_statistics();
	
	log_memory_info( "STATISTICS AFTER SHUTDOWN" );
	log_memory_infof( "Raw current size: %llu", stat.allocated_current_raw );
	log_memory_infof( "Current size:     %llu", stat.allocated_current );
	log_memory_info( "" );
	log_memory_infof( "Raw total size:   %llu", stat.allocated_total_raw );
	log_memory_infof( "Total size:       %llu", stat.allocated_total );
	log_memory_info( "" );
	log_memory_infof( "Raw count:        %llu", stat.allocations_current_raw );
	log_memory_infof( "Count:            %llu", stat.allocations_current );
	log_memory_info( "" );
	log_memory_infof( "Raw total count:  %llu", stat.allocations_total_raw );
	log_memory_infof( "Total count:      %llu", stat.allocations_total );
#endif
	
	for( i = 0; i < num_alloc_threads; ++i )
		EXPECT_EQ( threadres[i], 0 );

	return 0;
}


void test_alloc_declare( void )
{
	ADD_TEST( alloc, alloc );
	ADD_TEST( alloc, threaded );
}


test_suite_t test_alloc_suite = {
	test_alloc_application,
	test_alloc_memory_system,
	test_alloc_declare,
	test_alloc_initialize,
	test_alloc_shutdown
};


#if FOUNDATION_PLATFORM_ANDROID

int test_alloc_run( void )
{
	test_suite = test_alloc_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void )
{
	return test_alloc_suite;
}

#endif

*/