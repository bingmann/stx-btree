/***********************************************************************\
|									|
|	B+tree function implementation			C++		|
|									|
|									|
|	Jan Jannink	created 5/27/94		revised 6/15/95		|
|									|
\***********************************************************************/

#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>
#include "b++tree.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|	Implementation Hiding Macro Definitions				|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			/* low level definition of Nptr value usage */
#ifdef POINTER
#define nAdr(b) (b)->X
#define nodearrayhead tree
#else
#define nAdr(b) tree[(b)].X
#define nodearrayhead 0
#endif

			/* access keys and pointers in a node */
#define getkey(j, q) nAdr(j).e[(q)].key
#define getnode(j, q) nAdr(j).e[(q)].downNode
#define setkey(j, q, v) (nAdr(j).e[(q)].key = (v))
#define setnode(j, q, v) (nAdr(j).e[(q)].downNode = (v))

			/* access node flag values */
#define setflag(j, v) (nAdr(j).i.info.flags |= (v))
#define clrflag(j, v) (nAdr(j).i.info.flags &= ~(v))
#define getflags(j) nAdr(j).i.info.flags
#define clearflags(j) (nAdr(j).i.info.flags = (short) MAGIC)

			/* check that a node is in fact a node */
#define isnode(j) (((j) != NONODE) && ((nAdr(j).i.info.flags & MASK) == MAGIC))
#define isntnode(j) ((j) == NONODE)

			/* test individual flag values */
#define isinternal(j) ((nAdr(j).i.info.flags & isLEAF) == 0)
#define isleaf(j) ((nAdr(j).i.info.flags & isLEAF) == isLEAF)
#define isroot(j) ((nAdr(j).i.info.flags & isROOT) == isROOT)
#define isfull(j) ((nAdr(j).i.info.flags & isFULL) == isFULL)
#define isfew(j) ((nAdr(j).i.info.flags & FEWEST) == FEWEST)

			/* manage number of keys in a node */
#define numentries(j) nAdr(j).i.info.pairs
#define clearentries(j) (nAdr(j).i.info.pairs = 0)
#define incentries(j) (nAdr(j).i.info.pairs++)
#define decentries(j) (nAdr(j).i.info.pairs--)

			/* manage first/last node pointers in internal nodes */
#define setfirstnode(j, v) (nAdr(j).i.firstNode = (v))
#define getfirstnode(j) nAdr(j).i.firstNode
#define getlastnode(j) nAdr(j).e[nAdr(j).i.info.pairs].downNode

			/* manage pointers to next nodes in leaf nodes */
#define setnextnode(j, v) (nAdr(j).l.nextNode = (v))
#define getnextnode(j) nAdr(j).l.nextNode

			/* shift/transfer entries for insertion/deletion */
#define pushentry(j, q, v) (nAdr(j).e[(q) + (v)] = nAdr(j).e[(q)])
#define pullentry(j, q, v) (nAdr(j).e[(q)] = nAdr(j).e[(q) + (v)])
#define xferentry(j, q, v, z) (nAdr(v).e[(z)] = nAdr(j).e[(q)])
#define setentry(j, q, v, z) (nAdr(j).e[(q)].key = (v), nAdr(j).e[(q)].downNode = (z))

			/* access key and data values for B+tree methods */
			/* pass values to getSlot(), descend...() */
#define getfunkey theKey
#define getfundata theData
#define setfunkey(v) (theKey = (v))
#define setfundata(v) (theData = (v))

			/* define number of B+tree nodes for free node pool */
#define getpoolsize poolsize
#define setpoolsize(v) (poolsize = (v))

			/* access memory region containing B+tree nodes */
#define getnodearray tree
#define setnodearray(v) (tree = (Node *)(v))

			/* locations from which tree access begins */
#define getroot root
#define getleaf leaf
#define setroot(v) (root = (v))
#define setleaf(v) (leaf = (v))

			/* define max/min number of pointers per node */
#define getfanout fanout
#define getminfanout(j) ((nAdr(j).i.info.flags & isLEAF) ? fanout - minfanout: minfanout)
#define setfanout(v) (fanout = (v) - 1)
#define setminfanout(v) (minfanout = (v) - 1)

			/* manage B+tree height */
#define inittreeheight (height = 0)
#define inctreeheight height++
#define dectreeheight height--
#define gettreeheight height

			/* access pool of free nodes */
#define getfirstfreenode pool
#define setfirstfreenode(v) (pool = (v))

			/* handle split/merge points during insert/delete */
#define getsplitpath branch.split
#define getmergepath branch.merge
#define setsplitpath(v) (branch.split = (v))
#define setmergepath(v) (branch.merge = (v))

			/* exploit function to compare two B+tree keys */
#define comparekeys (keycmp)
#define setcomparekeys(v) (keycmp = (v))

			/* representation independent node numbering */
#define getnodeno(v) ((v) - nodearrayhead)


/*/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|	Sample Key Comparison Function					|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int compareKeys(keyT key1, keyT key2)
{
  return key1 - key2;		/* this is the case when keys are integer */
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|	B+tree Initialization Utilities					|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~   Set up B+tree structure   ~~~~~~~~~~~~~~~~~~~~~*/
Tree::Tree(unsigned int poolsz, unsigned int fan, KeyCmp keyCmp)
{
  setfanout(fan);
  setminfanout((fan + 1) >> 1);
  initFreeNodePool(poolsz);

  setleaf(getFreeNode());		/* set up the first leaf node */
  setroot(getleaf);			/* the root is initially the leaf */
  setflag(getroot, isLEAF);
  setflag(getroot, isROOT);
  setflag(getroot, FEWEST);
  inittreeheight;

  setfunkey(0);
  setfundata("0");
  setcomparekeys(keyCmp);

#ifdef DEBUG
  cerr << "INIT:  B+tree of fanout " << fan << '.' << endl;
  showBtree();
  showNode(getroot);
#endif
}


/*~~~~~~~~~~~~~~~~~~~   Clean up B+tree structure   ~~~~~~~~~~~~~~~~~~~*/
Tree::~Tree()
{
#ifdef DEBUG
  cerr << "FREE:  B+tree at " << setw(10) << (void *) this << '.' << endl;
#endif

  delete[] getnodearray;
}


/*/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|	Find location for data						|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~~   top level search call   ~~~~~~~~~~~~~~~~~~~~~*/
Nptr
Tree::Search(keyT key)
{
  Nptr	findNode;

#ifdef DEBUG
  cerr << "SEARCH:  key " << key << '.' << endl;
#endif

  setfunkey(key);			/* set search key */
  findNode = descendToLeaf(getroot);	/* start search from root node */

#ifdef DEBUG
  cerr << "SEARCH:  found on page " << getnodeno(findNode) << '.' << endl;
#endif

  return findNode;
}

/*~~~~~~~~~~~~~~~~~~~~~   `recurse' down B+tree   ~~~~~~~~~~~~~~~~~~~~~*/
Nptr
Tree::descendToLeaf(Nptr curr)
{
  int	slot;
  Nptr	findNode;

  for (slot = getSlot(curr); isinternal(curr); slot = getSlot(curr))
    curr = getnode(curr, slot);
  if ((slot > 0) && !comparekeys(getfunkey, getkey(curr, slot)))
    findNode = curr;			/* correct key value found */
  else
    findNode = NONODE;			/* key value not in tree */

  return findNode;
}

/*~~~~~~~~~~~~~~~~~~~   find slot for search key   ~~~~~~~~~~~~~~~~~~~~*/
int
Tree::getSlot(Nptr curr)
{
  int slot, entries;

  entries = numentries(curr);		/* need this if root is ever empty */
  slot = !entries ? 0 : findKey(curr, 1, entries);

#ifdef DEBUG
  cerr << "GETSLOT:  slot " << slot << '.' << endl;
#endif

  return slot;
}


/*/*~~~~~~~~~~~~~~~~~~~   recursive binary search   ~~~~~~~~~~~~~~~~~~~~~*/
int
Tree::findKey(Nptr curr, int lo, int hi)
{
  int mid, findslot;

#ifdef DEBUG
  cerr << "GETSLOT:  lo " << lo << ", hi " << hi << '.' << endl;
  showNode(curr);
#endif

  if (hi == lo) {
    findslot = bestMatch(curr, lo);		/* recursion base case */

#ifdef DEBUG
    if (findslot == ERROR)
      cerr << "Bad key ordering on node " << getnodeno(curr) << '.' << endl;
#endif

  }
  else {
    mid = (lo + hi) >> 1;
    switch (findslot = bestMatch(curr, mid)) {
    case LOWER:				/* check lower half of range */
      findslot = findKey(curr, lo, mid - 1);		/* never in 2-3+trees */
    break;
    case UPPER:				/* check upper half of range */
      findslot = findKey(curr, mid + 1, hi);
    break;

#ifdef DEBUG
    case ERROR:
      cerr << "Bad key ordering on node " << getnodeno(curr) << '.' << endl;
#endif

    }
  }
  return findslot;
}


/*~~~~~~~~~~~   comparison of key with a target key slot   ~~~~~~~~~~~~*/
int
Tree::bestMatch(Nptr curr, int slot)
{
  int diff, comp, findslot;

  diff = comparekeys(getfunkey, getkey(curr, slot));
  if (diff < 0) {		/* also check previous slot */
    if ((slot == 1) ||
		((comp = comparekeys(getfunkey, getkey(curr, slot - 1))) >= 0))
      findslot = slot - 1;

#ifdef DEBUG
    else if (comp < diff)
      findslot = ERROR;		/* inconsistent ordering of keys */
#endif

    else
      findslot = LOWER;		/* key must be below in node ordering */
  }
  else {			/* or check following slot */
    if ((slot == numentries(curr)) ||
		((comp = comparekeys(getfunkey, getkey(curr, slot + 1))) < 0))
      findslot = slot;
    else if (comp == 0)
      findslot = slot + 1;

#ifdef DEBUG
    else if (comp > diff)
      findslot = ERROR;		/* inconsistent ordering of keys */
#endif

    else
      findslot = UPPER;		/* key must be above in node ordering */
  }
  return findslot;
}


/*/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|	Insert new data into tree					|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~~   top level insert call   ~~~~~~~~~~~~~~~~~~~~~*/
void
Tree::Insert(keyT key)
{
  Nptr newNode;

#ifdef DEBUG
  cerr << "INSERT:  key " << key << '.' << endl;
#endif

  setfunkey(key);			/* set insertion key */
  setfundata("data");			/* a node containing data */
  setsplitpath(NONODE);
  newNode = descendSplit(getroot);	/* insertion point search from root */
  if (newNode != getsplitpath)		/* indicates the root node has split */
    makeNewRoot(getroot, newNode);
}


/*~~~~~~~~~~~~~~~~   recurse down and split back up   ~~~~~~~~~~~~~~~~~*/
Nptr
Tree::descendSplit(Nptr curr)
{
  Nptr	newMe, newNode;
  int	slot;

  if (!isfull(curr))
    setsplitpath(NONODE);
  else if (getsplitpath == NONODE)
    setsplitpath(curr);			/* indicates where nodes must split */

  slot = getSlot(curr);			/* is null only if the root is empty */
  if (isinternal(curr))			/* continue recursion to leaves */
    newMe = descendSplit(getnode(curr, slot));
  else if ((slot > 0) && !comparekeys(getfunkey, getkey(curr, slot))) {
    newMe = NONODE;			/* this code discards duplicates */
    setsplitpath(NONODE);
  }
  else
    newMe = getDataNode(getfunkey);	/* an insertion takes place */

  newNode = NONODE;			/* assume no node splitting necessary */

  if (newMe != NONODE) {		/* insert only where necessary */
    if (getsplitpath != NONODE)
      newNode = split(curr);		/* a sibling node is prepared */
    insertEntry(curr, slot, newNode, newMe);
  }

  return newNode;
}


/*/*~~~~~~~~~~~~~~   determine location of inserted key   ~~~~~~~~~~~~~~~*/
void
Tree::insertEntry(Nptr newNode, int slot, Nptr sibling, Nptr downPtr)
{
  int split, i, j, k, x, y;

  if (sibling == NONODE) {		/* no split occurred */
    placeEntry(newNode, slot + 1, downPtr);
    clrflag(newNode, FEWEST);
  }
  else {				/* split entries between the two */
    i = isinternal(newNode);		/* adjustment values */
    split = i ? getfanout - getminfanout(newNode): getminfanout(newNode);
    j = (slot != split);
    k = (slot >= split);

    for (x = split + k + j * i, y = 1; x <= getfanout; x++, y++) {
      xferentry(newNode, x, sibling, y);	/* copy entries to sibling */
      decentries(newNode);
      incentries(sibling);
    }
    if (numentries(sibling) == getfanout)
      setflag(sibling, isFULL);		/* only ever happens in 2-3+trees */

    if (i) {				/* set first pointer of internal node */
      if (j) {
        setfirstnode(sibling, getnode(newNode, split + k));
        decentries(newNode);
      }
      else
        setfirstnode(sibling, downPtr);
    }

    if (j) {				/* insert new entry into correct spot */
      x = getkey(newNode, split + k);
      if (k)
	placeEntry(sibling, slot - split + 1 - i, downPtr);
      else
	placeEntry(newNode, slot + 1, downPtr);
      setfunkey(x);			/* set key separating nodes */
    }
    else if (!i)
      placeEntry(sibling, 1, downPtr);

    clrflag(newNode, isFULL);		/* adjust node flags */
    if (numentries(newNode) == getminfanout(newNode))
      setflag(newNode, FEWEST);
    if (numentries(sibling) > getminfanout(sibling))
      clrflag(sibling, FEWEST);

#ifdef DEBUG
  cerr << "INSERT:  slot " << slot << ", node " << getnodeno(downPtr)
								<< '.' << endl;
  showNode(newNode);
  showNode(sibling);
#endif

  }
}

/*/*~~~~~~~~~~~   place key into appropriate node & slot   ~~~~~~~~~~~~~~*/
void
Tree::placeEntry(Nptr newNode, int slot, Nptr downPtr)
{
  int x;

  for (x = numentries(newNode); x >= slot; x--)	/* make room for new entry */
    pushentry(newNode, x, 1);
  setentry(newNode, slot, getfunkey, downPtr);	/* place it in correct slot */

  incentries(newNode);				/* adjust entry counter */
  if (numentries(newNode) == getfanout)
    setflag(newNode, isFULL);
}


/*~~~~~~~~~~~~~~~~   split full node and set flags   ~~~~~~~~~~~~~~~~~~*/
Nptr
Tree::split(Nptr newNode)
{
  Nptr sibling;

  sibling = getFreeNode();

  setflag(sibling, FEWEST);			/* set up node flags */

  if (isleaf(newNode)) {
    setflag(sibling, isLEAF);
    setnextnode(sibling, getnextnode(newNode));	/* adjust leaf pointers */
    setnextnode(newNode, sibling);
  }
  if (getsplitpath == newNode)
    setsplitpath(NONODE);			/* no more splitting needed */

  return sibling;
}


/*~~~~~~~~~~~~~~~~~~~~~   build new root node   ~~~~~~~~~~~~~~~~~~~~~~~*/
void
Tree::makeNewRoot(Nptr oldRoot, Nptr newNode)
{
  setroot(getFreeNode());

  setfirstnode(getroot, oldRoot);	/* old root becomes new root's child */
  setentry(getroot, 1, getfunkey, newNode);	/* old root's sibling also */
  incentries(getroot);

  clrflag(oldRoot, isROOT);
  setflag(getroot, isROOT);
  setflag(getroot, FEWEST);
  inctreeheight;
}


/*/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|	Delete data from tree						|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~~   top level delete call   ~~~~~~~~~~~~~~~~~~~~~*\
|
|	The recursive call for deletion carries 5 additional parameters
|	which may be needed to rebalance the B+tree when removing the key.
|	These parameters are:
|		1. immediate left neighbor of the current node
|		2. immediate right neighbor of the current node
|		3. the anchor of the current node and left neighbor
|		4. the anchor of the current node and right neighbor
|		5. the parent of the current node
|
|	All of these parameters are simple to calculate going along the
|	recursive path to the leaf nodes and the point of key deletion.
|	At that time, the algorithm determines which node manipulations
|	are most efficient, that is, cause the least rearranging of data,
|	and minimize the need for non-local key manipulation.
|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void
Tree::Delete(keyT key)
{
  Nptr newNode;

#ifdef DEBUG
  cerr << "DELETE:  key " << key << '.' << endl;
#endif

  setfunkey(key);			/* set deletion key */
  setmergepath(NONODE);
  newNode = descendBalance(getroot, NONODE, NONODE, NONODE, NONODE, NONODE);
  if (isnode(newNode))
    collapseRoot(getroot, newNode);	/* remove root when superfluous */
}


/*~~~~~~~~~~~~~~~~~~~~~   remove old root node   ~~~~~~~~~~~~~~~~~~~~~~*/
void
Tree::collapseRoot(Nptr oldRoot, Nptr newRoot)
{

#ifdef DEBUG
  cerr << "COLLAPSE:  old " << getnodeno(oldRoot);
  cerr << ", new " << getnodeno(newRoot) << '.' << endl;
  showNode(oldRoot);
  showNode(newRoot);
#endif

  setroot(newRoot);
  setflag(newRoot, isROOT);
  putFreeNode(oldRoot);
  dectreeheight;			/* the height of the tree decreases */
}


/*/*~~~~~~~~~~~~~~~   recurse down and balance back up   ~~~~~~~~~~~~~~~~*/
Nptr
Tree::descendBalance(Nptr curr, Nptr left, Nptr right, Nptr lAnc, Nptr rAnc, Nptr parent)
{
  Nptr	newMe, myLeft, myRight, lAnchor, rAnchor, newNode;
  int	slot, notleft, notright, fewleft, fewright, test;

  if (!isfew(curr))
    setmergepath(NONODE);
  else if (getmergepath == NONODE)
    setmergepath(curr);		/* mark which nodes may need rebalancing */

  slot = getSlot(curr);
  newNode = getnode(curr, slot);
  if (isinternal(curr)) {	/* set up next recursion call's parameters */
    if (slot == 0) {
      myLeft = isntnode(left) ? NONODE : getlastnode(left);
      lAnchor = lAnc;
    }
    else {
      myLeft = getnode(curr, slot - 1);
      lAnchor = curr;
    }
    if (slot == numentries(curr)) {
      myRight = isntnode(right) ? NONODE : getfirstnode(right);
      rAnchor = rAnc;
    }
    else {
      myRight = getnode(curr, slot + 1);
      rAnchor = curr;
    }
    newMe = descendBalance(newNode, myLeft, myRight, lAnchor, rAnchor, curr);
  }
  else if ((slot > 0) && !comparekeys(getfunkey, getkey(curr, slot)))
    newMe = newNode;		/* a key to be deleted is found */
  else {
    newMe = NONODE;		/* no deletion possible, key not found */
    setmergepath(NONODE);
  }

/*~~~~~~~~~~~~~~~~   rebalancing tree after deletion   ~~~~~~~~~~~~~~~~*\
|
|	The simplest B+tree rebalancing consists of the following rules.
|
|	If a node underflows:
|	CASE 1 check if it is the root, and collapse it if it is,
|	CASE 2 otherwise, check if both of its neighbors are minimum
|	    sized and merge the underflowing node with one of them,
|	CASE 3 otherwise shift surplus entries to the underflowing node.
|
|	The choice of which neighbor to use is optional.  However, the
|	rebalancing rules that follow also ensure whenever possible
|	that the merges and shifts which do occur use a neighbor whose
|	anchor is the parent of the underflowing node.
|
|	Cases 3, 4, 5 below are more an optimization than a requirement,
|	and can be omitted, with a change of the action value in case 6,
|	which actually corresponds to the third case described above.
|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*			/* begin deletion, working upwards from leaves */

  if (newMe != NONODE)	/* this node removal doesn't consider duplicates */
    removeEntry(curr, slot + (newMe != newNode));	/* removes one of two */

#ifdef DEBUG
  cerr << "DELETE:  slot " << slot << ", node " << getnodeno(newMe)
								<< '.' << endl;
  showNode(curr);
#endif

  if (getmergepath == NONODE)
    newNode = NONODE;
  else {		/* tree rebalancing rules for node merges and shifts */
    notleft = isntnode(left);
    notright = isntnode(right);
    fewleft = isfew(left);		/* only used when defined */
    fewright = isfew(right);

			/* CASE 1:  prepare root node (curr) for removal */
    if (notleft && notright) {
      test = isleaf(curr);		/* check if B+tree has become empty */
      newNode = test ? NONODE : getfirstnode(curr);
    }
			/* CASE 2:  the merging of two nodes is a must */
    else if ((notleft || fewleft) && (notright || fewright)) {
      test = !(lAnc == parent);
      newNode = test ? merge(curr, right, rAnc) : merge(left, curr, lAnc);
    }
			/* CASE 3: choose the better of a merge or a shift */
    else if (!notleft && fewleft && !notright && !fewright) {
      test = !(rAnc == parent) && (curr == getmergepath);
      newNode = test ? merge(left, curr, lAnc) : shift(curr, right, rAnc);
    }
			/* CASE 4: also choose between a merge or a shift */
    else if (!notleft && !fewleft && !notright && fewright) {
      test = !(lAnc == parent) && (curr == getmergepath);
      newNode = test ? merge(curr, right, rAnc) : shift(left, curr, lAnc);
    }
			/* CASE 5: choose the more effective of two shifts */
    else if (lAnc == rAnc) { 		/* => both anchors are the parent */
      test = (numentries(left) <= numentries(right));
      newNode = test ? shift(curr, right, rAnc) : shift(left, curr, lAnc);
    }
			/* CASE 6: choose the shift with more local effect */
    else {				/* if omitting cases 3,4,5 use below */
      test = (lAnc == parent);		/* test = (!notleft && !fewleft); */
      newNode = test ? shift(left, curr, lAnc) : shift(curr, right, rAnc);
    }
  }

  return newNode;
}


/*/*~~~~~~~~~~~~~~~   remove key and pointer from node   ~~~~~~~~~~~~~~~~*/
void
Tree::removeEntry(Nptr curr, int slot)
{
  int x;

  putFreeNode(getnode(curr, slot));	/* return deleted node to free list */
  for (x = slot; x < numentries(curr); x++)
    pullentry(curr, x, 1);		/* adjust node with removed key */
  decentries(curr);
  clrflag(curr, isFULL);		/* keep flag information up to date */
  if (isroot(curr)) {
    if (numentries(curr) == 1)
      setflag(curr, FEWEST);
  }
  else if (numentries(curr) == getminfanout(curr))
    setflag(curr, FEWEST);
}


/*~~~~~~   merge a node pair & set emptied node up for removal   ~~~~~~*/
Nptr
Tree::merge(Nptr left, Nptr right, Nptr anchor)
{
  int	x, y, z;

#ifdef DEBUG
  cerr << "MERGE:  left " << getnodeno(left);
  cerr << ", right " << getnodeno(right) << '.' << endl;
  showNode(left);
  showNode(right);
#endif

  if (isinternal(left)) {
    incentries(left);			/* copy key separating the nodes */
    setfunkey(getkey(right, 1));	/* defined but maybe just deleted */
    z = getSlot(anchor);		/* needs the just calculated key */
    setfunkey(getkey(anchor, z));	/* set slot to delete in anchor */
    setentry(left, numentries(left), getfunkey, getfirstnode(right));
  }
  else
    setnextnode(left, getnextnode(right));
  for (x = numentries(left) + 1, y = 1; y <= numentries(right); x++, y++) {
    incentries(left);
    xferentry(right, y, left, x);	/* transfer entries to left node */
  }
  if (numentries(left) > getminfanout(left))
    clrflag(left, FEWEST);
  if (numentries(left) == getfanout)
    setflag(left, isFULL);

  if (getmergepath == left || getmergepath == right)
    setmergepath(NONODE);		/* indicate rebalancing is complete */

  return right;
}


/*/*~~~~~   shift entries in a node pair & adjust anchor key value   ~~~~*/
Nptr
Tree::shift(Nptr left, Nptr right, Nptr anchor)
{
  int	i, x, y, z;

#ifdef DEBUG
  cerr << "SHIFT:  left " << getnodeno(left);
  cerr << ", right " << getnodeno(right) << '.' << endl;
  showNode(left);
  showNode(right);
#endif

  i = isinternal(left);
  if (numentries(left) < numentries(right)) {	/* shift entries to left */
    y = (numentries(right) - numentries(left)) >> 1;
    x = numentries(left) + y;
    setfunkey(getkey(right, y + 1 - i));	/* set new anchor key value */
    z = getSlot(anchor);			/* find slot in anchor node */
    if (i) {					/* move out old anchor value */
      decentries(right);			/* adjust for shifting anchor */
      incentries(left);
      setentry(left, numentries(left), getkey(anchor, z), getfirstnode(right));
      setfirstnode(right, getnode(right, y + 1 - i));
    }
    clrflag(right, isFULL);
    setkey(anchor, z, getfunkey);		/* set new anchor value */
    for (z = y, y -= i; y > 0; y--, x--) {
      decentries(right);			/* adjust entry count */
      incentries(left);
      xferentry(right, y, left, x);		/* transfer entries over */
    }

    for (x = 1; x <= numentries(right); x++)	/* adjust reduced node */
      pullentry(right, x, z);
  }
  else {					/* shift entries to right */
    y = (numentries(left) - numentries(right)) >> 1;
    x = numentries(left) - y + 1;

    for (z = numentries(right); z > 0; z--)	/* adjust increased node */
      pushentry(right, z, y);

    setfunkey(getkey(left, x));			/* set new anchor key value */
    z = getSlot(anchor) + 1;
    if (i) {
      decentries(left);
      incentries(right);
      setentry(right, y, getkey(anchor, z), getfirstnode(right));
      setfirstnode(right, getnode(left, x));
    }
    clrflag(left, isFULL);
    setkey(anchor, z, getfunkey);
    for (x = numentries(left) + i, y -= i; y > 0; y--, x--) {
      decentries(left);
      incentries(right);
      xferentry(left, x, right, y);		/* transfer entries over */
    }
  }
  if (numentries(left) == getminfanout(left))	/* adjust node flags */
    setflag(left, FEWEST);
  else
    clrflag(left, FEWEST);			/* never happens in 2-3+trees */
  if (numentries(right) == getminfanout(right))
    setflag(right, FEWEST);
  else
    clrflag(right, FEWEST);			/* never happens in 2-3+trees */
  setmergepath(NONODE);

#ifdef DEBUG
  showNode(left);
  showNode(right);
#endif

  return NONODE;
}


/*/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|	Empty Node Utilities						|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~   Set up pool of free nodes   ~~~~~~~~~~~~~~~~~~*/
void
Tree::initFreeNodePool(int quantity)
{
  int	i;
  Nptr	n;

  setpoolsize(quantity);
  setnodearray(new Node [quantity]);	/* node memory block */
  setfirstfreenode(nodearrayhead);	/* start a list of free nodes */
  for (n = getfirstfreenode, i = 0; i < quantity; n++, i++) {
    clearflags(n);
    clearentries(n);
    setnextnode(n, n + 1);		/* insert node into free node list */
  }
  setnextnode(--n, NONODE);		/* indicates end of free node list */
}


/*~~~~~~~~~~~~~   take a free B+tree node from the pool   ~~~~~~~~~~~~~*/
Nptr
Tree::getFreeNode()
{
  Nptr newNode = getfirstfreenode;

  if (newNode != NONODE) {
    setfirstfreenode(getnextnode(newNode));	/* adjust free node list */
    setnextnode(newNode, NONODE);		/* remove node from list */
  }
  else {
    cerr << "Out of tree nodes." << endl;	/* can't recover from this */
    exit(0);
  }
  return newNode;
}


/*~~~~~~~~~~~~   return a deleted B+tree node to the pool   ~~~~~~~~~~~*/
void
Tree::putFreeNode(Nptr self)
{
  clearflags(self);
  clearentries(self);
  setnextnode(self, getfirstfreenode);		/* add node to list */
  setfirstfreenode(self);			/* set it to be list head */
}


/*~~~~~~   fill a free data node with a key and associated data   ~~~~~*/
Nptr
Tree::getDataNode(keyT key)			/* can add data parameter */
{
  Nptr	newNode = getFreeNode();
  keyT	*value;

  value = (keyT *) &nAdr(newNode).d;
  *value = key;					/* can add code to fill node */

  return newNode;
}


/*/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|	B+tree Printing Utilities					|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef DEBUG
/*~~~~~~~~~~~~~~~~~~~~~~   B+tree node printer   ~~~~~~~~~~~~~~~~~~~~~~*/
void
Tree::showNode(Nptr n)
{
  int x;

  cerr << "-  --  --  --  --  --  --  --  --  --  --  --  -" << endl;
  cerr << "| node " << setw(6) << getnodeno(n) << "                 ";
  cerr << "  magic    " << hex << (getflags(n) & MASK) << "  |" << dec << endl;
  cerr << "-  --  --  --  --  --  --  --  --  --  --  --  -" << endl;
  cerr << "| flags   " << isfew(n) << isfull(n) << isroot(n) << isleaf(n) << ' ';
  cerr << "| keys = " << setw(5) << numentries(n) << ' ';
  cerr << "| node = " << setw(6) << getnodeno(getfirstnode(n)) << "  |" << endl;
  for (x = 1; x <= numentries(n); x++) {
    cerr << "| entry " << setw(6) << x << ' ';
    cerr << "| key = " << setw(6) << (getkey(n, x) & 0xFFFF) << ' ';
    cerr << "| node = " << setw(6) << getnodeno(getnode(n, x)) << "  |" << endl;
  }
  cerr << "-  --  --  --  --  --  --  --  --  --  --  --  -" << endl;
}

/*~~~~~~~~~~~~~~~~~   B+tree class variable printer   ~~~~~~~~~~~~~~~~~*/
void
Tree::showBtree()
{
  cerr << "-  --  --  --  --  --  -" << endl;
  cerr << "|  B+tree  " << setw(10) << (void *) this << "  |" << endl;
  cerr << "-  --  --  --  --  --  -" << endl;
  cerr << "|  root        " << setw(6) << getnodeno(getroot) << "  |" << endl;
  cerr << "|  leaf        " << setw(6) << getnodeno(getleaf) << "  |" << endl;
  cerr << "|  fanout         " << setw(3) << getfanout + 1 << "  |" << endl;
  cerr << "|  minfanout      " << setw(3) << getminfanout(getroot) + 1 << "  |" << endl;
  cerr << "|  height         " << setw(3) << gettreeheight << "  |" << endl;
  cerr << "|  freenode    " << setw(6) << getnodeno(getfirstfreenode) << "  |" << endl;
  cerr << "|  theKey      " << setw(6) << getfunkey << "  |" << endl;
  cerr << "|  theData     " << setw(6) << getfundata << "  |" << endl;
  cerr << "-  --  --  --  --  --  -" << endl;
}
#endif

/*~~~~~~~~~~~~~~~~~~~~~~   B+tree data printer   ~~~~~~~~~~~~~~~~~~~~~~*/
void
Tree::listBtreeValues(Nptr n, int num)
{
  int slot, prev = -1;

  for (slot = 1; (n != NONODE) && num && numentries(n); num--) {
    if (getkey(n, slot) <= prev) cerr << "BOMB";
    prev = getkey(n, slot);
    cerr << setw(8) << prev << (num & 7 ? ' ' : '\n');
    if (++slot > numentries(n))
      n = getnextnode(n), slot = 1;
  }
  cerr << '\n' << endl;
}

/*~~~~~~~~~~~~~~~~~~~   entire B+tree data printer   ~~~~~~~~~~~~~~~~~~*/
void
Tree::listAllBtreeValues()
{
  listBtreeValues(getleaf, ERROR);
}
