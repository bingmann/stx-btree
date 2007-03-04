/***********************************************************************\
|									|
|	B+tree function tests						|
|									|
|									|
|	Jan Jannink	created 12/22/94	revised 4/4/95		|
|									|
\***********************************************************************/

#include <iostream.h>
#include "b++tree.h"

inline long prngen(long& seed);

int main(void)
{
  Tree	*Bplus;
  Nptr	keyNode;
  int   i, j;
  long  seed = 231070;

  Bplus = new Tree(ARRAY_SIZE, NODE_SIZE / sizeof(Entry), compareKeys);

  for (i = 0, j = 0; i < 200; i++, j = (j + 52) % 400) {
    Bplus->Insert(j);
    Bplus->listAllBtreeValues();
  }

  for (i = 0, j = 0; i < 200; i++, j = (j + 52) % 400)
    if (Bplus->Search(j) == Bplus->Nonode())
      cerr << "Error: " << j << " not found\n";

/*
  Bplus->Insert(12);
  Bplus->Insert(11);
  Bplus->Insert(10);
  Bplus->Insert(9);
  Bplus->Insert(8);
  Bplus->Insert(7);
  Bplus->Insert(6);
  Bplus->Insert(5);
  Bplus->Insert(4);
  Bplus->Insert(3);
  Bplus->Insert(2);
  Bplus->Insert(1);
  keyNode = Bplus->Search(1);
  keyNode = Bplus->Search(6);
  keyNode = Bplus->Search(7);
  keyNode = Bplus->Search(12);
  Bplus->Delete(1);
  Bplus->Delete(2);
  Bplus->Delete(3);
  Bplus->Delete(4);
  Bplus->Delete(5);
  Bplus->Delete(6);
  Bplus->Delete(7);
  Bplus->Delete(8);
  Bplus->Delete(9);
  Bplus->Delete(10);
  Bplus->Delete(11);
  Bplus->Delete(12);
  for (i = 0; i < 32 /@@@@@@@@@ 524288 @@@@@@@@/; i++) {
    j = prngen(seed) >> 3 & 65535;
    if (Bplus->Search(j) == Bplus->Nonode())
      Bplus->Insert(j);
    else
      Bplus->Delete(j);
    if (i == 400)
      Bplus->listAllBtreeValues();
  }
*/

  delete Bplus;

  return 1;
}


	/* definitions for the random number generator (seed * gen % mod) */
#define gen 16807
#define mod 2147483647
 
inline long prngen(long& seed)
{
  static long   tmp;
 
  tmp = (seed >> 16) * gen;
  seed = (seed & 65535) * gen + ((tmp & 32767) << 16) + (tmp >> 15);
  return (seed -= (seed < 0 ? mod: 0));
}
 
