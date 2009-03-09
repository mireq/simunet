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
#ifndef SNBLOATLIST_H
#define SNBLOATLIST_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <QMutex>

/* zmenene na standardny komentar aby nebola tato trieda (zatial) automaticky
   dokumentovana, odporucam z autora zrusit mail (kvoli spamom)
*/
/*
 @author Samuel Kupka <skupka@sageteam.eu>
*/

struct SNBloatListObject {
    std::vector<uint32_t> v;
    uint32_t id;
};

struct SNBloatListID {
    std::string filename;
    SNBloat *bloat;
};

typedef std::map<std::string,SNBloatListObject> SNBloatListFMapType;

class SNBloatList
{
	public:
		SNBloatList();
		~SNBloatList();
		
        void scanDir(std::string dir);
        void addFile(std::string filename,uint32_t bloatId);
        int addBloat(std::string filename);
        void removeBloat(uint32_t bloatId);
        void setCheckSum(bool checksum=true);
        
        unsigned char *readToBuffer(std::string filename,size_t *filesize);
		
    private:
        SNBloatListFMapType filemap;
        std::map<uint32_t,SNBloatListID> bloatlist;
        std::string tempdir;
        uint32_t nowBloatId;
        QMutex mutex;
        bool doCheckSum;
};
#endif
