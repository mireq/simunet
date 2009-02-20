/***************************************************************************
 *   Simunet - Computer Network Simulator                                  *
 *   http://simunet.eu/                                                    *
 *                                                                         *
 *   Copyright (C) 2008 by Miroslav Bendik                                 *
 *   miroslav.bendik@gmail.com                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef SNBLOAT_H
#define SNBLOAT_H

#include <cstring>
#include <cstdio>
#include <malloc.h>
#include <string>
#include <vector>
#include <map>

#include <openssl/sha.h>

#define SIMUNETBLOAT_ID "SimuNetBloat"
#define SIMUNETBLOATHEAD_ID "SimuNetBloatHead"
#define SIMUNETBLOAT_MAX_FILENAME_LENGTH 2048
/* Size for straight to buffer file read = 16MiB */
#define SIMUNETBLOAT_MAXFILEBUFFER 16777216
#define SIMUNETBLOAT_FILEBUFFER 524288

struct SNBloatIndexObject{
    std::string virtfilename;
    size_t position;
    size_t size;
    size_t physSize; /* Real physical size */
    unsigned char sum[SHA512_DIGEST_LENGTH];
    std::map<std::string,std::string> vars;
};

struct SNBloatRWControl{
    FILE *s;
    SHA512_CTX ctx;
    bool checkSum;
    bool failed;
    size_t size;
    size_t physSize;
    SNBloatIndexObject bio;
};

/**
 @author Samuel Kupka <skupka@sageteam.eu>
*/
class SNBloat
{
	public:
		SNBloat(const std::string filename);
		~SNBloat();
		
		int openForReading(void);
		
		std::vector<SNBloatIndexObject> getList(void);
		int getBIO(std::string filename,SNBloatIndexObject *o);
		int checkSum(std::string filename);
		
		int readOpen(std::string filename,SNBloatRWControl *o,bool checkSum=false);
        size_t readNext(unsigned char *buffer,size_t size,SNBloatRWControl *o);
        int readClose(SNBloatRWControl *o);
		
		/* WARNING! Theese methods ARE NOT thread safe (for obvious reasons) */
		int openForWriting(bool eraseFirst=false);
		int writeOpen(std::string filename,SNBloatRWControl *o,std::map<std::string,std::string> vars);
        size_t writeNext(unsigned char *buffer,size_t size,SNBloatRWControl *o);
        int writeClose(SNBloatRWControl *o);
		
    private:
        std::string fname;
        std::map <std::string,SNBloatIndexObject> bloatmap;
        uint16_t majorVersion,minorVersion;
        uint32_t numFiles;
        std::string description;
        uint32_t purpose;
        
        int readHeader(FILE *s);
        int writeHeader(FILE *s);
        
        uint8_t read_uint8_t(FILE *s,SHA512_CTX *ctx=NULL);
        uint16_t read_uint16_t(FILE *s,SHA512_CTX *ctx=NULL);
        uint32_t read_uint24_t(FILE *s,SHA512_CTX *ctx=NULL);
        uint32_t read_uint32_t(FILE *s,SHA512_CTX *ctx=NULL);
        uint64_t read_uint64_t(FILE *s,SHA512_CTX *ctx=NULL);
        void read_chars(FILE *s,char *buffer,size_t size,SHA512_CTX *ctx=NULL);
        void read_buffer(FILE *s,void *buffer,size_t size,SHA512_CTX *ctx=NULL);
        
        void write_uint8_t(FILE *s,uint8_t n,SHA512_CTX *ctx=NULL);
        void write_uint16_t(FILE *s,uint16_t n,SHA512_CTX *ctx=NULL);
        void write_uint24_t(FILE *s,uint32_t n,SHA512_CTX *ctx=NULL);
        void write_uint32_t(FILE *s,uint32_t n,SHA512_CTX *ctx=NULL);
        void write_uint64_t(FILE *s,uint64_t n,SHA512_CTX *ctx=NULL);
        void write_chars(FILE *s,char *buffer,SHA512_CTX *ctx=NULL);
        void write_buffer(FILE *s,void *buffer,size_t size,SHA512_CTX *ctx=NULL);
};
#endif
