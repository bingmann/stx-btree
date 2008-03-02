// $Id$

#include <fstream>
#include <iostream>

class btree_pagefile
{
public:

    static const unsigned int	pagesize = 1024;

    typedef char*		page_buffer_type;

    typedef unsigned int	pageid_type;


    std::fstream		fs;

    std::fstream::pos_type	fsize;

    struct first_page
    {
	char 		signature[32];
	pageid_type	freelist;
	char		dbinfo[30][32];
    };

    char 			first_page_data[pagesize];

    struct first_page 		*firstpage;
    unsigned int		dbnumber;

    inline btree_pagefile(const char *filename, unsigned int dbnumber=0)
    {
	open(filename, dbnumber);
    }

    inline bool open(const char *filename, unsigned int dbnumber=0)
    {
	fs.open(filename, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
 	if (!fs.good()) {
	    return create(filename, dbnumber);
	}

	fs.seekp(0, std::fstream::end);
	fsize = fs.tellp();

	return check_signature(dbnumber);
    }

    inline bool create(const char *filename, unsigned int dbnumber=0)
    {
	fs.open(filename, std::ios_base::out | std::ios::trunc | std::ios_base::binary);
	if (!fs.good()) return false;
	fs.close();

	fs.open(filename, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
	write_new_signature();
	this->dbnumber = dbnumber;
	fsize = 1024;

	return true;
    }

    inline void close()
    {
	fs.close();
	fsize = 0;
    }

    inline bool check_signature(unsigned int dbnumber)
    {
	firstpage = reinterpret_cast<first_page*>(&first_page_data);

	if (!get(0, first_page_data))
	    return false;

	if (strncmp(firstpage->signature, "btree_pagefile", 14) != 0)
	    return false;

	this->dbnumber = dbnumber;

	return true;
    }

    inline void write_new_signature()
    {
	firstpage = reinterpret_cast<first_page*>(&first_page_data);

	memset(firstpage, 0, pagesize);
	strcpy(firstpage->signature, "btree_pagefile");
	firstpage->freelist = 0;

	put(0, first_page_data);
    }

    pageid_type allocate()
    {
	if (firstpage->freelist != 0)
	{
	    pageid_type freeid = firstpage->freelist;
	    char freepage[pagesize];

	    get(freeid, freepage);
	    
	    firstpage->freelist = *reinterpret_cast<pageid_type*>(freepage);
	    memset(freepage, 0x11, pagesize);

	    put(0, first_page_data);
	    put(freeid, freepage);

	    return freeid;
	}
	else
	{
	    pageid_type freeid = (fsize / pagesize);
	    char freepage[pagesize];

	    memset(freepage, 0x11, pagesize);
	    put(freeid, freepage);

	    fsize += pagesize;

	    return freeid;
	}
    }

    bool get(pageid_type pageid, page_buffer_type outbuffer)
    {
	fs.seekg(pageid * pagesize);
	fs.read(outbuffer, pagesize);
	return fs.good();
    }

    bool put(pageid_type pageid, page_buffer_type outbuffer)
    {
	fs.seekp(pageid * pagesize);
	fs.write(outbuffer, pagesize);
	return fs.good();
    }

    void free(pageid_type freeid)
    {
	char freepage[pagesize];
	get(freeid, freepage);

	*reinterpret_cast<pageid_type*>(freepage) = firstpage->freelist;

	put(0, first_page_data);
	put(freeid, freepage);
    }

    char* get_dbinfo()
    {
	return firstpage->dbinfo[dbnumber];
    }

    void set_root(char *dbinfo)
    {
	memcpy(firstpage->dbinfo[dbnumber], dbinfo, 32);
	put(0, first_page_data);
    }
};
