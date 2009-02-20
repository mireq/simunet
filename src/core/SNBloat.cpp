/***************************************************************************
 *   Simunet - Computer Network Simulator                                  *
 *   http://simunet.eu/                                                    *
 *                                                                         *
 *   Copyright (C) 2009 by Samuel Kupka                                    *
 *   skupka@sageteam.eu                                                    *
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
#include "SNLog.h"

/* Structure of SNBloat file:
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
|        0 |        2 | Length of the filename, max 2048 chars                   |
|        2 | variable | Filename, must begin with '/' and use slashes only       |
| variable |        4 | File size                                                |
| variable |        4 | Real file size on the disk (after compression)           |
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
    purpose=SNBLOAT_PURPOSE_GENERAL;
}

SNBloat::~SNBloat()
{
}

void SNBloat::setVar(std::string var, std::string value)
{
    globalVars[var]=value;
}

void SNBloat::unsetVar(std::string var)
{
    globalVars.erase(var);
}

int SNBloat::readHeader(FILE *s)
{
    try{
        char buffer[0x10000];
        char buffer2[0x10000];
        uint16_t l;
        
        SHA512_CTX ctx;
        SHA512_Init(&ctx);
        if(fseek(s,0,SEEK_SET)!=0) throw 2;
        
        /* ID */
        read_chars(s,buffer,strlen(SIMUNETBLOAT_ID),&ctx);
        if(strcmp(buffer,SIMUNETBLOAT_ID)) throw 1;
        SNLOG("%s",buffer);
        
        /* Version */
        majorVersion=read_uint16_t(s,&ctx);
        minorVersion=read_uint16_t(s,&ctx);
        SNLOG("%d.%d",majorVersion,minorVersion);
        
        /* Bitmap containing purpose of this file */
        purpose=read_uint32_t(s,&ctx);
        SNLOG("purpose: %u",purpose);
        
        /* Get and move to header position */
        size_t pos=read_uint32_t(s,&ctx);
        SNLOG("pos: %u",pos);
        if(fseek(s,pos,SEEK_SET)!=0) throw 2;
        
        
        /* HeadID */
        read_chars(s,buffer,strlen(SIMUNETBLOATHEAD_ID),&ctx);
        SNLOG("%s",buffer);
        if(strcmp(buffer,SIMUNETBLOATHEAD_ID)) throw 3;
        
        /* Read description */
        l=read_uint16_t(s,&ctx);
        read_chars(s,buffer,l,&ctx);
        description=buffer;
        
        /* Multiple SNBloatIndexObjects */
        numFiles=read_uint32_t(s,&ctx);
        for(uint32_t i=0;i<numFiles;i++){
            l=read_uint16_t(s,&ctx);
            if(l>SIMUNETBLOAT_MAX_FILENAME_LENGTH) throw 4;
            read_chars(s,buffer,l,&ctx);
            
            SNBloatIndexObject o;
            o.virtfilename=buffer;
            o.size=read_uint32_t(s,&ctx);
            o.physSize=read_uint32_t(s,&ctx);
            o.position=read_uint32_t(s,&ctx);
            
            read_buffer(s,o.sum,SHA512_DIGEST_LENGTH,&ctx);
            
            /* Load variables for this bloatobject */
            uint16_t x=read_uint16_t(s,&ctx);
            for(uint16_t j=0;j<x;j++){
                l=read_uint16_t(s,&ctx);
                read_chars(s,buffer,l,&ctx);
                l=read_uint16_t(s,&ctx);
                read_chars(s,buffer2,l,&ctx);
                o.vars[buffer]=buffer2;
            }
            
            bloatmap[o.virtfilename]=o;
        }
        
        unsigned char sumbuffer1[SHA512_DIGEST_LENGTH];
        unsigned char sumbuffer2[SHA512_DIGEST_LENGTH];
        SHA512_Final(sumbuffer1,&ctx);
        read_buffer(s,sumbuffer2,SHA512_DIGEST_LENGTH);
        if(memcmp(sumbuffer1,sumbuffer2,SHA512_DIGEST_LENGTH)) throw 5;
    }
    catch(int x){
        SNLOG("Threw up %d",x);
        return x;
    }
    return 0;
}

int SNBloat::writeHeader(FILE *s)
{
    try{
        char buffer[0x10000];
        uint16_t l;
        
        SHA512_CTX ctx;
        SHA512_Init(&ctx);
        if(fseek(s,0,SEEK_SET)!=0) throw 2;
        
        /* ID */
        write_chars(s,SIMUNETBLOAT_ID,&ctx);
        
        /* Version */
        write_uint16_t(s,0,&ctx);
        write_uint16_t(s,1,&ctx);
        
        /* Bitmap containing purpose of this file */
        write_uint32_t(s,purpose,&ctx);
        
        /* Now we are going to find, where ends the last stored object and place rest of the header right after it */
        /* It's a given, that all files are already inplace and we don't need to inflate the file anymore just to get to header. */
        size_t pos=24;
        for(std::map<std::string,SNBloatIndexObject>::const_iterator it=bloatmap.begin();it!=bloatmap.end();++it){
            size_t x=it->second.position+it->second.physSize;
            if(x>pos) pos=x;
        }
        
        /* Set and move to header position */
        write_uint32_t(s,pos,&ctx);
        if(fseek(s,pos,SEEK_SET)!=0) throw 2;
        
        /* HeadID */
        write_chars(s,SIMUNETBLOATHEAD_ID,&ctx);
        
        /* Read description */
        strcpy(buffer,description.c_str());
        l=strlen(buffer);
        write_uint16_t(s,l,&ctx);
        write_chars(s,buffer,&ctx);
        
        /* Multiple SNBloatIndexObjects */
        numFiles=bloatmap.size();
        write_uint32_t(s,numFiles,&ctx);
        for(std::map<std::string,SNBloatIndexObject>::const_iterator it=bloatmap.begin();it!=bloatmap.end();++it){
            SNBloatIndexObject o=it->second;
            
            strcpy(buffer,o.virtfilename.c_str());
            l=strlen(buffer);
            write_uint16_t(s,l,&ctx);
            write_chars(s,buffer,&ctx);
            
            write_uint32_t(s,o.size,&ctx);
            write_uint32_t(s,o.physSize,&ctx);
            write_uint32_t(s,o.position,&ctx);
            
            write_buffer(s,o.sum,SHA512_DIGEST_LENGTH,&ctx);
            
            /* Save variables for this bloatobject */
            write_uint16_t(s,o.vars.size(),&ctx);
            for(SNBloatVars::const_iterator v=o.vars.begin();v!=o.vars.end();++v){
                strcpy(buffer,v->first.c_str());
                l=strlen(buffer);
                write_uint16_t(s,l,&ctx);
                write_chars(s,buffer,&ctx);
                
                strcpy(buffer,v->second.c_str());
                l=strlen(buffer);
                write_uint16_t(s,l,&ctx);
                write_chars(s,buffer,&ctx);
            }
        }
        
        unsigned char sumbuffer[SHA512_DIGEST_LENGTH];
        SHA512_Final(sumbuffer,&ctx);
        write_buffer(s,sumbuffer,SHA512_DIGEST_LENGTH);
    }
    catch(int x){
        SNLOG("Threw up %d",x);
        return x;
    }
    return 0;
}

int SNBloat::openForReading(void)
{
    FILE *s;
    s=fopen(fname.c_str(),"rb");
    int r=readHeader(s);
    fclose(s);
    if(r) return 1;
    return 0;
}

/* This function reads header and rewrites it with new one. If the file doesn't exist, then it will create one and dump a new header into it. */
/* The idea when creating a new file or appending to one is to call this function, then append all files one by one and call this function again. */
int SNBloat::openForWriting(bool eraseFirst)
{
    FILE *s;
    int r;
    if(eraseFirst) s=fopen(fname.c_str(),"wb");
    else s=fopen(fname.c_str(),"rb+");
    if(s==NULL) return 2;
    r=writeHeader(s);
    fclose(s);
    if(r) return 1;
    return 0;
}

std::vector<SNBloatIndexObject> SNBloat::getList(void)
{
    std::vector<SNBloatIndexObject> v;

    /* Export map to vector, and what a surprise, it's already sorted by filename */
    for(std::map<std::string,SNBloatIndexObject>::const_iterator it=bloatmap.begin();it!=bloatmap.end();++it){
        v.push_back(it->second);
    }
    
    return v;
}

int SNBloat::getBIO(std::string filename,SNBloatIndexObject *o)
{
    if(bloatmap.count(filename)!=1) return -1;
    *o=bloatmap[filename];
    return 0;
}

int SNBloat::checkSum(std::string filename)
{
    SNBloatRWControl o;
    unsigned char *buf=NULL;
    buf=(unsigned char *)malloc(SIMUNETBLOAT_FILEBUFFER);
    try{
        if(readOpen(filename.c_str(),&o,true)!=0) throw -2;
        
        size_t now;
        while(o.physSize>0){
            if(o.physSize>SIMUNETBLOAT_FILEBUFFER) now=SIMUNETBLOAT_FILEBUFFER;
            else now=o.physSize;
            if(readNext(buf,now,&o)!=now) throw -5;
        }
        if(readClose(&o)!=0) throw -6;
        free(buf);
    }
    catch(int x){
        free(buf);
        readClose(&o);
        return x;
    }
    return 0;
}

unsigned char * SNBloat::readToBuffer(std::string filename)
{
    SNBloatRWControl o;
    unsigned char *buf=NULL;
    try{
        if(readOpen(filename.c_str(),&o,false)!=0) throw;
        
        size_t size=o.size;
        if(size>SIMUNETBLOAT_MAXFILEBUFFER) throw ;
        
        buf=(unsigned char *)malloc(size);
        if(buf==NULL) throw;
        
        if(readNext(buf,size,&o)!=size) throw ;
        if(readClose(&o)!=0) throw ;
    }
    catch(...){
        free(buf);
        readClose(&o);
        return NULL;
    }
    return buf;
}

int SNBloat::writeFromBuffer(std::string filename, unsigned char *buffer, size_t size, SNBloatVars vars)
{
    SNBloatRWControl o;
    try{
        if(writeOpen(filename.c_str(),&o,vars)!=0) throw;
        if(writeNext(buffer,size,&o)!=size) throw;
        if(writeClose(&o)!=0) throw;
    }
    catch(...){
        readClose(&o);
        return -1;
    }
    return 0;
}

int SNBloat::writeFromBuffer(std::string filename, unsigned char *buffer, size_t size)
{
    SNBloatVars v=globalVars;
    return writeFromBuffer(filename,buffer,size,v);
}

int SNBloat::readOpen(std::string filename,SNBloatRWControl *o,bool checkSum)
{
    FILE *s=NULL;
    try{
        if(bloatmap.count(filename)!=1) throw 1;
        s=fopen(fname.c_str(),"rb");
        if(s==NULL) throw 2;
        
        SNBloatIndexObject bio=bloatmap[filename];
        if(fseek(s,bio.position,SEEK_SET)!=0) throw 3;
        
        o->s=s;
        o->bio=bloatmap[filename];
        o->size=bio.size;
        o->physSize=bio.physSize;
        o->checkSum=checkSum;
        o->failed=false;
        if(checkSum) SHA512_Init(&(o->ctx));
    }
    catch(int x){
        if(s!=NULL) fclose(s);
        return x;
    }
    return 0;
}

size_t SNBloat::readNext(unsigned char *buffer,size_t size,SNBloatRWControl *o)
{
    try{
        if(size>o->physSize){
            size=o->physSize;
        }
        if(o->checkSum) read_buffer(o->s,buffer,size,&(o->ctx));
        else  read_buffer(o->s,buffer,size);
        
        o->physSize-=size;
        o->size-=size;
    }
    catch(...){
        o->physSize=0;
        o->size=0;
        o->failed=true;
        return 0;
    }
    return size;
}

int SNBloat::readClose(SNBloatRWControl *o)
{
    fclose(o->s);
    if(o->checkSum){
        unsigned char sumbuffer[SHA512_DIGEST_LENGTH];
        SHA512_Final(sumbuffer,&(o->ctx));
        if(memcmp(sumbuffer,o->bio.sum,SHA512_DIGEST_LENGTH)) return 1;
    }
    return 0;
}

int SNBloat::writeOpen(std::string filename,SNBloatRWControl *o,SNBloatVars vars)
{
    FILE *s=NULL;
    o->failed=true;
    try{
        if(bloatmap.count(filename)!=0) throw 1;
        s=fopen(fname.c_str(),"rb+");
        if(s==NULL) throw 2;

        size_t pos=24;
        for(std::map<std::string,SNBloatIndexObject>::const_iterator it=bloatmap.begin();it!=bloatmap.end();++it){
            size_t x=it->second.position+it->second.physSize;
            if(x>pos) pos=x;
        }
        
        if(fseek(s,pos,SEEK_SET)!=0) throw 3;
        
        o->s=s;
        o->bio.position=pos;
        o->bio.virtfilename=filename;
        o->bio.vars=vars;
        o->size=0;
        o->physSize=0;
        o->failed=false;
        SHA512_Init(&(o->ctx));
    }
    catch(int x){
        if(s!=NULL) fclose(s);
        return x;
    }
    return 0;
}

size_t SNBloat::writeNext(unsigned char *buffer,size_t size,SNBloatRWControl *o)
{
    if(o->failed) return 0;
    try{
        write_buffer(o->s,buffer,size,&(o->ctx));
        o->size+=size;
        o->physSize+=size;
    }
    catch(...){
        o->failed=true;
        fclose(o->s);
        return 0;
    }
    return size;
}

int SNBloat::writeClose(SNBloatRWControl *o)
{
    if(o->failed) return 1;
    fclose(o->s);
    SHA512_Final(o->bio.sum,&(o->ctx));
    o->bio.size=o->size;
    o->bio.physSize=o->physSize;
    bloatmap[o->bio.virtfilename]=o->bio;
    return 0;
}

uint8_t SNBloat::read_uint8_t(FILE *s,SHA512_CTX *ctx)
{
    if(s==NULL) throw 47;
    uint8_t x;
    if(fread(&x,sizeof(uint8_t),1,s)!=1) throw 47;
    if(ctx!=NULL) SHA512_Update(ctx,&x,1);
    return x;
}

uint16_t SNBloat::read_uint16_t(FILE *s,SHA512_CTX *ctx)
{
    if(s==NULL) throw 47;
    uint8_t x[2];
    if(fread(x,sizeof(uint8_t),2,s)!=2) throw 47;
    SNLOG("%d, %d",x[0],x[1]);
    if(ctx!=NULL) SHA512_Update(ctx,x,2);
    return (x[1]<<8)|x[0];
}

uint32_t SNBloat::read_uint24_t(FILE *s,SHA512_CTX *ctx)
{
    if(s==NULL) throw 47;
    uint8_t x[3];
    if(fread(x,sizeof(uint8_t),3,s)!=3) throw 47;
    if(ctx!=NULL) SHA512_Update(ctx,x,3);
    return (x[2]<<16)|(x[1]<<8)|x[0];
}

uint32_t SNBloat::read_uint32_t(FILE *s,SHA512_CTX *ctx)
{
    if(s==NULL) throw 47;
    uint8_t x[4];
    if(fread(x,sizeof(uint8_t),4,s)!=4) throw 47;
    SNLOG("%d, %d, %d, %d",x[0],x[1],x[2],x[3]);
    if(ctx!=NULL) SHA512_Update(ctx,x,4);
    return (x[3]<<24)|(x[2]<<16)|(x[1]<<8)|x[0];
}

uint64_t SNBloat::read_uint64_t(FILE *s,SHA512_CTX *ctx)
{
    if(s==NULL) throw 47;
    uint8_t x[8];
    if(fread(x,sizeof(uint8_t),8,s)!=8) throw 47;
    if(ctx!=NULL) SHA512_Update(ctx,x,8);
    return ((uint64_t)x[7]<<56)|
            ((uint64_t)x[6]<<48)|
            ((uint64_t)x[5]<<40)|
            ((uint64_t)x[4]<<32)|
            ((uint64_t)x[3]<<24)|
            ((uint64_t)x[2]<<16)|
            ((uint64_t)x[1]<<8)|
            x[0];
}

void SNBloat::read_chars(FILE *s,char *buffer,size_t size,SHA512_CTX *ctx)
{
    if(fread(buffer,1,size,s)!=size) throw 47;
    if(ctx!=NULL) SHA512_Update(ctx,buffer,size);
    buffer[size]='\0';
}

void SNBloat::read_buffer(FILE *s,void *buffer,size_t size,SHA512_CTX *ctx)
{
    if(fread(buffer,1,size,s)!=size) throw 47;
    if(ctx!=NULL) SHA512_Update(ctx,buffer,size);
}

void SNBloat::write_uint8_t(FILE *s,uint8_t n,SHA512_CTX *ctx)
{
    if(s==NULL) throw 47;
    if(fwrite(&n,sizeof(uint8_t),1,s)!=1) throw 47;
    if(ctx!=NULL) SHA512_Update(ctx,&n,1);
}

void SNBloat::write_uint16_t(FILE *s,uint16_t n,SHA512_CTX *ctx)
{
    if(s==NULL) throw 47;
    uint8_t x[2];
    x[0]=n&0xFF;n>>=8;
    x[1]=n&0xFF;
    SNLOG("%d, %d",x[0],x[1]);
    if(fwrite(x,sizeof(uint8_t),2,s)!=2) throw 47;
    if(ctx!=NULL) SHA512_Update(ctx,x,2);
}

void SNBloat::write_uint24_t(FILE *s,uint32_t n,SHA512_CTX *ctx)
{
    if(s==NULL) throw 47;
    uint8_t x[3];
    x[0]=n&0xFF;n>>=8;
    x[1]=n&0xFF;n>>=8;
    x[2]=n&0xFF;
    if(fwrite(x,sizeof(uint8_t),3,s)!=3) throw 47;
    if(ctx!=NULL) SHA512_Update(ctx,x,3);
}

void SNBloat::write_uint32_t(FILE *s,uint32_t n,SHA512_CTX *ctx)
{
    if(s==NULL) throw 47;
    uint8_t x[4];
    x[0]=n&0xFF;n>>=8;
    x[1]=n&0xFF;n>>=8;
    x[2]=n&0xFF;n>>=8;
    x[3]=n&0xFF;
    SNLOG("%d, %d, %d, %d",x[0],x[1],x[2],x[3]);
    if(fwrite(x,sizeof(uint8_t),4,s)!=4) throw 47;
    if(ctx!=NULL) SHA512_Update(ctx,x,4);
}

void SNBloat::write_uint64_t(FILE *s,uint64_t n,SHA512_CTX *ctx)
{
    if(s==NULL) throw 47;
    uint8_t x[8];
    x[0]=n&0xFF;n>>=8;
    x[1]=n&0xFF;n>>=8;
    x[2]=n&0xFF;n>>=8;
    x[3]=n&0xFF;n>>=8;
    x[4]=n&0xFF;n>>=8;
    x[5]=n&0xFF;n>>=8;
    x[6]=n&0xFF;n>>=8;
    x[7]=n&0xFF;
    if(fwrite(x,sizeof(uint8_t),8,s)!=8) throw 47;
    if(ctx!=NULL) SHA512_Update(ctx,x,8);
}

void SNBloat::write_chars(FILE *s,char *buffer,SHA512_CTX *ctx)
{
    size_t size=strlen(buffer);
    if(fwrite(buffer,1,size,s)!=size) throw 47;
    if(ctx!=NULL) SHA512_Update(ctx,buffer,size);
}

void SNBloat::write_buffer(FILE *s,void *buffer,size_t size,SHA512_CTX *ctx)
{
    if(fwrite(buffer,1,size,s)!=size) throw 47;
    if(ctx!=NULL) SHA512_Update(ctx,buffer,size);
}
