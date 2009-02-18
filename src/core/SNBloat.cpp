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
#include "SNBloat.h"

/* Struktura SNBloat suboru:
+----------+----------+----------------------------------------------------------+
| Pos      | Length   | Description                                              |
+----------+----------+----------------------------------------------------------+
|        0 |       12 | ID "SimuNetBloat"                                        |
|       12 |        2 | Major version                                            |
|       14 |        2 | Minor version                                            |
|       16 |        4 | Purposes of this file                                    |
|       20 |        4 | Position of header from beginning (SEEK_SET)             |
|       24 | variable | DATA                                                     |
|          |      ... |                                                          |
| variable |       16 | ID "SimuNetBloatHead"                                    |
| variable | variable | File description, max length 65536                       |
| variable |        4 | Number of files stored in this bloat                     |
| variable | variable | Multiple SNBloatIndexObjects                             |
| variable |       64 | SHA512 of the whole file except DATA section             |
+----------+----------+----------------------------------------------------------+

Single SNBloatIndexObject
+----------+----------+----------------------------------------------------------+
| Pos      | Length   | Description                                              |
+----------+----------+----------------------------------------------------------+
|        0 |        4 | Length of the filename, max 2048 chars                   |
|        4 | variable | Filename, must begin with '/' and use slashes only       |
| variable |        4 | File size                                                |
| variable |        4 | File position in bloat from beginning (SEEK_SET)         |
| variable |       64 | SHA512 of the file                                       |
| variable |        2 | Number of variables                                      |
| variable |        2 | Variable name length (max 32 bytes)                      |
| variable | variable | Variable name                                            |
| variable |        2 | Variable data length (max 65536 bytes)                   |
| variable | variable | Variable data                                            |
|          |      ... |                                                          |
+----------+----------+----------------------------------------------------------+
*/

SNBloat::SNBloat(const std::string filename)
{
    fname=filename;
    bloatmap.clear();
    s=NULL;
    lastRead=0;
    calcSum=false;
}

SNBloat::~SNBloat()
{
    if(s!=NULL){
        fclose(s);
    }
}

bool SNBloat::openForReading(void)
{
    try{
        s=fopen(fname.c_str(),"rb");
        if(s==NULL) throw;
        
        char buffer[0x10000];
        char buffer2[0x10000];
        uint16_t l=read_uint16_t();
        
        SHA512_Init(&ctx);
        calcSum=true;
        
        /* ID */
        read_chars(buffer,strlen(SIMUNETBLOAT_ID));
        if(strcmp(buffer,SIMUNETBLOAT_ID)) throw;
        
        /* Version */
        majorVersion=read_uint16_t();
        minorVersion=read_uint16_t();
        
        /* Bitmap containing purpose of this file */
        purpose=read_uint32_t();
        
        /* Get and move to header position */
        uint32_t pos=read_uint32_t();
        if(fseek(s,pos,SEEK_SET)!=0) throw;
        
        /* HeadID */
        read_chars(buffer,strlen(SIMUNETBLOATHEAD_ID));
        if(strcmp(buffer,SIMUNETBLOATHEAD_ID)) throw;
        
        /* Read description */
        l=read_uint16_t();
        read_chars(buffer,l);
        description=buffer;
        
        /* Multiple SNBloatIndexObjects */
        numFiles=read_uint32_t();
        for(uint32_t i=0;i<numFiles;i++){
            l=read_uint16_t();
            if(l>SIMUNETBLOAT_MAX_FILENAME_LENGTH) throw;
            read_chars(buffer,l);
            
            SNBloatIndexObject o;
            o.virtfilename=buffer;
            o.size=read_uint32_t();
            o.position=read_uint32_t();
            
            read_buffer(o.sum,SHA512_DIGEST_LENGTH);
            
            /* Load variables for this bloatobject */
            uint16_t x=read_uint16_t();
            for(uint16_t j=0;j<x;j++){
                l=read_uint16_t();
                read_chars(buffer,l);
                l=read_uint16_t();
                read_chars(buffer2,l);
                o.vars[buffer]=buffer2;
            }
            
            bloatmap[o.virtfilename]=o;
        }
        
        calcSum=false;
        SHA512_Final((unsigned char *)buffer,&ctx);
        read_buffer(buffer2,SHA512_DIGEST_LENGTH);
        if(memcmp(buffer,buffer2,SHA512_DIGEST_LENGTH)) throw;
        
        lastRead=time(NULL);
    }
    catch(...){
        if(s!=NULL){
            fclose(s);
        }
        return false;
    }
    return true;
}

uint8_t SNBloat::read_uint8_t(void)
{
    if(s==NULL) throw;
    uint8_t x;
    if(fread(&x,sizeof(uint8_t),1,s)!=1) throw;
    if(calcSum) SHA512_Update(&ctx,&x,1);
    return x;
}

uint16_t SNBloat::read_uint16_t(void)
{
    if(s==NULL) throw;
    uint8_t x[2];
    if(fread(x,sizeof(uint8_t),2,s)!=2) throw;
    if(calcSum) SHA512_Update(&ctx,x,2);
    return (x[1]<<8)|x[0];
}

uint32_t SNBloat::read_uint24_t(void)
{
    if(s==NULL) throw;
    uint32_t x[3];
    if(fread(x,sizeof(uint8_t),3,s)!=3) throw;
    if(calcSum) SHA512_Update(&ctx,x,3);
    return (x[2]<<16)|(x[1]<<8)|x[0];
}

uint32_t SNBloat::read_uint32_t(void)
{
    if(s==NULL) throw;
    uint32_t x[4];
    if(fread(x,sizeof(uint8_t),4,s)!=4) throw;
    if(calcSum) SHA512_Update(&ctx,x,4);
    return (x[3]<<24)|(x[2]<<16)|(x[1]<<8)|x[0];
}

void SNBloat::read_chars(char *buffer, size_t size)
{
    if(fread(buffer,1,size,s)!=size) throw;
    if(calcSum) SHA512_Update(&ctx,buffer,size);
    buffer[size]='\0';
}

void SNBloat::read_buffer(void *buffer, size_t size)
{
    if(fread(buffer,1,size,s)!=size) throw;
    if(calcSum) SHA512_Update(&ctx,buffer,size);
}

void SNBloat::write_uint8_t(uint8_t n)
{
    if(s==NULL) throw;
    if(fwrite(&n,sizeof(uint8_t),1,s)!=1) throw;
    if(calcSum) SHA512_Update(&ctx,&n,1);
}

void SNBloat::write_uint16_t(uint16_t n)
{
    if(s==NULL) throw;
    uint8_t x[2];
    x[0]=n&0xFF;n>>=8;
    x[1]=n&0xFF;
    if(fwrite(x,sizeof(uint8_t),2,s)!=2) throw;
    if(calcSum) SHA512_Update(&ctx,x,2);
}

void SNBloat::write_uint24_t(uint32_t n)
{
    if(s==NULL) throw;
    uint8_t x[3];
    x[0]=n&0xFF;n>>=8;
    x[1]=n&0xFF;n>>=8;
    x[2]=n&0xFF;
    if(fwrite(x,sizeof(uint8_t),3,s)!=3) throw;
    if(calcSum) SHA512_Update(&ctx,x,3);
}

void SNBloat::write_uint32_t(uint32_t n)
{
    if(s==NULL) throw;
    uint8_t x[4];
    x[0]=n&0xFF;n>>=8;
    x[1]=n&0xFF;n>>=8;
    x[2]=n&0xFF;n>>=8;
    x[3]=n&0xFF;
    if(fwrite(x,sizeof(uint8_t),4,s)!=4) throw;
    if(calcSum) SHA512_Update(&ctx,x,4);
}
