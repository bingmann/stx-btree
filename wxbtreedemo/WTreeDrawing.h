// $Id$

#ifndef _WTreeDrawing_H_
#define _WTreeDrawing_H_

#include <wx/wx.h>

class WTreeDrawing : public wxScrolledWindow
{
public:
    WTreeDrawing(wxWindow *parent, int id);

    wxSize		oldTreeSize;

    virtual void	OnDraw(wxDC& dc);
    void		OnSize(wxSizeEvent &se);

    void		DrawBTree(wxDC &dc);

    struct BTreeOp_Draw
    {
	BTreeOp_Draw(WTreeDrawing &_w, wxDC &_dc, const class BTreeBundle &_tb)
	    : w(_w), dc(_dc), tb(_tb)
	{
	}

	WTreeDrawing &w;
	wxDC &dc;
	const BTreeBundle &tb;

	typedef	wxSize	result_type;

	template <class BTreeType>
	wxSize draw_node(int offsetx, int offsety, const class BTreeType::btree_impl::node* node);

	template <class BTreeType>
	wxSize draw_tree(BTreeType &bt);

	template <class BTreeType>
	wxSize opInteger(BTreeType &bt);

	template <class BTreeType>
	wxSize opString(BTreeType &bt);
    };

    class WMain* 	wmain;
    void		SetWMain(class WMain *wm);

    DECLARE_EVENT_TABLE();
};

#endif // _WTreeDrawing_H_
