#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// boolean: is_verb = is verbose
// int: sib = set index bits, assoc = associativity, blocbits = block bits
// char*: trace_name = name of valgrind trace
int is_verb, sib, assoc, blocbits;
char* trace_name;
int access_num = 0;
// op = L, S, or M
char op;
char *str1;
char *str2;

int hits = 0;
int misses = 0;
int evictions = 0;

typedef struct 
{
    int valid_bit;
    int tag;
    // most recent access number
    int rec_acc_num;
    int block_start;

}line;

typedef struct 
{
    line* lines;
}set;

typedef struct
{
    set* sets;
}cache;

void parse(char* filename, cache* c);
void evict(cache* c, int addr, int size);
void load(cache* c, int addr, int size);
void store(cache* c, int addr, int size);
void modify(cache* c, int addr, int size);

void cache_free(cache* c)
{
    free(c->sets->lines);
    free(c->sets);
    free(c);
}

cache *cache_init()
{
    cache* return_cache = malloc(sizeof(cache));
    return_cache->sets = (set*) (malloc((1 << sib ) * sizeof(set)));
    int i;
    for(i=0; i<(1<<sib); i++)
    {
        return_cache->sets[i].lines = (line*) (malloc(assoc * sizeof(line)));
    }
    return return_cache;
}



void parse(char* filename, cache* c)
{
    FILE* fi;
    char line[256];
    fi = fopen(filename, "r");
    if(fi == NULL){
        exit(EXIT_FAILURE);
    }
    while(fgets(line, sizeof(line), fi))
    {
        // address/pointer
        int addr;
        int size;
        char junk;
        op = line[1];

        // data load
        if(op == 'L') 
        {
            sscanf(line, " %c %x,%d", &junk, &addr, &size);
            load(c, addr, size);
            if(is_verb)
                printf("%c %x,%d %s\n", op, addr, size, str1);
        }

        // data store
        else if(op == 'S') 
        {
            sscanf(line, " %c %x,%d", &junk, &addr, &size);
            store(c, addr, size);
            if(is_verb)
                printf("%c %x,%d %s\n", op, addr, size, str1);
        }
        
        // data modify
        else if(op == 'M') 
        {
            sscanf(line, " %c %x,%d", &junk, &addr, &size);
            modify(c, addr, size);
            if(is_verb)
                printf("%c %x,%d %s %s\n", op, addr, size, str2, str1);
        }
    }

}

void load(cache* c, int addr, int size)
{
    access_num++;
    int block_start_index = (addr/(1<<blocbits)) * (1<<blocbits);
    int set_index = (addr>>blocbits) & ((1<<sib)-1);
    int tag = (int)((unsigned int)addr >> (blocbits+sib));
    
    // check if any tags match for a hit
    int i;
    for(i=0; i<assoc; i++)
    {
        if((c->sets[set_index].lines[i].tag == tag) && c->sets[set_index].lines[i].valid_bit == 1)
        {
            c->sets[set_index].lines[i].block_start = block_start_index;
            c->sets[set_index].lines[i].valid_bit = 1;
            c->sets[set_index].lines[i].rec_acc_num = access_num;
            hits++;
            str1 = "hit";
            //if(is_verb) 
              //  printf("%c %x,%d hit\n", op, addr, size);
            return;
        }
    }
    
    // check if there is open line
    for(i=0; i<assoc; i++)
    {
        if(c->sets[set_index].lines[i].valid_bit == 0)
        {
            c->sets[set_index].lines[i].block_start = block_start_index;
            c->sets[set_index].lines[i].valid_bit = 1;
            c->sets[set_index].lines[i].rec_acc_num = access_num;
            c->sets[set_index].lines[i].tag = tag;
            misses++;
            str1 = "miss";
            //if(is_verb)
             // ;;  printf("%c %x,%d miss\n", op, addr, size);
            return;
        }
    }
    
    // if nothing else, evict LRU
    int min=0;
    int min_index=0;
    for(i=0; i<assoc; i++)
    {
            int m;
            if(((m=c->sets[set_index].lines[i].rec_acc_num) < min)||min==0)
            {
                min=m;
                min_index=i;
            }
        
    }    
    c->sets[set_index].lines[min_index].block_start = block_start_index;
    c->sets[set_index].lines[min_index].valid_bit = 1;
    c->sets[set_index].lines[min_index].rec_acc_num = access_num;
    c->sets[set_index].lines[min_index].tag = tag;
    evictions++;
    misses++;
    str1 = "miss eviction";
    //if(is_verb)
      //  printf("%c %x,%d miss evict\n", op, addr, size);
    return;  


}

void store(cache* c, int addr, int size)
{
    access_num++;
   // int block_start_index = (addr/(1<<blocbits)) * (1<<blocbits);
    int set_index = (addr>>blocbits) & ((1<<sib)-1);
    int tag = (int)((unsigned int)addr >> (blocbits+sib));
    
    // check if any tags match for a hit
    int i;
    for(i=0; i<assoc; i++)
    {
        if(c->sets[set_index].lines[i].tag == tag && c->sets[set_index].lines[i].valid_bit == 1)
        {
            hits++;
            str1 = "hit";
          //  if(is_verb) 
            //    printf("%c %x,%d hit\n", op, addr, size);
            return;
        }
    }
    load(c, addr, size);
    return;
}

void modify(cache* c, int addr, int size)
{
    load(c, addr, size);
    str2 = str1;
    store(c, addr, size);
}

void evict(cache* c, int addr, int size)
{
    int block_start_index = (addr/(1<<blocbits)) * (1<<blocbits);
    int set_index = (addr>>blocbits) & ((1<<sib)-1);
    int tag = (int)((unsigned int)addr >> (blocbits+sib));
        
    int i;
    int min=0;
    int min_index=0;
    for(i=0; i<assoc; i++)
    {
            int m;
            if(((m=c->sets[set_index].lines[i].rec_acc_num) < min)||min==0)
            {
                min=m;
                min_index=i;
            }
        
    }    
    c->sets[set_index].lines[min_index].block_start = block_start_index;
    c->sets[set_index].lines[min_index].valid_bit = 1;
    c->sets[set_index].lines[min_index].rec_acc_num = access_num;
    c->sets[set_index].lines[min_index].tag = tag;
    evictions++;
    misses++;
  //  if(is_verb)
    //    printf("%c %x,%d miss evict\n", op, addr, size);
    return;  

}

// void printSummary(int h, int m, int e)
// {
//     printf("hits:%d misses:%d evictions:%d\n", h, m, e);
// }


void simulate(int argc, char* argv[])
{
    int i;
    for(i=1; i<argc; i++)
    {
        if(!strcmp(argv[i], "-v")) is_verb = 1;
        else if(!strcmp(argv[i], "-s"))
        {
            sib = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i], "-E"))
        {
            assoc = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i], "-b"))
        {
            blocbits = atoi(argv[++i]);
        }
        else if(!strcmp(argv[i], "-t"))
        {
            trace_name = argv[++i];
        }
    }

    cache *tor = cache_init();
    parse(trace_name, tor);
    cache_free(tor);
    return;
}

int main(int argc, char* argv[])
{
    simulate(argc, argv);
    printSummary(hits, misses, evictions);

    return 0;
}