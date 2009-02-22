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
#include "SNBloatList.h"

using namespace std;

SNBloatList::SNBloatList()
{
    filemap.clear();
    nowBloatId=1;
    doCheckSum=false;
}

SNBloatList::~SNBloatList()
{
}

void SNBloatList::scanDir(std::string dir)
{
}

void SNBloatList::setCheckSum(bool checksum)
{
    doCheckSum=checksum;
}

void SNBloatList::addFile(std::string filename,uint32_t bloatId)
{
    mutex.lock();
    filemap[filename].v.push_back(bloatId);
    filemap[filename].id=bloatId;
    mutex.unlock();
}

int SNBloatList::addBloat(std::string filename)
{
    uint32_t id;
    SNBloat *b=NULL;
    mutex.lock();
    id=nowBloatId;
    nowBloatId++;
    try{
        b=new SNBloat(filename);
        std::vector<SNBloatIndexObject> v=b->getList();
        if(v.size()<1) throw;
        
        SNBloatListID lid;
        lid.filename=filename;
        lid.bloat=b;
        bloatlist[id]=lid;
        
        for(uint32_t i=0;i<v.size();i++){
            addFile(v[i].virtfilename,id);
        }
    }
    catch(...){
        if(b!=NULL) delete(b);
        id=0;
    }
    mutex.unlock();
    return id;
}

void SNBloatList::removeBloat(uint32_t bloatId)
{
    mutex.lock();
    
    for(SNBloatListFMapType::iterator it=filemap.begin();it!=filemap.end();++it){
        for(vector<uint32_t>::iterator i=it->second.v.begin();i!=it->second.v.end();++i){
            if(*i==bloatId) it->second.v.erase(i);
        }
        it->second.id=it->second.v.back();
    }
    
    delete(bloatlist[bloatId].bloat);
    bloatlist.erase(bloatId);
    
    mutex.unlock();
}

unsigned char * SNBloatList::readToBuffer(std::string filename,size_t *filesize)
{
    /* TODO: Tu treba dat semafor, ale to nie je momentalne take zavazne. */
    try{
        if(filemap.count(filename)!=1) throw;
        return bloatlist[filemap[filename].id].bloat->readToBuffer(filename,filesize,doCheckSum);
    }
    catch(...){
    }
    return NULL;
}
