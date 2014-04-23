#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define stat xv6_stat  // avoid clash with host struct stat
#include "types.h"
#include "fs.h"
#include "stat.h"
#include "param.h"

int nlog = LOGSIZE;
int ninodes = 200;
int size = 1024*32;
int nblocks = 985;

int fsfd;
struct superblock sb;
char zeroes[512];
uint freeblock;
uint usedblocks;
uint bitblocks;
uint freeinode = 1;



void balloc(int);
void wsect(uint, void*);
void winode(uint, struct dinode*);
void rinode(uint inum, struct dinode *ip);
void rsect(uint sec, void *buf);
uint ialloc(ushort type);
void iappend(uint inum, void *p, int n);

// convert to intel byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar*)&y;
  a[0] = x;
  a[1] = x >> 8;
  return y;
}

uint
xint(uint x)
{
  uint y;
  uchar *a = (uchar*)&y;
  a[0] = x;
  a[1] = x >> 8;
  a[2] = x >> 16;
  a[3] = x >> 24;
  return y;
}

// *added To create directories like bin , home
void 
adddir(char *name,uint* curino ,uint parino){

  uint inum;
  struct dirent de;

  *curino = inum = ialloc(T_DIR); 
  bzero(&de, sizeof(de));
  de.inum = xshort(inum);
  strcpy(de.name, name);
  iappend(parino, &de, sizeof(de));

  bzero(&de, sizeof(de));
  de.inum = xshort(inum);
  strcpy(de.name, ".");
  iappend(inum, &de, sizeof(de));

  bzero(&de, sizeof(de));
  de.inum = xshort(parino);
  strcpy(de.name, "..");
  iappend(inum, &de, sizeof(de));
}
int
main(int argc, char *argv[])  // make file system using arguments given
{
  int i, cc, fd;
  uint rootino, inum, off, binino, docino, tmp;
  struct dirent de;
  char buf[512];
  struct dinode din;

  if(argc < 2){
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  assert((512 % sizeof(struct dinode)) == 0);
  assert((512 % sizeof(struct dirent)) == 0);

  fsfd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666);
  if(fsfd < 0){
    perror(argv[1]);
    exit(1);
  }

  sb.size = xint(size);
  sb.nblocks = xint(nblocks); // so whole disk is size sectors
  sb.ninodes = xint(ninodes);
  sb.nlog = xint(nlog);

  bitblocks = size/(512*8) + 1;
  usedblocks = ninodes / IPB + 3 + bitblocks;
  freeblock = usedblocks;
  nblocks = size - usedblocks - LOGSIZE;

  printf("used %d (bit %d ninode %zu) free %u log %u total %d and value of nblocks %d\n", usedblocks,
         bitblocks, ninodes/IPB + 1, freeblock, nlog, nblocks+usedblocks+nlog,nblocks);

  assert(nblocks + usedblocks + nlog == size);

  for(i = 0; i < nblocks + usedblocks + nlog; i++)
    wsect(i, zeroes);

  memset(buf, 0, sizeof(buf));
  memmove(buf, &sb, sizeof(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  assert(rootino == ROOTINO);

  bzero(&de, sizeof(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, sizeof(de));

  bzero(&de, sizeof(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, sizeof(de));

  adddir("bin", &binino, rootino);
  adddir("doc", &docino, rootino);
  adddir("home", &inum, rootino);


  for(i = 2; i < argc; i++){ 
    assert(index(argv[i], '/') == 0);
    tmp = rootino;
    if((fd = open(argv[i], 0)) < 0){
      perror(argv[i]);
      exit(1);
    }
    
    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    if(argv[i][0] == '_'){
//      if(argv[i][1] != 'i' )
        tmp = binino;
      ++argv[i];
    }
    else
      tmp = docino;

    inum = ialloc(T_FILE);

    bzero(&de, sizeof(de));
    de.inum = xshort(inum);
    strncpy(de.name, argv[i], DIRSIZ);
    iappend(tmp, &de, sizeof(de));

    while((cc = read(fd, buf, sizeof(buf))) > 0)
      iappend(inum, buf, cc);

    close(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.size);
  off = ((off/BSIZE) + 1) * BSIZE;
  din.size = xint(off);
  winode(rootino, &din);

  balloc(usedblocks);

  exit(0);
}

void
wsect(uint sec, void *buf)    // writes data on given sector give sector number and buffer
{
  if(lseek(fsfd, sec * 512L, 0) != sec * 512L){
    perror("lseek");
    exit(1);
  }
  if(write(fsfd, buf, 512) != 512){
    perror("write");
    exit(1);
  }
}

uint
i2b(uint inum)     // tells block number in which inode is stored  
{
  return (inum / IPB) + 2; // why +2 ?
}

void
winode(uint inum, struct dinode *ip)   // 
{
  char buf[512];
  uint bn;
  struct dinode *dip;

  bn = i2b(inum);   // block in which inode will be stored  
  rsect(bn, buf);   // it will read  block into  buffer
  dip = ((struct dinode*)buf) + (inum % IPB); //  what is use of dip here  
  *dip = *ip;
  wsect(bn, buf); // write into sector pointer by buffer
}

void
rinode(uint inum, struct dinode *ip)   // read buffer from it 
{
  char buf[512];
  uint bn;
  struct dinode *dip;

  bn = i2b(inum);
  rsect(bn, buf);
  dip = ((struct dinode*)buf) + (inum % IPB);
  *ip = *dip;
}

void
rsect(uint sec, void *buf)  // read sectors 
{
  if(lseek(fsfd, sec * 512L, 0) != sec * 512L){
    perror("lseek");
    exit(1);
  }
  if(read(fsfd, buf, 512) != 512){
    perror("read");
    exit(1);
  }
}

uint
ialloc(ushort type)  // allocates inodes
{
  uint inum = freeinode++;
  struct dinode din;

  bzero(&din, sizeof(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  return inum;
}
 
void   		//need to make changes here as it will create problems 
balloc(int used)  // allocate sector in bitmap mark it as 1 and write it back
{
  uchar buf[512];
  int i,j;

  printf("balloc: first %d blocks have been allocated\n", used);
  assert(used < bitblocks*512*8);  //*changed
  bzero(buf, 512);  // make zero in buffer 
  for(j = 0; j < bitblocks ; j++){ //*changed for multiple bitmap blocks
    for(i = 0; i < 512*8 && (i+j*512*8) < used; i++){
      buf[i/8] = buf[i/8] | (0x1 << (i%8));  // make all used ones set 
    }
//    printf("balloc: write bitmap block at sector %zu\n", ninodes/IPB + 3+j);
    wsect(ninodes / IPB + 3 + j, buf); // written back to fs
    bzero(buf, 512);
  }
  
}

#define min(a, b) ((a) < (b) ? (a) : (b))

void
iappend(uint inum, void *xp, int n)  // this function take 512 buffer writes into fs and do work if size greater thatn where to put buffer
{
  char *p = (char*)xp;
  uint fbn, off, n1;
  struct dinode din;
  char buf[512];
  uint tmp,tmp1;
  uint indirect[NINDIRECT];
  uint indirect_w[NINDIRECT];  // for reading first indirect block
  uint dindirect[NINDIRECT];  // for rading second indiret block
  
  uint x;

  rinode(inum, &din);  // read inode i din

  off = xint(din.size); // offset set to current size
  while(n > 0){
    fbn = off / 512;  // calculate how many blocks already used
    assert(fbn < MAXFILE); 
    if(fbn < (NDIRECT-1)){  
      if(xint(din.addrs[fbn]) == 0){    // 
        din.addrs[fbn] = xint(freeblock++);  // free block count increased and usedblock count then it is written to fs
        usedblocks++;
      }
      x = xint(din.addrs[fbn]);
    } else if(fbn < (NDIRECT + NINDIRECT-1)){
      if(xint(din.addrs[NDIRECT-1]) == 0){
//        printf("allocate indirect block %d\n", freeblock+1);
        din.addrs[NDIRECT-1] =  xint(freeblock++);
        usedblocks++;
      }
      // printf("read indirect block\n");
      rsect(xint(din.addrs[NDIRECT-1]), (char*)indirect);
      if(indirect[fbn - NDIRECT + 1] == 0){
        indirect[fbn - NDIRECT + 1] =  xint(freeblock++);
        usedblocks++;
        wsect(xint(din.addrs[NDIRECT-1]), (char*)indirect);
      }
      x = xint(indirect[fbn-NDIRECT+1]);
    }

// for double indirect 
     else{
      if(xint(din.addrs[NDIRECT]) == 0){
// allocate doub'le indirect blocks here 
        din.addrs[NDIRECT]  = xint(freeblock++);
        usedblocks++;
       }
	// get first indirect block then get the 2nd indirect block from other
      rsect(xint(din.addrs[NDIRECT]), (char*)indirect_w);
      tmp1 = (fbn-NINDIRECT-NDIRECT +1);
      tmp = tmp1/NINDIRECT;
      if(indirect_w[tmp] == 0){
	indirect_w[tmp] =  xint(freeblock++);
        usedblocks++;
        printf("Indirect 1st block allocated is %d\n",usedblocks-1);
        wsect(xint(din.addrs[NDIRECT]), (char*)indirect_w);
      }

      rsect(indirect_w[tmp], (char*)dindirect);
      if(dindirect[tmp1 % NINDIRECT] == 0){
        dindirect[tmp1 % NINDIRECT] =  xint(freeblock++);
        usedblocks++;
	wsect(indirect_w[tmp], (char*)dindirect);
      }
      x = xint(dindirect[tmp1 % NINDIRECT]);    
    }	
    
    n1 = min(n, (fbn + 1) * 512 - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * 512), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.size = xint(off);
//  printf("final check on %d %d\n", din.addrs[NDIRECT-1],din.addrs[NDIRECT]);
  winode(inum, &din);
}
