/*
 * Copyright 2008 Search Solution Corporation
 * Copyright 2016 CUBRID Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

/*
 * memory_monitor.hpp - cubrid memory monitoring module
 */

#ifndef _MEMORY_MONITOR_HPP_
#define _MEMORY_MONITOR_HPP_


#include "perf_def.hpp"
#include "thread_compat.hpp"

//#ifdef __cplusplus
#include <string>
#include <type_traits>
#include <cstdint>
#include <atomic>
#include <vector>

#include <cassert>

#define MMM_MAX_COMP_NUM 32

typedef enum
{
	MEMMON_INFO_DUPERR = -1,
	MEMMON_INFO_NOMATCHING_TYPE,
	MEMMON_INFO_DEFAULT,
	MEMMON_INFO_SHOWALL,
	MEMMON_INFO_MODULE,
	MEMMON_INFO_TRANSACTION,
	MEMMON_INFO_MODTRANS,
	MEMMON_INFO_HELP
} MEMMON_INFO_TYPE;
#ifdef SERVER_MODE
namespace cubperf
{
	class MMM;
	class MMM_printer;
	
	typedef enum mmm_stat_name
	{
		HEAP = 0,
		STAT_END
	}MMM_STAT_NAME;

	typedef enum mmm_module_name
	{
		HEAP_MODULE = 1,
		LOCK_MODULE
	}MMM_MODULE_NAME;

	typedef struct mmm_stat_info
	{
		MMM_STAT_NAME stat;
		MMM_MODULE_NAME module;
		int component_index;
		int subcomp_index;			// if -1, no subcomp
	}MMM_STAT_INFO;

	typedef struct mmm_base_stat
	{
		std::atomic<uint64_t> init_stat;
		std::atomic<uint64_t> cur_stat;
		std::atomic<uint64_t> peak_stat;
		std::atomic<uint32_t> expands;
	}MMM_BASE_STAT;

	class template_stats
	{
	public:
		template_stats(char *module_name)
		{
			this->module_name = new char[strlen(module_name) + 1];
			strcpy(this->module_name, module_name);
		}
		template_stats() {}
		char *module_name;
		MMM_BASE_STAT base_stat[MMM_MAX_COMP_NUM];
		virtual void print_stats(char *buffer);
		virtual void subcomp_fetch_add(int index, int size);
		virtual void subcomp_fetch_sub(int index, int size);

		~template_stats() {}
	};

	class sub_component
	{
	public:
		sub_component(char *name, int index)
		{
			this->subcomp_name = new char[strlen(name) + 1];
			strcpy(this->subcomp_name, name);
			this->comp_index = index;
		}
		char *subcomp_name;
		int comp_index;
		std::atomic<uint64_t> cur_stat;
		~sub_component() { delete subcomp_name; }
	};

	// you have to inheritance "template_stats" class to your module
	class heap_stats : public template_stats
	{
	public:
		heap_stats(char *module_name) : template_stats(module_name) {}
		static const uint8_t TOTAL_COMP = 3;
		static const uint8_t TOTAL_SUBCOMP = 2;
		// you can make sub_comp array for some components' subcomponent
		sub_component *subcomp[TOTAL_SUBCOMP] = {
			new sub_component("free list", 1),
			new sub_component("hash table", 2)
		};

		char *comp_list[TOTAL_COMP + 1] = {
			"",
			"Classrepr Cache",
			"Scan Cache",
			"Bestspace Cache"	
		};

		virtual void print_stats(char *buffer);

		inline void subcomp_fetch_add(int index, int size)
		{
			subcomp[index]->cur_stat.fetch_add(size);
		}

		inline void subcomp_fetch_sub(int index, int size)
		{
			subcomp[index]->cur_stat.fetch_sub(size);
		}

		~heap_stats() { delete module_name; }
	};

	class MMM_printer
	{
	public:
		MMM_printer(MMM *mmm) { this->mmm = mmm; }
		MMM *mmm;						// link of mmm
		int print_base(char *buffer);
		int print_default(char *buffer);
		int print_transaction(char *buffer, int display_size);
		void print_help(char *buffer);
		void print_error(int err_code);

		static int comp(const void *a, const void *b)
		{
			uint64_t x1 = ((std::pair<int, uint64_t> *)a)->second;
			uint64_t x2 = ((std::pair<int, uint64_t> *)b)->second;

			if (x1 < x2)
				return 1;
			else if (x1 > x2)
				return -1;

			return 0;
		}

		~MMM_printer() {}
	};

	// you have to add your modules in Memory Monitoring Manager(MMM) class
	class MMM
	{
	public:
		static const uint8_t TYPE_MODULE = 0;
		//static const uint8_t TYPE_SUBCOMP = 1;
		
		// When you add your module for monitoring,
		// you must increase this declared value
		static const uint8_t NUM_MODULES = 1;

		char *server_name;
		std::atomic<uint64_t> total_mem_usage;

		// TODO: 모듈 이름과 이에 대응하는 index 매칭하는 구조체 필요
		// TODO: 모듈 이름에 맞는 인덱스 찾아주는 함수 필요(for loop 돌리더라도)

		// Your monitring stats should add in this array with appropriate format
		MMM_STAT_INFO stats_info[STAT_END] = {
			{HEAP, HEAP_MODULE, 0, -1}
		};

		// Your module class should add in this array with print function
		template_stats *modules[NUM_MODULES + 1] = {
			new template_stats(),				// dummy
			new heap_stats("heap")
		};

		MMM_printer *printer;
		
		MMM(const char *server_name)
		{
			this->server_name = new char[strlen(server_name) + 1];
			strcpy(this->server_name, server_name);
			printer = new MMM_printer(this);
		}

		int memmon_add_stat(uint64_t size, MMM_STAT_NAME stat_name, THREAD_ENTRY *thread_p);
		int memmon_sub_stat(uint64_t size, MMM_STAT_NAME stat_name, THREAD_ENTRY *thread_p);
		int memmon_resize_stat(uint64_t before_size, uint64_t after_size,
							MMM_STAT_NAME stat_name, THREAD_ENTRY *thread_p);
		int memmon_move_stat(uint64_t size, MMM_STAT_NAME before_stat_name,
							MMM_STAT_NAME after_stat_name, THREAD_ENTRY *thread_p);
  	char * print_meminfo(MEMMON_INFO_TYPE type, int module_index, char *module_name, int display_size);
		int get_module_index(char *name)
		{
			for (int i = 1; i <= NUM_MODULES; i++)
			{
				if (!strcmp(modules[i]->module_name, name))
					return i;
			}
			return 0;		// error case
		}

		~MMM()
		{
			delete server_name;
			delete printer;
			for (int i = 0; i < NUM_MODULES; i++)
			{
				delete modules[i];
			}
		}
	};
	extern MMM *MMM_global;
	extern std::atomic<uint64_t> test_meminfo;

/*} //namespace cubperf
#else

	void print_module_stats(template_stats *module, char *buffer);
*/
} // namespace cubperf

//#endif // __cplusplus
#endif
#endif // _MEMORY_MONITOR_HPP_

