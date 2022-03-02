#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/shm.h>


#include <time.h>

#include "client_library.h"

#include "include.h"

// need procedure that takes in byte array and sync calls the server
// and then gets stuff in return
// returns pointer to compressed buffer


void send_original_file(unsigned char *data, unsigned long file_len, mqd_t *return_q_ptr, int *return_q_id) {
  mqd_t mq_snd_open = mq_open(MAIN_QUEUE_PATH, O_WRONLY);

  // make shared memory here
  int segment_id = shmget(IPC_PRIVATE, file_len, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

  char *sh_mem = (char *) shmat(segment_id, NULL, 0);

  /* printf("virtual address: %d\n", (int) sh_mem); */

  //char * text = "hello there\n";
  memmove(sh_mem, data, file_len );


  // random number generator init
  srand(time(0));

  int upper = 9999999;
  int lower = 1000000;
  int randomId = (rand() % (upper - lower + 1)) + lower;


  // create a new message queue for recieving a return message
  struct mq_attr attr;
  attr.mq_curmsgs = 0;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = 50;
  char id[8]; // 7 long

  *return_q_id = randomId;
  sprintf(id, "%d", randomId);

  char idPath[9];
  sprintf(idPath, "/%d", randomId);

  mqd_t mq_return = mq_open(idPath, O_CREAT | O_RDWR, 0777, &attr);
  *return_q_ptr = mq_return;
  // need to error check to see that this queue is new

  char buf[8192]; // figure out what this is for

  printf("segment id: %d\n", segment_id);
  sprintf(buf, "%d%d", randomId, segment_id); // id, then stringified segment id
  int len = strlen(buf);
  printf("the q id: %d\n", randomId);
  printf("the whole message: %s\n", buf);


  int mq_ret = mq_send(mq_snd_open, buf, len+1, 0);
  if (mq_ret == -1){
    printf(" messeage que is not working\n");

  }else{
    printf("Message q is working\n");
  }


}

unsigned char * sync_compress(unsigned char *data, unsigned long file_len) {
  // qclient.c code here
  // but not the file reader part

  mqd_t *return_q;
  int * return_q_id;
  send_original_file(data, file_len, return_q, return_q_id);

  char return_message_buffer[64]; // should probably unify these sizes

  // now do sync call to wait to receive a message back
  mq_receive(*return_q, return_message_buffer, sizeof(return_message_buffer), NULL);
  /* printf("message queue has: %s\n", return_message_buffer); */

  // now destroy the message queue that was used to get the compressed file back
  mq_close(*return_q);
  char idPath[9];
  sprintf(idPath, "/%d", *return_q_id);
  mq_unlink(idPath);


  // just need to grab the segment id from the message

  int segment_id = atoi(return_message_buffer);

  char *sh_mem = (char *) shmat(segment_id, NULL, 0);

  // memcpy stuff from shared mem to the compressed data buffer
  // TODO:


  unsigned char *compressed_buffer;
  return compressed_buffer;
}

void print_stuff() {
  printf("this is stuff\n");
}
