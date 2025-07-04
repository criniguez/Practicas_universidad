/*
AUTHORS == Cristian Iñiguez Rodriguez 1566514
En el SLURM cambiar la compilacion a "gcc -Ofast -march=nehalem CH1.c -o CH1"
12,092043149 seconds time elapsed
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> 

unsigned int seed;

unsigned int myRandom() {
  seed = (214013*seed+2531011);
  return (seed>>13);
}

struct node {
  unsigned int x;
  unsigned int y;
  uint8_t value;
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
    uint8_t val = p1->value;
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

void InitBOARD ( uint8_t BOARD[], struct node *L, int D )
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

#define MIN(a,b) (((a)<=(b))?(a):(b))
#define MAX(a,b) (((a)>=(b))?(a):(b))
void UpdateBOARD ( uint8_t IN[], uint8_t OUT[], int D, uint8_t MAX_VAL )
{
  int aux = D - 1;
  for (int y=1; y<aux; y++){
    int motion = D*y;
    for (int x=1; x<aux; x++)
    {
      int index = x+motion;
      const uint8_t V1 = OUT[index + 1];
      const uint8_t V2 = OUT[index - 1];
      const uint8_t V3 = OUT[index - D];
      const uint8_t V4 = OUT[index + D];
      //code
      const uint8_t min1 = MIN(V1, V2); 
      const uint8_t min2 = MIN(V3, V4); 
      const uint8_t b    = MAX(min1, min2); 
      const uint8_t max1 = MAX(V1, V2); 
      const uint8_t max2 = MAX(V3, V4); 
      const uint8_t c    = MIN(max1, max2);
      const uint8_t sum  = b + c; 
      //out
      IN[index] = (sum >= MAX_VAL) ? ((uint8_t)(sum-MAX_VAL)) : sum;
    }
  }
}

struct node * SortBOARD ( uint8_t BOARD[], int D )
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
  uint8_t *BOARD = (uint8_t*)calloc(DD, sizeof(uint8_t));
  uint8_t *TMP = (uint8_t*)malloc(DD*sizeof(uint8_t));

  for (int i=0; i<DD; i++)
  {
    unsigned int x   = myRandom() % D;
    unsigned int y   = myRandom() % D;
    uint8_t val = myRandom() % 99;
    BOARD[x+y*D] = val;
  }


  for (int i=0; i<N; i++)
  {
    memcpy(TMP, BOARD, DD*sizeof(uint8_t));
    for (int r=0; r<Iter; r++)
    {
      UpdateBOARD( TMP, BOARD, D, 99 );
      uint8_t *tmp = TMP;
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
