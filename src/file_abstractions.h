/**
 *
 *                     LightStringGraph
 *
 *            Lightweight String Graph Construction.
 *
 * Copyright (C) 2015 Yuri Pirola
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

#ifndef FILE_ABSTRACTIONS_H
#define FILE_ABSTRACTIONS_H

#include <string>
#include <cstdlib>
#include <algorithm>
#include <type_traits>

#include <zlib.h>

// We need zlib >= 1.2.5 for transparent writing
#if ZLIB_VERNUM < 0x1250
#error zlib >= 1.2.5 is required
#endif

#define USE_GZBUFFER
#define GZBUFFER_SIZE (128*1024)

// Never write a compressed file ("transparent" writing)
#define GZFILE_WRITE_MODE "wTb"

class gzip_file_t {
private:
  gzFile file;
public:

  static gzFile
  gzfile_open(const std::string& filename,
              const bool write_mode,
              const bool fail_if_error= true) {
    gzFile file= gzopen(filename.c_str(), (write_mode ? GZFILE_WRITE_MODE : "rb"));
    const bool result= (file != Z_NULL);
    if (fail_if_error && !result) {
      _FAIL_AND_LOG("Impossible to open file '" + filename +
                    "' for " + (write_mode ? "writing" : "reading"));
    }
#ifdef USE_GZBUFFER
    if (!write_mode && result) gzbuffer(file, GZBUFFER_SIZE);
#endif
    return file;
  }

  gzip_file_t() : file(Z_NULL) {}
  ~gzip_file_t() { close(); }

  bool open(const std::string& filename) {
    bool result= true;
    if (file != Z_NULL) result= close();
    if (result) {
      file= gzfile_open(filename, false, false);
      result= (file != Z_NULL);
    }
    return result;
  }

  bool close() {
    bool result= true;
    if (file != Z_NULL) {
      /*
        gzclose will return Z_STREAM_ERROR if file is not valid, Z_ERRNO on a
        file operation error, Z_MEM_ERROR if out of memory, Z_BUF_ERROR if the
        last read ended in the middle of a gzip stream, or Z_OK on success. */
      const int intresult= gzclose(file);
      result= (intresult!=Z_ERRNO) & (intresult!=Z_MEM_ERROR);
      if (result) file = Z_NULL;
    }
    return result;
  }

  template <typename TFileValue>
  bool read(TFileValue& value) {
    const int byteread= gzread(file, (void*)&value, sizeof(TFileValue) );
    return (byteread == sizeof(TFileValue));
  }

  template <typename TFileValue>
  size_t read(TFileValue* value, const size_t len) {
    const int byteread= gzread(file, (void*)value, sizeof(TFileValue)*len );
    return byteread/sizeof(TFileValue);
  }
};


#ifndef BUFFERED_FILE_BUFFERSIZE
#define BUFFERED_FILE_BUFFERSIZE 8192
#endif

template <class TFile, size_t THIS_BUFFERSIZE=BUFFERED_FILE_BUFFERSIZE>
class buffered_file_t {
private:
  TFile file;
  char* buffer;
  size_t bpos;
  size_t blen;

  void fill_buffer() {
    bpos= blen= 0;
    blen= file.read(buffer, THIS_BUFFERSIZE);
  }

  void fill_buffer_if_empty() {
    if ((blen == 0) | (bpos == blen))
      fill_buffer();
  }

public:
  buffered_file_t() : file(), buffer(new char[THIS_BUFFERSIZE]) {}
  ~buffered_file_t() { close(); delete [] buffer;}

  bool open(const std::string& filename) {
    const bool result= file.open(filename);
    fill_buffer();
    return result;
  }

  bool close() {
    const bool result= file.close();
    bpos= blen= 0;
    return result;
  }

  template <typename TFileValue>
  bool read(TFileValue& value, typename std::enable_if< !std::is_same<char,TFileValue>::value>::type* dummy=0) {
    size_t byte_copied= 0;
    while (byte_copied < sizeof(TFileValue)) {
      fill_buffer_if_empty();
      const size_t num= std::min(blen-bpos, sizeof(TFileValue)-byte_copied);
      if (num == 0) {
        return false;
      }
      memcpy((&value)+byte_copied, buffer+bpos, num);
      byte_copied += num;
      bpos += num;
    }
    return (byte_copied == sizeof(TFileValue));
  }

  template <typename TFileValue>
  bool read(TFileValue& value, typename std::enable_if< std::is_same<char,TFileValue>::value>::type* dummy=0) {
    fill_buffer_if_empty();
    if (bpos >= blen)
      return false;
    value= *(buffer+bpos);
    ++bpos;
    return true;
  }

};



typedef buffered_file_t<gzip_file_t> auto_file_t;


#endif
