// -*- c++ -*-

/**
 *
 *                     LightStringGraph
 *
 *            Lightweight String Graph Construction.
 *
 * Copyright (C) 2013, 2014 Stefano Beretta, Yuri Pirola, Marco Previtali
 *
 * Distributed under the terms of the GNU General Public License (or the Lesser
 * GPL).
 *
 * This file is part of LightStringGraph.
 *
 * LighStringGraph is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LightStringGraph is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LightStringGraph.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#ifndef UTIL_LOG_H
#define UTIL_LOG_H

#ifdef DEBUG
#define DEBUG_LOG(s)                                                    \
  do {                                                                  \
    std::cerr << "DBG:"                                                 \
              << __FILE__ << ":"                                        \
              << std::setiosflags (std::ios::left) << std::setw(4)      \
              << __LINE__                                               \
              << "  " << s << std::endl;                                \
  } while (0)
#else
#define DEBUG_LOG(s) do {} while(0)
#endif

#ifdef DEBUG_VERBOSE
#define DEBUG_LOG_VERBOSE(s) DEBUG_LOG(s)
#else
#define DEBUG_LOG_VERBOSE(s) do {} while(0)
#endif



#define QUOTE(str) #str
#define EXPAND_AND_QUOTE(str) QUOTE(str)

#define _MY_FAIL                               \
  do {                                         \
    DEBUG_LOG("Failure.");                     \
    throw std::logic_error("Failure at " +     \
                           std::string(__FILE__) + ":" +              \
                           std::string(EXPAND_AND_QUOTE(__LINE__)) ); \
  } while (0)

#define _FAIL_AND_LOG( _MESSAGE_ )     \
  do {                                 \
    DEBUG_LOG( _MESSAGE_ );            \
    throw std::logic_error(_MESSAGE_); \
  } while (0)

#define _FAIL_IF( _TEST_ )                    \
  do {                                        \
    if (_TEST_) {                             \
      DEBUG_LOG("Condition '" <<              \
                EXPAND_AND_QUOTE(_TEST_) <<   \
                "' verified.");               \
      _MY_FAIL;                               \
    }                                         \
  } while (0)



#ifdef DEBUG  // DEBUG is ON

#define _FAIL_IF_DBG( _TEST_ )                \
  do {                                        \
    if (_TEST_) {                             \
      DEBUG_LOG("Condition '" <<              \
                EXPAND_AND_QUOTE(_TEST_) <<   \
                "' verified.");               \
      _MY_FAIL;                               \
    }                                         \
  } while (0)

#define PERFORM_AND_CHECK( TEST )                    \
  do {                                               \
    bool __test_perform_##__LINE__ = TEST;           \
      if (!__test_perform_##__LINE__) {              \
        DEBUG_LOG("Error while performing '" <<      \
                  EXPAND_AND_QUOTE(TEST) << "'.");   \
        _MY_FAIL;                                    \
      }                                              \
  } while(0)

#else // DEBUG is OFF

#define _FAIL_IF_DBG( _TEST_ )                \
  do {                                        \
  } while (0)

#define PERFORM_AND_CHECK( TEST )  \
  do {                             \
    TEST;                          \
  } while(0)

#endif

#endif
