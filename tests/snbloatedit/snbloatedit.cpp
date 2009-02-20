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

#include "core/SNBloat.h"
#include <cstdlib>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#define BUFSIZE 524288
unsigned char copybuffer[BUFSIZE];

using namespace std;

int addfile(SNBloat *bloat,const char *fname,const char *virtfname,std::map<std::string,std::string> vars)
{
    SNBloatRWControl o;
    FILE *s=fopen(fname,"rb");
    if(s==0) return -1;
    printf("Adding '%s' as '%s' ... ",fname,virtfname);
    fflush(stdout);
    try{
        if(bloat->writeOpen(virtfname,&o,vars)!=0) throw -2;
        
        if(fseek(s,0,SEEK_END)!=0) throw -3;
        size_t size=ftell(s);
        if(fseek(s,0,SEEK_SET)!=0) throw -3;
        
        size_t now;
        while(size>0){
            if(size>BUFSIZE) now=BUFSIZE;
            else now=size;
            
            if(fread(copybuffer,1,now,s)!=now) throw -4;
            if(bloat->writeNext(copybuffer,now,&o)!=now) throw -5;
            
            size-=now;
        }
        
        fclose(s);
        bloat->writeClose(&o);
        printf("SUCCESS\n");
    }
    catch(int x){
        printf("ERROR %d\n",x);
        fclose(s);
        bloat->writeClose(&o);
        return x;
    }
    return 0;
}

void showlist(std::vector<SNBloatIndexObject> v)
{
    printf("Listing files:\n");
    
    for(uint32_t i=0;i<v.size();i++){
        SNBloatIndexObject x=v[i];
        printf("%s\t",x.virtfilename.c_str());
        printf("Pos: %d ",x.position);
        printf("Size: %d\n",x.physSize);
    }
    
    printf("%d files listed.\n",v.size());
}

void showusage(char *cmd)
{
    printf("Usage: %s ACTION [OPTIONS] BLOATFILE [FILE1] [FILE2] ...\n\n",cmd);
    printf("Action:\n");
    printf("a - add files to bloat\n");
    printf("c - create new bloat and add files\n");
    printf("l - list files in bloat\n");
    printf("x - extract files from bloat\n");
    printf("s - check all sums in bloat\n");
    
    exit(1);
}

const char *getfilename(const char *fname)
{
    for(int i=strlen(fname)-1;i>=0;i--){
        if(fname[i]=='\\'||fname[i]=='/') return fname+(i+1);
    }
    return fname;
}

const char *getextension(const char *fname)
{
    for(int i=strlen(fname)-1;i>=0;i--){
        if(fname[i]=='\\'||fname[i]=='/') break;
        if(fname[i]=='.') return fname+(i+1);
    }
    return "";
}

int crawl(SNBloat *bloat,const char *fname,const char *path,const char *d_name)
{
    struct stat st;
    char newfname[SIMUNETBLOAT_MAX_FILENAME_LENGTH];
    char newpath[SIMUNETBLOAT_MAX_FILENAME_LENGTH];
    if(stat(fname,&st)!=0) return -1;
    if(st.st_mode&S_IFREG){
        snprintf(newpath,SIMUNETBLOAT_MAX_FILENAME_LENGTH,"%s%s",path,getfilename(fname));
        std::map<std::string,std::string> vars;
        return addfile(bloat,fname,newpath,vars);
    }
    
    if(!(st.st_mode&S_IFDIR)) return -2;

    snprintf(newpath,SIMUNETBLOAT_MAX_FILENAME_LENGTH,"%s%s",path,d_name);
    
    DIR * dir = opendir(fname);
    if(dir==NULL) return -3;
    struct dirent *d;
    
    while(d=readdir(dir)){
        if(strlen(d->d_name)>0&&d->d_name[0]!='.'&&d->d_name[0]!='-'&&strcasecmp(getextension(d->d_name),"snbloat")){
            snprintf(newfname,SIMUNETBLOAT_MAX_FILENAME_LENGTH,"%s/%s",fname,d->d_name);
            if(stat(newfname,&st)==0){
                if(st.st_mode&(S_IFREG|S_IFDIR)){
                    crawl(bloat,newfname,newpath,strcat(d->d_name,"/"));
                }
            }
        }
    }
    closedir(dir);
    
    return 0;
}

int execute(SNBloat *bloat,char action,vector<string> files)
{
    if(action=='l'){
        if(bloat->openForReading()) return 3;
        showlist(bloat->getList());
    }

    if(action=='s'){
        if(bloat->openForReading()) return 3;
        std::vector<SNBloatIndexObject> v=bloat->getList();
        printf("Checking all sums.\n");
        for(uint32_t i=0;i<v.size();i++){
            printf("%s ... ",v[i].virtfilename.c_str());
            if(bloat->checkSum(v[i].virtfilename)!=0) printf("Bad checksum!\n");
            else printf("OK\n");
        }
        printf("Checking finished.\n");
    }

    if(action=='a'||action=='c'){
        if(action=='c'&&bloat->openForWriting(true)) return 4;
        if(action=='a'&&bloat->openForReading()&&bloat->openForWriting()) return 3;
        
        printf("Preparing to add new files.\n");
        for(uint32_t i=0;i<files.size();i++){
            crawl(bloat,files[i].c_str(),"/","");
        }
        printf("Adding files finished.\n");
        
        if(bloat->openForWriting()!=0) return 4;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    /* TODO: toto zmenit na poriadny zapis */
    printf("SNBloatEditor\n");
    printf("Copyright (C) 2009 by Samuel Kupka, skupka@sageteam.eu\n\n");
    if(argc<3) showusage(argv[0]);
    if(strcmp(argv[1],"a")&&
        strcmp(argv[1],"c")&&
        strcmp(argv[1],"l")&&
        strcmp(argv[1],"s")&&
        strcmp(argv[1],"x")) showusage(argv[0]);
        
    int now=2;
    for(;argv[now][0]=='-'&&now<argc;now++){
    }
    
    SNBloat *bloat=NULL;
    if(now<argc){
        printf("Using bloat '%s'.\n",argv[now]);
        bloat=new SNBloat(argv[now]);
        now++;
    }
    else showusage(argv[0]);
    
    vector<string> v;
    for(;now<argc;now++){
        v.push_back(argv[now]);
    }
    int r=execute(bloat,argv[1][0],v);
    
    delete(bloat);
    if(r==3) fputs("ERROR: Broken bloat header!\n",stderr);
    if(r==4) fputs("ERROR: Can't write bloat header!\n",stderr);
    return r;
}
