/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

//
// Created by Longda on 2021/5/3.
//


#include "init.h"
#include "ini_setting.h"

#include "common/conf/ini.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/math/math.h"
#include "common/os/path.h"
#include "common/os/pidfile.h"
#include "common/os/process.h"
#include "common/os/signal.h"
#include "common/seda/init.h"
#include "common/seda/stage_factory.h"

#include "common/metrics/log_reporter.h"
#include "common/metrics/metrics_registry.h"
#include "session/session_stage.h"
#include "sql/executor/execute_stage.h"
#include "sql/optimizer/optimize_stage.h"
#include "sql/parser/parse_stage.h"
#include "sql/parser/resolve_stage.h"
#include "sql/plan_cache/plan_cache_stage.h"
#include "sql/query_cache/query_cache_stage.h"
#include "storage/default/default_storage_stage.h"
#include "storage/mem/mem_storage_stage.h"

using namespace common;

bool *&_get_init() {
  static bool util_init = false;
  static bool *util_init_p = &util_init;
  return util_init_p;
}

bool get_init() { return *_get_init(); }

void set_init(bool value) {
  *_get_init() = value;
  return;
}

void sig_handler(int sig) {
  // TODO Signal handler will be add in the next step.
  //  Add action to shutdown

  LOG_INFO("Receive one signal of %d.", sig);

  return;
}

int init_log(ProcessParam *process_cfg, Ini &properties) {
  const std::string &proc_name = process_cfg->get_process_name();
  try {
    // we had better alloc one lock to do so, but simplify the logic
    if (g_log) {
      return 0;
    }

    const std::string log_section_name = "LOG";
    std::map<std::string, std::string> log_section =
        properties.get(log_section_name);

    std::string log_file_name;

    // get log file name
    std::string key = "LOG_FILE_NAME";
    std::map<std::string, std::string>::iterator it = log_section.find(key);
    if (it == log_section.end()) {
      log_file_name = proc_name + ".log";
      std::cout << "Not set log file name, use default " << log_file_name
                << std::endl;
    } else {
      log_file_name = it->second;
    }

    log_file_name = getAboslutPath(log_file_name.c_str());

    LOG_LEVEL log_level = LOG_LEVEL_INFO;
    key = ("LOG_FILE_LEVEL");
    it = log_section.find(key);
    if (it != log_section.end()) {
      int log = (int)log_level;
      str_to_val(it->second, log);
      log_level = (LOG_LEVEL)log;
    }

    LOG_LEVEL console_level = LOG_LEVEL_INFO;
    key = ("LOG_CONSOLE_LEVEL");
    it = log_section.find(key);
    if (it != log_section.end()) {
      int log = (int)console_level;
      str_to_val(it->second, log);
      console_level = (LOG_LEVEL)log;
    }

    LoggerFactory::init_default(log_file_name, log_level, console_level);

    key = ("DefaultLogModules");
    it = log_section.find(key);
    if (it != log_section.end()) {
      g_log->set_default_module(it->second);
    }

    if (process_cfg->is_demon()) {
      sys_log_redirect(log_file_name.c_str(), log_file_name.c_str());
    }

    return 0;
  } catch (std::exception &e) {
    std::cerr << "Failed to init log for " << proc_name << SYS_OUTPUT_FILE_POS
              << SYS_OUTPUT_ERROR << std::endl;
    return errno;
  }

  return 0;
}

void cleanup_log() {

  if (g_log) {
    delete g_log;
    g_log = nullptr;
  }
  return;
}

int prepare_init_seda() {
  static StageFactory session_stage_factory("SessionStage",
                                          &SessionStage::make_stage);
  static StageFactory resolve_stage_factory("ResolveStage",
                                          &ResolveStage::make_stage);
  static StageFactory query_cache_stage_factory("QueryCacheStage",
                                             &QueryCacheStage::make_stage);
  static StageFactory parse_stage_factory("ParseStage", &ParseStage::make_stage);
  static StageFactory plan_cache_factory("PlanCacheStage",
                                       &PlanCacheStage::make_stage);
  static StageFactory optimize_factory("OptimizeStage",
                                      &OptimizeStage::make_stage);
  static StageFactory execute_factory("ExecuteStage", &ExecuteStage::make_stage);
  static StageFactory default_storage_factory("DefaultStorageStage",
                                            &DefaultStorageStage::make_stage);
  static StageFactory mem_storage_factory("MemStorageStage",
                                        &MemStorageStage::make_stage);
  return 0;
}

int init(ProcessParam *process_param) {

  if (get_init()) {

    return 0;
  }

  set_init(true);

  // Run as daemon if daemonization requested
  int rc = STATUS_SUCCESS;
  if (process_param->is_demon()) {
    rc = daemonize_service(process_param->get_std_out().c_str(),
                          process_param->get_std_err().c_str());
    if (rc != 0) {
      std::cerr << "Shutdown due to failed to daemon current process!"
                << std::endl;
      return rc;
    }
  }

  writePidFile(process_param->get_process_name().c_str());

  // Initialize global variables before enter multi-thread mode
  // to avoid race condition
  theSwVersion();

  // Read Configuration files
  rc = get_g_properties()->load(process_param->get_conf());
  if (rc) {
    std::cerr << "Failed to load configuration files" << std::endl;
    return rc;
  }

  // Init tracer
  rc = init_log(process_param, *get_g_properties());
  if (rc) {
    std::cerr << "Failed to init Log" << std::endl;
    return rc;
  }

  std::string conf_data;
  get_g_properties()->to_string(conf_data);
  LOG_INFO("Output configuration \n%s", conf_data.c_str());

  // seda is used for backend async event handler
  // the latency of seda is slow, it isn't used for critical latency
  // environment.
  prepare_init_seda();
  rc = initSeda(process_param);
  if (rc) {
    LOG_ERROR("Failed to init seda configuration!");
    return rc;
  }

  LogReporter *log_reporter = get_g_log_reporter();
  MetricsRegistry &metricsRegistry = get_g_metrics_registry();

  metricsRegistry.add_reporter(log_reporter);

  // Block interrupt signals before creating child threads.
  // setSignalHandler(sig_handler);
  // sigset_t newSigset, oset;
  // blockDefaultSignals(&newSigset, &oset);
  //  wait interrupt signals
  // startWaitForSignals(&newSigset);

  seedRandom();

  LOG_INFO("Successfully init utility");

  return STATUS_SUCCESS;
}

void cleanup_util() {

  if (nullptr != get_g_properties()) {
    delete get_g_properties();
    get_g_properties() = nullptr;
  }

  LOG_INFO("Shutdown Cleanly!");

  // Finalize tracer
  cleanup_log();

  set_init(false);
  return;
}

void cleanup() {}
