#include "config.h"

#include <openssl/evp.h>
#include <assert.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <xalloc.h>

#include "xchdir.h"
#include "contents.h"
#include "atom.h"
#include "hash.h"
#include "hash_md5_sha1.h"
#include "cur_sys_pkg.h"


#define HASH_SIZE 32
#define SIZE_STR_VAR_DB_PKG 12

//private

//data
typedef struct cur_pkg_tree_node {
  char *key;
  char *hash_buffer;
  struct cur_pkg_tree_node *greater;
  struct cur_pkg_tree_node *minor;
}cur_pkg_tree_node;


//functions
static void add_node(cur_pkg_tree_node **root,char *data,char *key)
{
  if(*root==NULL)
  {
    *root=xmalloc(sizeof(**root));
    (*root)->key=key;
    (*root)->hash_buffer=data;
    (*root)->greater=NULL;
    (*root)->minor=NULL;
    return;
  }

  int is_greater=strncmp(key,(*root)->key,HASH_SIZE);
  
if(!is_greater){
    printf("you are reading the same file twice, check CONTENTS file\n");
  }
  
  if(is_greater > 0){
    return add_node(&(*root)->greater,data,key);
  }
  return add_node(&(*root)->minor,data,key);
}

static char *hash_from_file(char *file_path_complete)
{
  char *out = NULL;
  int fd = open(file_path_complete,O_RDONLY);
  out=hash_file_at(fd,file_path_complete,HASH_MD5);
  close(fd);
  return strdup(out);
}

void read_file_add_data(void *root_to_conv)
{
  cur_pkg_tree_node ** root=(cur_pkg_tree_node **) root_to_conv;
  FILE *CONTENTS=fopen("./CONTENTS","r");
  int byte_read = 0;
  char *line_buffer=NULL;
  size_t line_buffer_size=0;
  contents_entry *line_cont=NULL;
  char *key=NULL;

  //read file CONTENTS
  while( (byte_read=getline(&line_buffer,&line_buffer_size,CONTENTS)) != -1 )
  {
    if(line_buffer[0]=='o' && line_buffer[1]=='b' && line_buffer[2]=='j')
    {
      line_cont=contents_parse_line_general(line_buffer,byte_read);
      assert(line_cont!=NULL);
      key=hash_from_string(line_cont->name,(size_t) ((line_cont->digest-1)- line_cont->name));
      add_node(root,strdup(line_cont->digest),strdup(key));
      key=NULL;
    }
  }

  fclose(CONTENTS);
  free(line_buffer);
}

static int find_in_tree(cur_pkg_tree_node **root,char * key,char *hash)
{
  if(!strcmp(hash,"-1")) return 1;

  if((*root) != NULL)
  { 
    int is_greater=strncmp(key,(*root)->key,HASH_SIZE);
    
    if(is_greater == 0){
      return !strcmp(hash,(*root)->hash_buffer); 
    }else if(is_greater < 0){
      return find_in_tree(&(*root)->minor,key,hash);
    }else {
      return find_in_tree(&(*root)->greater,key,hash);
    }
  }
  return 0;
}

//public
int is_default(cur_pkg_tree_node *root,char *file_path_complete)
{
  char *key;
  int res=0;
  char *hash =NULL;

  hash = hash_from_file(file_path_complete);
  key= hash_from_string(file_path_complete,strlen(file_path_complete));
  res = find_in_tree(&root,key,hash);

  free(hash);
  free(key);
  key=NULL;
  hash=NULL;

  return res;
}

void destroy_cur_pkg_tree(cur_pkg_tree_node **root)
{
  
  if((*root)!=NULL)
  {
    destroy_cur_pkg_tree(&(*root)->greater);
    destroy_cur_pkg_tree(&(*root)->minor);

    free((*root)->hash_buffer);
    (*root)->hash_buffer=NULL;

    free((*root)->key);
    (*root)->key=NULL;
    
    free((*root));
    (*root)=NULL;

    *root = NULL;
  }
}

void in_order_visit(cur_pkg_tree_node *root)
{
  if(root!=NULL)
  {
    if(root->minor!=NULL) in_order_visit(root->minor);
    printf("[%s,%s]\n",root->key,root->hash_buffer);
    if(root->greater!=NULL) in_order_visit(root->greater);
  }
}
