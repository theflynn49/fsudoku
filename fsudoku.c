//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
//
//
extern void gdrawcell(int i, int type) ;
extern void printInfoInt(char *fmt, int val) ;
extern void printInfo(char *msg) ;
extern char solution_ok ;

#define RULE4_0
//
// groups of cells containing numbers 1-9
// 
unsigned char groups[][9] = {
{ 0,  1,  2,  3,  4,  5,  6,  7,  8}, 
{ 9, 10, 11, 12, 13, 14, 15, 16, 17}, 
{18, 19, 20, 21, 22, 23, 24, 25, 26}, 
{27, 28, 29, 30, 31, 32, 33, 34, 35}, 
{36, 37, 38, 39, 40, 41, 42, 43, 44}, 
{45, 46, 47, 48, 49, 50, 51, 52, 53}, 
{54, 55, 56, 57, 58, 59, 60, 61, 62}, 
{63, 64, 65, 66, 67, 68, 69, 70, 71}, 
{72, 73, 74, 75, 76, 77, 78, 79, 80},

{ 0,  9, 18, 27, 36, 45, 54, 63, 72}, 
{ 1, 10, 19, 28, 37, 46, 55, 64, 73}, 
{ 2, 11, 20, 29, 38, 47, 56, 65, 74}, 
{ 3, 12, 21, 30, 39, 48, 57, 66, 75}, 
{ 4, 13, 22, 31, 40, 49, 58, 67, 76}, 
{ 5, 14, 23, 32, 41, 50, 59, 68, 77}, 
{ 6, 15, 24, 33, 42, 51, 60, 69, 78}, 
{ 7, 16, 25, 34, 43, 52, 61, 70, 79}, 
{ 8, 17, 26, 35, 44, 53, 62, 71, 80},

{ 0,  1,  2,  9, 10, 11, 18, 19, 20}, 
{ 3,  4,  5, 12, 13, 14, 21, 22, 23}, 
{ 6,  7,  8, 15, 16, 17, 24, 25, 26}, 
{27, 28, 29, 36, 37, 38, 45, 46, 47}, 
{30, 31, 32, 39, 40, 41, 48, 49, 50}, 
{33, 34, 35, 42, 43, 44, 51, 52, 53}, 
{54, 55, 56, 63, 64, 65, 72, 73, 74}, 
{57, 58, 59, 66, 67, 68, 75, 76, 77}, 
{60, 61, 62, 69, 70, 71, 78, 79, 80}, 

{ 0,  1,  2,  9, 10, 11, 18, 19, 20}, 
{ 3,  4,  5, 12, 13, 14, 21, 22, 23}, 
{ 6,  7,  8, 15, 16, 17, 24, 25, 26}, 
{27, 28, 29, 36, 37, 38, 45, 46, 47}, 
{30, 31, 32, 39, 40, 41, 48, 49, 50}, 
{33, 34, 35, 42, 43, 44, 51, 52, 53}, 
{54, 55, 56, 63, 64, 65, 72, 73, 74}, 
{57, 58, 59, 66, 67, 68, 75, 76, 77}, 
{60, 61, 62, 69, 70, 71, 78, 79, 80}, 

{ 0,  1,  9, 10, 11, 19, 20, 21, 29}, 
{ 2,  3,  4,  5,  6, 12, 13, 14, 22}, 
{ 7,  8, 15, 16, 17, 23, 24, 25, 33}, 
{18, 27, 28, 36, 37, 38, 45, 46, 54}, 
{30, 31, 32, 39, 40, 41, 48, 49, 50}, 
{26, 34, 35, 42, 43, 44, 52, 53, 62}, 
{47, 55, 56, 57, 63, 64, 65, 72, 73}, 
{58, 66, 67, 68, 74, 75, 76, 77, 78}, 
{51, 59, 60, 61, 69, 70, 71, 79, 80}, 

{ 0,  1,  2,  9, 10, 11, 12, 18, 20}, 
{ 3,  4,  5, 13, 14, 15, 21, 22, 23}, 
{ 6,  7,  8, 16, 17, 24, 25, 26, 34}, 
{19, 27, 28, 29, 36, 37, 38, 45, 47}, 
{30, 31, 32, 39, 40, 41, 48, 49, 50}, 
{33, 35, 42, 43, 44, 51, 52, 53, 61}, 
{46, 54, 55, 56, 63, 64, 72, 73, 74}, 
{57, 58, 59, 65, 66, 67, 75, 76, 77}, 
{60, 62, 68, 69, 70, 71, 78, 79, 80} 
} ;

unsigned int NBB[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 } ; // number of bits in a nibble 

int explain=0 ;
int type_puzzle = 0 ; // 0=classic 1=aztec 2=jigsaw
int diff_puzzle = 2 ; // 0=easiest 4=hardest
int STACK_PTR, max_stack, max_stack_total ;
// grid format :
// bits 0-3 : 0 if cell empty or else digit set in current cell
// bit 4 : lock bit : cell is part of the puzzle def and can't be edited
// bits 15-7 : 9 possible digits, used by the solver or set by the user
// bits 5-6 : unused
unsigned int grid[81] ;
unsigned int sol_grid[81] ;
unsigned int sol_Ztree[81] ;
unsigned int Ztree[81] ;
unsigned int Ctree[81] ;
unsigned int Ltree[81] ;
unsigned int tree[81][81] ;
//

void GoToTypeGroups()
{
     memcpy(groups[2*9], groups[9*(3+type_puzzle)],  81) ; 
}

void printGrid(void)
{
  int i, j ;
  for (i=0; i<9; ++i)
  {
    for (j=0; j<9; ++j)
    {
	 unsigned int c = grid[j+9*i] ;
	 if ((c & 0xF) != 0)
		// printf(" \033[1m%c\033[0m ", (c&0xF)+'0') ; else
		printf(" %c ", (c&0xF)+'0') ; else
		printf(" . ") ;
	 if (j==2 || j==5) printf(" | ") ;
    }
    printf("\n") ;
    if (i==2 || i==5) printf("- - - - - - - - - - - - - - - - -\n") ;
  }
  printf("\n") ;
}

void printFileDefs(FILE *f)
{
  int i ;
  unsigned int c ;

  for (i=0;i<81;++i) {
          c = grid[i] & 0xF ;	  
	  if (c==0) fprintf(f, ".") ; else fprintf(f, "%d", c) ;
  }	
  fprintf(f, "\n") ;
}

void printDefs(int typ)
{
  int i ;
  unsigned int c ;

  if (typ & 1)
  {
     for (i=0;i<81;++i) {
          c = grid[i] & 0xF ;	  
	  if (c==0) printf(".") ; else printf("%d", c) ;
     }			  
     printf("\n") ;
  }
  if (typ & 2)
  {
     for (i=0;i!=89;i+=9) {
	  if (i>=81) i-=80 ;
          c = grid[i] & 0xF ;	  
	  if (c==0) printf("_") ; else printf("%c", c+'a') ;

     }			  
     printf("\n") ;
  }
  if (typ==3) printf("___________________________________________\n") ;
}

void printGridHint(void)
{
	
  int i, j ;
  unsigned int c ;
  for (i=0; i<9; ++i)
  {
    for (j=0; j<9; ++j)
    {
	 c = grid[j+9*i] ;
	 if ((c & 0xF) != 0)
		printf("   ") ; else
		{
			if (c & 0x80) printf("1") ; else printf(" ") ;
			if (c & 0x100) printf("2") ; else printf(" ") ;
			if (c & 0x200) printf("3") ; else printf(" ") ;
		}
    	 printf(" ") ;
	 if (j==2 || j==5) printf(" | ") ;
    }
    printf("\n") ;
    for (j=0; j<9; ++j)
    {
	 c = grid[j+9*i] ;
	 if ((c & 0xF) != 0)
		printf(" \033[1m%c\033[0m ", (c&0xF)+'0') ; else
		{
			if (c & 0x400)  printf("4") ; else printf(" ") ;
			if (c & 0x800)  printf("5") ; else printf(" ") ;
			if (c & 0x1000) printf("6") ; else printf(" ") ;
		}
    	 printf(" ") ;
	 if (j==2 || j==5) printf(" | ") ;
    }
    printf("\n") ;
    for (j=0; j<9; ++j)
    {
	 c = grid[j+9*i] ;
	 if ((c & 0xF) != 0)
		printf("   ") ; else
		{
			if (c & 0x2000) printf("7") ; else printf(" ") ;
			if (c & 0x4000) printf("8") ; else printf(" ") ;
			if (c & 0x8000) printf("9") ; else printf(" ") ;
		}
    	 printf(" ") ;
	 if (j==2 || j==5) printf(" | ") ;
    }
    printf("\n\n") ;
    if (i==2 || i==5) printf("- - - - - - - - - - - - - - - - - - - - -\n") ;
  }
  printDefs(3) ;
}

//
// Check sudoku rule for display purpose only.
//
int const_errors()
{
  unsigned char (*cst)[9] ;
  unsigned char dgt[16] ;
  int cndx ;
  int err=0 ;
  for (cndx=0; cndx<81; ++cndx) grid[cndx] &= 0xFFDF ; // reset bit 0x0020
  for (cndx=0; cndx<27; ++cndx)
  {
    cst = &(groups[cndx]) ;
    memset(dgt, 0xFF, sizeof(dgt)) ;
    for (int i=0; i<9; ++i)
    {
      int ndx = (*cst)[i] ;
      int cell = grid[ndx] & 0x0F ;
      if (dgt[cell]==0xFF) dgt[cell] = ndx ; else
      {
	grid[ndx]|=0x20 ; // error flag
	grid[dgt[cell]] |= 0x20 ;
      }
    }
  }
  for (cndx=0; cndx<81; ++cndx) if (grid[cndx] & 0x20) err++ ; // reset bit 0x0020
  return err ;
}

//
// Check constraints. return 0 : ok  -  1: conflict found, unstack.
//
int  constraint(unsigned char (*cst)[9])
{
  unsigned int mask = 0xFF80 ;
  unsigned int ndx, cell ;
  int i ;
  for (i=0; i<9; ++i)
  {
    ndx = (*cst)[i] ;
    cell = grid[ndx] & 0x0F ;
    // printf("cst %d -> %hhu cell -> %u \n", i, ndx, cell) ;
    if (cell > 0) 
    {
	    unsigned int bitm = 0x0080<<(cell-1) ;
	   if ((mask & bitm)==0) 
	   { 
	//	   printf("bit %d of %d should not be set!\n", i, ndx) ; 
		   return(1) ;
	   } ;
	   mask ^= bitm ;
	//  printf(" mask -> %02X\n", mask );
    }
  }
  // printf("mask %02X\n", mask );

  for (i=0; i<9; ++i)
  {
    ndx = (*cst)[i] ;
    cell = grid[ndx] & 0x0F ;
    // printf("cst %d -> %hhu cell -> %u \n", i, ndx, cell) ;
    if (cell == 0) 
    {
	   // if ((grid[ndx] & mask) != grid[ndx])  printf("constraint modified cell %d mask %03X\n", ndx, mask);
	   grid[ndx] &= mask ;
    }
  }
  return 0 ;
}

// rule_end
//
int rule_end()
{
  for(int i=0; i<81; ++i)
	  if ((grid[i] & 0xF) == 0) return 0 ;
  return 1 ;
}

// rule0 : if a cell has only one possible, place it.
//  return 0 : no change has ben made
//  return 1 : some cell has been set

int rule0()
{
  unsigned int cell, dg ;
  int hit=0 ;

  for (int i=0; i<81; ++i)
  {
    cell = grid[i] & 0x0F ;
    if (cell==0)
    {
      dg=0 ;
      switch(grid[i] & 0xFF80)
      {
	 case 0x8000 : dg = 9 ; break ;
	 case 0x4000 : dg = 8 ; break ;
	 case 0x2000 : dg = 7 ; break ;
	 case 0x1000 : dg = 6 ; break ;
	 case 0x0800 : dg = 5 ; break ;
	 case 0x0400 : dg = 4 ; break ;
	 case 0x0200 : dg = 3 ; break ;
	 case 0x0100 : dg = 2 ; break ;
	 case 0x0080 : dg = 1 ; break ;
	 case 0x0000 : {
//		         printf("rule0 : cell %d empty : no possible left !\n", i) ;
			 return -1 ;
		         break ;
		       }
      }
      if (dg!=0)
      {
	 grid[i] = dg ;
	 // if (explain) printf("rule0 : cell %d has only one possible %d\n", i, dg) ;
	 if (explain) gdrawcell(i,3) ;
	 hit=1 ;
      }
    }
  }
  return hit ;
}
// rule1 : if a number can only go to 1 place in a constraint, place it.
// return 0 : no change has been made
//        1 : some number was placed, so we made progress
int rule1()
{
  for (int ndx=0; ndx<27; ++ndx)
  {
    unsigned char (*cst)[9] = &(groups[ndx]) ;
    unsigned char hits[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
    unsigned char places[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
    for (int i=0; i<9; ++i)
    {
      unsigned int ndx = (*cst)[i] ;
      unsigned int cell = grid[ndx] & 0x0F ;
      if (cell==0)
      {
	for (int j=0; j<9; ++j)
	if (grid[ndx] & (0x0080 << j))	
	{
	  hits[j]++ ;
	  places[j] = ndx ;
	}
      }
    }
    for (int i=0; i<9; ++i)
	    if (hits[i] == 1)
	    {
		    grid[places[i]] = i+1 ;
		    // if (explain) printf("rule1 found cstr=%d, ndx=%d val=%d\n", ndx, places[i], i+1) ;
	 	    if (explain) gdrawcell(places[i],3) ;
		    return 1 ;
	    }
  }
  return 0 ;
}

// rule2 : if a place can contain only one number, place it.
// return 0 : no change has been made
//        1 : some number was placed, so we made progress
int rule2()
{
  int hit=0 ;
  int hits=0 ;
    for (int ndx=0; ndx<81; ++ndx)
    {
      unsigned int cell = grid[ndx] & 0x0F ;
      if (cell == 0)
      {	 
	 cell = grid[ndx] ;
	 hit = 1 ;
	 switch(cell) 
	 {
  	
	   case 0x8000 : grid[ndx]=9; break ;      
	   case 0x4000 : grid[ndx]=8; break ;      
	   case 0x2000 : grid[ndx]=7; break ;      
	   case 0x1000 : grid[ndx]=6; break ;      
	   case 0x0800 : grid[ndx]=5; break ;      
	   case 0x0400 : grid[ndx]=4; break ;      
	   case 0x0200 : grid[ndx]=3; break ;      
	   case 0x0100 : grid[ndx]=2; break ;      
	   case 0x0080 : grid[ndx]=1; break ;      
	   default : hit=0 ; break ;
	 }
	 if (hit==1) 
	 { 
	   hits=1 ;
	   // if (explain) printf("rule2 found ndx=%d val=%d\n", ndx, grid[ndx]) ;    
	   if (explain) gdrawcell(ndx,3) ;
	}
      }
  }
  return hits ;
}

// rule3 : if two groups overlap, if a number is only present in the common cells, that number cannot be anywhere in the non-common cells.
// return 0 : no change has been made
//        1 : some cells possibles have been reduced, grid has changed.
int rule3g1g2(int g1, int g2)
{
  int hit=0 ;
  unsigned int ndx  ;
  unsigned int S_OR1, S_OR2 ;
  unsigned int CST1[81] ;
  unsigned int CST2[81] ;
  unsigned char (*cst1)[9] = &(groups[g1]) ;
  unsigned char (*cst2)[9] = &(groups[g2]) ;
  memset(CST1, 0, sizeof(CST1)) ;
  memset(CST2, 0, sizeof(CST2)) ;
  for (int i=0; i<9; ++i)
  {
      ndx = (*cst1)[i] ;
      unsigned int cell = grid[ndx] & 0x0F ;
      if (cell==0) CST1[ndx] = grid[ndx] ;
  }
  S_OR1 = 0 ;
  for (int i=0; i<9; ++i)
  {
      ndx = (*cst2)[i] ;
      unsigned int cell = grid[ndx] & 0x0F ;
      if (cell==0) {
	      CST2[ndx] = grid[ndx] ;
	      S_OR1 |= CST1[ndx] ;
      }
  }
  // S_OR contains the OR_sum of all common cases
  S_OR2 = S_OR1 ;
  for (int i=0; i<9; ++i)
  {
      ndx = (*cst1)[i] ;
      unsigned int cell = grid[ndx] & 0x0F ;
      // cell==0 means cell not set, contains possibles; if CST2[N]==0 means not common cell, thus the bits in CST1[] must be removed from S_OR2, bit that will be reset from cst2 at the end 
      if ((cell==0) && (CST2[ndx]==0)) S_OR2 &= ~CST1[ndx] ;
      ndx = (*cst2)[i] ;
      cell = grid[ndx] & 0x0F ;
      if ((cell==0) && (CST1[ndx]==0)) S_OR1 &= ~CST2[ndx] ;
  }
  for (int i=0; i<9; ++i)
  {
      ndx = (*cst1)[i] ;
      unsigned int cell = grid[ndx] & 0x0F ;
      if ((cell==0) && (CST2[ndx]==0)) {
	      // if (explain) if (grid[ndx] & S_OR1) printf("rule3: removing possible(s) from cell %d groups(%d,%d) hex %03X\n", ndx, g1, g2, S_OR1>>7) ;
	      hit |= grid[ndx] & S_OR1 ;
	      grid[ndx] &= ~S_OR1 ;
      }
      ndx = (*cst2)[i] ;
      cell = grid[ndx] & 0x0F ;
      if ((cell==0) && (CST1[ndx]==0)) {
	      // if (explain) if (grid[ndx] & S_OR2) printf("rule3: removing possible(s) from cell %d groups(%d,%d) hex %03X\n", ndx, g1, g2, S_OR2>>7) ;
	      hit |= grid[ndx] & S_OR2 ;
	      grid[ndx] &= ~S_OR2 ;
      }
  }
  return hit ;
}

//
// groups 0-8 overlaps with groups 9-17 over only 1 cell, so this doesn't apply to them.
// We cross only groups 0-17 over 18-26, with 3 cells overlapping in normal sudoku rules.
//
int rule3()
{
   int hit=0 ;
   for (int i=0; i<18; ++i)
	for (int j=18; j<27; ++j)
		hit |= rule3g1g2(i, j) ;
   return hit ;
}
//
// rule4
// For each subset of a constraint, there is a complementary subset. Remove the already set cells from these subsets.
// if a subset contains digits that are not part of the complementary subset, and their number is bigger than the cardinal of the subset, it's a dead end because there is not enough
// room to store all these digits.
// But more than this, if the number of digits is exactly equal to the subset's cardinal, all other digits can be removed from this subset, because there is no room for them.
//

// returns 1 : if cells have been cleaned
//         0 : no change
//         -1 : error dead end detected
int rule4_cst(int g0)
{
  int gset=0 ;
  int gres=0 ;
  int gdigits = 0 ;
  int ndx ;
  unsigned int bndx ;
  unsigned int cell, cell_s, cell_cs ;
  unsigned char (*cst)[9] = &(groups[g0]) ; 
  bndx=0 ;
  for (int i=0; i<=8; ++i)
  {
    ndx = (*cst)[i] ;
    cell = grid[ndx] ;
    if ((cell & 0x0F) != 0)
    {
	 bndx++ ;
	 gset |= 1<<i ;
	 gdigits |= 1 << ((cell & 0x0F)+7-1) ;
    }
  }
  gdigits = (~gdigits) & 0xFF80 ;
  if (bndx>=8) return 0 ; // too many cells already defined for this to be useful.
//  printf("rule4 : constraint %d, gset=%04X, gdigits=%04X\n", g0, gset, gdigits) ;
  for (int sub=1; sub<256; ++sub) // for the first half the subsets
  {
    int csub ;
    if (sub & gset) continue ;
    csub = (0x1FF ^ sub) & ~gset ; // complementary group
    cell_s = cell_cs = 0 ;
    bndx=1 ;
    for (int i=0; i<9; ++i) 
    {
	ndx = (*cst)[i] ;
	cell = grid[ndx] ;
	if (sub & bndx) cell_s |= cell ; 
	if (csub & bndx) cell_cs |= cell ; 
	bndx*=2 ;
    }
    // printf("cell %d (%04X) sub %04X csub %04X cell_s %04X cell_cs %04X\n", ndx, cell, sub, csub, cell_s, cell_cs) ;
    if (cell_s==0 || cell_cs==0 || (cell_s & 0x7F)!=0 || (cell_cs & 0x7F)!=0) continue ;
      cell_s &= gdigits ;
      cell_cs &= gdigits ;
      bndx =  (cell_s & ~cell_cs) ;
      int room = (NBB[sub&0xF] + NBB[sub>>4]) - (NBB[bndx>>12] + NBB[(bndx>>8)&0xF] + NBB[(bndx>>4)&0x8]) ; // cells bits are 8 to 16, 0-7 are for digits aready set; 
      if(room<0) 
      {
//	printf("rule4 dead end pt0 !\n") ;
//        printf("bndx %04X sub %04X csub %04X cell_s %04X cell_cs %04X\n", bndx, sub, csub, cell_s, cell_cs) ;
//	printGridHint() ;
	return -1 ;
      }
#ifdef RULE4_0
      if (room == 0) // exact room to place all digits
      {
    	for (int i=0; i<9; ++i) 
    	{
	  if (sub & (1<<i)) // cell is in the sub ?
	  {
	    ndx = (*cst)[i] ;
	    cell = grid[ndx] & bndx ;
	    if (cell==0) {	
		    // printf("rule4 dead end pt1!\n") ;
		    return -1 ; // dead end
 	    }
	    if (cell != grid[ndx]) 
	    {
		    // if (explain) printf("rule4 found something SUB group %d sub %04X bndx %04X csub %04X cell ndx=%d cell=%04X oldcell=%04X\n", g0, sub, bndx, csub, ndx, cell, grid[ndx]) ;
//	printGridHint() ;
		    gres=1 ;
	    }
            switch(cell)
       	    {
       		case 0x8000 : grid[ndx]=9; break ;
		case 0x4000 : grid[ndx]=8; break ;
        	case 0x2000 : grid[ndx]=7; break ;
        	case 0x1000 : grid[ndx]=6; break ;
        	case 0x0800 : grid[ndx]=5; break ;
        	case 0x0400 : grid[ndx]=4; break ;
        	case 0x0200 : grid[ndx]=3; break ;
        	case 0x0100 : grid[ndx]=2; break ;
        	case 0x0080 : grid[ndx]=1; break ;
        	default : grid[ndx]=cell ; break ;
            }
	  } 
	}
      }
#endif
      bndx =  (cell_cs & ~cell_s) ;
      room = (NBB[csub&0xF] + NBB[(csub>>4)&0xF] + NBB[csub>>8]) - (NBB[bndx>>12] + NBB[(bndx>>8)&0xF] + NBB[(bndx>>4)&0x8]) ; // cells bits are 8 to 16, 0-7 are for digits aready set; 
      if (room<0)
      {
//	printf("rule4 dead end pt2 !\n") ;
//        printf("bndx %04X sub %04X csub %04X cell_s %04X cell_cs %04X\n", bndx, sub, csub, cell_s, cell_cs) ;
//	printGridHint() ;
	return -1 ;
      }
#ifdef RULE4_0
      if (room == 0) // exact room to place all digits
      {
    	for (int i=0; i<9; ++i) 
    	{
	  if (csub & (1<<i)) // cell is in the sub ?
	  {
	    ndx = (*cst)[i] ;
	    cell = grid[ndx] & bndx ;
	    if (cell==0) {	
//		    printf("rule4 dead end pt3!\n") ;
		    return -1 ; // dead end
 	    }
	    if (cell != grid[ndx]) 
	    {
		 // if (explain)   printf("rule4 found something CSUB group %d sub %04X csub %04X bndx %04X cell ndx=%d cell=%04X oldcell=%04X\n", g0, sub, csub, bndx, ndx, cell, grid[ndx]) ;
//	printGridHint() ;
		    gres=1 ;
	    }
            switch(cell)
       	    {
       		case 0x8000 : grid[ndx]=9; break ;
		case 0x4000 : grid[ndx]=8; break ;
       		case 0x2000 : grid[ndx]=7; break ;
       		case 0x1000 : grid[ndx]=6; break ;
       		case 0x0800 : grid[ndx]=5; break ;
       		case 0x0400 : grid[ndx]=4; break ;
       		case 0x0200 : grid[ndx]=3; break ;
       		case 0x0100 : grid[ndx]=2; break ;
       		case 0x0080 : grid[ndx]=1; break ;
       		default : grid[ndx]=cell ; break ;
            }
	  } 
	}
      }
#endif
  }
  return gres ;
}

// returns 1 : if cells have been cleaned
//         0 : no change
//         -1 : error dead end detected
int rule4()
{ 
  int gres=0 ;	
  for (int ndx=0; ndx<27; ++ndx)
  {
    int res = rule4_cst(ndx) ;
    if (res<0) return res ;
    gres |= res ;
  }
  return gres ;
}

//
//
int stack_push()
{
	unsigned int nb_dg[9] ;
    unsigned int nbb0, cell0, cellN  ; 
//
	//	printf("STACKPUSH : \n") ; printGridHint() ;
	       // couldn't find a solution with rules, need to guess and stack things 
		memcpy(tree[STACK_PTR], grid, sizeof(grid)) ;
		Ctree[STACK_PTR] = 0 ;
		if (++STACK_PTR >= 81)
		{
		  printf("STACK_PTR overflow\n") ;
		  exit(3) ;
		};
		max_stack_total++ ;
		if (max_stack<STACK_PTR) max_stack=STACK_PTR ;
		memset(nb_dg, 0, sizeof(nb_dg)) ;
		for(int i=0; i<81; ++i) 
		{
		  unsigned int cell = grid[i] ;
		  if ((cell & 0x0F) == 0)
		  {
		    if (cell & 0x8000) nb_dg[8]++ ;
		    if (cell & 0x4000) nb_dg[7]++ ;
		    if (cell & 0x2000) nb_dg[6]++ ;
		    if (cell & 0x1000) nb_dg[5]++ ;
		    if (cell & 0x0800) nb_dg[4]++ ;
		    if (cell & 0x0400) nb_dg[3]++ ;
		    if (cell & 0x0200) nb_dg[2]++ ;
		    if (cell & 0x0100) nb_dg[1]++ ;
		    if (cell & 0x0080) nb_dg[0]++ ;
		  } 
		}
		// printf("number of digits : ") ;
		// for (int i=0; i<9; ++i) printf(" %d=>%d ", i+1, nb_dg[i]) ;
		// printf("\n") ;
		nbb0 = 0xFFFF ;
		cell0 = 0xFFFF ;
		for(int i=0; i<81; ++i) 
		{
		  unsigned int cell = grid[i] ;
		  unsigned int nbb = 0 ;
		  if ((cell & 0x0F) == 0) 
		  {
		    if (cell & 0x8000)  nbb += nb_dg[8] ; 
		    if (cell & 0x4000)  nbb += nb_dg[7] ; 
		    if (cell & 0x2000)  nbb += nb_dg[6] ; 
		    if (cell & 0x1000)  nbb += nb_dg[5] ; 
		    if (cell & 0x0800)  nbb += nb_dg[4] ; 
		    if (cell & 0x0400)  nbb += nb_dg[3] ; 
		    if (cell & 0x0200)  nbb += nb_dg[2] ; 
		    if (cell & 0x0100)  nbb += nb_dg[1] ; 
		    if (cell & 0x0080)  nbb += nb_dg[0] ; 
		    if (nbb < nbb0)
		    {
			  nbb0 = nbb ;
			  cell0 = i ;
		    }
		  }
		}
		if (cell0==0xFFFF) {
			// printf("No more possible guesses\n") ;
			STACK_PTR-- ;
			exit(1) ;
		} else {
		  Ltree[STACK_PTR-1] = cell0 ;
		  Ctree[STACK_PTR-1] = 0 ;
		  nbb0=0 ;
		  for (int i=0; i<81; ++i) if (grid[i]&0xF!=0) nbb0++ ;
		  Ztree[STACK_PTR-1] = nbb0 ;
		}
	//	printf("STACKPUSH : Ctree : ") ; for (int i=0; i<16; ++i) printf("%d:%d ", i, Ctree[i]) ; printf("\n") ;
	//	printf("STACKPUSH : Ltree : ") ; for (int i=0; i<16; ++i) printf("%d:%d ", i, Ltree[i]) ; printf("\n") ;
		return cell0 ;
}
//
//
//
int stack_next(unsigned int cell0, unsigned int curGuess)
{
	unsigned int nextGuess = 0xFFFF;
	// printf("STACKNEXT : Time to guess : cell %d contains %04X at level %d previous guess=%d\n", cell0, grid[cell0], STACK_PTR,curGuess ) ;
	//if (STACK_PTR==0) {
	//	printf("ERROR: calling STACKNEXT without STACK_PUSH\n") ;
	//	return 0xFFFF ;
	// }
	// if (curGuess!=0) 
  		memcpy(grid, tree[STACK_PTR-1], sizeof(grid)) ;
		for (int i=0; i<9; ++i) 
		{
			unsigned int cellN = grid[cell0] ;
			if (((cellN >> i) & 0x0080) && ((i+1)>curGuess))
			{
			  nextGuess = i+1 ;
			  grid[cell0] = nextGuess ;
			  Ctree[STACK_PTR-1] = nextGuess ;
			  if (explain) gdrawcell(cell0,2) ;
			  // if (explain) printf("Guessing %d in cell %d at level %d\n", nextGuess, cell0, STACK_PTR) ;
			  break ; 
			}
		
		}
		/* if ((grid[cell0] & 0xF) ==0)
		{
			// printf("cell empty : no more guesses\n") ;
			// STACK_PTR-- ;
			// return 0xFFFF ;
		}*/
		// break ;
		return nextGuess ;
}
//
int stack_pop()
{
  Ctree[STACK_PTR] = 0 ;
  if (STACK_PTR==0) return 0 ;
  // if (explain) printf("STACKPOP : Backtracking stack_ptr=%d, ... prev cell=%d prev value=%d\n", STACK_PTR-1, Ltree[STACK_PTR-1],Ctree[STACK_PTR-1]) ;
  STACK_PTR-- ;
  memcpy(grid, tree[STACK_PTR], sizeof(grid)) ;
	//	printf("STACKPOP : Ctree : ") ; for (int i=0; i<16; ++i) printf("%d:%d ", i, Ctree[i]) ; printf("\n") ;
	//	printf("STACKPOP : Ltree : ") ; for (int i=0; i<16; ++i) printf("%d:%d ", i, Ltree[i]) ; printf("\n") ;
  return 1 ; // ok
}
//
//
//

void printNbSol(int nb)
{
  if (nb==0) printf("No"); else if (nb==1) printf("one"); else printf("multiple") ;
  printf(" solution") ;
  if (nb>1) printf("s") ;
  printf(" found\n") ;
}

void reset()
{
	STACK_PTR=0 ;
	max_stack=max_stack_total=0 ;
	memset(tree, 0, sizeof(tree)) ;
	memset(Ctree, 0, sizeof(Ctree)) ;
	memset(Ztree, 0, sizeof(Ctree)) ;
}

int solve(int findall, int savefirst, int limit)
{
    unsigned int end, curGuess, cell0, nbSol ; 
//
	reset() ;
        for (int j=0; j<81; ++j) if ((grid[j]&0xF)==0) grid[j]=0xFF80 ; // reset constaints
	nbSol = 0 ;
	curGuess = 0 ; // 0 : no guesses yet  - 1 to 9 : current guess - 0xFFFF : no more available guesses in the current cell
	while(1) 
	{
	    int ret ;
	    // if (max_stack_total > 30) return 0 ;
	    end=0 ;
	    for(int c=0; c<27; ++c) if (constraint(&(groups[c]))!=0) 
	    {
	      end = 2 ;
	    } ;
	    if (end!=0)
	    {
	    } else if (rule_end()) 
	    {
	//	end=1 ;
	  //	printGridHint() ;
	  	if ((nbSol==0)&&(savefirst>0)) // save first solution
		{
	  	  memcpy(sol_grid, grid, sizeof(grid)) ;
    		  memcpy(sol_Ztree, Ztree, sizeof(grid)) ;
		}
	  	if (nbSol==0)
		{
//		  printGrid() ;
		  //if (explain)  printf("Found. max stack=%d, total guesses=%d nbSol=%d\n", max_stack, max_stack_total, nbSol+1 ) ;
		  //explain=0 ;
		}
		nbSol++ ;
		if ((STACK_PTR==0) || (findall==0) || (nbSol>=2) ) 
		{
		  // printf("Found1. max stack=%d, total guesses=%d nbSol=%d\n", max_stack, max_stack_total, nbSol ) ;
		  return (nbSol) ;
		}
		end=2 ;
	    } else if (ret=rule0())
  	    {
 	      if (ret<0) 
	      {
		end=2 ;
	      }
	    } else if (rule1())  
	    { 
	      // printf("RULE1 found something\n") ;
	    } else if (rule2())
            { 
	    } else if (rule3())
            { 
	    } else if (ret=rule4())
	    {
		    if (ret<0)
		    {
		      end = 2 ; // dead end, backtrack.
		    } else {
		      // situation evolved, go on.
		    }
            } else {
	      end=3 ;
	    }
	    if (end)
            {
	   //    printf("STACKING LOGIC end=%d\n", end) ;
	       if (STACK_PTR==0 && end==2) end = 4 ; // can't pop if stack never moved.
	       while(end!=0 && end!=4)
	       {
	         // printGridHint() ;
		  if (end==3)
		  { 
//printf("stacking logic STACKPTR=%d  curGuess=%d\n", STACK_PTR, curGuess) ;
		 //  curGuess = Ctree[STACK_PTR] ;
		    // if (curGuess==0) 
			cell0 = stack_push() ;
		    // curGuess = Ctree[STACK_PTR-1] ; // may change
		    curGuess =  0 ;
		  }
		  if ((curGuess = stack_next(cell0, curGuess))==0xFFFF) 
		  {
	//	    printf("stacking logic end=%d, curGuess=%d, stack_ptr=%d\n", end, curGuess, STACK_PTR) ;
		    end=0 ;
	            if (limit && ((max_stack_total-STACK_PTR)>15)) return -1 ;
	            // couldn't find a solution - need to backtrack from tree or giving up
	  	    if (stack_pop() == 0) 
	  	    {
		      // if (nbSol==0) printf("No solution\n") ; 
		      memcpy(grid, sol_grid, sizeof(grid)) ;
//	printf("NOT Found2. max stack=%d, total guesses=%d nbSol=%d\n", max_stack, max_stack_total, nbSol ) ;
		      return (nbSol) ;
		      // end=4 ;
		      // break ; // no more
	  	    } else end=5 ; // loop
	  	    curGuess = Ctree[STACK_PTR-1] ;
	  	    cell0 = Ltree[STACK_PTR-1] ;
// printf("stacking logic settingi after POP : STACKPTR=%d cell0=%d  curGuess=%d\n", STACK_PTR, cell0, curGuess) ;
		    
		  } else end=0 ; 
		}
	      } 
        //  Ctree[STACK_PTR] = 0 ;
	//  printGridHint() ;
	      if (end==4) break ;
	}
	// printGridHint() ;
	// printf("Found2. max stack=%d, total guesses=%d nbSol=%d\n", max_stack, max_stack_total, nbSol ) ;
        memcpy(grid, sol_grid, sizeof(grid)) ;
	return (nbSol) ;
}
//

void exch_lng(unsigned int *grid, int a, int b) 
{
	if ((a>=0)&&(a<9)&&(b>=0)&&(b<9)&&(a!=b))
	{
		for(int i=0; i<9; ++i)
		{
			int ndxa = 9*a+i ;
			int ndxb = 9*b+i ;
			unsigned int temp=grid[ndxa] ;
			grid[ndxa]=grid[ndxb] ;
			grid[ndxb]=temp ;
		}
	}
}

void exch_col(unsigned int *grid, int a, int b) 
{
	if ((a>=0)&&(a<9)&&(b>=0)&&(b<9)&&(a!=b))
	{
		for(int i=0; i<9; ++i)
		{
			int ndxa = 9*i+a ;
			int ndxb = 9*i+b ;
			unsigned int temp=grid[ndxa] ;
			grid[ndxa]=grid[ndxb] ;
			grid[ndxb]=temp ;
		}
	}
}

void exch_grp_lng(unsigned int *grid, int grp1, int grp2)
{
	exch_lng(grid, 3*grp1+0, 3*grp2+0) ;
	exch_lng(grid, 3*grp1+1, 3*grp2+1) ;
	exch_lng(grid, 3*grp1+2, 3*grp2+2) ;
}

void exch_grp_col(unsigned int *grid, int grp1, int grp2)
{
	exch_col(grid, 3*grp1+0, 3*grp2+0) ;
	exch_col(grid, 3*grp1+1, 3*grp2+1) ;
	exch_col(grid, 3*grp1+2, 3*grp2+2) ;
}

void mix_lng(unsigned int *grid, int grp)
{
	switch(rand()%6)
	{
	   case 0 : break ;
	   case 1 : exch_lng(grid, grp+1, grp+2) ;
		    break ;
	   case 2 : exch_lng(grid, grp+0, grp+1) ;
		    break ;
	   case 3 : exch_lng(grid, grp+0, grp+1) ;
		    exch_lng(grid, grp+1, grp+2) ;
		    break ;
	   case 4 : exch_lng(grid, grp+0, grp+2) ;
		    break ;
	   case 5 : exch_lng(grid, grp+0, grp+2) ;
		    exch_lng(grid, grp+1, grp+2) ;
		    break ;
	}
}

void mix_col(unsigned int *grid, int grp)
{
	switch(rand()%6)
	{
	   case 0 : break ;
	   case 1 : exch_col(grid, grp+1, grp+2) ;
		    break ;
	   case 2 : exch_col(grid, grp+0, grp+1) ;
		    break ;
	   case 3 : exch_col(grid, grp+0, grp+1) ;
		    exch_col(grid, grp+1, grp+2) ;
		    break ;
	   case 4 : exch_col(grid, grp+0, grp+2) ;
		    break ;
	   case 5 : exch_col(grid, grp+0, grp+2) ;
		    exch_col(grid, grp+1, grp+2) ;
		    break ;
	}
}

void mix_grp_lng(unsigned int *grid, int grp)
{
	switch(rand()%6)
	{
	   case 0 : break ;
	   case 1 : exch_grp_lng(grid, grp+1, grp+2) ;
		    break ;
	   case 2 : exch_grp_lng(grid, grp+0, grp+1) ;
		    break ;
	   case 3 : exch_grp_lng(grid, grp+0, grp+1) ;
		    exch_grp_lng(grid, grp+1, grp+2) ;
		    break ;
	   case 4 : exch_grp_lng(grid, grp+0, grp+2) ;
		    break ;
	   case 5 : exch_grp_lng(grid, grp+0, grp+2) ;
		    exch_grp_lng(grid, grp+1, grp+2) ;
		    break ;
	}
}

void mix_grp_col(unsigned int *grid, int grp)
{
	switch(rand()%6)
	{
	   case 0 : break ;
	   case 1 : exch_grp_col(grid, grp+1, grp+2) ;
		    break ;
	   case 2 : exch_grp_col(grid, grp+0, grp+1) ;
		    break ;
	   case 3 : exch_grp_col(grid, grp+0, grp+1) ;
		    exch_grp_col(grid, grp+1, grp+2) ;
		    break ;
	   case 4 : exch_grp_col(grid, grp+0, grp+2) ;
		    break ;
	   case 5 : exch_grp_col(grid, grp+0, grp+2) ;
		    exch_grp_col(grid, grp+1, grp+2) ;
		    break ;
	}
}
/*
void mix_symH(unsigned int *grid) 
{
  if ((rand()%2)==0)
  {
    for(int x=0; x<=3; ++x) 
	    for (int y=0; y<9; ++y)
	    {
		    int ndx = (8-x)+9*y ; 
		    int a=grid[x+9*y] ;
		    grid[x+9*y]=grid[ndx] ;
		    grid[ndx]=a ;
	    }
  }
}

void mix_symV(unsigned int *grid) 
{
  if ((rand()%2)==0)
  {
    for(int y=0; y<=3; ++y) 
	    for (int x=0; x<9; ++x)
	    {
		    int ndx = x+9*(8-y) ; 
		    int a=grid[x+9*y] ;
		    grid[x+9*y]=grid[ndx] ;
		    grid[ndx]=a ;
	    }
  }
}
*/

void mix_rotate45(unsigned int *grid)
{
   int c ;
   for (int x=0; x<=4; ++x)
     for (int y=0; y<4; ++y)
     {
	 c = grid[x+9*y] ;
	  grid[x+9*y] = grid[y+9*(8-x)] ;
	  grid[y+9*(8-x)] = grid[8-x+9*(8-y)] ;
	  grid[8-x+9*(8-y)] = grid[8-y+9*x] ;
	  grid[8-y+9*x] = c ;
     }
}
void mix_rotate(unsigned int *grid)
{
  int nb = rand() %4 ;
  if (nb==0) return ;
  for (int i=0; i<nb; ++i) mix_rotate45(grid) ;
}

void mix_digits(unsigned int *grid)
{
	int i ;
	int mixc[10] ;
	for (i=0; i<10; ++i) mixc[i]=0 ;
	for (i=1; i<=9; ++i)
	{
		int ndx = 1+rand()%9 ;
		while(mixc[ndx]!=0) { ndx++ ; if (ndx>9) ndx=1 ; }
		mixc[ndx]=i ;
	}
	for(i=0; i<81; ++i) if ((grid[i]&0xF)!=0) 
	{
		int ndx=grid[i]&0xF ;
		grid[i] = mixc[ndx] | 0x10 ;
	}
}

void mixer(unsigned int *grid)
{
    if (type_puzzle==0) {
	mix_lng(grid, 0) ;
	mix_lng(grid, 3) ;
	mix_lng(grid, 6) ;
	mix_col(grid, 0) ;
	mix_col(grid, 3) ;
	mix_col(grid, 6) ;
	mix_grp_lng(grid, 0) ;
	mix_grp_lng(grid, 3) ;
	mix_grp_lng(grid, 6) ;
	mix_grp_col(grid, 0) ;
	mix_grp_col(grid, 3) ;
	mix_grp_col(grid, 6) ;
    }
    mix_rotate(grid) ;
    mix_digits(grid) ;
}

int translate_grid(char *sgrd, unsigned int *grd)
{
	int ptr=0 ;
	for (int i=0; i<200; ++i)
	{
	  char c0 = sgrd[i] ;
	  if (c0==0) return 0 ;	  
	  if ((c0>='1') && (c0<='9')) grd[ptr++] = 0x10|(unsigned int)(c0-'0') ; else 
          if ((c0=='.') || (c0=='0')) grd[ptr++] = 0 ;  
	  if (ptr>80) return 1 ;
	}
	return 0 ;
}

#ifdef MAINS

int main(int argc, char **argv)
{
    int nb ;
    char buf[200] ;
    //
    explain=1 ;
    printf("Fsudoku\n") ;
    srand(time(NULL)) ;
    nb=100 ;
    while(rand()<0x100) if (nb--<=0) break ;
    while (fgets(buf, sizeof(buf)-1, stdin))
    { 
	if (strlen(buf)<81) return 1 ;
	// printf("line from input file\n%s", buf) ;
	if (translate_grid(buf, grid)) return 0 ;
        printGrid() ;
	// mixer(grid) ; printGrid() ;
	printDefs(3) ;
	nb = solve(1, 1, 0) ;
	printNbSol(nb) ;
        printGrid() ;
	printf("max stack=%d, total guesses=%d \n", max_stack, max_stack_total ) ;
    }
    return 0 ;
}

#endif

//
//
int gener_rnd(unsigned int c)
{
   int x, y ;
   int end=1000 ;

   c &=0xF ;
   y = 1 << 6+c ;
   while(end)
   {
     x = rand() % 81 ;
     // printf("%d ", x) ;
     if (--end<=0) return -1 ;
     if (grid[x]&y) end=0 ;
   }
   grid[x] = c | 0x10;
   for(int c=0; c<27; ++c) if (constraint(&(groups[c]))!=0) 
   {
      printf("Internal error in gen\n") ;
      exit(1) ;
   } ;
   return 0 ;
}


int generate(int type, int difficulty)
{
  unsigned int c ;
  int  err ;
    reset() ;
    // printf("\n") ;
    for (int i = 0; i<sizeof(grid); ++i) grid[i] = 0xFF80 ;
  // generate 17 random digits, and make sure at least 8 are used. A puzzle must not have two or more unused digits or else it has multiple solutions.
  // Also, a minimum puzzle has 17 digits, oe else it has either no solution or multiple ones.
    for (int i=1; i<=8; ++i) {
	  if (gener_rnd(i)<0) return -1 ;
    }
    for (int i=1; i<=9; ++i) {
	c = 1 + rand() % 9 ;
	err=1 ;
 	for (int j=0; j<9; ++j)
	{
	  if (gener_rnd(c)>=0) { err=0 ; break ; } 
	  if (++c>9) c=1 ;
	}
	if (err) return -1 ;
    }
    // printf("\n") ;
    err=0 ;
    for(int ndx=0; ndx<81; ++ndx) if ((grid[ndx]&0xF)!=0) err++ ;
    if (err!=17)
    {
	    printf("WTF? nb=%d\n", err);
	    printGrid() ;
	    exit(1) ;
    }
    return 0 ;
}

unsigned int sav_grid[81] ;

// int main(int argc, char *argv[]) {

int generate_puzzle() {
    int nb, nbb, nbl ;

    explain=1 ;
    nb=-1 ;
    while(nb<=0) 
    {
      while(generate(0, 0)<0) ;
    // printGrid() ;
      printInfo("Solving the random puzzle") ;
      memcpy(sav_grid, grid, sizeof(grid)) ;
      for (int i=0; i<81; ++i) gdrawcell(i,0) ;
      nb=solve(1, 1, 1) ;
      if (nb<=0) printInfo("Too bad distrbution, reshuffling ...") ;
    }
    printInfo("Adding hints to remove multiple solutions") ;

    nbl=2 ;
    while((nb>1) &&(nbl>0))
    {
      nbb=0 ; 
        for (int i=0; i<STACK_PTR; ++i)
        {
   	   if (Ztree[i+1]-Ztree[i]>=nbl) {
		   nbb++ ;
		   sav_grid[Ltree[i]]  =  grid[Ltree[i]] ;
  	   }
	   // printf("Level %d : Ctree %d Ltree %d (%d/%d)i Ztree %d\n", i, Ctree[i], Ltree[i], Ltree[i]/9, Ltree[i]%9, Ztree[i]) ;
        }
      if ((nbb==0)&&(nbl>0)) nbl-- ; else
      {
        memcpy(grid, sav_grid, sizeof(grid)) ;
        reset() ;
        // printGrid() ;
        nb=solve(1, 0, 1) ;
 //        printNbSol(nb) ;
      }
      // if (nb==0) exit(0) ;
    }
    if (nb<0) return -1 ;
    //
    // simplify it
    //
    // printf("simplify\n") ;
    if (diff_puzzle>2)
    {
      printInfo("Trying to make it harder by removing hints") ;
     for (int i=0; i<81; ++i) if ((sav_grid[i]&0xF)!=0)
      {
        memcpy(grid, sav_grid, sizeof(grid)) ;
        grid[i] = 0xFF80 ;
        nb = solve(1, 0, 0) ;
        // if ((nb==1)&&(max_stack_total==0))
        if (nb==1)
        {
          // printf("simplify i=%d nb=%d\n", i, nb) ;
 	  sav_grid[i] = 0xFF80 ;
        }
      }
    }
    for (int i=0; i<81; ++i) gdrawcell(i,0) ;
    //
    // and complexify it
    //
    /* 
    for (int i=0; i<81; ++i) if ((sav_grid[i]&0xF)!=0)
    {
      memcpy(grid, sav_grid, sizeof(grid)) ;
      grid[i] = 0xFF80 ;
      nb = solve(1, 0, 0) ;
      if (nb==1)
      {
         printf("complexify i=%d nb=%d\n", i, nb) ;
 	 sav_grid[i] = 0xFF80 ;
      }
    }
    */
    if (diff_puzzle>2)
    {
      memcpy(grid, sav_grid, sizeof(grid)) ;
      nb=solve(1, 0, 0) ;
    }
    // printGrid() ;
    memcpy(grid, sav_grid, sizeof(grid)) ;
    // printGrid() ;
    nbb=0 ;
    for (int i=0; i<81; ++i) if ((grid[i]&0xF)!=0) { nbb++ ; grid[i] |= 0x10 ; } else grid[i]=0 ;
    printInfoInt("This puzzle has %d Hints ", nbb) ;
    mixer(grid) ; 
    memcpy(sav_grid, grid, sizeof(grid)) ;
    for (int i=0; i<81; ++i) gdrawcell(i,0) ;
    nb=solve(0, 1, 0) ;
    if (nb==1) solution_ok=1 ; else solution_ok=0 ;
    // explain=0 ;
    memcpy(grid, sav_grid, sizeof(grid)) ;
    return solution_ok ;
}

 
void reset_constraints(int force) {
    int i ;
    if (force) for (int i=0; i<81; ++i) if ((grid[i]&0xF)==0) grid[i]=0xFF80 ;
    for(int i=0; i<27; ++i) constraint(&(groups[i]))  ;
}
