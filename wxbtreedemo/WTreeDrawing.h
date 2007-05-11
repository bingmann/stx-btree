// $Id$

#ifndef _WTreeDrawing_H_
#define _WTreeDrawing_H_

#include <wx/wx.h>

#include "WMain.h"

class WTreeDrawing : public wxScrolledWindow
{
public:
    WTreeDrawing(wxWindow *parent, int id);
    ~WTreeDrawing();

    wxSize		oldTreeSize;

    virtual void	OnDraw(wxDC& dc);

    void		DrawBTree(wxDC &dc);

    static wxSize	DrawBTreeNode(wxDC &dc, int offsetx, int offsety, const class WMain::btree_type::btree_impl::node* node);

    class WMain* 	wmain;
    void		SetWMain(class WMain *wm);

    DECLARE_EVENT_TABLE();
};

#endif // _WTreeDrawing_H_
