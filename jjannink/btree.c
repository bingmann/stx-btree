/***********************************************************************\
|									|
|	B+tree function implementation					|
|									|
|									|
|	Jan Jannink	created 5/27/94		revised 6/16/95		|
|									|
\***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|	Implementation Hiding Macro Definitions				|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			/* low level definition of Nptr value usage */
#ifdef POINTER
#define nAdr(b) (b)->X
#define nodearrayhead B->tree
#else
#define nAdr(b) B->tree[(b)].X
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
#define getfunkey B->theKey
#define getfundata B->theData
#define setfunkey(v) (B->theKey = (v))
#define setfundata(v) (B->theData = (v))

			/* define number of B+tree nodes for free node pool */
#define getpoolsize B->poolsize
#define setpoolsize(v) (B->poolsize = (v))

			/* access memory region containing B+tree nodes */
#define getnodearray B->tree
#define setnodearray(v) (B->tree = (Node *)(v))

			/* locations from which tree access begins */
#define getroot B->root
#define setroot(v) (B->root = (v))
#define getleaf B->leaf
#define setleaf(v) (B->leaf = (v))

			/* define max/min number of pointers per node */
#define getfanout B->fanout
#define setfanout(v) (B->fanout = (v) - 1)
#define getminfanout(j) ((nAdr(j).i.info.flags & isLEAF) ? B->fanout - B->minfanout: B->minfanout)
#define setminfanout(v) (B->minfanout = (v) - 1)

			/* manage B+tree height */
#define inittreeheight (B->height = 0)
#define inctreeheight B->height++
#define dectreeheight B->height--
#define gettreeheight B->height

			/* access pool of free nodes */
#define getfirstfreenode B->pool
#define setfirstfreenode(v) (B->pool = (v))

			/* handle split/merge points during insert/delete */
#define getsplitpath B->branch.split
#define setsplitpath(v) (B->branch.split = (v))
#define getmergepath B->branch.merge
#define setmergepath(v) (B->branch.merge = (v))

			/* exploit function to compare two B+tree keys */
#define comparekeys (*B->keycmp)
#define setcomparekeys(v) (B->keycmp = (v))

			/* location containing B+tree class variables */
#define setbplustree(v) (B = (Tree *)(v))

			/* representation independent node numbering */
#define getnodenumber(v) ((v) - nodearrayhead)


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

/*~~~~~~~~~~~~~~~~~~~~~~   private functions   ~~~~~~~~~~~~~~~~~~~~~~~~*/
void initFreeNodePool(Tree *B, int quantity);
Nptr getFreeNode(Tree *B);
void putFreeNode(Tree *B, Nptr self);

/*~~~~~~~~~~~~~~~~~~~   Set up B+tree structure   ~~~~~~~~~~~~~~~~~~~~~*/
Tree *initBtree(unsigned int poolsz, unsigned int fan, KeyCmp keyCmp)
{
  Tree *B;

  setbplustree(malloc(sizeof(Tree)));
  setfanout(fan);
  setminfanout((fan + 1) >> 1);
  initFreeNodePool(B, poolsz);

  setleaf(getFreeNode(B));		/* set up the first leaf node */
  setroot(getleaf);			/* the root is initially the leaf */
  setflag(getroot, isLEAF);
  setflag(getroot, isROOT);
  setflag(getroot, FEWEST);
  inittreeheight;

  setfunkey(0);
  setfundata("0");
  setcomparekeys(keyCmp);

#ifdef DEBUG
  fprintf(stderr, "INIT:  B+tree of fanout %d.\n", fan);
  showBtree(B);
  showNode(B, getroot);
#endif

  return B;
}

/*~~~~~~~~~~~~~~~~~~~   Clean up B+tree structure   ~~~~~~~~~~~~~~~~~~~*/
void freeBtree(Tree *B)
{
#ifdef DEBUG
  fprintf(stderr, "FREE:  B+tree at %10p.\n", (void *) B);
#endif

  free((void *) getnodearray);
  free((void *) B);
}


/*/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|	Find location for data						|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~~~   private functions   ~~~~~~~~~~~~~~~~~~~~~~~~*/
Nptr descendToLeaf(Tree *B, Nptr curr);
int getSlot(Tree *B, Nptr curr);
int findKey(Tree *B, Nptr curr, int lo, int hi);
int bestMatch(Tree *B, Nptr curr, int slot);

/*~~~~~~~~~~~~~~~~~~~~~   top level search call   ~~~~~~~~~~~~~~~~~~~~~*/
Nptr search(Tree *B, keyT key)
{
  Nptr	findNode;

#ifdef DEBUG
  fprintf(stderr, "SEARCH:  key %d.\n", key);
#endif

  setfunkey(key);			/* set search key */
  findNode = descendToLeaf(B, getroot);	/* start search from root node */

#ifdef DEBUG
  fprintf(stderr, "SEARCH:  found on page %d.\n", getnodenumber(findNode));
#endif

  return findNode;
}

/*~~~~~~~~~~~~~~~~~~~~~   `recurse' down B+tree   ~~~~~~~~~~~~~~~~~~~~~*/
Nptr descendToLeaf(Tree *B, Nptr curr)
{
  int	slot;
  Nptr	findNode;

  for (slot = getSlot(B, curr); isinternal(curr); slot = getSlot(B, curr))
    curr = getnode(curr, slot);
  if ((slot > 0) && !comparekeys(getfunkey, getkey(curr, slot)))
    findNode = curr;			/* correct key value found */
  else
    findNode = NONODE;			/* key value not in tree */

  return findNode;
}

/*~~~~~~~~~~~~~~~~~~~   find slot for search key   ~~~~~~~~~~~~~~~~~~~~*/
int getSlot(Tree *B, Nptr curr)
{
  int slot, entries;

  entries = numentries(curr);		/* need this if root is ever empty */
  slot = !entries ? 0 : findKey(B, curr, 1, entries);

#ifdef DEBUG
  fprintf(stderr, "GETSLOT:  slot %d.\n", slot);
#endif

  return slot;
}


/*/*~~~~~~~~~~~~~~~~~~~   recursive binary search   ~~~~~~~~~~~~~~~~~~~~~*/
int findKey(Tree *B, Nptr curr, int lo, int hi)
{
  int mid, findslot;

#ifdef DEBUG
  fprintf(stderr, "GETSLOT:  lo %d, hi %d.\n", lo, hi);
  showNode(B, curr);
#endif

  if (hi == lo) {
    findslot = bestMatch(B, curr, lo);		/* recursion base case */

#ifdef DEBUG
    if (findslot == ERROR)
      fprintf(stderr, "Bad key ordering on node %d\n", getnodenumber(curr));
#endif

  }
  else {
    mid = (lo + hi) >> 1;
    switch (findslot = bestMatch(B, curr, mid)) {
    case LOWER:				/* check lower half of range */
      findslot = findKey(B, curr, lo, mid - 1);		/* never in 2-3+trees */
    break;
    case UPPER:				/* check upper half of range */
      findslot = findKey(B, curr, mid + 1, hi);
    break;

#ifdef DEBUG
    case ERROR:
      fprintf(stderr, "Bad key ordering on node %d\n", getnodenumber(curr));
#endif

    }
  }
  return findslot;
}


/*/*~~~~~~~~~~~   comparison of key with a target key slot   ~~~~~~~~~~~~*/
int bestMatch(Tree *B, Nptr curr, int slot)
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

/*~~~~~~~~~~~~~~~~~~~~~~   private functions   ~~~~~~~~~~~~~~~~~~~~~~~~*/
Nptr getDataNode(Tree *B, keyT key);
Nptr descendSplit(Tree *B, Nptr curr);
void insertEntry(Tree *B, Nptr node, int slot, Nptr sibling, Nptr downPtr);
void placeEntry(Tree *B, Nptr node, int slot, Nptr downPtr);
Nptr split(Tree *B, Nptr node);
void makeNewRoot(Tree *B, Nptr oldRoot, Nptr newNode);

/*~~~~~~~~~~~~~~~~~~~~~   top level insert call   ~~~~~~~~~~~~~~~~~~~~~*/
void insert(Tree *B, keyT key)
{
  Nptr newNode;

#ifdef DEBUG
  fprintf(stderr, "INSERT:  key %d.\n", key);
#endif

  setfunkey(key);			/* set insertion key */
  setfundata("data");			/* a node containing data */
  setsplitpath(NONODE);
  newNode = descendSplit(B, getroot);	/* insertion point search from root */
  if (newNode != getsplitpath)		/* indicates the root node has split */
    makeNewRoot(B, getroot, newNode);
}


/*~~~~~~~~~~~~~~~~   recurse down and split back up   ~~~~~~~~~~~~~~~~~*/
Nptr descendSplit(Tree *B, Nptr curr)
{
  Nptr	newMe, newNode;
  int	slot;

  if (!isfull(curr))
    setsplitpath(NONODE);
  else if (getsplitpath == NONODE)
    setsplitpath(curr);			/* indicates where nodes must split */

  slot = getSlot(B, curr);		/* is null only if the root is empty */
  if (isinternal(curr))			/* continue recursion to leaves */
    newMe = descendSplit(B, getnode(curr, slot));
  else if ((slot > 0) && !comparekeys(getfunkey, getkey(curr, slot))) {
    newMe = NONODE;			/* this code discards duplicates */
    setsplitpath(NONODE);
  }
  else
    newMe = getDataNode(B, getfunkey);	/* an insertion takes place */

  newNode = NONODE;			/* assume no node splitting necessary */

  if (newMe != NONODE) {		/* insert only where necessary */
    if (getsplitpath != NONODE)
      newNode = split(B, curr);		/* a sibling node is prepared */
    insertEntry(B, curr, slot, newNode, newMe);
  }

  return newNode;
}

/*/*~~~~~~~~~~~~~~   determine location of inserted key   ~~~~~~~~~~~~~~~*/
void insertEntry(Tree *B, Nptr newNode, int slot, Nptr sibling, Nptr downPtr)
{
  int split, i, j, k, x, y;

  if (sibling == NONODE) {		/* no split occurred */
    placeEntry(B, newNode, slot + 1, downPtr);
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
	placeEntry(B, sibling, slot - split + 1 - i, downPtr);
      else
	placeEntry(B, newNode, slot + 1, downPtr);
      setfunkey(x);			/* set key separating nodes */
    }
    else if (!i)
      placeEntry(B, sibling, 1, downPtr);

    clrflag(newNode, isFULL);		/* adjust node flags */
    if (numentries(newNode) == getminfanout(newNode))
      setflag(newNode, FEWEST);		/* never happens in even size nodes */
    if (numentries(sibling) > getminfanout(sibling))
      clrflag(sibling, FEWEST);

#ifdef DEBUG
  fprintf(stderr, "INSERT:  slot %d, node %d.\n", slot, getnodenumber(downPtr));
  showNode(B, newNode);
  showNode(B, sibling);
#endif

  }
}

/*/*~~~~~~~~~~~   place key into appropriate node & slot   ~~~~~~~~~~~~~~*/
void placeEntry(Tree *B, Nptr newNode, int slot, Nptr downPtr)
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
Nptr split(Tree *B, Nptr newNode)
{
  Nptr sibling;

  sibling = getFreeNode(B);

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
void makeNewRoot(Tree *B, Nptr oldRoot, Nptr newNode)
{
  setroot(getFreeNode(B));

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

/*~~~~~~~~~~~~~~~~~~~~~~   private functions   ~~~~~~~~~~~~~~~~~~~~~~~~*/
Nptr descendBalance(Tree *B, Nptr curr, Nptr left, Nptr right, Nptr lAnc, Nptr rAnc, Nptr parent);
void collapseRoot(Tree *B, Nptr oldRoot, Nptr newRoot);
void removeEntry(Tree *B, Nptr curr, int slot);
Nptr merge(Tree *B, Nptr left, Nptr right, Nptr anchor);
Nptr shift(Tree *B, Nptr left, Nptr right, Nptr anchor);

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
void delete(Tree *B, keyT key)
{
  Nptr newNode;

#ifdef DEBUG
  fprintf(stderr, "DELETE:  key %d.\n", key);
#endif

  setfunkey(key);			/* set deletion key */
  setmergepath(NONODE);
  newNode = descendBalance(B, getroot, NONODE, NONODE, NONODE, NONODE, NONODE);
  if (isnode(newNode))
    collapseRoot(B, getroot, newNode);	/* remove root when superfluous */
}


/*~~~~~~~~~~~~~~~~~~~~~   remove old root node   ~~~~~~~~~~~~~~~~~~~~~~*/
void collapseRoot(Tree *B, Nptr oldRoot, Nptr newRoot)
{

#ifdef DEBUG
  fprintf(stderr, "COLLAPSE:  old %d, new %d.\n", getnodenumber(oldRoot), getnodenumber(newRoot));
  showNode(B, oldRoot);
  showNode(B, newRoot);
#endif

  setroot(newRoot);
  setflag(newRoot, isROOT);
  putFreeNode(B, oldRoot);
  dectreeheight;			/* the height of the tree decreases */
}


/*/*~~~~~~~~~~~~~~~   recurse down and balance back up   ~~~~~~~~~~~~~~~~*/
Nptr descendBalance(Tree *B, Nptr curr, Nptr left, Nptr right, Nptr lAnc, Nptr rAnc, Nptr parent)
{
  Nptr	newMe, myLeft, myRight, lAnchor, rAnchor, newNode;
  int	slot, notleft, notright, fewleft, fewright, test;

  if (!isfew(curr))
    setmergepath(NONODE);
  else if (getmergepath == NONODE)
    setmergepath(curr);		/* mark which nodes may need rebalancing */

  slot = getSlot(B, curr);
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
    newMe = descendBalance(B, newNode, myLeft, myRight, lAnchor, rAnchor, curr);
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
    removeEntry(B, curr, slot + (newMe != newNode));	/* removes one of two */

#ifdef DEBUG
  fprintf(stderr, "DELETE:  slot %d, node %d.\n", slot, getnodenumber(newMe));
  showNode(B, curr);
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
      newNode = test ? merge(B, curr, right, rAnc) : merge(B, left, curr, lAnc);
    }
			/* CASE 3: choose the better of a merge or a shift */
    else if (!notleft && fewleft && !notright && !fewright) {
      test = !(rAnc == parent) && (curr == getmergepath);
      newNode = test ? merge(B, left, curr, lAnc) : shift(B, curr, right, rAnc);
    }
			/* CASE 4: also choose between a merge or a shift */
    else if (!notleft && !fewleft && !notright && fewright) {
      test = !(lAnc == parent) && (curr == getmergepath);
      newNode = test ? merge(B, curr, right, rAnc) : shift(B, left, curr, lAnc);
    }
			/* CASE 5: choose the more effective of two shifts */
    else if (lAnc == rAnc) { 		/* => both anchors are the parent */
      test = (numentries(left) <= numentries(right));
      newNode = test ? shift(B, curr, right, rAnc) : shift(B, left, curr, lAnc);
    }
			/* CASE 6: choose the shift with more local effect */
    else {				/* if omitting cases 3,4,5 use below */
      test = (lAnc == parent);		/* test = (!notleft && !fewleft); */
      newNode = test ? shift(B, left, curr, lAnc) : shift(B, curr, right, rAnc);
    }
  }

  return newNode;
}


/*/*~~~~~~~~~~~~~~~   remove key and pointer from node   ~~~~~~~~~~~~~~~~*/
void removeEntry(Tree *B, Nptr curr, int slot)
{
  int x;

  putFreeNode(B, getnode(curr, slot));	/* return deleted node to free list */
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
Nptr merge(Tree *B, Nptr left, Nptr right, Nptr anchor)
{
  int	x, y, z;

#ifdef DEBUG
  fprintf(stderr, "MERGE:  left %d, right %d.\n", getnodenumber(left), getnodenumber(right));
  showNode(B, left);
  showNode(B, right);
#endif

  if (isinternal(left)) {
    incentries(left);			/* copy key separating the nodes */
    setfunkey(getkey(right, 1));	/* defined but maybe just deleted */
    z = getSlot(B, anchor);		/* needs the just calculated key */
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
    setflag(left, isFULL);		/* never happens in even size nodes */

  if (getmergepath == left || getmergepath == right)
    setmergepath(NONODE);		/* indicate rebalancing is complete */

  return right;
}


/*/*~~~~~   shift entries in a node pair & adjust anchor key value   ~~~~*/
Nptr shift(Tree *B, Nptr left, Nptr right, Nptr anchor)
{
  int	i, x, y, z;

#ifdef DEBUG
  fprintf(stderr, "SHIFT:  left %d, right %d.\n", getnodenumber(left), getnodenumber(right));
  showNode(B, left);
  showNode(B, right);
#endif

  i = isinternal(left);
  if (numentries(left) < numentries(right)) {	/* shift entries to left */
    y = (numentries(right) - numentries(left)) >> 1;
    x = numentries(left) + y;
    setfunkey(getkey(right, y + 1 - i));	/* set new anchor key value */
    z = getSlot(B, anchor);			/* find slot in anchor node */
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
    z = getSlot(B, anchor) + 1;
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
  if (numentries(left) == getminfanout(left))		/* adjust node flags */
    setflag(left, FEWEST);
  else
    clrflag(left, FEWEST);			/* never happens in 2-3+trees */
  if (numentries(right) == getminfanout(right))
    setflag(right, FEWEST);
  else
    clrflag(right, FEWEST);			/* never happens in 2-3+trees */
  setmergepath(NONODE);

#ifdef DEBUG
  showNode(B, left);
  showNode(B, right);
#endif

  return NONODE;
}


/*/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|	Empty Node Utilities						|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~   Set up pool of free nodes   ~~~~~~~~~~~~~~~~~~*/
void initFreeNodePool(Tree *B, int quantity)
{
  int	i;
  Nptr	n;

  setpoolsize(quantity);
  setnodearray(malloc(quantity * sizeof(Node)));	/* node memory block */
  setfirstfreenode(nodearrayhead);	/* start a list of free nodes */
  for (n = getfirstfreenode, i = 0; i < quantity; n++, i++) {
    clearflags(n);
    clearentries(n);
    setnextnode(n, n + 1);		/* insert node into free node list */
  }
  setnextnode(--n, NONODE);		/* indicates end of free node list */
}


/*~~~~~~~~~~~~~   take a free B+tree node from the pool   ~~~~~~~~~~~~~*/
Nptr getFreeNode(Tree *B)
{
  Nptr newNode = getfirstfreenode;

  if (newNode != NONODE) {
    setfirstfreenode(getnextnode(newNode));	/* adjust free node list */
    setnextnode(newNode, NONODE);		/* remove node from list */
  }
  else {
    fprintf(stderr, "Out of tree nodes.");	/* can't recover from this */
    exit(0);
  }
  return newNode;
}


/*~~~~~~~~~~~~   return a deleted B+tree node to the pool   ~~~~~~~~~~~*/
void putFreeNode(Tree *B, Nptr self)
{
  clearflags(self);
  clearentries(self);
  setnextnode(self, getfirstfreenode);		/* add node to list */
  setfirstfreenode(self);			/* set it to be list head */
}


/*~~~~~~   fill a free data node with a key and associated data   ~~~~~*/
Nptr getDataNode(Tree *B, keyT key)		/* can add data parameter */
{
  Nptr	newNode = getFreeNode(B);
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
void showNode(Tree *B, Nptr n)
{
  int x;

  fprintf(stderr, "-  --  --  --  --  --  --  --  --  --  --  --  -\n");
  fprintf(stderr, "| node %6d                 ", getnodenumber(n));
  fprintf(stderr, "  magic    %4x  |\n", getflags(n) & MASK);
  fprintf(stderr, "-  --  --  --  --  --  --  --  --  --  --  --  -\n");
  fprintf(stderr, "| flags   %1d%1d%1d%1d ", isfew(n), isfull(n), isroot(n), isleaf(n));
  fprintf(stderr, "| keys = %5d ", numentries(n));
  fprintf(stderr, "| node = %6d  |\n", getnodenumber(getfirstnode(n)));
  for (x = 1; x <= numentries(n); x++) {
    fprintf(stderr, "| entry %6d ", x);
    fprintf(stderr, "| key = %6d ", getkey(n, x) & 0xFFFF);
    fprintf(stderr, "| node = %6d  |\n", getnodenumber(getnode(n, x)));
  }
  fprintf(stderr, "-  --  --  --  --  --  --  --  --  --  --  --  -\n");
}

/*~~~~~~~~~~~~~~~~~   B+tree class variable printer   ~~~~~~~~~~~~~~~~~*/
void showBtree(Tree *B)
{
  fprintf(stderr, "-  --  --  --  --  --  -\n");
  fprintf(stderr, "|  B+tree  %10p  |\n", (void *) B);
  fprintf(stderr, "-  --  --  --  --  --  -\n");
  fprintf(stderr, "|  root        %6d  |\n", getnodenumber(getroot));
  fprintf(stderr, "|  leaf        %6d  |\n", getnodenumber(getleaf));
  fprintf(stderr, "|  fanout         %3d  |\n", getfanout + 1);
  fprintf(stderr, "|  minfanout      %3d  |\n", getminfanout(getroot) + 1);
  fprintf(stderr, "|  height         %3d  |\n", gettreeheight);
  fprintf(stderr, "|  freenode    %6d  |\n", getnodenumber(getfirstfreenode));
  fprintf(stderr, "|  theKey      %6d  |\n", getfunkey);
  fprintf(stderr, "|  theData     %6s  |\n", getfundata);
  fprintf(stderr, "-  --  --  --  --  --  -\n");
}
#endif

/*~~~~~~~~~~~~~~~~~~~~~~   B+tree data printer   ~~~~~~~~~~~~~~~~~~~~~~*/
void listBtreeValues(Tree *B, Nptr n, int num)
{
  int slot, prev = -1;

  for (slot = 1; (n != NONODE) && num && numentries(n); num--) {
    if (getkey(n, slot) <= prev) fprintf(stderr, "BOMB");
    prev = getkey(n, slot);
    fprintf(stderr, "%8d%c", prev, (num & 7 ? ' ' : '\n'));
    if (++slot > numentries(n))
      n = getnextnode(n), slot = 1;
  }
  fprintf(stderr, "\n\n");
}

/*~~~~~~~~~~~~~~~~~~~   entire B+tree data printer   ~~~~~~~~~~~~~~~~~~*/
void listAllBtreeValues(Tree *B)
{
  listBtreeValues(B, getleaf, ERROR);
}
