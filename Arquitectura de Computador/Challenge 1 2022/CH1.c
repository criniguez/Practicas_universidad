/*
AUTHORS == Cristian Iñiguez Rodriguez 1566514
En el SLURM cambiar la compilacion a "gcc -Ofast -march=nehalem CH1.c -o CH1"
12,092043149 seconds time elapsed
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int seed;

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

struct node* MergeSortedList(struct node* p1, struct node* p2)
{
  struct node* result = NULL;

  if (p1 == NULL)
    return (p2);
  else if (p2 == NULL)
    return (p1);

  if (p1->value < p2->value|| ( (p1->value == p2->value) && (p1->x < p2->x) ) ||
                           ( (p1->value == p2->value) && (p1->x == p2->x) && (p1->y < p2->y)) )
  {
    result = p1;
    result->next = MergeSortedList(p1->next, p2);
  }
  else {
    result = p2;
    result->next = MergeSortedList(p1, p2->next);
  }
  return result;
}

void SplitList(struct node* head, struct node** p1, struct node** p2)
{
  struct node* pt1;
  struct node* pt2;
  pt2 = head;
  pt1 = head->next;

  while (pt1 != NULL) {
    pt1 = pt1->next;
    if (pt1 != NULL) {
      pt2 = pt2->next;
      pt1 = pt1->next;
    }
  }

  *p1 = head;
  *p2 = pt2->next;
  pt2->next = NULL;
}


void MergeSort(struct node** List)
{
  struct node* head = *List;
  struct node* p1;
  struct node* p2;

  if ((head == NULL) || (head->next == NULL)) {
    return;
  }

  SplitList(head, &p1, &p2);

  MergeSort(&p1);
  MergeSort(&p2);

  *List = MergeSortedList(p1, p2);
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

void InitBOARD ( unsigned BOARD[], struct node *L, int D )
{
  while ( L != NULL )
  {
    struct node *p = L;
    L = L->next;
    BOARD[p->x+p->y*D] = p->value;
  }

  while ( L != NULL )
  {
    struct node *p = L;
    L = L->next;
    free(p);
  }
}

void UpdateBOARD ( unsigned IN[], unsigned OUT[], int D, unsigned int MAX_VAL )
{
  unsigned V[4];
  int aux = D - 1;
  int min, max, sum;
  for (int y=1; y<aux; y++){
    int motion = D*y;
    for (int x=1; x<aux; x++)
    {
      int index = x+motion;
      V[0] = OUT[index + 1];
      V[1] = OUT[index - 1];
      V[2] = OUT[index - D];
      V[3] = OUT[index + D];
      sum = V[0]; min = V[0]; max = V[0];
      for(int i = 1; i < 4; i++)
      {
        sum += V[i];
        min = (min > V[i])?V[i]:min;
        max = (max < V[i])?V[i]:max;
      }
      IN[index] = (sum - min - max) %MAX_VAL;
    }
  }
}

struct node * SortBOARD ( unsigned BOARD[], int D )
{
  struct node *List=NULL;
  for (int y=0; y<D; y++)
  {
    int motion = y*D;
    for (int x=0; x<D; x++)
    {
      struct node * new = (struct node *)malloc(sizeof(struct node));
      new->value = BOARD[x+motion];
      new->x = x;
      new->y = y;
      new->next = List;
      List = new;
    }
  }
  MergeSort(&List);
  return List;
}

int main (int argc, char **argv)
{
  int N=3000, D=50, Iter=1000;

  seed = 12345;

  if (argc>1) { D   = atoi(argv[1]); }
  if (argc>2) { N   = atoi(argv[2]); }
  if (argc>3) { Iter= atoi(argv[3]); }

  printf("Challenge #1: DIM= %d, N= %d, Iter= %d\n", D, N, Iter);
  int DD = D*D;
  struct node * List;
  unsigned *BOARD = (unsigned*)calloc(DD, sizeof(unsigned));
  unsigned *TMP = (unsigned*)malloc(DD*sizeof(unsigned));

  for (int i=0; i<DD; i++)
  {
    unsigned int x   = myRandom() % D;
    unsigned int y   = myRandom() % D;
    unsigned int val = myRandom() % 99;
    BOARD[x+y*D] = val;
  }


  for (int i=0; i<N; i++)
  {
    memcpy(TMP, BOARD, DD*sizeof(unsigned));
    for (int r=0; r<Iter; r++)
    {
      UpdateBOARD( TMP, BOARD, D, 99 );
      unsigned *tmp = TMP;
      TMP = BOARD;
      BOARD = tmp;
    }

    List = SortBOARD( BOARD, D );
    List = ReverseValues( List, D);
    InitBOARD ( BOARD, List, D );
  }

  for (int x=0; x<D; x++)
  {
    int motion = 0;
    for (int y=0; y<D; y++)
    {
      printf("%2d ",BOARD[x+motion]);
      motion += D;
    }
    printf("\n");
  }

  free(BOARD);
  free(TMP);
  return 0;
}