/* { dg-do compile } */ 
/* { dg-options "-O2 -fgraphite -fdump-tree-graphite-all" } */

#  define EXTERN(type, array)  extern type array[]
#  define DECLARE(type, array, size)  type array[size]
#  define ALLOC(type, array, size)
#  define FREE(array)

typedef unsigned char  uch;
typedef unsigned short ush;

EXTERN(uch, window);
EXTERN(ush, prev);

#ifndef WSIZE
#  define WSIZE 0x8000
#endif                
#define MIN_MATCH  3
#define MAX_MATCH  258
#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)
#define MAX_DIST  (WSIZE-MIN_LOOKAHEAD)

#define near
typedef ush Pos;
typedef unsigned IPos;

unsigned near max_chain_length;
DECLARE(uch, window, 2L*WSIZE);
DECLARE(Pos, prev, WSIZE);

extern unsigned near strstart;

unsigned int near prev_length;
#define NIL 0
unsigned near good_match;
unsigned near match_start;
int near nice_match;
#define WMASK     (WSIZE-1)
#define WSIZE 0x8000

int longest_match(IPos cur_match)
{
    unsigned chain_length = max_chain_length;
    register uch *scan = window + strstart;  
    register uch *match;                     
    register int len;                        
    int best_len = prev_length;              
    IPos limit = strstart > (IPos)MAX_DIST ? strstart - (IPos)MAX_DIST : NIL;

    register uch *strend = window + strstart + MAX_MATCH;
    register uch scan_end1  = scan[best_len-1];
    register uch scan_end   = scan[best_len];

    if (prev_length >= good_match) {
        chain_length >>= 2;
    }

    do {
        match = window + cur_match;

        if (match[best_len]   != scan_end  ||
            match[best_len-1] != scan_end1 ||
            *match            != *scan     ||
            *++match          != scan[1])      continue;


        scan += 2, match++;
	
        do {
        } while (*++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 scan < strend);
	
        len = MAX_MATCH - (int)(strend - scan);
        scan = strend - MAX_MATCH;


        if (len > best_len) {
            match_start = cur_match;
            best_len = len;
            if (len >= nice_match) break;
            scan_end1  = scan[best_len-1];
            scan_end   = scan[best_len];
        }

    } while ((cur_match = prev[cur_match & WMASK]) > limit
	     && --chain_length != 0);

    return best_len;
}
