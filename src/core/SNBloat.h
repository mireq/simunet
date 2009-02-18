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
#include <string>
#include <vector>
#include <map>

#include <openssl/sha.h>

#define SIMUNETBLOAT_ID "SimuNetBloat"
#define SIMUNETBLOATHEAD_ID "SimuNetBloatHead"
#define SIMUNETBLOAT_MAX_FILENAME_LENGTH 2048
/**
 @author Samuel Kupka <skupka@sageteam.eu>
*/
class SNBloat
{
    struct SNBloatIndexObject{
        std::string virtfilename;
        size_t position;
        size_t size;
        unsigned char sum[SHA512_DIGEST_LENGTH];
        std::map<std::string,std::string> vars;
    };

	public:
		SNBloat(const std::string filename);
		~SNBloat();
		
		bool openForReading(void);
		
    private:
        std::string fname;
        std::map <std::string,SNBloatIndexObject> bloatmap;
        uint16_t majorVersion,minorVersion;
        uint32_t numFiles;
        std::string description;
        uint32_t purpose;
        
        uint8_t read_uint8_t(FILE *s,SHA512_CTX *ctx=NULL);
        uint16_t read_uint16_t(FILE *s,SHA512_CTX *ctx=NULL);
        uint32_t read_uint24_t(FILE *s,SHA512_CTX *ctx=NULL);
        uint32_t read_uint32_t(FILE *s,SHA512_CTX *ctx=NULL);
        void read_chars(FILE *s,char *buffer,size_t size,SHA512_CTX *ctx=NULL);
        void read_buffer(FILE *s,void *buffer,size_t size,SHA512_CTX *ctx=NULL);
        
        void write_uint8_t(FILE *s,uint8_t n,SHA512_CTX *ctx=NULL);
        void write_uint16_t(FILE *s,uint16_t n,SHA512_CTX *ctx=NULL);
        void write_uint24_t(FILE *s,uint32_t n,SHA512_CTX *ctx=NULL);
        void write_uint32_t(FILE *s,uint32_t n,SHA512_CTX *ctx=NULL);
};
#endif
