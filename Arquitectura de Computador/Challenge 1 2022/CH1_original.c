#include <stdio.h>
#include <stdlib.h>

// Variable used to generate pseudo-random numbers
unsigned int seed;

// Function to generate pseudo-random numbers
unsigned int myRandom() {
  seed = (214013*seed+2531011);
  return (seed>>13);
}

struct node { 
  unsigned int x; 
  unsigned int y; 
  unsigned int value;
  struct node * next;
};


struct node * GenerateList ( int DIM, unsigned int MAX_VAL, int N )
{
  struct node *List=NULL;
  struct node *new;
  struct node *prev;
  struct node *nxt;

  // Special case for first element
  unsigned int x   = myRandom() % DIM;
  unsigned int y   = myRandom() % DIM;
  unsigned int val = myRandom() % MAX_VAL;

  // insert unique node in list
  new = (struct node *) malloc( sizeof(struct node) );
  new->x     = x;
  new->y     = y;
  new->value = val;
  new->next  = NULL;
  List = new;

  // generate and insert N-1 nodes
  for ( int i=1; i<N; i++ ) 
  {
    x   = myRandom() % DIM;
    y   = myRandom() % DIM;
    val = myRandom() % MAX_VAL;
    
    // Insert into sorted list
    prev= List;
    nxt = List;
    while ( nxt != NULL && ( (nxt->x < x) || ((nxt->x == x) && (nxt->y < y)) ) )
    {
      prev = nxt;
      nxt  = nxt->next;
    }

    if ( (nxt != NULL) && (nxt->x == x) && (nxt->y == y) )
    { // position already in list: override value
      nxt->value = val;
    }
    else
    { // new position: insert new node
      new = (struct node *) malloc( sizeof(struct node) );
      new->x     = x;
      new->y     = y;
      new->value = val;
      new->next  = nxt;
      if (nxt == List)
        List = new;
      else
        prev->next = new;
    }
  }
  return List;
}

struct node * InsertSorted ( struct node *List, unsigned int Val, unsigned int x, unsigned int y )
{
  struct node *prev;
  struct node *nxt;
  struct node *new;

  // Insert into sorted list
  prev= List;
  nxt = List;
  while ( nxt != NULL && (  nxt->value < Val || ( (nxt->value == Val) && (nxt->x < x) ) || 
                           ( (nxt->value == Val) && (nxt->x == x) && (nxt->y < y)) ) )
  {
    prev = nxt;
    nxt  = nxt->next;
  }

  // insert new node
  new = (struct node *) malloc( sizeof(struct node) );
  new->x     = x;
  new->y     = y;
  new->value = Val;
  new->next  = nxt;
  if (nxt == List)
    List = new;
  else
    prev->next = new;
  return List;
}

struct node * ReverseValues( struct node *L, int D)
{
  struct node *New;
  struct node *p1;
  struct node *p2;

  // move halve the nodes in List into second list in reverse order
  New= NULL;
  for (int i=0; i<(D*D/2) && L!=NULL; i++)
  {
    // get first node from L
    p1 = L; L= L->next;
   
    // insert first into New
    p1->next = New;  New = p1; 
  }

  // traverse both lists and swap their values
  p1= New; p2=L;
  while (p1!= NULL && p2 != NULL)
  {
    int val = p1->value;
    p1->value = p2->value;
    p2->value = val;
    p1= p1->next;
    p2= p2->next;
  }

  // join both lists into a single list
  p1= New;
  while (p1!=NULL)
  {
    p1 = p1->next;
    New->next = L;
    L = New;
    New = p1;
  }
  return L;
}

void FreeList ( struct node *L )
{
  while ( L != NULL )
  {
    struct node *p = L;
    L = L->next;
    free(p);
  }
}


void PrintList ( struct node *L )
{
  while ( L != NULL )
  {
    struct node *p = L;
    L = L->next;
    printf("(%d,%d)= %d\n", p->x, p->y, p->value);
  } 
}

void InitBOARD ( unsigned BOARD[], struct node *L, int D )
{
  while ( L != NULL )
  {
    struct node *p = L;
    L = L->next;
    BOARD[p->x+p->y*D] = p->value;
  } 
}


void ClearBOARD ( unsigned BOARD[], int D )
{
  for (int x=0; x<D; x++)
    for (int y=0; y<D; y++)
      BOARD[x+D*y] = 0;
}


void UpdateBOARD ( unsigned IN[], unsigned OUT[], int D, unsigned int MAX_VAL )
{
  unsigned V[4], tmp;
  for (int x=1; x<D-1; x++)
    for (int y=1; y<D-1; y++)
    {
      // copy values of neighbour elements to vector V
      V[0]= IN[x+1+D*y];
      V[1]= IN[x-1+D*y];
      V[2]= IN[x+D*(y-1)];
      V[3]= IN[x+D*(y+1)];

      // sort values in vector V
      for (int i=0; i<3; i++)
        if (V[i]>V[3])
        { tmp = V[i]; V[i]= V[3]; V[3]= tmp; }
      for (int i=0; i<2; i++)
        if (V[i]>V[2])
        { tmp = V[i]; V[i]= V[2]; V[2]= tmp; }
      for (int i=0; i<1; i++)
        if (V[i]>V[1])
        { tmp = V[i]; V[i]= V[1]; V[1]= tmp; }

      OUT[x+D*y] = (V[1]+V[2]);
      OUT[x+D*y] = OUT[x+D*y] % MAX_VAL;
    }
}


void CopyBOARD ( unsigned IN[], unsigned OUT[], int D )
{
  for (int x=1; x<D-1; x++)
    for (int y=1; y<D-1; y++)
      OUT[x+D*y] = IN[x+D*y];
}

void PrintBOARD ( unsigned BOARD[], int D )
{
  for (int x=0; x<D; x++)
  {
    for (int y=0; y<D; y++)
    {
      printf("%2d ",BOARD[x+D*y]);
    }
    printf("\n");
  }
}

struct node * SortBOARD ( unsigned BOARD[], int D )
{
  struct node *List=NULL;
  for (int x=0; x<D; x++)
  {
    for (int y=0; y<D; y++)
    {
      List = InsertSorted (List, BOARD[x+D*y], x, y);
    }
  }
  return List;
}

int main (int argc, char **argv)
{
  int N=3000, D=50, Iter=1000;

  seed = 12345;

  // obtain parameters at run time
  if (argc>1) { D   = atoi(argv[1]); }
  if (argc>2) { N   = atoi(argv[2]); }
  if (argc>3) { Iter= atoi(argv[3]); }

  printf("Challenge #1: DIM= %d, N= %d, Iter= %d\n", D, N, Iter);

  struct node * List;
  unsigned BOARD[D*D];
  unsigned TMP[D*D];

  List = GenerateList ( D, 99, D*D );
  ClearBOARD ( BOARD, D );
  InitBOARD ( BOARD, List, D );
  FreeList ( List );

  for (int i=0; i<N; i++)
  {
    // PrintBOARD ( BOARD, D );
    for (int r=0; r<Iter; r++)
    {
      UpdateBOARD( BOARD, TMP, D, 99 );
      CopyBOARD  ( TMP, BOARD, D );
    }

    // PrintBOARD ( BOARD, D );

    List = SortBOARD( BOARD, D );
    List = ReverseValues( List, D);
    InitBOARD ( BOARD, List, D );
    FreeList ( List );
  }

  PrintBOARD ( BOARD, D );

  return 0;
}
