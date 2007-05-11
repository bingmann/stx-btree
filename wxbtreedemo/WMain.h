// $Id$

#ifndef _WMain_H_
#define _WMain_H_

#include "WMain_wxg.h"

#define BTREE_FRIENDS	friend class ::WTreeDrawing;

#include <stx/btree_map.h>
#include <string>

template <int IS, int LS>
struct btree_traits_nodebug
{
    static const bool	selfverify = true;
    static const bool	debug = false;

    static const int 	leafslots = IS;
    static const int	innerslots = LS;
};

class WMain : public WMain_wxg
{
public:
    WMain();

    typedef stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<8, 8> > btree_type;

    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<8, 8> >		btree_int_4slots;

    void	OnButtonInsert(wxCommandEvent &ce);
    void	OnButtonErase(wxCommandEvent &ce);
    void	OnButtonInsertRandom(wxCommandEvent &ce);

    void	OnButtonClear(wxCommandEvent &ce);

    void	OnMenuInsertRandom(wxCommandEvent &ce);

    void	UpdateTextDump();

    DECLARE_EVENT_TABLE();
};

#endif // _WMain_H_
