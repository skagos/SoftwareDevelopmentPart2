#include "../include/core.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#define PRIME 401
int EditDistance(char* a, int na, char* b, int nb)
{
	return 0;
}

unsigned int HammingDistance(char* a, int na, char* b, int nb)
{
	return 0;
}

struct HammingDistanceStruct* HammingDistanceStructNode;
Index*  BKTreeIndexEdit;
struct Exact_Root* HashTableExact;
int bucket_sizeofHashTableExact;


ErrorCode InitializeIndex(){
	BKTreeIndexEdit=malloc(sizeof(Index));
	HashTableExact=NULL;
	bucket_sizeofHashTableExact=5;/*starting bucket size of hash array*/
	HashTableExact=malloc(sizeof(struct Exact_Root));
	HashTableExact->array=malloc(bucket_sizeofHashTableExact*sizeof(struct Exact_Node*));
	for(int i=0;i<bucket_sizeofHashTableExact;i++)
		HashTableExact->array[i]=NULL;
	/*Hamming struct initilization*/
	int HammingIndexSize=(MAX_WORD_LENGTH-MIN_WORD_LENGTH)+1;
	HammingDistanceStructNode=NULL;
	HammingDistanceStructNode=malloc(sizeof(struct HammingDistanceStruct));
	HammingDistanceStructNode->word_RootPtrArray=malloc(HammingIndexSize*sizeof(struct word_RootPtr));
	for(int i=0;i<HammingIndexSize;i++){
		HammingDistanceStructNode->word_RootPtrArray[i].IndexPtr=NULL;
		HammingDistanceStructNode->word_RootPtrArray[i].word_length=4+i;
	}
	/*******************************/
	if(BKTreeIndexEdit==NULL) return EC_FAIL;
	if(HashTableExact==NULL) return EC_FAIL;
	if(HammingDistanceStructNode==NULL) return EC_FAIL;
	return EC_SUCCESS;
}

ErrorCode DestroyIndex(){
	for(int i=0; i<bucket_sizeofHashTableExact; i++){
		if(HashTableExact->array[i] == NULL) continue;
		struct Exact_Node* start = HashTableExact->array[i];
		struct Exact_Node* start_next = start->next;
		while(1){
			struct payload_node* start1=start->beg;
			struct payload_node* start2=start1->next;
			while(1){
				free(start1);
				start1=start2;
				if(start1==NULL)
					break;
				start2=start2->next;
			}
			free(start->wd);
			free(start);
			start = start_next;
			if(start == NULL) break;
			start_next = start_next->next;
		}
	}
	free(HashTableExact->array);
	free(HashTableExact);
	//call destroy gia to BKTree */
	free(BKTreeIndexEdit);
	free(HammingDistanceStructNode->word_RootPtrArray);
	free(HammingDistanceStructNode);	
	return EC_SUCCESS;
}

ErrorCode StartQuery(QueryID query_id, const char* query_str, MatchType match_type, unsigned int match_dist)
{
	//printf("-----------------------------\n");
	int words_num=0;
	char** words_ofquery=Deduplicate_Method(query_str,&words_num);
	for(int i=0;i<words_num;i++)
		printf("--%s\n",words_ofquery[i]);
	printf("-----------------------------\n");
	if(match_type==0){
		Exact_Put(words_ofquery,words_num,query_id);
		printf("EXACT MATCH\n");
	}
	else if(match_type==1){
		//printf("HAMMING DISTANCE\n");
	}	
	else if(match_type==2){
		//printf("EDIT DISTANCE\n");
	}
	for(int i=0;i<words_num;i++)
		free(words_ofquery[i]);
	free(words_ofquery);
	words_ofquery=NULL;
	if(words_ofquery!=NULL)
		return EC_FAIL;
	/*********************/
	return EC_SUCCESS;
}

ErrorCode EndQuery(QueryID query_id)
{
	return EC_SUCCESS;
}

ErrorCode MatchDocument(DocID doc_id, const char* doc_str)
{
	/*char** words_oftext=Deduplicate_Method(doc_str,&words_num);
	printf("words_num=%d\n",words_num);
	for(int i=0;i<words_num;i++)
		printf("--%s\n",words_oftext[i]);
	for(int i=0;i<words_num;i++)
		free(words_oftext[i]);
	free(words_oftext);*/
	return EC_SUCCESS;
}


ErrorCode GetNextAvailRes(DocID* p_doc_id, unsigned int* p_num_res, QueryID** p_query_ids)
{
	return EC_SUCCESS;
}


ErrorCode build_entry_index(const entry_list* el,MatchType type,Index** ix){
	return EC_SUCCESS;
}


int NextPrime(int N)
{
    bool found = false;
    while(!found){
        found = isPrime(N);
        N++;
    }
    return (N - 1);
}


bool isPrime(int N){
    
    for(int i = 2; i <= (N/2); i++){
        if(N % i == 0) return false;
    }
    return true;
}



unsigned int hashing(const char *word) {
    unsigned int hash = 0, c;

    for (size_t i = 0; word[i] != '\0'; i++) {
        c = (unsigned char)word[i];
        hash = (hash << 3) + (hash >> (sizeof(hash) * CHAR_BIT - 3)) + c;
    }
    return hash;
}

float counting_load_factor(int n, int k){
	float re;
	re=(float)n/(float)k;
	return re;

}

char** Deduplicate_Method(const char* query_str,int* size){
	int BucketsHashTable=10;
	char** word_array=NULL;
	struct Deduplicate_Hash_Array* Deduplication_Array=Initialize_Hash_Array(BucketsHashTable);
	char word[MAX_WORD_LENGTH];
	int len=0;
	for(int i=0;i<strlen(query_str);i++){
		if(query_str[i]==' '){
			word[len]='\0';
			len=0;
			if(search_hash_array(Deduplication_Array,BucketsHashTable,word)==true){
				memset(word,0,MAX_WORD_LENGTH);
				continue;
			}
			float load_factor=counting_load_factor(Deduplication_Array->entries_counter+1,BucketsHashTable);
			if(load_factor>=0.8){
				int old_bucket_num=BucketsHashTable;
				BucketsHashTable=NextPrime(BucketsHashTable*2);
				struct Deduplicate_Hash_Array* new_Deduplicate_Hash_Array=Initialize_Hash_Array(BucketsHashTable);
				for(int j=0;j<old_bucket_num;j++){
					if(Deduplication_Array->array[j]==NULL) continue;
					struct Deduplicate_Node* start=Deduplication_Array->array[j];
					struct Deduplicate_Node* next_start=start->next;
					while(1){
						insert_hash_array(&new_Deduplicate_Hash_Array,BucketsHashTable,start->the_word);
						free(start->the_word);
						free(start);
						start=next_start;
						if(start==NULL) break;
						next_start=next_start->next;
					}
				}
				free(Deduplication_Array->array);
				free(Deduplication_Array);
				insert_hash_array(&new_Deduplicate_Hash_Array,BucketsHashTable,word);
				Deduplication_Array=new_Deduplicate_Hash_Array;
			}
			else insert_hash_array(&Deduplication_Array,BucketsHashTable,word);
			memset(word,0,MAX_WORD_LENGTH);
			continue;
		}
		if(i==strlen(query_str)-1){
			word[len++]=query_str[i];
			word[len]='\0';
			len=0;
			if(search_hash_array(Deduplication_Array,BucketsHashTable,word)==true){
				memset(word,0,MAX_WORD_LENGTH);
				continue;
			}
			float load_factor=counting_load_factor(Deduplication_Array->entries_counter+1,BucketsHashTable);
			if(load_factor>=0.8){
				int old_bucket_num=BucketsHashTable;
				BucketsHashTable=NextPrime(BucketsHashTable*2);
				struct Deduplicate_Hash_Array* new_Deduplicate_Hash_Array=Initialize_Hash_Array(BucketsHashTable);
				for(int j=0;j<old_bucket_num;j++){
					if(Deduplication_Array->array[j]==NULL) continue;
					struct Deduplicate_Node* start=Deduplication_Array->array[j];
					struct Deduplicate_Node* next_start=start->next;
					while(1){
						insert_hash_array(&new_Deduplicate_Hash_Array,BucketsHashTable,start->the_word);
						free(start->the_word);
						free(start);
						start=next_start;
						if(start==NULL) break;
						next_start=next_start->next;
					}
				}
				free(Deduplication_Array->array);
				free(Deduplication_Array);
				insert_hash_array(&new_Deduplicate_Hash_Array,BucketsHashTable,word);
				Deduplication_Array=new_Deduplicate_Hash_Array;
			}
			else insert_hash_array(&Deduplication_Array,BucketsHashTable,word);
			memset(word,0,MAX_WORD_LENGTH);
			continue;
		}
		word[len++]=query_str[i];
	}
	word_array=malloc(Deduplication_Array->entries_counter*sizeof(char*));
	int coun=0;
	for(int i=0;i<BucketsHashTable;i++){
		if(Deduplication_Array->array[i]==NULL)	continue;
		struct Deduplicate_Node* start=Deduplication_Array->array[i];
		while(1){
			word_array[coun]=malloc((strlen(start->the_word)+1)*sizeof(char));
			strcpy(word_array[coun],start->the_word);
			coun++;
			start=start->next;
			if(start==NULL) break;
		}
	}
	*size=coun;
	free_Deduplication_Hash_Array(Deduplication_Array,BucketsHashTable);
	return word_array;
} 



ErrorCode destroy_entry_index(Index* ix){
	if(ix==NULL) return EC_FAIL;
	destroy_index_nodes(ix->root);
	free(ix);
	ix=NULL;
	if(ix!=NULL) return EC_FAIL;	
	return EC_SUCCESS;
}

void destroy_index_nodes(struct NodeIndex* node){
	for(struct NodeIndex* s=node->firstChild;s!=NULL;s=s->next){
		destroy_index_nodes(s);
	}
	free(node->wd);
	free(node);
}



void free_Deduplication_Hash_Array(struct Deduplicate_Hash_Array* hash,int BucketsHashTable){
	for(int i=0;i<BucketsHashTable;i++){
		if(hash->array[i]==NULL) continue;
		struct Deduplicate_Node* start=hash->array[i];
		struct Deduplicate_Node* next_start=start->next;
		while(1){
			free(start->the_word);
			free(start);
			start=next_start;
			if(start==NULL) break;
			next_start=next_start->next;
		}
	}
	free(hash->array);
	free(hash);
}

struct Deduplicate_Hash_Array* Initialize_Hash_Array(int BucketsHashTable){
	struct Deduplicate_Hash_Array* Deduplication_Array=malloc(sizeof(struct Deduplicate_Hash_Array));
	Deduplication_Array->entries_counter=0;
	Deduplication_Array->array=NULL;
	Deduplication_Array->array=malloc(BucketsHashTable*sizeof(struct Deduplicate_Node));
	for(int i=0;i<BucketsHashTable;i++)
		Deduplication_Array->array[i]=NULL;
	return Deduplication_Array;
}



void insert_hash_array(struct Deduplicate_Hash_Array** hash,int BucketsHashTable,char* word){
	int bucket_num=hashing(word)%BucketsHashTable;
	(*hash)->entries_counter++;
	struct Deduplicate_Node* start=(*hash)->array[bucket_num];
	struct Deduplicate_Node* new_node=malloc(sizeof(struct Deduplicate_Node));
	new_node->next=NULL;
	new_node->the_word=malloc((strlen(word)+1)*sizeof(char));
	strcpy(new_node->the_word,word);
	if(start==NULL) (*hash)->array[bucket_num]=new_node;
	else{
		struct Deduplicate_Node* node= (*hash)->array[bucket_num];
		while(1){
			if(node->next==NULL){
				node->next=new_node;
				break;
			}
			node=node->next;
		}
	}	
}

bool search_hash_array(struct Deduplicate_Hash_Array* hash,int BucketsHashTable,char* word){
	int bucket_num=hashing(word)%BucketsHashTable;
	int found=0;
	struct Deduplicate_Node* start=hash->array[bucket_num];
	if(start==NULL) return false;
	else{
		struct Deduplicate_Node* node=start;
		while(1){
			if(!strcmp(node->the_word,word)){
				found=1;
				break;
			}
			node=node->next;
			if(node==NULL) break;
		}
	}
	if(found==1) return true;
	else return false;
}


void Exact_Put(char** words,int num,QueryID query_id){
	for(int i=0;i<num;i++){
		int bucket_num=hashing(words[i])%bucket_sizeofHashTableExact;
		bool val1=check_if_word_exists(words[i],bucket_num,query_id);
		if(val1==true) continue;
		float load_factor=counting_load_factor(HashTableExact->entries_counter+1,bucket_sizeofHashTableExact);
		if(load_factor>=0.8){
			int old_size=bucket_sizeofHashTableExact;
			bucket_sizeofHashTableExact=NextPrime(bucket_sizeofHashTableExact*2);
			struct Exact_Root* NewHashTableExact=malloc(sizeof(struct Exact_Root));
			bucket_num=hashing(words[i])%bucket_sizeofHashTableExact;
			NewHashTableExact->entries_counter=0;
			bucket_num=hashing(words[i])%bucket_sizeofHashTableExact;
			NewHashTableExact->array=NULL;
			NewHashTableExact->array=malloc(bucket_sizeofHashTableExact*sizeof(struct Exact_Node*));
			for(int j=0;j<bucket_sizeofHashTableExact;j++)
				NewHashTableExact->array[j]=NULL;
			for(int j=0;j<old_size;j++){
				if(HashTableExact->array[j]==NULL) continue;
				struct Exact_Node* start=HashTableExact->array[j];
				struct Exact_Node* next_start=start->next;
				while(1){
					struct payload_node* ptr_beg=start->beg;
					insert_HashTableExact_V2(NewHashTableExact,start->wd,bucket_num,ptr_beg);
					free(start->wd);
					free(start);
					start=next_start;
					if(start==NULL) break;
					next_start=next_start->next;
				}
			}
			free(HashTableExact->array);
			free(HashTableExact);
			HashTableExact=NewHashTableExact;
		}
		else insert_HashTableExact(words[i],bucket_num,query_id);
	}
}


void insert_HashTableExact(const char* word,int bucket_num,QueryID query_id){
	struct Exact_Node* node=NULL;
	node=malloc(sizeof(struct Exact_Node));
	node->next=NULL;
	node->wd=NULL;
	node->beg=NULL;
	node->wd=malloc((strlen(word)+1)*sizeof(char));
	strcpy(node->wd,word);
	struct payload_node* Pnode=NULL;
	Pnode=malloc(sizeof(struct payload_node));
	Pnode->next=NULL;
	Pnode->query_id=query_id;
	node->beg=Pnode;
	struct Exact_Node* start=HashTableExact->array[bucket_num];
	if(start==NULL) HashTableExact->array[bucket_num]=node;
	else{
		while(1){
			if(start->next==NULL){
				start->next=node;
				break;
			}
			start=start->next;
		}
	}
	HashTableExact->entries_counter++;
}


void insert_HashTableExact_V2(struct Exact_Root* head,char* word,int bucket_num,struct payload_node* payload_ptr){
	struct Exact_Node* node=malloc(sizeof(struct Exact_Node));
	node->next=NULL;
	node->wd=NULL;
	node->beg=NULL;
	node->beg=payload_ptr;
	node->wd=malloc((strlen(word)+1)*sizeof(char));
	strcpy(node->wd,word);
	struct Exact_Node* start=head->array[bucket_num];
	if(start==NULL) head->array[bucket_num]=node;
	else{
		while(1){
			if(start->next==NULL){
				start->next=node;
				break;
			}
			start=start->next;
		}
	}
	head->entries_counter++;
}



bool check_if_word_exists(char* word,int bucket_num,QueryID query_id){
	struct Exact_Node* start=HashTableExact->array[bucket_num];
	bool found=false;
	if(start==NULL) return found;
	while(1){
		if(!strcmp(word,start->wd)){
			struct payload_node* Pnode=NULL;
			Pnode=malloc(sizeof(struct payload_node));
			Pnode->next=NULL;
			Pnode->query_id=query_id;
			struct payload_node* s1=start->beg;
			if(s1==NULL) start->beg=Pnode;
			else{
				while(1){
					if(s1->next==NULL){
						s1->next=Pnode;
						break;
					}
					s1=s1->next;
				}
			}
			found=true;
			break;
		}
		start=start->next;
		if(start==NULL)
			break;
	}
	return found;
}