// $Id$

#ifndef _WMain_H_
#define _WMain_H_

#include "WMain_wxg.h"

#include "WTreeDrawing.h"

struct BTreeBundle;

#define BTREE_FRIENDS	friend struct ::WTreeDrawing::BTreeOp_Draw; \
                        friend struct ::BTreeBundle;

#include <stx/btree_map.h>
#include <stx/btree_multimap.h>
#include <string>

/// The demo allows many different B+ trees to be used. All these must be
/// instantiated and correctly switched using this tree bundling class.
class BTreeBundle
{
public:

    /// Traits structure for the enclosed B+ tree instances.
    template <int Slots>
    struct btree_traits_nodebug
    {
	static const bool	selfverify = true;
	static const bool	debug = false;

	static const int 	leafslots = Slots;
	static const int	innerslots = Slots;
    };

    // *** Many many instantiations of the B+ tree classes

    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<4> >		btmap_int_4_slots;
    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<5> >		btmap_int_5_slots;
    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<6> >		btmap_int_6_slots;
    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<7> >		btmap_int_7_slots;
    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<8> >		btmap_int_8_slots;
    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<9> >		btmap_int_9_slots;
    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<10> >		btmap_int_10_slots;
    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<11> >		btmap_int_11_slots;
    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<12> >		btmap_int_12_slots;
    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<13> >		btmap_int_13_slots;
    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<14> >		btmap_int_14_slots;
    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<15> >		btmap_int_15_slots;
    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<16> >		btmap_int_16_slots;
    stx::btree_map<int, int, std::less<int>, btree_traits_nodebug<32> >		btmap_int_32_slots;

    stx::btree_map<wxString, wxString, std::less<wxString>, btree_traits_nodebug<4> >		btmap_string_4_slots;
    stx::btree_map<wxString, wxString, std::less<wxString>, btree_traits_nodebug<5> >		btmap_string_5_slots;
    stx::btree_map<wxString, wxString, std::less<wxString>, btree_traits_nodebug<6> >		btmap_string_6_slots;
    stx::btree_map<wxString, wxString, std::less<wxString>, btree_traits_nodebug<7> >		btmap_string_7_slots;
    stx::btree_map<wxString, wxString, std::less<wxString>, btree_traits_nodebug<8> >		btmap_string_8_slots;
    stx::btree_map<wxString, wxString, std::less<wxString>, btree_traits_nodebug<9> >		btmap_string_9_slots;
    stx::btree_map<wxString, wxString, std::less<wxString>, btree_traits_nodebug<10> >		btmap_string_10_slots;
    stx::btree_map<wxString, wxString, std::less<wxString>, btree_traits_nodebug<11> >		btmap_string_11_slots;
    stx::btree_map<wxString, wxString, std::less<wxString>, btree_traits_nodebug<12> >		btmap_string_12_slots;
    stx::btree_map<wxString, wxString, std::less<wxString>, btree_traits_nodebug<13> >		btmap_string_13_slots;
    stx::btree_map<wxString, wxString, std::less<wxString>, btree_traits_nodebug<14> >		btmap_string_14_slots;
    stx::btree_map<wxString, wxString, std::less<wxString>, btree_traits_nodebug<15> >		btmap_string_15_slots;
    stx::btree_map<wxString, wxString, std::less<wxString>, btree_traits_nodebug<16> >		btmap_string_16_slots;
    stx::btree_map<wxString, wxString, std::less<wxString>, btree_traits_nodebug<32> >		btmap_string_32_slots;

    stx::btree_multimap<int, int, std::less<int>, btree_traits_nodebug<4> >		btmultimap_int_4_slots;
    stx::btree_multimap<int, int, std::less<int>, btree_traits_nodebug<5> >		btmultimap_int_5_slots;
    stx::btree_multimap<int, int, std::less<int>, btree_traits_nodebug<6> >		btmultimap_int_6_slots;
    stx::btree_multimap<int, int, std::less<int>, btree_traits_nodebug<7> >		btmultimap_int_7_slots;
    stx::btree_multimap<int, int, std::less<int>, btree_traits_nodebug<8> >		btmultimap_int_8_slots;
    stx::btree_multimap<int, int, std::less<int>, btree_traits_nodebug<9> >		btmultimap_int_9_slots;
    stx::btree_multimap<int, int, std::less<int>, btree_traits_nodebug<10> >		btmultimap_int_10_slots;
    stx::btree_multimap<int, int, std::less<int>, btree_traits_nodebug<11> >		btmultimap_int_11_slots;
    stx::btree_multimap<int, int, std::less<int>, btree_traits_nodebug<12> >		btmultimap_int_12_slots;
    stx::btree_multimap<int, int, std::less<int>, btree_traits_nodebug<13> >		btmultimap_int_13_slots;
    stx::btree_multimap<int, int, std::less<int>, btree_traits_nodebug<14> >		btmultimap_int_14_slots;
    stx::btree_multimap<int, int, std::less<int>, btree_traits_nodebug<15> >		btmultimap_int_15_slots;
    stx::btree_multimap<int, int, std::less<int>, btree_traits_nodebug<16> >		btmultimap_int_16_slots;
    stx::btree_multimap<int, int, std::less<int>, btree_traits_nodebug<32> >		btmultimap_int_32_slots;

    stx::btree_multimap<wxString, wxString, std::less<wxString>, btree_traits_nodebug<4> >		btmultimap_string_4_slots;
    stx::btree_multimap<wxString, wxString, std::less<wxString>, btree_traits_nodebug<5> >		btmultimap_string_5_slots;
    stx::btree_multimap<wxString, wxString, std::less<wxString>, btree_traits_nodebug<6> >		btmultimap_string_6_slots;
    stx::btree_multimap<wxString, wxString, std::less<wxString>, btree_traits_nodebug<7> >		btmultimap_string_7_slots;
    stx::btree_multimap<wxString, wxString, std::less<wxString>, btree_traits_nodebug<8> >		btmultimap_string_8_slots;
    stx::btree_multimap<wxString, wxString, std::less<wxString>, btree_traits_nodebug<9> >		btmultimap_string_9_slots;
    stx::btree_multimap<wxString, wxString, std::less<wxString>, btree_traits_nodebug<10> >		btmultimap_string_10_slots;
    stx::btree_multimap<wxString, wxString, std::less<wxString>, btree_traits_nodebug<11> >		btmultimap_string_11_slots;
    stx::btree_multimap<wxString, wxString, std::less<wxString>, btree_traits_nodebug<12> >		btmultimap_string_12_slots;
    stx::btree_multimap<wxString, wxString, std::less<wxString>, btree_traits_nodebug<13> >		btmultimap_string_13_slots;
    stx::btree_multimap<wxString, wxString, std::less<wxString>, btree_traits_nodebug<14> >		btmultimap_string_14_slots;
    stx::btree_multimap<wxString, wxString, std::less<wxString>, btree_traits_nodebug<15> >		btmultimap_string_15_slots;
    stx::btree_multimap<wxString, wxString, std::less<wxString>, btree_traits_nodebug<16> >		btmultimap_string_16_slots;
    stx::btree_multimap<wxString, wxString, std::less<wxString>, btree_traits_nodebug<32> >		btmultimap_string_32_slots;

    /// Selects the active tree: type == 0 -> integer, type == 1 -> string.
    int		selected_type;

    /// Selects the active tree: slots is the number of slots.
    int		selected_slots;

    /// Selects the active tree: map or mulitmap.
    bool	selected_multimap;

    /// Test if this is a integer tree
    inline bool	isIntegerType() const
    {
	return (selected_type == 0);
    }

    /// Test if this is a string tree
    inline bool	isStringType() const
    {
	return (selected_type == 1);
    }

    /// Test if the selected tree allows duplicates
    inline bool isMultimap() const
    {
	return selected_multimap;
    }

    template <class Operation>
    typename Operation::result_type	run(Operation operation)
    {
	if (isIntegerType() && !isMultimap())
	{
	    if (selected_slots == 4) {
		return operation.opInteger(btmap_int_4_slots);
	    }
	    else if (selected_slots == 5) {
		return operation.opInteger(btmap_int_5_slots);
	    }
	    else if (selected_slots == 6) {
		return operation.opInteger(btmap_int_6_slots);
	    }
	    else if (selected_slots == 7) {
		return operation.opInteger(btmap_int_7_slots);
	    }
	    else if (selected_slots == 8) {
		return operation.opInteger(btmap_int_8_slots);
	    }
	    else if (selected_slots == 9) {
		return operation.opInteger(btmap_int_9_slots);
	    }
	    else if (selected_slots == 10) {
		return operation.opInteger(btmap_int_10_slots);
	    }
	    else if (selected_slots == 11) {
		return operation.opInteger(btmap_int_11_slots);
	    }
	    else if (selected_slots == 12) {
		return operation.opInteger(btmap_int_12_slots);
	    }
	    else if (selected_slots == 13) {
		return operation.opInteger(btmap_int_13_slots);
	    }
	    else if (selected_slots == 14) {
		return operation.opInteger(btmap_int_14_slots);
	    }
	    else if (selected_slots == 15) {
		return operation.opInteger(btmap_int_15_slots);
	    }
	    else if (selected_slots == 16) {
		return operation.opInteger(btmap_int_16_slots);
	    }
	    else if (selected_slots == 32) {
		return operation.opInteger(btmap_int_32_slots);
	    }
	}
	else if (isStringType() && !isMultimap())
	{
	    if (selected_slots == 4) {
		return operation.opString(btmap_string_4_slots);
	    }
	    else if (selected_slots == 5) {
		return operation.opString(btmap_string_5_slots);
	    }
	    else if (selected_slots == 6) {
		return operation.opString(btmap_string_6_slots);
	    }
	    else if (selected_slots == 7) {
		return operation.opString(btmap_string_7_slots);
	    }
	    else if (selected_slots == 8) {
		return operation.opString(btmap_string_8_slots);
	    }
	    else if (selected_slots == 9) {
		return operation.opString(btmap_string_9_slots);
	    }
	    else if (selected_slots == 10) {
		return operation.opString(btmap_string_10_slots);
	    }
	    else if (selected_slots == 11) {
		return operation.opString(btmap_string_11_slots);
	    }
	    else if (selected_slots == 12) {
		return operation.opString(btmap_string_12_slots);
	    }
	    else if (selected_slots == 13) {
		return operation.opString(btmap_string_13_slots);
	    }
	    else if (selected_slots == 14) {
		return operation.opString(btmap_string_14_slots);
	    }
	    else if (selected_slots == 15) {
		return operation.opString(btmap_string_15_slots);
	    }
	    else if (selected_slots == 16) {
		return operation.opString(btmap_string_16_slots);
	    }
	    else if (selected_slots == 32) {
		return operation.opString(btmap_string_32_slots);
	    }
	}
	else if (isIntegerType() && isMultimap())
	{
	    if (selected_slots == 4) {
		return operation.opIntegerMulti(btmultimap_int_4_slots);
	    }
	    else if (selected_slots == 5) {
		return operation.opIntegerMulti(btmultimap_int_5_slots);
	    }
	    else if (selected_slots == 6) {
		return operation.opIntegerMulti(btmultimap_int_6_slots);
	    }
	    else if (selected_slots == 7) {
		return operation.opIntegerMulti(btmultimap_int_7_slots);
	    }
	    else if (selected_slots == 8) {
		return operation.opIntegerMulti(btmultimap_int_8_slots);
	    }
	    else if (selected_slots == 9) {
		return operation.opIntegerMulti(btmultimap_int_9_slots);
	    }
	    else if (selected_slots == 10) {
		return operation.opIntegerMulti(btmultimap_int_10_slots);
	    }
	    else if (selected_slots == 11) {
		return operation.opIntegerMulti(btmultimap_int_11_slots);
	    }
	    else if (selected_slots == 12) {
		return operation.opIntegerMulti(btmultimap_int_12_slots);
	    }
	    else if (selected_slots == 13) {
		return operation.opIntegerMulti(btmultimap_int_13_slots);
	    }
	    else if (selected_slots == 14) {
		return operation.opIntegerMulti(btmultimap_int_14_slots);
	    }
	    else if (selected_slots == 15) {
		return operation.opIntegerMulti(btmultimap_int_15_slots);
	    }
	    else if (selected_slots == 16) {
		return operation.opIntegerMulti(btmultimap_int_16_slots);
	    }
	    else if (selected_slots == 32) {
		return operation.opIntegerMulti(btmultimap_int_32_slots);
	    }
	}
	else if (isStringType() && isMultimap())
	{
	    if (selected_slots == 4) {
		return operation.opStringMulti(btmultimap_string_4_slots);
	    }
	    else if (selected_slots == 5) {
		return operation.opStringMulti(btmultimap_string_5_slots);
	    }
	    else if (selected_slots == 6) {
		return operation.opStringMulti(btmultimap_string_6_slots);
	    }
	    else if (selected_slots == 7) {
		return operation.opStringMulti(btmultimap_string_7_slots);
	    }
	    else if (selected_slots == 8) {
		return operation.opStringMulti(btmultimap_string_8_slots);
	    }
	    else if (selected_slots == 9) {
		return operation.opStringMulti(btmultimap_string_9_slots);
	    }
	    else if (selected_slots == 10) {
		return operation.opStringMulti(btmultimap_string_10_slots);
	    }
	    else if (selected_slots == 11) {
		return operation.opStringMulti(btmultimap_string_11_slots);
	    }
	    else if (selected_slots == 12) {
		return operation.opStringMulti(btmultimap_string_12_slots);
	    }
	    else if (selected_slots == 13) {
		return operation.opStringMulti(btmultimap_string_13_slots);
	    }
	    else if (selected_slots == 14) {
		return operation.opStringMulti(btmultimap_string_14_slots);
	    }
	    else if (selected_slots == 15) {
		return operation.opStringMulti(btmultimap_string_15_slots);
	    }
	    else if (selected_slots == 16) {
		return operation.opStringMulti(btmultimap_string_16_slots);
	    }
	    else if (selected_slots == 32) {
		return operation.opStringMulti(btmultimap_string_32_slots);
	    }
	}

	throw(wxT("Program Error: could not find selected B+ tree"));
    }

    // *** Marked Node Slots

    /// node pointer of the first mark
    const void*	mark1_node;
    /// slot number of the first mark
    int		mark1_slot;

    /// node pointer of the second mark
    const void*	mark2_node;
    /// slot number of the second mark
    int		mark2_slot;

    /// Clear both marks
    inline void clearMarks()
    {
	mark1_node = 0;
	mark1_slot = 0;
	mark2_node = 0;
	mark2_slot = 0;
    }

    /// Set the first mark, clear the second
    template <class BTreeIter>
    inline void setMark1(const BTreeIter &iter)
    {
	mark1_node = iter.currnode;
	mark1_slot = iter.currslot;
	mark2_node = 0;
	mark2_slot = 0;
    }

    /// Set the second mark
    template <class BTreeIter>
    inline void setMark2(const BTreeIter &iter)
    {
	mark2_node = iter.currnode;
	mark2_slot = iter.currslot;
    }

    /// Compare to the first mark
    inline bool isMark1(const void* node, int slot) const
    {
	return (mark1_node == node) && (mark1_slot == slot);
    }

    /// Compare to the second mark
    inline bool isMark2(const void* node, int slot) const
    {
	return (mark2_node == node) && (mark2_slot == slot);
    }
};

/** Main Window class */
class WMain : public WMain_wxg
{
public:
    WMain();

    class BTreeBundle	treebundle;

    void	OnClose(wxCloseEvent &ce);

    void	OnChoiceDataType(wxCommandEvent &ce);
    void	OnChoiceNodeSlots(wxCommandEvent &ce);
    void	OnCheckboxDuplicates(wxCommandEvent &ce);

    void	OnButtonInsert(wxCommandEvent &ce);
    void	OnButtonErase(wxCommandEvent &ce);
    void	OnButtonInsertRandom(wxCommandEvent &ce);
    void	OnButtonFindKey(wxCommandEvent &ce);
    void	OnButtonEqualRange(wxCommandEvent &ce);
    void	OnButtonClear(wxCommandEvent &ce);

    void	OnMenuInsertRandom(wxCommandEvent &ce);

    void	UpdateViews();

    DECLARE_EVENT_TABLE();
};

#endif // _WMain_H_
