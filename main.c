#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define VERSION " Version 1.1 2023/09   "
#define TGT_VERSION     " Version 1.1RC2 23/09  "

extern int generate_puzzle() ;
extern int solve(int findall, int savefirst, int limit) ;
extern void mixer(unsigned int *grid) ;
extern void GoToTypeGroups() ;
extern int const_errors() ;
extern void reset_constraints(int force) ;
extern int translate_grid(char *sgrd, unsigned int *grd) ;
extern int explain ;
extern int type_puzzle ; // 0=classic 1=aztec 2=jigsaw
extern int diff_puzzle ; // 0=easiest ... 4=hardest
extern unsigned int grid[81] ;
extern unsigned int sol_grid[81] ;
extern unsigned int sav_grid[81] ;

unsigned int notes[81] ;
char notes_in = 0 ;
char solution_ok = 0 ;
char error_type=0 ; // 0=no error, 1=sudoku errors, 2=all errors
int last_errors = 0 ;
int current_type_puzzle = 0 ;
char info[81] ;
char *grid0 = "..84...3....3.....9....157479...8........7..514.....2...9.6...2.5....4......9..56\n" ;
char *grid0S= "518476239427359618963821574795248361832617945146935827379564182651782493284193756\n" ;
unsigned char gener_in_process = 0 ;
int target=0 ; // target constant to change, i.e. colors of the grid
int grid_bg = 0x80 ;
int cell_bg0 = 0x88 ;
int cell_bg1 = 0x84 ;
int cell_bg2 = 0x83 ;
int cell_bg3 = 0x89 ;
int cell_fg0 = 0 ;
int cell_fg1 = 9 ;
int cell_fg2 = 0 ;
int cell_fg3 = 4 ;
int grid_fg0 = 0 ;
int grid_fg1 = 1 ;
#define grid_bg00  (0x80) 
#define grid_bg10  (0x80) 
#define grid_fg00  (0) 
#define grid_fg01  (9) 
#define grid_fg10  (7)
#define grid_fg11  (0) 
#define menu_fg00  (11)
#define menu_bg00  (0x84)

int curPosX = 0 ;
int curPosY = 0 ;

char *patterns[] =
{
	"001001001", 
	"223223223",

	"012000301",
	"201203003",
	"120332031",
	"013001301",
	"003001201",
	"031223121",
	"300312013",
	"003001201",
	"010000100",

	"001201201",
	"020300301",
	"313223203",
	"001001131",
	"021001001",
	"313223203",
	"003003131",
	"012012001",
	"001001001"
} ;

#define max_stack 41
int undo_stack_ptr, max_stack_ptr, min_stack_ptr ;
int undo_stack[81*max_stack] ;

#define bias_X 3
#define bias_XM 17
#define bias_Y 4

void rst18l(char *p, int len) __preserves_regs(IX,IY,DE) 
{
#asm
	ld	hl,2
	add	hl,sp
	push	de
	ld	c,(hl)
	inc	hl
	ld 	b,(hl)
	inc	hl
	ld	e,(hl)
	inc 	hl
	ld	d,(hl)
	ex	de,hl
	pop	de
	push	iy
	push	ix
	defb	0x49
	rst	0x18
	pop	ix
	pop	iy
#endasm
}

int moscall(int cde) __preserves_regs(IX,IY,DE) 
{
#asm
	ld	hl,2
	add	hl,sp
	ld 	a,(hl)
	push	de
	push	iy
	push	ix
	defb	0x49
	rst	0x08
	pop	de
	pop	ix
	pop	iy
	ld	l,a
	ld	h,0
#endasm
}

int moscall8() __preserves_regs(IX,IY,DE) 
{
#asm
	push	de
	push	iy
	push	ix
	ld 	a,0x08
	defb	0x49
	rst	0x08
	defb	0x49
	ld 	a,(IX+17h) ;
	ld 	h,a
	defb	0x49
	ld 	a,(IX+06h) ;
	pop	de
	pop	ix
	pop	iy
	ld	l,a
#endasm
}

int moscall8_getpos() __preserves_regs(IX,IY,DE) 
{
#asm
	
	push	de
	push	iy
	push	ix
	ld 	a,0x08
	defb	0x49
	rst	0x08
	defb	0x49
	ld 	a,(IX+07h) ;
	ld 	h,a
	defb	0x49
	ld 	a,(IX+08h) ;
	pop	de
	pop	ix
	pop	iy
	ld	l,a
#endasm
}

void GoToType()
{
  GoToTypeGroups() ;
  if (type_puzzle==0)
  {
    grid_fg0 = grid_fg00 ;
    grid_fg1 = grid_fg01 ;
    grid_bg = grid_bg00 ;
  } else {
    grid_fg0 = grid_fg10 ;
    grid_fg1 = grid_fg11 ;
    grid_bg = grid_bg10 ;
  }
}

void cursor(int on)
{
  char *c="\x17\x01\x01" ;
  c[2] = (char)on ;
  rst18l(c, 3) ;
}

void CLR_EOL()
{
 /*
  int n ;
  unsigned int pos, pos1 ;
  rst18l("\x17\x00\x82", 3) ;
  pos  = moscall8_getpos() ;
  pos1 = pos ;
  n = 64-(pos1 >> 8) ;
  for (; n>0; --n) rst18l(" ", 1) ;
  */
}

void color(int col, int bkcol)
{
  char *c="\x11\x01\x11\x01" ;
  c[1] = (char)col ;
  c[3] = (char)bkcol ;
  //printf("\x11%c\x11%c",  (unsigned char)col, (unsigned char)bkcol) ;
  rst18l(c, 4) ;
}

void tab(int colX, int colY)
{
	char *ini = "\x1F\x01\x01" ;
	ini[1] = (char)colX ;
	ini[2] = (char)colY ;
	rst18l(ini,3) ;
}
void drawline0(int colX, colY)
{
	color(grid_fg0,grid_bg) ;
	tab(colX, colY) ;
	rst18l("/---", 4) ;
	for (int i=0; i<8; ++i) rst18l("+---", 4) ;
	rst18l("\\",1) ;
}

char * get_pattern(int lg, int ndx)
{
  int nx ;
  if (ndx==0) 
  {
    if (((lg+1)%3)==0) nx=1 ; else nx = 0 ;
    return patterns[nx] ;
  } else {
    return patterns[lg+2+9*(ndx-1)] ;
  }
}

void drawlineM(int colX, int colY, int lg, int ndx)
{
	char *pt = get_pattern(lg, ndx) ;
	char c ;
	tab(colX, colY) ;
	for (int i=0; i<9; ++i) {
  	if (i==0) c='1' ; else c =*(pt+i-1) ;
	if (c&1)
			color(grid_fg0, grid_bg) ; else
			color(grid_fg1, grid_bg) ;
	  rst18l("|   ", 4) ;
	}
	color(grid_fg0,grid_bg) ;
	rst18l("|",1) ;
}

void drawline1(int colX, int colY, int lg, int ndx)
{
	char c, c1 ;
	char *pt = get_pattern(lg, ndx) ;
	tab(colX, colY) ;
	for (int i=0; i<9; ++i) {
  	   c =*(pt+i) ;
	   if (i==0) c1='3' ; else c1 = *(pt+i-1) ;
	   if (((c1&3)!=0)||((c&2)!=0)) 
			color(grid_fg0, grid_bg) ; else
			color(grid_fg1, grid_bg) ; 
	   rst18l("+",1) ;
	   if ((c&2)!=0) 
			color(grid_fg0, grid_bg) ; else
			color(grid_fg1, grid_bg) ; 
	   rst18l("---", 3) ;
		  // rst18l("+\x11\x01---", 6) ;
	}
	color(grid_fg0, grid_bg) ;
	rst18l("+",1) ;
}

void drawemptygrid()
{
	int colX = bias_X ;
	int colY = bias_Y ;
	color(15,0x80) ;
	rst18l("\x0c", 1) ;
	tab(bias_X+4, bias_Y-2) ;
	color(menu_fg00, menu_bg00) ;
	printf(" Puzzle  ") ;
	switch(type_puzzle)
	{
	  case 0 : printf("C l a s s i c ") ; break ;
	  case 1 : printf("A z t e c ") ; break ;
	  case 2 : printf("J i g s a w ") ; break ;
	}
	color(15, grid_bg) ;
	drawline0(colX, colY) ; colY++ ;
	color(grid_fg1, grid_bg) ;
	for (int i=0; i<9; ++i) {
		for (int j=0; j<3; ++j) { // printf("X%dX", j) ;
			drawlineM(colX, colY, i, type_puzzle) ;
			colY++ ;
		}
		if (i!=8) {
		  drawline1(colX, colY, i, type_puzzle) ;
		  colY++ ;
		}
	}
	tab(colX, colY) ;
	color(grid_fg0, grid_bg) ;
	rst18l("\\---", 4) ;
	for (int i=0; i<8; ++i) rst18l("+---", 4) ;
	rst18l("/",1) ;
	color(15,0x80) ;
}

void pr_error_type()
{
	tab(bias_X+4*9+15, bias_Y+15);
	color(menu_fg00, menu_bg00) ;
	switch(error_type) {
		case 0 : printf("None  ") ; break ;
		case 1 : printf("Sudoku") ; break ;
		case 2 : printf("All   ") ; break ;
	}
}

void pr_item(int Y)
{
	tab(bias_X+4*9+11, bias_Y+Y);
	color(menu_fg00, menu_bg00) ;
	switch(target) {
		case 0 : printf("grid BG  ") ; break ;
		case 1 : printf("cell BG0 ") ; break ;
		case 2 : printf("cell BG1 ") ; break ;
		case 3 : printf("cell BG2 ") ; break ;
		case 4 : printf("cell BG3 ") ; break ;
		case 5 : printf("cell FG0 ") ; break ;
		case 6 : printf("cell FG1 ") ; break ;
		case 7 : printf("cell FG2 ") ; break ;
		case 8 : printf("cell FG3 ") ; break ;
		case 9 : printf("grid FG0 ") ; break ;
		case 10: printf("grid FG1 ") ; break ;
	}
}

void setCursor()
{
	tab(bias_X+2+4*curPosX, bias_Y+2+4*curPosY) ; 
}

void drawMainMenu()
{
	char *bs = "                       " ;
	 color(menu_fg00, menu_bg00) ;
	 tab(bias_X+bias_XM, bias_Y-1) ; printf(bs+8) ;
	 tab(bias_X+bias_XM, bias_Y) ;   printf(" F S U D O K U ") ; CLR_EOL() ;
	 tab(bias_X+bias_XM, bias_Y+1) ; printf(bs+8) ; 
	 tab(bias_X+bias_XM+17, bias_Y+1) ; printf(" for AGON Light ") ; CLR_EOL() ;
	 tab(bias_X+bias_XM, bias_Y+5) ; printf(" -Main Menu") ; CLR_EOL() ;
	 tab(bias_X+bias_XM, bias_Y+6) ; printf(bs) ; 
	 tab(bias_X+bias_XM, bias_Y+7) ; printf("T     : puzzle style    ") ; 
	 switch(type_puzzle)
	 {
	   case 0 : printf("Classic  ") ; break ;
	   case 1 : printf("Aztec    ") ; break ;
	   case 2 : printf("Jigsaw   ") ; break ;
	 }
	 tab(bias_X+bias_XM, bias_Y+9) ; printf("D     : difficulty      ") ; CLR_EOL() ;
	 switch(diff_puzzle)
	 {
	   case 0 : printf("Easy     ") ; break ;
	   case 1 : printf("Moderate ") ; break ;
	   case 2 : printf("Normal   ") ; break ;
	   case 3 : printf("Hard     ") ; break ;
	   case 4 : printf("Hellish  ") ; break ;
	 }
	 tab(bias_X+bias_XM, bias_Y+8) ; printf(bs) ; 
	 tab(bias_X+bias_XM, bias_Y+10) ; printf(bs) ; 
	 tab(bias_X+bias_XM, bias_Y+11)  ; printf("L     : new Lib. game  ") ; CLR_EOL() ;
	 tab(bias_X+bias_XM, bias_Y+12) ; printf(bs) ; 
	 tab(bias_X+bias_XM, bias_Y+13) ; printf("G     : Generate puzzle") ; CLR_EOL() ;
	 tab(bias_X+bias_XM, bias_Y+14) ; printf(bs) ; 
	 tab(bias_X+bias_XM, bias_Y+15) ; printf("E     : Edit own puzzle") ; CLR_EOL() ;
	 tab(bias_X+bias_XM, bias_Y+16) ; printf(bs) ; 
	 tab(bias_X+bias_XM, bias_Y+17) ; printf("C     : Current game   ") ; CLR_EOL() ;
	 tab(bias_X+bias_XM, bias_Y+18) ; printf(bs) ; 
	 tab(bias_X+bias_XM, bias_Y+19) ; printf("X     : return to MOS  ") ; CLR_EOL() ;
	 tab(bias_X+bias_XM, bias_Y+20) ; printf(bs) ; 
	 tab(bias_X+bias_XM, bias_Y+22) ; printf(bs) ; 
	 tab(bias_X+bias_XM, bias_Y+23) ; printf(VERSION) ; CLR_EOL() ;
	 tab(bias_X+bias_XM, bias_Y+24) ; printf(bs) ; 
	 color(15,80) ; tab(0, 46) ; 
	 printf("Check for updates at https://github.com/theflynn49/fsudoku ") ; CLR_EOL() ;
}

void drawEditMenu()
{
	char *bs="               " ;
	 color(menu_fg00, menu_bg00) ;
	 tab(bias_X+4*10, bias_Y-1) ; printf(bs) ;
	 tab(bias_X+4*10, bias_Y) ;   printf(" F S U D O K U ") ;
	 tab(bias_X+4*10, bias_Y+1) ; printf(bs) ;
	 tab(bias_X+4*9+1, bias_Y+3) ; printf(" -Edit Menu            ") ;
	 tab(bias_X+4*9+1, bias_Y+5) ; printf("arrows: move cursor    ") ;
	 tab(bias_X+4*9+1, bias_Y+6) ; printf("1-9   : set a cell     ") ;
	 tab(bias_X+4*9+1, bias_Y+7) ; printf("0,' ' : reset a cell   ") ;

	 tab(bias_X+4*9+1, bias_Y+9) ; printf(" -Tools               ") ;
	 tab(bias_X+4*9+1, bias_Y+11) ; printf("P     : hide/show notes") ;
	 tab(bias_X+4*9+1, bias_Y+12) ; printf("Z     : Clear puzzle   ") ;
	 tab(bias_X+4*9+1, bias_Y+13) ; printf("T     : Test puzzle    ") ;
	 tab(bias_X+4*9+1, bias_Y+14) ; printf("L     : Load puzzle    ") ;
	 tab(bias_X+4*9+1, bias_Y+15) ; printf("S     : Save puzzle    ") ;

	 tab(bias_X+4*9+1, bias_Y+17) ; printf(" -Misc.                ") ;
	 tab(bias_X+4*9+1, bias_Y+19) ; printf("X     : return to Menu ") ;
	 tab(bias_X+4*9+1, bias_Y+20) ; printf("/,+,- : change colors  ") ;
	 tab(bias_X+4*9+1, bias_Y+21) ; printf("of item : ") ;
	 pr_item(21) ; printf("    ") ;
}

void drawMenu()
{
	char *bs="               " ;
	 color(menu_fg00, menu_bg00) ;
	 tab(bias_X+4*10, bias_Y-1) ; printf(bs) ;
	 tab(bias_X+4*10, bias_Y) ;   printf(" F S U D O K U ") ;
	 tab(bias_X+4*10, bias_Y+1) ; printf(bs) ;
	 tab(bias_X+4*9+1, bias_Y+3) ; printf(" -Player Menu          ") ;
	 tab(bias_X+4*9+1, bias_Y+5) ; printf("arrows: move cursor    ") ;
	 tab(bias_X+4*9+1, bias_Y+6) ; printf("1-9   : set a cell     ") ;
	 tab(bias_X+4*9+1, bias_Y+7) ; printf(" 0    : reset a cell   ") ;
	 tab(bias_X+4*9+1, bias_Y+8) ; printf("Alt1-9: set a note     ") ;
	 tab(bias_X+4*9+1, bias_Y+9) ; printf("BS    : Undo           ") ;
	 tab(bias_X+4*9+1, bias_Y+10) ; printf("tab   : Redo           ") ;

	 tab(bias_X+4*9+1, bias_Y+12) ; printf(" -Help                 ") ;
	 tab(bias_X+4*9+1, bias_Y+14) ; printf("P     : compute notes  ") ;
	 tab(bias_X+4*9+1, bias_Y+15) ; printf("W     : Warn    ") ;
	 tab(bias_X+4*9+1, bias_Y+16) ; printf("S     : Show Solution  ") ;

	 tab(bias_X+4*9+1, bias_Y+18) ; printf(" -Misc.                ") ;
	 tab(bias_X+4*9+1, bias_Y+20) ; printf("X     : return to Menu ") ;
	 tab(bias_X+4*9+1, bias_Y+21) ; printf("R     : Restart game   ") ;
	 tab(bias_X+4*9+1, bias_Y+22) ; printf("/,+,- : change colors  ") ;
	 tab(bias_X+4*9+1, bias_Y+23) ; printf("of item : ") ;
	 pr_item(23) ; printf("    ") ;
	 pr_error_type() ; printf("   ") ;
}


void drawCell(unsigned int *grd, int i, int j)
{
	int cell = grd[i+9*j] ;
	char t[9] ;
	memset(t, ' ', sizeof(t)) ;
	tab(bias_X+1+i*4, bias_Y+1+j*4) ;
	if ((cell&0xF) == 0)
	{
	  color(cell_fg0, cell_bg0) ;
	  if (gener_in_process==0) for (int i=0; i<9; ++i) t[i] = cell & (1<<(i+7)) ? i+'1' : ' ' ;
	  rst18l(t,3) ;
	  rst18l("\x08\x08\x08\x0A",4) ;
	  rst18l(t+3,3) ;
	  rst18l("\x08\x08\x08\x0A",4) ;
	  rst18l(t+6,3) ;
	} else {
	  if (error_type==-1) { // edit mode
		if (grid[i+9*j]&0x20) color(cell_fg3, cell_bg3) ; else 
			if (grid[i+9*j]&0x10) color(cell_fg1, cell_bg1) ; else  color(cell_fg2, cell_bg2) ;
	  } else if (cell&0x10)
	    color(cell_fg1, cell_bg1) ; else
	    switch(error_type) {
		case 0 : color(cell_fg2, cell_bg2) ; break ;
		case 1 : if (grid[i+9*j]&0x20) color(cell_fg3, cell_bg3) ; else color(cell_fg2, cell_bg2) ;
			 break ;
		case 2 : if ((cell&0xF)==(sol_grid[i+9*j]&0xF)) color(cell_fg2, cell_bg2) ; else color(cell_fg3, cell_bg3) ; 
	    }
	  rst18l("   \x08\x08\x08\x0A", 7) ;
	  printf(" %d ", grd[i+9*j]&0xF) ;
	  rst18l("\x08\x08\x08\x0A   ", 7) ;
	}
}

void gdrawcell(int i, int type)
{
	int x = i%9 ;
	int y = i/9 ;
	if ((type==0)||((grid[i]&0xF)==0)) drawCell(grid, x, y); else {
	  tab(bias_X+1+x*4, bias_Y+1+y*4) ;
	 switch(type){
		case 0 : color(cell_fg0, cell_bg0) ; break ;
		case 1 : color(cell_fg1, cell_bg1) ; break ;
		case 2 : color(cell_fg2, cell_bg2) ; break ;
		case 3 : color(cell_fg3, cell_bg3) ; break ;
	  }
	  rst18l("   \x08\x08\x08\x0A", 7) ;
	  printf(" %d ", grid[i]&0xF) ;
	  rst18l("\x08\x08\x08\x0A   ", 7) ;
	}
}

// redraw one character in the cell
void cellColor(unsigned int *grd, int i, int j) 
{
	int cell = grd[i+9*j] ;
	tab(bias_X+2+i*4, bias_Y+2+j*4) ;
	if ((cell&0xF) == 0)
	{
	  color(cell_fg0, cell_bg0) ;
	  if ((cell & 0x0800)&&(gener_in_process==0)) // 5 in the middle ?
		printf("5") ; else
	  	printf(" ") ;
	} else {
	  if (grd[i+9*j]&0x10)
	    color(cell_fg1, cell_bg1) ; else
	    color(cell_fg2, cell_bg2) ;
	  printf("%d",grd[i+9*j]&0xF) ;
	}
}

void drawgrid(unsigned int *grd)
{
    // drawemptygrid() ;
    for (int i=0; i<9; ++i)
	    for (int j=0; j<9; ++j)
	    {
		    drawCell(grd, i, j) ;
	    }
    setCursor() ;
}

void drawNotes(unsigned int *grd)
{
    for (int i=0; i<9; ++i)
	    for (int j=0; j<9; ++j)
	    {
		 if ((grd[i+9*j]&0x0f)==0)
		    drawCell(grd, i, j) ;
	    }
    setCursor() ;
}

char Pdigits[4][10]= {
	{' ',' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0x80},
	{' ',' ', ' ', ' ', ' ', ' ', 0x86,0x87,0x88,0x88},
	{' ',' ', ' ', ' ', 0x83,0x84,0x85,0x85,0x85,0x85},
	{' ',0x80,0x81,0x82,0x82,0x82,0x82,0x82,0x82,0x82}
} ;

void countDigits(unsigned int *grd)
{
   int count[16] ;
   
   color(cell_fg1, cell_bg1) ;
   memset(count, 0, sizeof(count)) ;
   for (int i=0; i<81; ++i) count[grd[i]&0x0F]++ ;
   // for (int i=0; i<10; ++i) count[i]=i ;
   for (int j=0; j<4; ++j)
   {
     tab(bias_X+1+10*4, bias_Y+1+7*4+j) ;
     for (int i=1; i<10; ++i)
     {
       char c = (9-count[i]) ;
       if (c<0) c=0 ;
       rst18l(Pdigits[j]+c, 1) ; 
     }
   }
   tab(bias_X+1+10*4, bias_Y+1+7*4+5) ;
   printf("123456789") ;
   /*
   tab(bias_X+1+10*4, bias_Y+1+8*3+6) ;
     for (int i=1; i<10; ++i)
     {
	     printf("%d", 9-count[i]) ;
     }
     */
}

//
// library mngt
//
int getLib()
{
   FILE *f ;
// long pos, pos1 ;
   int pos, pos1 ;
   int nb, st ;
   char *head="00-" ;
   char buf[100] ;
   head[0]='0'+type_puzzle ;
   head[1]='0'+diff_puzzle ;
   nb=0 ; st=0 ;
   f=fopen("fsudoku.lib", "r") ; 
   if (f==NULL) return -1 ;
   pos=0 ;
   while (1)
   {
      //pos=ftell(f) ;
      pos++ ;
      if (!fgets(buf, sizeof(buf), f) ) break ;
      if (st==0)
      {
	if (strncmp(buf, head, 3)) continue ;
	st=1 ;
	nb=1 ;
	pos1=pos ;
      } else if (st==1) {
	if (!strncmp(buf, head, 3)) {
	  nb++ ;
	} else {
	  break ;
	}
      } else {
      }
   }
   if (nb<=0)
   {
     fclose(f) ;
     return -2 ;
   }
   st=pos1-1+rand()%nb ;
   fclose(f) ;
   f=fopen("fsudoku.lib", "r") ; 
   // rewind(f) ;
   // fseek(f, pos1, SEEK_SET) ;
   fgets(buf, sizeof(buf), f) ;
   if (st>0) for (int i=0; i<st; ++i) fgets(buf, sizeof(buf), f) ;
   translate_grid(buf+3, grid) ;
   fclose(f) ;
   mixer(grid) ;
   memcpy(sav_grid, grid, sizeof(grid)) ;
   explain=0 ;
   nb=solve(0,1,0) ;
   if (nb==1) solution_ok=1 ; else solution_ok=0 ;
   memcpy(grid, sav_grid, sizeof(grid)) ;
   return solution_ok ;
}
//
// Manage the colors. Not much to see here
//

int inc8x(int val) { return (val+1) & 0x1F | 0x80 ;  } 
int dec8x(int val) { return (val-1) & 0x1F | 0x80 ;  } 
int inc0x(int val) { return (val+1) & 0x1F ;  } 
int dec0x(int val) { return (val-1) & 0x1F ;  } 

void increase(int val)
{
	switch(target)
	{
	  case 0 : grid_bg = inc8x(grid_bg); break ;
	  case 1 : cell_bg0 = inc8x(cell_bg0); break ;
	  case 2 : cell_bg1 = inc8x(cell_bg1); break ;
	  case 3 : cell_bg2 = inc8x(cell_bg2); break ;
	  case 4 : cell_bg3 = inc8x(cell_bg3); break ;
	  case 5 : cell_fg0 = inc0x(cell_fg0); break ;
	  case 6 : cell_fg1 = inc0x(cell_fg1); break ;
	  case 7 : cell_fg2 = inc0x(cell_fg2); break ;
	  case 8 : cell_fg3 = inc0x(cell_fg3); break ;
	  case 9 : grid_fg0 = inc0x(grid_fg0); break ;
	  case 10: grid_fg1 = inc0x(grid_fg1); break ;
	}
    	drawemptygrid() ;
	drawgrid(grid) ;
	drawMenu() ;
	countDigits(grid) ;
}

void decrease(int val)
{
	switch(target)
	{
	  case 0 : grid_bg = dec8x(grid_bg); break ;
	  case 1 : cell_bg0 = dec8x(cell_bg0); break ;
	  case 2 : cell_bg1 = dec8x(cell_bg1); break ;
	  case 3 : cell_bg2 = dec8x(cell_bg2); break ;
	  case 4 : cell_bg3 = dec8x(cell_bg3); break ;
	  case 5 : cell_fg0 = dec0x(cell_fg0); break ;
	  case 6 : cell_fg1 = dec0x(cell_fg1); break ;
	  case 7 : cell_fg2 = dec0x(cell_fg2); break ;
	  case 8 : cell_fg3 = dec0x(cell_fg3); break ;
	  case 9 : grid_fg0 = dec0x(grid_fg0); break ;
	  case 10: grid_fg1 = dec0x(grid_fg1); break ;
	}
    	drawemptygrid() ;
	drawgrid(grid) ;
	drawMenu() ;
	countDigits(grid) ;
}

//
// Get and process commands
//
void clr_info() 
{
	tab(0,44) ; 
	color(7, 0x80) ; 
	for (int i=0; i<8; ++i) rst18l("       ", 8) ;
	setCursor() ;
}

void printInfo(char *msg)
{
  if (msg) strcpy(info, msg) ;
	tab(0,44) ; 
	color(7, 0x80) ; 
	printf("%s          ", info) ; 
	setCursor() ;
}

void printInfoInt(char *fmt, int val)
{
	sprintf(info, fmt, val) ;
	printInfo(NULL) ;
}

void redraw() {
	if (error_type==1) last_errors = const_errors() ;
	drawemptygrid() ; 
	drawgrid(grid) ; 	     
	drawMenu() ; 
        countDigits(grid) ;
	printInfo(NULL) ;
	// setCursor() ; 
}

int test_end()
{
	int err=1 ;
	for (int i=0; i<81; ++i)
	{
	  if ((grid[i]&0xF)==0) return 0 ; // not finished
	  if ((grid[i]&0xF) != (sol_grid[i]&0xF)) err = -1 ; // err
	}
	return err ;
}


void notes_back()
{
	for(int i=0; i<81; ++i) if ((grid[i]&0xF)==0) grid[i]=notes[i]&0xFF80 ;
	notes_in=0 ;
	clr_info() ;
}

int mainEditloop()
{
	FILE *f ;
	char c ;
	int mod,tmp,i,r ;
	unsigned int val ;
	error_type=-1 ;
	cellColor(grid, curPosX, curPosY) ; // set cursor colors to actual colors in display, or else cursor don't appear
	setCursor() ;
	// c = getkey() ;
	c = moscall(0) ;
	mod=moscall8() ;
//		tab(0,44) ; color(15, 0x80) ; printf("key=%04X  ", r) ; 
        r = rand() ;
	switch (c)
	{
	  case '+' : increase(target) ; break ;
	  case '-' : decrease(target) ; break ;
	  case '/' : if (++target>10) target=0 ; pr_item(21) ; setCursor() ; break ;
	  case 'x' :
	  case 'X' : error_type=0 ;
		     for (i=0; i<81; ++i) if ((grid[i]&0x1F)==0) grid[i]=0 ; // reset notes
		     return 0 ;
	  case 'z' :
	  case 'Z' : memset(grid, 0, sizeof(grid)) ; // reset all
		     drawgrid(grid) ;
		     break ;
	  case 't' :
	  case 'T' : explain=1 ; 
		     printInfo("Solving this puzzle, please wait ...") ;
		     memcpy(sav_grid, grid, sizeof(grid)) ;
		     tmp=solve(1, 1, 0) ; 
		     explain=0 ;
		     switch(tmp) {
			 case 0 : printInfo("This puzzle has no solution           ") ; break ;
			 case 1 : printInfo("This puzzle has exactly one solution  ") ; solution_ok=1 ; break ;
			 case 2 : printInfo("This puzzle has several solutions     ") ; 
				  for (i=0; i<81; ++i) {
				      grid[i] &= 0xF ;
				      if (sav_grid[i] & 0x10) grid[i] |= 0x10 ; else
				      if ((sol_grid[i]&0xF)!=(grid[i]&0xF)) 
				      {
					      grid[i] = 0x20 | (0x40<<(grid[i]&0xF)) | (0x40<<(sol_grid[i]&0xF)) ; 
				      } 
				  }
				  gener_in_process=0 ; // show notes
		     		  drawNotes(grid) ;
				  break ;
		     }
		     
		     if (tmp!=2) memcpy(grid, sav_grid, sizeof(grid)) ;
		     drawgrid(grid) ;
		     break ;
	  case 'p' :
	  case 'P' : gener_in_process ^= 1 ;
		     reset_constraints(1) ;  
		     drawNotes(grid) ;
		     break ;
	  case 'l' :
	  case 'L' :
   		     f=fopen("fsudoku.sav", "r") ; 
		     if (f!=NULL)
		     {
   			fgets(grid0, 81, f) ;
   			translate_grid(grid0, grid) ;
   			fclose(f) ;
		        drawgrid(grid) ;
			printInfo("puzzle read.") ;
			solution_ok=0 ;
		     } else printInfo("file fsudoku.sav not found") ;
		     break ;
	  case 's' :
	  case 'S' :
   		     f=fopen("fsudoku.sav", "w") ; 
		     if (f!=NULL)
		     {
			for (tmp=0; tmp<81; tmp++) if ((grid[tmp]&0x10)==0) grid0[tmp]='.' ; else grid0[tmp]='0'|(grid[tmp]&0xF) ;
   			fputs(grid0, f) ;
   			fclose(f) ;
			printInfo("puzzle saved into fsudoku.sav file") ;
		     } else printInfo("error cannot open fsudoku.sav ") ;
		     break ;
	  case 0x0A : if (++curPosY>=9) curPosY=0; setCursor() ; break ;
	  case 0x0B : if (--curPosY<0) curPosY=8; setCursor() ; break ;
	  case 0x15 : if (++curPosX>=9) curPosX=0; setCursor() ; break ;
	  case 0x08 : if (--curPosX<0) curPosX=8; setCursor() ;  break ;
          default : if (((c>='0')&&(c<='9'))||(c==' ')) { // allow space='0'
	    if (mod & 0xC) { // Alt or Alt-Gr?
	    } else {
		if ((c=='0')||(c==' ')) val=0 ; else val = c-'0' ;
		  for (i=0; i<81; ++i) 
		      if ((grid[i]&0x10)==0) {
			 grid[i] &= 0xFF80 ; // reset computed if any
			 drawCell(grid, i%9, i/9) ;
		  }
		  grid[curPosX+9*curPosY] = val | 0x10 ;
		  solution_ok=0 ;
		  tmp = const_errors() ;
		  if (tmp != last_errors) drawgrid(grid) ; 
		  last_errors = tmp ;
		  drawCell(grid, curPosX, curPosY) ;
		  setCursor() ;
		  countDigits(grid) ;
		  reset_constraints(1) ; 
		  if (gener_in_process==0) drawNotes(grid) ;
	          if (++curPosX>=9) { // move cursor one step right to easy puzzle dataentry
		    curPosX=0; 
	  	    if (++curPosY>=9) curPosY=0; 
		  }
		  setCursor() ;
	    }	
	  } else {
		 // tab(30,43) ; color(15, 0x80) ; printf("key-mod=%02X-%02X  ", c, mod) ; 
	  }
 	  break ;
	}	     
//	tab(30,44) ; color(7, 0x80) ; printf("target=%04X-%04X  ", target, r) ; setCursor() ;
	return 1 ;
}


int mainloop()
{
	char c ;
	int r,mod ;
	unsigned int val ;
	cellColor(grid, curPosX, curPosY) ; // set cursor colors to actual colors in display, or else cursor don't appear
	setCursor() ;
	// c = getkey() ;
	c = moscall(0) ;
	mod=moscall8() ;
//		tab(0,44) ; color(15, 0x80) ; printf("key=%04X  ", r) ; 
        r = rand() ;
	switch (c)
	{
	  case '+' : increase(target) ; break ;
	  case '-' : decrease(target) ; break ;
	  case '/' : if (++target>10) target=0 ; pr_item(23) ; setCursor() ; break ;
	  case 'x' :
	  case 'X' : return 0 ;
	  case 'w' :
	  case 'W' : if (++error_type>2) error_type=0 ;
		     pr_error_type() ;
		     if (error_type==1) last_errors = const_errors() ;
		     drawgrid(grid) ;
		     switch(error_type) {
			     case 0 : printInfo("No errors will be displayed             ") ; break ;
			     case 1 : printInfo("Only Sudoku rule errors will be notified  ") ; break ;
			     case 2 : printInfo("Every wrong digit will be shown         ") ; break ;
		     }
		     break ;
	  case 'p' :
	  case 'P' : if (notes_in==0) 
		     { 
		  	memcpy(notes, grid, sizeof(grid)) ;
			reset_constraints(1) ;  
			notes_in=1 ;
			printInfo("Hit P again to get your notes back") ; 
		     } else {
			notes_back() ;
			printInfo(NULL) ;
		     }
		     drawNotes(grid) ;
		     break ;
	  case 'r' :   
	  case 'R' : memcpy(grid, sav_grid, sizeof(grid)) ;
		     redraw() ;
		     undo_stack_ptr=max_stack_ptr=1;
		     min_stack_ptr=0 ;
		     memcpy(undo_stack, grid, sizeof(grid)) ;
		     break ;
	  // case 'Z' : explain=1 ; solve(0, 1, 0) ; explain=0 ;
		     //memcpy(grid, sav_grid, sizeof(grid)) ;
		     // fall-through
	  case 's' :
	  case 'S' : memcpy(grid, sol_grid, sizeof(grid)) ;
	  	     redraw() ; break ;
	  case 0x0A : if (++curPosY>=9) curPosY=0; setCursor() ; break ;
	  case 0x0B : if (--curPosY<0) curPosY=8; setCursor() ; break ;
	  case 0x15 : if (++curPosX>=9) curPosX=0; setCursor() ; break ;
	  case 0x08 : if (--curPosX<0) curPosX=8; setCursor() ;  break ;
	  case 0x7F : 
		      if (undo_stack_ptr==max_stack_ptr) // top of the stack => SP-=2
		         if (undo_stack_ptr==0) undo_stack_ptr=max_stack-1 ; else undo_stack_ptr-- ;
		      if (undo_stack_ptr!=min_stack_ptr) // UNDO=bs
		      {
			      if (undo_stack_ptr==0) undo_stack_ptr=max_stack-1 ; else 
			        undo_stack_ptr-- ;
			      memcpy(grid, undo_stack+undo_stack_ptr*sizeof(grid), sizeof(grid)) ;
			      drawgrid(grid) ;
			      countDigits(grid) ;
		      } ; break ;
	  case 0x09 : if (undo_stack_ptr!=max_stack_ptr) { // REDO=tab
			        undo_stack_ptr++ ;
			        if (undo_stack_ptr>=max_stack) undo_stack_ptr=0 ;
			        if (undo_stack_ptr==min_stack_ptr) {
				      min_stack_ptr++ ;
				      if (min_stack_ptr>=max_stack) min_stack_ptr=0 ;
			        }
			      memcpy(grid, undo_stack+undo_stack_ptr*sizeof(grid), sizeof(grid)) ;
			      drawgrid(grid) ;
			      countDigits(grid) ;
			      val = undo_stack_ptr+1 ;
			      if (val==max_stack) val=0 ;
			      if (val==max_stack_ptr) // reach the top ?
			      {
				      undo_stack_ptr=val ; // back to normal
			      } ; 
		      }
		      break ;
          default : if ((c>='0')&&(c<='9')) {
	    if (mod & 0xC) { // Alt or Alt-Gr?
	      // if ((mod & 0xC)==0xC)  // both ? -> debug stack
	      // {
	      // int ptr = c-'0'-1 ;
	      // printInfoInt("peek stack %d ", ptr) ;
	      //  memcpy(grid, undo_stack+ptr*sizeof(grid), sizeof(grid)) ;
	      //  drawgrid(grid) ;
	      //} else {
		val = grid[curPosX+9*curPosY] ;
		if ((val & 0xF)==0) {
		  if (c=='0') val = 0 ; else
		     val ^= 1<<(6+c-'0') ;
		  grid[curPosX+9*curPosY] = val ;
		  drawCell(grid, curPosX, curPosY) ;
		  memcpy(notes, grid, sizeof(grid)) ;
		  notes_in=0 ;
		  clr_info() ;
		}
	      // }
	    } else {
		if (c=='0') val=0 ; else val = c-'0' ;
		if (((grid[curPosX+9*curPosY]&0x10)==0)&&(grid[curPosX+9*curPosY] != val)) {
		  grid[curPosX+9*curPosY] = val ;
		  memcpy(undo_stack+undo_stack_ptr*sizeof(grid), grid, sizeof(grid)) ;
		  undo_stack_ptr++ ;
		  if (undo_stack_ptr>=max_stack) undo_stack_ptr=0 ;
		  max_stack_ptr=undo_stack_ptr ;
		  if (undo_stack_ptr==min_stack_ptr) {
		    min_stack_ptr++ ;
		    if (min_stack_ptr>=max_stack) min_stack_ptr=0 ;
		  }
		  if (error_type==1) {
			  int tmp = const_errors() ;
			  if (tmp != last_errors) drawgrid(grid) ; 
			  last_errors = tmp ;
		  }
		  drawCell(grid, curPosX, curPosY) ;
		  countDigits(grid) ;

		  if ((val & 0xF)!=0) { // real digit?
		    val=test_end() ;
		    if (val==-1)
			printInfo("You completed the grid, but it doesn't look quite right ... ") ; 
		    else if (val==1)
			printInfo("Congrats ! You completed the grid successfully ! ") ; 
		  }	    
		} else {
		}
	    }
	  } else {
		 // tab(30,43) ; color(15, 0x80) ; printf("key-mod=%02X-%02X  ", c, mod) ; 
	  }
	}	     
//	tab(30,44) ; color(7, 0x80) ; printf("target=%04X-%04X  ", target, r) ; setCursor() ;
	return 1 ;
}

int mainMenu()
{
	char c ;
	int r,mod ;
	unsigned int val ;
	cursor(0) ;
	drawMainMenu() ;
	// c = getkey() ;
	c = moscall(0) ;
	mod=moscall8() ;
//		tab(0,44) ; color(15, 0x80) ; printf("key=%04X  ", r) ; 
        r = rand() ; // try to catch some entropy
	switch (c)
	{
	  case 'T' :
	  case 't' : 
		     if (++type_puzzle>2) type_puzzle=0; GoToType() ; break ; 
	  case 'D' :
	  case 'd' : 
		     if (++diff_puzzle>4) diff_puzzle=0; break ; 
	  case 'X' : 
	  case 'x' :
		     return 0 ;
	  case 'L' :
	  case 'l' : 
		     printInfo("reading fsudoku.lib file ...") ;
		     if (getLib()<0) {
			printInfo("cannot read fsudoku.lib file") ;
		     } else {
   		        notes_in=0 ;
			redraw() ;
			undo_stack_ptr=max_stack_ptr=1;
			min_stack_ptr=0 ;
		  	memcpy(undo_stack, grid, sizeof(grid)) ;
			current_type_puzzle = type_puzzle ;
		        printInfo("                            ") ;
                        cursor(1) ;
                        while(mainloop()) ;
                        cursor(0) ;
                        color(15,0x80) ;
		        tab(0,0) ;
                        rst18l(" \x0c", 2) ;
		     } ;
		     break ;
	  case 'G' : 
	  case 'g' : 
    		     drawemptygrid() ; drawgrid(grid) ; drawMenu() ;
		     setCursor() ;
		     gener_in_process=1 ;
		     while(generate_puzzle()<0) ; 
   		     notes_in=0 ;
		     redraw() ; 
		     gener_in_process=0 ;
		     undo_stack_ptr=max_stack_ptr=1;
		     min_stack_ptr=0 ;
		     memcpy(undo_stack, grid, sizeof(grid)) ;
		     countDigits(grid) ;
		     current_type_puzzle = type_puzzle ;
		     cursor(1) ;
		     while(mainloop()) ;
		     cursor(0) ;
	 	     color(15,0x80) ;
		     tab(0,0) ;
	   	     rst18l(" \x0c", 2) ;
		     break ;
	  case 'e' :
	  case 'E' : drawemptygrid() ; drawgrid(grid) ; drawEditMenu() ;
		     gener_in_process=1 ;
		     setCursor() ;
		     reset_constraints(1) ; 
		     last_errors = const_errors() ;
		     for (r=0; r<81; ++r) if ((grid[r]&0x10)==0) grid[r]=0 ; // keep only grid def
		     drawgrid(grid) ; 
		     countDigits(grid) ;
		     cursor(1) ;
		     while(mainEditloop()) ;
		     gener_in_process=0 ;
		     cursor(0) ;
                     color(15,0x80) ;
                     tab(0,0) ;
                     rst18l(" \x0c", 2) ;
                     break ;
	  case 'C' :
	  case 'c' : if (solution_ok==0)
		     {
			 printInfo("I have no solution yet for this puzzle (try Edit->Test)") ;
			 break ;
		     }
		     if (current_type_puzzle != type_puzzle) {
		       type_puzzle=current_type_puzzle; 
		       GoToType() ;  
		     } ;
    		     drawemptygrid() ; drawgrid(grid) ; drawMenu() ;
		     setCursor() ;
		     countDigits(grid) ;
		     cursor(1) ;
		     while(mainloop()) ;
		     cursor(0) ;
	 	     color(15,0x80) ;
		     tab(0,0) ;
	   	     rst18l(" \x0c", 2) ;
		     break ;
	}	     
//	tab(30,44) ; color(7, 0x80) ; printf("target=%04X-%04X  ", target, r) ; setCursor() ;
	return 1 ;
}

#define Xbar "\x7e\x7e"
void initCharSet()
{
	rst18l("\x17\x80\0\0\0\0\0\0"Xbar, 10) ;
	rst18l("\x17\x81\0\0\0"Xbar"\0"Xbar, 10) ;
	rst18l("\x17\x82"Xbar"\0"Xbar"\0"Xbar, 10) ;
	rst18l("\x17\x83\0\0\0\0\0"Xbar"\0", 10) ;
	rst18l("\x17\x84\0\0"Xbar"\0"Xbar"\0", 10) ;
	rst18l("\x17\x85\x7e\0"Xbar"\0"Xbar"\0", 10) ;
	rst18l("\x17\x86\0\0\0\0\0\0\0\x7e", 10) ;
	rst18l("\x17\x87\0\0\0\0"Xbar"\0\x7e", 10) ;
	rst18l("\x17\x88\0"Xbar"\0"Xbar"\0\x7e", 10) ;
}

int main(int argc, char *argv[])
{
	int nb ;
	char c = ' ' ;
    	srand(time(NULL)) ;
    	nb=100 ;
    	while(rand()<0x100) if (nb--<=0) break ;
	rst18l("\x17\x00\xC1\x01  \x16\x01                    \n", 27) ; // force legacy mode and mode 1
	initCharSet() ;
	translate_grid(grid0S, sol_grid) ;
	translate_grid(grid0, grid) ;
	memcpy(sav_grid, grid, sizeof(grid)) ;
	for (int i=0; i<81;++i) if ((grid[i]&0xF)==0) sol_grid[i]&=0xEF ; // 
	undo_stack_ptr=max_stack_ptr=1;
	solution_ok=1 ;
	min_stack_ptr=0 ;
	memcpy(undo_stack, grid, sizeof(grid)) ;
	while(mainMenu()) ;
	
	color(15,0x80) ;
	rst18l(" \x17\x00\xC1\x00  \x16\x01  \x0C\r\n", 14) ; // remove legacy mode and set mode 1
	return 0 ;
}
