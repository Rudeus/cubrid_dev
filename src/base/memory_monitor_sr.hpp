/*
 *
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
 * memory_monitor_sr.hpp - memory monitoring module header
 */

#ifndef _MEMORY_MONITOR_SR_HPP_
#define _MEMORY_MONITOR_SR_HPP_

#if !defined (SERVER_MODE)
#error Belongs to server module
#endif /* SERVER_MODE */

#include "perf_def.hpp"
#include "memory_monitor_common.h"

#include <cstring>
#include <type_traits>
#include <cstdint>
#include <atomic>
#include <vector>

#include <cassert>

#define MMM_PARSE_MASK 0x0000FFFF
#define MMM_MAKE_MODULE_INIT_STAT_ID(module_idx) ((module_idx) << 16)
#define MMM_MODULE_IDX_FROM_STAT_ID(stat) ((stat) >> 16)
#define MMM_COMP_INFO_IDX_FROM_STAT_ID((stat) & MMM_PARSE_MASK)

typedef enum
{
  HEAP = MMM_MAKE_MODULE_INIT_STAT_ID (MMM_HEAP_MODULE),
  HEAP_CLASSREPR,
  MMM_STAT_END = MMM_MAKE_MODULE_INIT_STAT_ID (MMM_MODULE_END)
} MMM_STAT_ID;

int memmon_add_stat (THREAD_ENTRY *thread_p, MMM_STATS stat, uint64_t size);
int memmon_sub_stat (THREAD_ENTRY *thread_p, MMM_STATS stat, uint64_t size);
int memmon_move_stat (THREAD_ENTRY *thread_p, MMM_STATS src, MMM_STATS dest, uint64_t size);
int memmon_resize_stat (THREAD_ENTRY *thread_p, MMM_STATS stat, uint64_t old_size, uint64_t new_size);

namespace cubperf
{
  class memory_monitoring_manager;
  class mmm_aggregater;

  struct mmm_comp_info
  {
    MMM_STAT_ID idx;
    string comp_name;
    string subcomp_name;
  }

  typedef struct mmm_mem_stat
  {
    std::atomic<uint64_t> init_stat;
    std::atomic<uint64_t> cur_stat;
    std::atomic<uint64_t> peak_stat;
    std::atomic<uint32_t> expand_count;
  } MMM_MEM_STAT;

  MMM_comp_info heap_comp_info[] =
  {
    {HEAP, "", ""}
  };

  class mmm_subcomponent
  {
    public:
      mmm_subcomponent (const char *name)
      {
	m_subcomp_name = new char[strlen (name) + 1];
	strcpy (m_subcomp_name, name);
      }

      /* variable */
      char *m_subcomp_name;
      atomic<uint64_t> m_cur_stat;

      ~mmm_subcomponent()
      {
	delete[] m_subcomp_name;
      }
  };

  class mmm_component
  {
    public:
      mmm_component (const char *name)
      {
	m_comp_name = new char[strlen (name) + 1];
	strcpy (m_comp_name, name);
      }

      /* variable */
      char *m_comp_name;
      MMM_MEM_STAT m_stat;
      std::vector<mmm_subcomponent> m_subcomponent;

      /* function */
      int add_subcomponent (const char *name);

      ~mmm_component()
      {
	delete[] m_comp_name;
      }
  };

  class mmm_module
  {
    public:
      mmm_module (const char *name, mmm_comp_info *info);
      mmm_module() {} /* for dummy */

      /* const */
      /* max index of component or subcomponent is 0x0000FFFF
       * if some stats have max index of component or subcomponent,
       * we don't have to increase it */
      static constexpr int m_max_idx = 0x0000FFFF;

      /* variable */
      char *m_module_name;
      MMM_MEM_STAT m_stat;
      vector<mmm_component> m_component;
      vector<int> m_comp_idx_map;

      /* function */
      virtual int aggregate_stats (MEMMON_MODULE_INFO *info);
      int add_component (char *name);
      static inline int MMM_MAKE_STAT_IDX_MAP (int comp_idx, int subcomp_idx)
      {
	return (comp_idx << 16 | subcomp_idx);
      }

      ~mmm_module()
      {
	if (m_module_name)
	  {
	    delete[] m_module_name;
	  }
      }
  };

  // you have to inheritance "MMM_module" class to your module
  class mmm_heap_module : public mmm_module
  {
    public:
      mmm_heap_module (const char *name, mmm_comp_info *info) : mmm_module (name, info) {}

      /* function */
      virtual int aggregate_stats (MEMMON_MODULE_INFO *info);

      ~mmm_heap_module()
      {
	if (m_module_name)
	  {
	    delete[] m_module_name;
	  }
      }
  };

  class mmm_aggregater
  {
    public:
      mmm_aggregater (memory_monitoring_manager *mmm)
      {
	this->m_memmon_mgr = mmm;
      }

      /*  backlink of memory_monitoring_manager class */
      memory_monitoring_manager *m_memmon_mgr;

      /* function */
      int get_server_info (MEMMON_SERVER_INFO *info);
      int get_module_info (MEMMON_MODULE_INFO *info);
      int get_transaction_info (MEMMON_TRAN_INFO *info);

      ~mmm_printer() {}
  };

  /* you have to add your modules in Memory Monitoring Manager(MMM) class */
  class memory_monitoring_manager
  {
    public:
      memory_monitoring_manager (const char *name)
      {
	this->m_server_name = new char[strlen (name) + 1];
	strcpy (this->m_server_name, name);
	aggregater = new MMM_aggregater (this);
      }
      /* variable */
      char *m_server_name;
      std::atomic<uint64_t> m_total_mem_usage;
      mmm_aggregater *m_aggregater;

      /* Your module class should add in this array with print function */
      mmm_module *m_module[MMM_MODULE_END] =
      {
	new mmm_module(),				  /* dummy */
	new mmm_heap_module ("heap", heap_comp_info)
      };

      /* function */
      int add_stat (THREAD_ENTRY *thread_p, MMM_STATS stat_name, uint64_t size);
      int sub_stat (THREAD_ENTRY *thread_p, MMM_STATS stat_name, uint64_t size);
      int resize_stat (THREAD_ENTRY *thread_p, MMM_STATS stat_name, uint64_t before_size, uint64_t after_size);
      int move_stat (THREAD_ENTRY *thread_p, MMM_STATS before_stat_name, MMM_STATS after_stat_name, uint64_t size);
      int aggregate_module_info (MEMMON_MODULE_INFO *info, int module_index);
      int aggregate_tran_info (MEMMON_TRAN_INFO *info, int tran_count);
      int get_module_index (char *name)
      {
	for (int i = 1; i <= MMM_MODULE_END; i++)
	  {
	    if (!strcmp (modules[i]->module_name, name))
	      {
		return i;
	      }
	  }
	return 0;		// error case
      }

      static inline int MMM_COMP_IDX_FROM_COMP_IDX_MAP (int comp_idx_map_val)
      {
	return comp_info_map_val >> 16;
      }

      static inline int MMM_SUBCOMP_IDX_FROM_COMP_IDX_MAP (int comp_idx_map_val)
      {
	return comp_info_map_val & MMM_PARSE_MASK;
      }

      ~memory_monitoring_manager()
      {
	delete[] m_server_name;
	delete m_aggregater;
	for (int i = 0; i < MMM_MODULE_END; i++)
	  {
	    delete m_module[i];
	  }
      }
  };
  extern memory_monitoring_manager *MMM_global;
#if defined(NDEBUG)
  extern std::atomic<uint64_t> test_meminfo;
#endif
} // namespace cubperf

#endif // _MEMORY_MONITOR_SR_HPP_

