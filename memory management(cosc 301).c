#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#define stat xv6_stat  // avoid clash with host struct stat
#define NINODES 200
#include "types.h"
#include "fs.h"
#include "stat.h"
#include "buf.h"

int fsfd; // access file through this descriptor; use the file descriptor to read or write the file
struct superblock sb;

//Read contents of indicated block
struct buf *bread(uint fsfd, uint blockno) { //need to malloc
  struct buf *b = malloc(sizeof(struct buf));
  lseek(fsfd,blockno*BSIZE,SEEK_SET);
  read(fsfd, b->data, BSIZE);
  b->blockno=blockno;
  b->dev=fsfd;
  return b;
}

//Free the malloced buffer
void brelse(struct buf *b){
  free(b);
}

//Read the super block.
void
readsb(int dev, struct superblock *sb)
{
  struct buf *bp;
  bp = bread(dev, 1);
  memmove(sb, bp->data, sizeof(*sb));
  //printf("sb.size = %i ",sb.size);
  brelse(bp);
}

//compare directory name
int
namecmp(const char *s, const char *t)
{
  return strncmp(s, t, DIRSIZ);
}


//Check file system
/*
Detect ERROR: bad inode (ERROR 1)
Check: Each inode is either unallocated or one of the valid types (T_FILE, T_DIR, T_DEV).

Detect ERROR: bad address in inode (ERROR 2)
Check: For in-use inodes, each address that is used by an inode is valid (i.e., points to a valid data block address within the image)
*/
void check_inode(){
  struct dinode *dip;
  struct buf *bp = 0;
  for(int inum = 0; inum < sb.ninodes; inum++) {
    bp = bread(fsfd, IBLOCK(inum, sb));
    dip = (struct dinode*)bp->data + inum%IPB;
    //check that inode is of a valid type or unallocated
    if(dip->type != 0 && dip->type != T_FILE && dip->type != T_DIR && dip->type != T_DEV) {
      printf("ERROR: bad inode\n");
      exit(1);
    }
    if(dip->type != 0){
      //check that addresses in pointers are valid
      for(int i=0; i<NDIRECT+1; i++){
        if(dip->addrs[i] > sb.size || (dip->addrs[i] < sb.size-sb.nblocks && dip->addrs[i] != 0)){ //need to check to make sure address isnt too small. calculate that address
          printf("ERROR: bad address in inode\n");
          exit(2);
        }
      }
      //check addresses used by indirect pointer
      bp = bread(fsfd, dip->addrs[NDIRECT]);
      for(int i=0; i<NINDIRECT; i++){
        if(((int *)bp->data)[i] > sb.size || (((int *)bp->data)[i] < sb.size-sb.nblocks && ((int *)bp->data)[i] != 0)){
          printf("inode num : %d, i = %d, addrs = %d\n",inum, i, dip->addrs[i]);
          printf("ERROR: bad address in inode\n");
          exit(2);
        }
      }
    }
    brelse(bp);
  }
}

// Detect ERROR: directory not properly formatted (ERROR 3)
// Check: Each directory contains . and .. entries
void check_dir_contents(){
  struct dinode *dip;
  struct dirent *de;
  struct buf *bp = 0;
  //Loop through all INODES
  for(int inum = 0; inum < sb.ninodes; inum++) {
    bp = bread(fsfd, IBLOCK(inum, sb));
    dip = (struct dinode*)bp->data + inum%IPB;
    //Loop through DIRECT POINTERS to DIRECTORY data blocks
    for(int dpNum=0; dpNum<NDIRECT; dpNum++){
      if(dip->type == T_DIR){
        bool dotFound=false;
        bool dotdotFound=false;
        struct buf *dirblock = 0;
        dirblock = bread(fsfd,dip->addrs[dpNum]);
        //loop through DIRECTORY ENTRIES
        for(int direntNum=0; direntNum<DPB; direntNum++){
          de = (struct dirent*)dirblock->data +direntNum;
          if(namecmp(de->name,"..")==0){
            dotdotFound=true;
          }
          else if(namecmp(de->name,".")==0){
            dotFound=true;
          }
        }
        if (dotFound!=true || dotdotFound !=true){
          printf("ERROR: directory not properly formatted\n");
          exit(3);
        }
        break;
      }
    }
    brelse(bp);
  }
}


// TODO: Detect ERROR: parent directory mismatch (ERROR 4)
// Check: Each .. entry in directory refers to the proper parent inode, and parent inode points back to it
void parent_dir_check(){
  struct dinode *dip;
  struct dirent *de;
  struct buf *bp = 0;

  //Loop through all INODES
  for(int inum = 0; inum < sb.ninodes; inum++) {
    bp = bread(fsfd, IBLOCK(inum, sb));
    dip = (struct dinode*)bp->data + inum%IPB;
    //Loop through DIRECT POINTERS
    for(int dpNum=0; dpNum<NDIRECT; dpNum++){
      if(dip->type == T_DIR && (namecmp(de->name,"..")==0)){
        struct buf *dirblock = 0;
        dirblock = bread(fsfd,dip->addrs[dpNum]);
        //Loop through DIECTORY ENTRIES
        for(int direntNum=0; direntNum<DPB; direntNum++){
          de = (struct dirent*)dirblock->data +direntNum%DPB;
          if(de->inum != inum){
            printf("ERROR: parent directory mismatch\n");
            exit(4);
          }
        }
      }
    }
    brelse(bp);
  }
}

// get bit corresponsing to block number in bitmap
void get_bit_in_bmap(uint bn){
  int byteLoc;
  int bitLoc;
  unsigned char byte;
  struct buf *blk;
  int mask;

  blk = bread(fsfd, BBLOCK(bn, sb));
  byteLoc = bn/8;
  bitLoc = bn%8;
  byte=blk->data[byteLoc];
  mask = 1 >> (bitLoc);
  //printf("byte = %d\n", byte&mask);
  //printf("byte = %d\n", byte);

  //(blk->data[byte] & mask)
  if((byte & mask) == 0){
    printf("ERROR: address used by inode but marked free in bitmap\n");
    exit(1);
  }
}

// Detect ERROR: address used by inode but marked free in bitmap (ERROR 5)
// Check: For in-use inodes, each address in use is also marked in use in the bitmap.
void check_inode2bitmap(){
  struct dinode *dip;
  struct buf *bp = 0;
  uint bblockNum;

  //Loop through all INODES
  for(int inum = 0; inum < sb.ninodes; inum++) {
    bp = bread(fsfd, IBLOCK(inum, sb));
    dip = (struct dinode*)bp->data + inum%IPB;
    if(dip->type != 0){
      bblockNum = BBLOCK(inum,sb);
      get_bit_in_bmap(bblockNum);
    }
    //Loop through INDIRECT
    if(dip->addrs[NINDIRECT] != 0){
      bp = bread(fsfd, dip->addrs[NDIRECT]);
      int *adrs = (int *)bp->data;
      for(int i=0; i<NINDIRECT; i++){
        get_bit_in_bmap(adrs[i]);
      }
    }
    brelse(bp);
  }
}


// TODO: Detect ERROR: bitmap marks block in use but it is not in use (ERROR 6)
// Check: For blocks marked in-use in bitmap, actually is in-use in an inode or indirect block somewhere.
//void check_bitmap2inode(){
  // struct dinode *dip;
  // struct dirent *de;
  // struct buf *bp = 0;
  // uint bblockNum;
  // int bit;
  // int mask;
  // int curBit;
  // for(int block=0; block<sb.size; block++){
  //   bp = bread(fsfd, i+sb.bmapstart)
  //   //for each byte in block
  //   for (int byte = 0; byte< BSIZE; byte++){
  //     //for each bit in byte
  //     }
  //   }
  //   bit = get_bit_in_bmap
  //   bp = bread(fsfd, i);
  //   if(bp == 1){
  //     dip = IBLOCK(i,sb);
  //     if(dip->type == 0){
  //     }
  //   }


// Detect ERROR: address used more than once (ERROR 7)
// Check: For in-use inodes, any address in use is only used once.
// go through inodes and check to see if addess is in the list
// if it is, then return error
void address_duplicate(){
  struct dinode *dip;
  int last_index=0;//remember the last position that was edited in the array (just for efficiency)
  struct buf *bp = 0;
  uint addresses[1000];
  for (int g=0;g<=1000;g++){
    addresses[g]=-1;
  }
  for(int inum = 0; inum < sb.ninodes; inum++) {

    bp = bread(fsfd, IBLOCK(inum, sb)); //read inode blocks
    dip = (struct dinode*)bp->data + inum%IPB;

    if (dip->type!=0){
      for (int i=0;i<NDIRECT;i++){ //loop through direct pointers
        //printf("test 2 %d\n", dip->addrs[i]);

        if (dip->addrs[i]!=0){

          for (int y=0;y<last_index;y++){ //check all the filled positions in the array
            //printf("___ %d, %d \n", addresses[y],dip->addrs[i]);
            if (addresses[y]==dip->addrs[i]){ //if the address is already on the list throw the error
              printf("ERROR: address used more than once \n");
              exit(1);
            }
          }
          //printf("*******%d",dip->addrs[i]);
          addresses[last_index]=dip->addrs[i];  //if you check the whole list and no error is thrown, add it to the end of the list
          last_index++; //increment the last edited position in the array
        }
      }
      //printf("before %d, %d , %d\n",dip->addrs[NDIRECT],NDIRECT,(int) sizeof(dip->addrs));
      if (dip->addrs[NDIRECT]!=0){
        //printf("after \n");
        struct buf *bp1;
        bp1 = bread(fsfd, dip->addrs[NDIRECT]);
        int *adrs = (int *)bp1->data;

        for(int f=0; f<NINDIRECT; f++){
          if (adrs[f]!=0){
            for(int k=0;k<last_index;k++){
              //printf("%d, %d \n", addresses[k], adrs[f]);
              if (addresses[k]==adrs[f]){
                printf("ERROR: address used more than once \n");
                exit(1);
              }
            }
            //printf("**%d, %d, %d \n",adrs[f], addresses[last_index], last_index);
            addresses[last_index]=adrs[f];
            //printf("%d, %d \n",adrs[f], addresses[last_index]);
            last_index++;
          }
        }
        brelse(bp1);
      }
    }
  }
  brelse(bp);
}

  // Detect ERROR: inode marked used but not found in a directory (ERROR 8)
  // Check: For inodes marked used in inode table, must be referred to in at least one directory.
  void check_inode_dir(){
    struct dinode *dip;
    struct buf *bp = 0;
    bool match=false;
    for(int inum = 0; inum < sb.ninodes; inum++) {
      bp = bread(fsfd, IBLOCK(inum, sb));
      dip = (struct dinode*)bp->data + inum%IPB;
      if(dip->type != 0) { //check that inode is used
        for(int inum2=0; inum2<sb.ninodes; inum2++){
          struct buf *bp2 = 0;
          bp2=bread(fsfd, IBLOCK(inum, sb));
          for(int i=0; i<IPB; i++){
            struct dinode *dip1;
            dip1 = (struct dinode*)bp2->data + inum%IPB;
            for(int dpNum=0; dpNum<NDIRECT; dpNum++){
              if(dip1->type == T_DIR){
                struct buf *dirBlock = 0;
                dirBlock = bread(fsfd,dip1->addrs[dpNum]);
                for (int k = 0; k <DPB; k++){
                  struct dirent de = ((struct dirent*)dirBlock->data)[k];
                  if(de.inum == inum2 + ((inum -sb.inodestart) *IPB)){
                    match = true;
                  }
                }
                brelse(dirBlock);
              }
            }
          }
          brelse(bp2);
        }
      }
      brelse(bp);
    }
    if(match){
      printf("ERROR: inode marked used but not found in a directory\n");
      exit(1);
    }
  }

// TODO: Detect ERROR: inode referred to in directory but marked free (ERROR 9)
// Check: For inode numbers referred to in a valid directory, actually marked in use in inode table.
void referred_marked_free(){
  struct dinode *dip;
  struct buf *bp = 0;

  for(int inum = 0; inum < sb.ninodes; inum++) {
    bp = bread(fsfd, IBLOCK(inum, sb)); //read inode blocks
    dip = (struct dinode*)bp->data + inum%IPB;
    if (dip->type== T_DIR){
      struct buf *bp1;
      for(int k =0;k<NDIRECT;k++){
        if (dip->addrs[k]!=0){
          bp1=bread(fsfd,dip->addrs[k]);
          for (int i=0;i<DPB;i++){
            struct dirent dir= ((struct dirent*)bp1->data)[i];

            if(dir.inum!=0 && strcmp("",dir.name)!=0){
              struct buf *bp2;
              for (uint y= sb.inodestart; y<sb.inodestart+(sb.ninodes/IPB); y++){
                bp2 = bread(fsfd,y);
                for (int j=0; j<IPB;j++){
                    struct dinode *dip2;
                    dip2 = (struct dinode*)bp2->data +j;

                    if((j+((y-sb.inodestart)*IPB)) ==dir.inum &&dip2->type ==0){
                      printf("ERROR: inode referred to in directory but marked free \n");
                      exit(9);
                    }
                }

              }
              brelse(bp2);
            }
          }
        }
      }
      brelse(bp1);
      if (dip->addrs[NDIRECT]!=0){
        struct buf *bp3;
        bp3 = bread(fsfd,dip->addrs[NDIRECT]);
        struct dinode *dip3 = (struct dinode*)bp3->data + inum%IPB;
        for(int k =0;k<NINDIRECT;k++){
          //printf("test");
          if (dip3->addrs[k]!=0){
            bp1=bread(fsfd,dip3->addrs[k]);
            for (int i=0;i<DPB;i++){
              struct dirent dir= ((struct dirent*)bp1->data)[i];

              if(dir.inum!=0 && strcmp("",dir.name)!=0){
                struct buf *bp2;
                for (uint y= sb.inodestart; y<sb.inodestart+(sb.ninodes/IPB); y++){
                  bp2 = bread(fsfd,y);
                  for (int j=0; j<IPB;j++){
                      struct dinode *dip2;
                      dip2 = (struct dinode*)bp2->data +j;

                      if((j+((y-sb.inodestart)*IPB)) ==dir.inum &&dip2->type ==0){
                        printf("ERROR: inode referred to in directory but marked free \n");
                        exit(9);
                      }
                  }
                }
                brelse(bp2);
              }
            }
          }
        }
      }
    }
  }
  brelse(bp);
}


    int main(int argc, char *argv[]) {
      // Confirm filesystem image name is provided as an argument
      if (argc < 2) {
        printf("Usage: chkfs IMAGE\n");
        return 1;
      }

      // Open file system image
      fsfd = open(argv[1], O_RDONLY); //disk is a file (its basically the device)
      if (fsfd < 0) {
        perror(argv[1]);
        return 1;
      }

      //read superblock
      readsb(fsfd,&sb);

      //check inodes
      check_inode(); //errors 1 and 2
      check_dir_contents(); //error 3
      parent_dir_check(); //error 4
      check_inode2bitmap(); //Error 5
      //check_bitmap2inode(); //Error 6
      address_duplicate();//Error 7
      check_inode_dir(); //Error 8
      referred_marked_free(); //Error 9



      // Close file system image
      close(fsfd);
      return 0;
    }
