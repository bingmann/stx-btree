// $Id$

#include "WTreeDrawing.h"
#include "WMain.h"

#include <vector>

WTreeDrawing::WTreeDrawing(wxWindow *parent, int id)
    : wxScrolledWindow(parent, id),
      wmain(NULL)
{

}

WTreeDrawing::~WTreeDrawing()
{
}

void WTreeDrawing::SetWMain(WMain *wm)
{
    wmain = wm;
}

void WTreeDrawing::OnDraw(wxDC &dc)
{
    DrawBTree(dc);
}

wxSize WTreeDrawing::DrawBTreeNode(wxDC &dc, int offsetx, int offsety, const class WMain::btree_type::btree_impl::node* node)
{
    typedef WMain::btree_type::btree_impl btree_impl;

    const int textpadding = 3;
    const int nodepadding = 10;

    if (node->isleafnode())
    {
	const btree_impl::leaf_node *leafnode = static_cast<const btree_impl::leaf_node*>(node);

	int textx = 0, texty = 0;
	int maxh = 0;
	for (unsigned int slot = 0; slot < leafnode->slotuse; ++slot)
	{
	    int textkeyw, textkeyh;
	    int textvalw, textvalh;

	    wxString textkey;
	    textkey << leafnode->slotkey[slot];
	    dc.GetTextExtent(textkey, &textkeyw, &textkeyh);

	    wxString textval;
	    textval << leafnode->slotdata[slot];
	    dc.GetTextExtent(textval, &textvalw, &textvalh);

	    int maxw = std::max(textkeyw, textvalw);
	    int addkeyw = (maxw - textkeyw) / 2;
	    int addvalw = (maxw - textvalw) / 2;

	    if (offsetx >= 0)
	    {
		dc.DrawRectangle(offsetx + textx, offsety + texty,
				 maxw + 2*textpadding, textkeyh + 2*textpadding);

		dc.DrawText(textkey,
			    offsetx + textx + textpadding + addkeyw,
			    offsety + texty + textpadding);

		dc.DrawRectangle(offsetx + textx, offsety + texty + textkeyh + 2*textpadding - 1,
				 maxw + 2*textpadding, textvalh + 2*textpadding);

		dc.DrawText(textval,
			    offsetx + textx + textpadding + addvalw,
			    offsety + texty + textkeyh + 2*textpadding + textpadding - 1);
	    }

	    textx += maxw + 2*textpadding - 1;
	    maxh = std::max(maxh, textkeyh + 2*textpadding + textvalh + 2*textpadding - 1);
	}

	return wxSize(textx, maxh);
    }
    else
    {
	const btree_impl::inner_node *innernode = static_cast<const btree_impl::inner_node*>(node);

	const int childnum = (innernode->slotuse + 1);
	// find the maximium width and height of all children
	int childmaxw = 0, childmaxh = 0;

	std::vector<int> childw;
	for (unsigned int slot = 0; slot <= innernode->slotuse; ++slot)
	{
	    wxSize cs = DrawBTreeNode(dc, -1, -1, innernode->childid[slot]);
	    childmaxw = std::max(childmaxw, cs.GetWidth());
	    childmaxh = std::max(childmaxh, cs.GetHeight());
	    childw.push_back(cs.GetWidth());
	}

	int textx = 0, texty = 0;
	int childx = 0, childy = 60;
	int maxh = 0;

	// calculate width of box.
	int allchildw = (childnum + 1) * (childmaxw / 2 + nodepadding) - 2*nodepadding;

	if (childnum % 2 == 0)
	{
	    allchildw += (childmaxw / 2 + nodepadding);
	}

	// debug rectangle for total size of children dc.DrawRectangle(childx, childy-5, allchildw, 2);

	// calc width of node's keys box
	int keyboxh = 0;
	for (unsigned int slot = 0; slot < innernode->slotuse; ++slot)
	{
	    int textkeyw, textkeyh;

	    wxString textkey;
	    textkey << innernode->slotkey[slot];
	    dc.GetTextExtent(textkey, &textkeyw, &textkeyh);

	    textx += textkeyw + 2*textpadding - 1;
	    keyboxh = std::max(keyboxh, textkeyh + 2*textpadding - 1);
	}

	textx = std::max(0, allchildw - textx) / 2;
	childy = keyboxh + nodepadding*4;

	for (unsigned int slot = 0; slot < innernode->slotuse; ++slot)
	{
	    int textkeyw, textkeyh;

	    wxString textkey;
	    textkey << innernode->slotkey[slot];
	    dc.GetTextExtent(textkey, &textkeyw, &textkeyh);

	    if (offsetx >= 0)
	    {
		dc.DrawRectangle(offsetx + textx, offsety + texty,
				 textkeyw + 2*textpadding, textkeyh + 2*textpadding);
		dc.DrawText(textkey,
			    offsetx + textx + textpadding,
			    offsety + texty + textpadding);

		// draw child
		if (innernode->level == 1)
		{
		    // draw leaf node with border to see free slots
		    dc.DrawRectangle(offsetx + childx, offsety + childy,
				     childmaxw, childmaxh);

		    DrawBTreeNode(dc, offsetx + childx, offsety + childy, innernode->childid[slot]);
		}
		else
		{
		    // draw centered inner node
		    DrawBTreeNode(dc,
				  offsetx + childx + std::max(0, (childmaxw - childw[slot]) / 2),
				  offsety + childy,
				  innernode->childid[slot]);
		}

		// calculate spline from key anchor to middle of child's box
		wxPoint splinept[4];
		splinept[0] = wxPoint(offsetx + textx, offsety + texty + keyboxh);
		splinept[1] = wxPoint(offsetx + textx, offsety + texty + keyboxh + 20);

		splinept[2] = wxPoint(offsetx + childx + childmaxw / 2, offsety + childy - 20);
		splinept[3] = wxPoint(offsetx + childx + childmaxw / 2, offsety + childy);

		dc.DrawSpline(4, splinept);
	    }

	    // advance text position
	    textx += textkeyw + 2*textpadding - 1;

	    // advance child position
	    childx += childmaxw / 2 + nodepadding;

	    if ((slot+1) * 2 == childnum)
	    {
		childx += childmaxw / 2 + nodepadding;
	    }
	    else if (slot < innernode->slotuse / 2)
	    {
		childy += childmaxh + nodepadding;
	    }
	    else
		childy -= childmaxh + nodepadding;

	    maxh = std::max(maxh, childy + childmaxh);
	}

	if (offsetx >= 0)
	{
	    // draw child
	    if (innernode->level == 1)
	    {
		// draw leaf node with border to see free slots
		dc.DrawRectangle(offsetx + childx, offsety + childy,
				 childmaxw, childmaxh);

		DrawBTreeNode(dc, offsetx + childx, offsety + childy, innernode->childid[innernode->slotuse]);
	    }
	    else
	    {
		// draw centered inner node
		DrawBTreeNode(dc,
			      offsetx + childx + std::max(0, (childmaxw - childw[innernode->slotuse]) / 2),
			      offsety + childy,
			      innernode->childid[innernode->slotuse]);
	    }

	    // calculate spline from key anchor to middle of child's box
	    wxPoint splinept[4];
	    splinept[0] = wxPoint(offsetx + textx, offsety + texty + keyboxh);
	    splinept[1] = wxPoint(offsetx + textx, offsety + texty + keyboxh + 20);

	    splinept[2] = wxPoint(offsetx + childx + childmaxw / 2, offsety + childy - 20);
	    splinept[3] = wxPoint(offsetx + childx + childmaxw / 2, offsety + childy);

	    dc.DrawSpline(4, splinept);
	}

	return wxSize(allchildw, maxh);
    }
}

void WTreeDrawing::DrawBTree(wxDC &dc)
{
    typedef WMain::btree_type btree_type;
    if (!wmain) return;

    const btree_type& bt = wmain->btree_int_4slots;

    dc.SetFont(*wxNORMAL_FONT);
    dc.SetPen(*wxBLACK_PEN);

    if (bt.tree.root)
    {
	if (bt.tree.root->level >= 2) {
	    dc.SetFont(*wxSMALL_FONT);
	}

	wxSize ts = DrawBTreeNode(dc, 0, 0, bt.tree.root);

	if (ts != oldTreeSize)
	{
	    int scrx, scry;
	    GetViewStart(&scrx, &scry);
	    SetScrollbars(10, 10, ts.GetWidth() / 10, ts.GetHeight() / 10, scrx, scry);
	    oldTreeSize = ts;
	    Refresh();
	}
    }
}

BEGIN_EVENT_TABLE(WTreeDrawing, wxScrolledWindow)


END_EVENT_TABLE()