#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bitmap.h"
#include "list.h"
#include "hash.h"
#include "hex_dump.h"

/* the number of list, hash, bitmap */
size_t l_size = 0; int l_idx = 0;
size_t h_size = 0; int h_idx = 0;
size_t b_size = 0; int b_idx = 0;

typedef struct {
	char name[100]; /* name of list variable */
	struct list *l;
} L;

typedef struct {
	char name[100]; /* name of hash */
	struct hash *h;
} H;

typedef struct {
	char name[100]; /* name of bitmap */
	struct bitmap *b;
} B;


/* memory of LIST, HASH, BITMAP */
/* the numebr of LIST, HASH, BITMAP is less than or equal to 10 */
L arrL[30];
H arrH[30];
B arrB[30];

/* find list, hash, bitmap such name is *name */
struct list* findList(char *name){
	for(int i = 0; i < l_idx; i++){
		if(strcmp(arrL[i].name, name) == 0){
			return arrL[i].l;
		}
	}
	return NULL; /* cannot find list */
}

struct hash* find_hash(char *name){
	for(int i = 0; i < h_idx; i++){
		if(strcmp(arrH[i].name, name) == 0){
			return arrH[i].h;
		}
	}
	return NULL; /* cannot find hash */
}

struct bitmap* find_bitmap(char *name){
	for(int i = 0; i < b_idx; i++){
		if(strcmp(arrB[i].name, name) == 0){
			return arrB[i].b;
		}
	}
	return NULL; /* cannot find bitmap */
}

int main(){

	while(1){
		char buffer[100] = {0, }; /* input whole string */
		char cmd[100] = {0, };	/* command */
		char s1[100] = {0, };	/* Data Structure */
		char s2[100] = {0, };
		char s3[100] = {0, };
		char s4[100] = {0, };
		char s5[100] = {0, };

		fgets(buffer, 102, stdin);
		sscanf(buffer, "%s %s %s %s %s %s", cmd, s1, s2, s3, s4, s5);
		/* terminate this program */
		if(strcmp(cmd, "quit") == 0) break;

	
		if(strcmp(cmd, "create") == 0){
			/* s1 is data structure : list, hashtable, bitmap */
			/* s2 is variable name : list0, hash2, ... */

			/* create list */
			if(strcmp(s1, "list") == 0){
				/* s2 is list name*/
				arrL[l_idx].l = (struct list*)malloc(sizeof(struct list));
				list_init(arrL[l_idx].l);
				arrL[l_idx].name[0] = 0;
				strcpy(arrL[l_idx].name, s2);
			
				l_idx++;
				l_size++;
			} // create list
			else if(strcmp(s1, "hashtable") == 0){
				/* s2 is hashtable name */
				arrH[h_idx].h = (struct hash *)malloc(sizeof(struct hash));
				hash_init(arrH[h_idx].h, &my_hash_func, &my_hash_less, NULL);
				
				arrH[h_idx].name[0] = 0;
				strcpy(arrH[h_idx].name, s2);

				h_idx++;
				h_size++;
			}

			else if(strcmp(s1, "bitmap") == 0){
				/* s2 is bitmap name */
				/* s3 is bit_cnt of bitmap*/
				size_t bit_cnt = (size_t)atoi(s3);
				arrB[b_idx].b = bitmap_create(bit_cnt);
				arrB[b_idx].name[0] = 0;
				strcpy(arrB[b_idx].name, s2);

				b_idx++;
				b_size++;
			}
		}
		else if(strcmp(cmd, "delete") == 0){
			/* free list, hash, bitmap */
			/* s1 is list, hash, bitmap name */
			/* search order : list -> hash -> bitmap */
			struct list *l = findList(s1);
			if(l != NULL) {
				free(l);
				l_size--;
			}
			struct hash *h = find_hash(s1);
			if(h != NULL) {
				hash_destroy(h, &destructor);
				h_size--;
			}

			struct bitmap *b = find_bitmap(s1);
			if(b != NULL){
				bitmap_destroy(b);
				b_size--;
			}
			
			//printf("I deleted list %s\n", s1);
		}
		else if(strcmp(cmd, "dumpdata") == 0){
			/* s1 is list, hash, bitmap name */ 
			int isEmpty = 0;
			
			/* assume the list */
			struct list *l = findList(s1);
			if(l != NULL) {
				struct list_elem *e;
				if(list_begin(l) == list_end(l)) isEmpty = 1;
				for(e = list_begin(l); e != list_end(l); e = list_next(e)){
					struct list_item *temp = list_entry(e, struct list_item, elem);
					printf ("%d ", temp->data);
				}
				if(!isEmpty) printf ("\n");
			}

			/* assume the hash */
			struct hash *h = find_hash(s1);
			if(h != NULL){
				if(hash_size(h) == 0) isEmpty = 1;
				struct hash_iterator i;
				hash_first(&i, h);
				while(hash_next(&i)){
					struct hash_elem *he = hash_cur(&i);
					struct hash_item *item = hash_entry(he, struct hash_item, helem);
					printf ("%d ", item->key);
				}

				if(!isEmpty) printf ("\n");
			}

			/* assume the bitmap */
			struct bitmap *b = find_bitmap(s1);
			if(b != NULL){
				if(bitmap_size(b) == 0) isEmpty = 1;
				for(size_t i = 0; i < bitmap_size(b); i++){
					if(bitmap_test(b, i) == true) printf ("1");
					else printf ("0");
				}
				if(!isEmpty) printf ("\n");
			}

		}// dumpdata(list, hash, bitmap)


		/* commands for list */
		else if(strcmp(cmd, "list_front") == 0){
			/* s1 is list name */
			struct list *l = findList(s1);
			struct list_elem *e = list_front(l);
			struct list_item *temp = list_entry(e, struct list_item, elem);
			printf ("%d\n", temp->data);

		}

		else if(strcmp(cmd, "list_back") == 0){
			/* s1 is list name */
			struct list *l = findList(s1);
			struct list_elem *e = list_back(l);
			struct list_item *temp = list_entry(e, struct list_item, elem);
			printf ("%d\n", temp->data);
		}

		else if(strcmp(cmd, "list_insert") == 0){
			/* s1 is list name, s2 is index of list, s3 is input value */
			int idx = atoi(s2), data = atoi(s3);
			struct list *l = findList(s1);
			struct list_elem *b, *e;
			e = (struct list_elem *)malloc(sizeof(struct list_elem));
			struct list_item *temp = list_entry(e, struct list_item, elem);
			temp->data = data;

	
			int iter = 0;
			for(b = list_begin(l); ; b = list_next(b), iter++){
				//printf ("iter = %d", iter);
				if(iter == idx){
					list_insert(b, e);
					break;
				}
			}
			

		}

		else if(strcmp(cmd, "list_insert_ordered") == 0){
			/* s1 is list name, s2 is input value */
			int data = atoi(s2);
			struct list_elem *e = (struct list_elem *)malloc(sizeof(struct list_elem));
			struct list_item *temp = list_entry(e, struct list_item, elem);
			temp->data = data;
			struct list *l = findList(s1);
			list_insert_ordered(l, e, &cmpData, NULL);
		}

		else if(strcmp(cmd, "list_empty") == 0){
			/* s1 is list name */
			struct list *l = findList(s1);
			if(list_empty(l)) printf ("true\n");
			else printf ("false\n");
		}

		else if(strcmp(cmd, "list_size") == 0){
			/* s1 is list name */
			struct list *l = findList(s1);
			printf ("%zu\n", list_size(l));

		}

		else if(strcmp(cmd, "list_max") == 0){
			/* s1 is list name */
			struct list *l = findList(s1);
			struct list_elem *e = list_max(l, &cmpData, NULL);
			struct list_item *item = list_entry(e, struct list_item, elem);
			printf ("%d\n", item->data);

		}

		else if(strcmp(cmd, "list_min") == 0){
			/* s1 is list name */
			struct list *l = findList(s1);
			struct list_elem *e = list_min(l, &cmpData, NULL);
			struct list_item *item = list_entry(e, struct list_item, elem);
			printf ("%d\n", item->data);
		}

		else if(strcmp(cmd, "list_push_back") == 0){
			/* s1 is list name */
			/* s2 is int_type data */
			/* find list(s1) */
			struct list *l = findList(s1);
			struct list_elem *e = (struct list_elem *)malloc(sizeof(struct list_elem));
			struct list_item *temp = list_entry(e, struct list_item, elem);
			temp->data = atoi(s2);	/* data */
			list_push_back(l, e);
		}

		else if(strcmp(cmd, "list_push_front") == 0){
			/* s1 is list name */
			/* s2 is int_type data */
			/* find list(s1) */
			struct list *l = findList(s1);
			struct list_elem *e = (struct list_elem *)malloc(sizeof(struct list_elem));
			struct list_item *temp = list_entry(e, struct list_item, elem);
			temp->data = atoi(s2);	/* data */
			list_push_front(l, e);
		}

		else if(strcmp(cmd, "list_pop_back") == 0){
			/* s1 is list name */
			/* find list(s1 */
			struct list *l = findList(s1);
			struct list_elem *e = list_pop_back(l);
			free(e);
		}
		else if(strcmp(cmd, "list_pop_front") == 0){
			/* s1 is list name */
			struct list *l = findList(s1);
			struct list_elem *e = list_pop_front(l);
			free(e);
		}
		else if(strcmp(cmd, "list_remove") == 0){
			/* s1 is list name */
			/* s2 is index, start with 0 */
			int index = atoi(s2);
			int cur = 0;
			struct list *l = findList(s1);
			struct list_elem *e, *re;
			for(e = list_begin(l); e != list_end(l);  e = list_next(e), cur++){
				if(cur == index){
					e = list_remove(e);
					break;
				}
			}
			
		}

		else if(strcmp(cmd, "list_reverse") == 0){
			/* s1 is list name */
			struct list *l = findList(s1);
			int temp[100];
			int i_temp = 0;
			struct list_elem *e;
			for(e = list_begin(l) ; e != list_end(l); e = list_next(e)){
				struct list_item *cur = list_entry(e, struct list_item, elem);
				temp[i_temp++] = cur->data;
			}
			i_temp--;
			for(e = list_begin(l); e != list_end(l); e = list_next(e)){
				struct list_item *cur = list_entry(e, struct list_item, elem);
				cur->data = temp[i_temp--];
			}

		}

		else if(strcmp(cmd, "list_shuffle") == 0){
			/* s1 is list name */
			struct list *l = findList(s1);
			list_shuffle(l);

		}

		else if(strcmp(cmd, "list_sort") == 0){
			/* s1 is list name */
			struct list *l = findList(s1);
			//list_less_func less;
			list_sort(l, &cmpData, NULL);
		}

		else if(strcmp(cmd, "list_splice") == 0){
			/* s1 and s3 are list name */
			/* s2 is target index of list(s1) */
			/* s4 and s5 are range of list(s3) */
			int dst = atoi(s2);
			int src1 = atoi(s4), src2 = atoi(s5);
			struct list_elem *before, *first, *last;
			struct list_elem *e;
			struct list *l1, *l2;
			l1 = findList(s1); l2 = findList(s3);
			
			/* find bigin */
			int iter = 0;
			for(e = list_begin(l1); e != list_end(l1); e = list_next(e), iter++){
				if(iter == dst){
					before = e;
					break;
				}
			}

			/* find first, last */
			iter = 0;
			for(e = list_begin(l2); e != list_end(l2); e = list_next(e), iter++){
				if(iter == src1){
					first = e;
				}
				if(iter == src2){
					last = e;
					break;
				}
			}

			list_splice(before, first, last);

		}

		else if(strcmp(cmd, "list_swap") == 0){
			/* s1 is list name, s2 and s3 is index of list */
			struct list *l = findList(s1);
			struct list_elem *e, *e1, *e2;
			int src = atoi(s2), dst = atoi(s3);
			int iter = 0;
			for(e = list_begin(l); e != list_end(l); e = list_next(e), iter++){
				if(iter == src) e1 = e; 
				if(iter == dst) e2 = e; 
			}
			list_swap(e1, e2);
		}

		else if(strcmp(cmd, "list_unique") == 0){
			/* s1, s2 are list name */
			struct list *l1, *l2;
			l1 = findList(s1);
			l2 = findList(s2);
			list_unique(l1, l2, &cmpData, NULL);

		}

		/* commands for hash */
		else if(strcmp(cmd, "hash_delete") == 0){
			/* s1 is hash name */
			/* s2 is key will be deleted */
			struct hash *h = find_hash(s1);
			struct hash_elem *he = (struct hash_elem *)malloc(sizeof(struct hash_elem));
			struct hash_item *item = hash_entry(he, struct hash_item, helem);
			item->key = atoi(s2);
			hash_delete(h, he);

		}
		
		else if(strcmp(cmd, "hash_apply") == 0){
			/* s1 is hash name */
			/* s2 is action_func name */
			struct hash *h = find_hash(s1);
			if(strcmp(s2, "square") == 0){
				hash_apply(h, &square);
			}
			else if(strcmp(s2, "triple") == 0){
				hash_apply(h, &triple);
			}
		}

		else if(strcmp(cmd, "hash_size") == 0){
			/* s1 is hash name */
			struct hash *h = find_hash(s1);
			printf ("%zu\n", hash_size(h));
		}

		else if(strcmp(cmd, "hash_empty") == 0){
			/* s1 is hash name */
			struct hash *h = find_hash(s1);
			if(hash_size(h) == 0) printf ("true\n");
			else printf ("false\n");
		}

		else if(strcmp(cmd, "hash_clear") == 0){
			/* s1 is hash name */
			struct hash *h = find_hash(s1);
			hash_clear(h, destructor);

		}
		else if(strcmp(cmd, "hash_find") == 0){
			/* s1 is hash name */
			/* s2 is target key(== value) */
			struct hash *h = find_hash(s1);
			struct hash_elem *he = (struct hash_elem *)malloc(sizeof(struct hash_elem));
			struct hash_item *item = hash_entry(he, struct hash_item, helem);
			item->key = atoi(s2);
			struct hash_elem *target = hash_find(h, he);
			if(target != NULL){
				printf ("%d\n", item->key);
			}
		}

		else if(strcmp(cmd, "hash_insert") == 0){
			/* s1 is hash name */
			/* s2 is key */
			struct hash *h = find_hash(s1);
			struct hash_elem *he = (struct hash_elem *)malloc(sizeof(struct hash_elem));
			struct hash_item *item = hash_entry(he, struct hash_item, helem);
			item->key = atoi(s2);
			hash_insert(h, he);
		}

		else if(strcmp(cmd, "hash_replace") == 0){
			/* s1 is hash name */
			/* s2 is key */
			struct hash *h = find_hash(s1);
			struct hash_elem *he = (struct hash_elem *)malloc(sizeof(struct hash_elem));
			struct hash_item *item = hash_entry(he, struct hash_item, helem);
			item->key = atoi(s2);
			hash_replace(h, he);
		}




		/* commands for bitmap */
		else if(strcmp(cmd, "bitmap_all") == 0){
			/* s1 is bitmap name */
			/* s2 is start, s3 is cnt */
			struct bitmap *b = find_bitmap(s1);
			size_t start = (size_t)atoi(s2);
			size_t cnt = (size_t)atoi(s3);
			if(bitmap_all(b, start, cnt) == 0) printf ("false\n");
			else printf ("true\n");
		}

		else if(strcmp(cmd, "bitmap_any") == 0){
			struct bitmap *b = find_bitmap(s1);
			size_t start, cnt;
			start = (size_t)atoi(s2);
			cnt = (size_t)atoi(s3);
			if(bitmap_any(b, start, cnt) == 0) printf ("false\n");
			else printf ("true\n");
		}

		else if(strcmp(cmd, "bitmap_contains") == 0){
			/* s1 is bitmap name */
			/* s2 is start, s3 is cnt, s4 is boolean value */
			struct bitmap *b = find_bitmap(s1);
			size_t start = (size_t)atoi(s2);
			size_t cnt = (size_t)atoi(s3);
			bool value = true;
			if(strcmp(s4, "false") == 0) value = false;
			if(bitmap_contains(b, start, cnt, value) == 0) printf ("false\n");
			else printf ("true\n");

		}

		else if(strcmp(cmd, "bitmap_count") == 0){
			/* s1 is bitmap name */
			/* s2 is start, s3 is cnt, s4 is boolean value */
			struct bitmap *b = find_bitmap(s1);
			size_t start = (size_t)atoi(s2);
			size_t cnt = (size_t)atoi(s3);
			bool value = true;
			if(strcmp(s4, "false") == 0) value = false;
			printf ("%zu\n", bitmap_count(b, start, cnt, value));
		}

		else if(strcmp(cmd, "bitmap_dump") == 0){
			/* s1 is bitmap name */
			struct bitmap *b = find_bitmap(s1);
			bitmap_dump(b);
		}

		else if(strcmp(cmd, "bitmap_expand") == 0){
			/* s1 is bitmap name */
			/* s2 is additional size */
			struct bitmap *b = find_bitmap(s1);
			int size = atoi(s2);

			for(int i = 0; i < b_idx; i++){
				if(strcmp(arrB[i].name, s1) == 0){
					arrB[i].b = bitmap_expand(b, size);
				}
			}
		}

		else if(strcmp(cmd, "bitmap_flip") == 0){
			/* s1 is bitmap name */
			/* s2 is index */
			struct bitmap *b = find_bitmap(s1);
			size_t bit_idx = (size_t)atoi(s2);
			bitmap_flip(b, bit_idx);
		}

		else if(strcmp(cmd, "bitmap_mark") == 0){
			/* s1 is bitmap name */
			/* s2 is mark position */
			struct bitmap *b = find_bitmap(s1);
			size_t idx = (size_t)atoi(s2);
			bitmap_mark(b, idx);
		}		

		else if(strcmp(cmd, "bitmap_none") == 0){
			/* s1 is bitmap name */
			/* s2 is start, s3 is cnt */
			struct bitmap *b = find_bitmap(s1);
			size_t start = (size_t)atoi(s2);
			size_t cnt = (size_t)atoi(s3);
			if(bitmap_none(b, start, cnt) == 0) printf ("false\n");
			else printf ("true\n");
		}

		else if(strcmp(cmd, "bitmap_reset") == 0 ){
			/* s1 is bitmap name */
			/* s2 is bitmap_index */
			struct bitmap *b = find_bitmap(s1);
			size_t idx = (size_t)atoi(s2);
			bitmap_reset(b, idx);
		}


		else if(strcmp(cmd, "bitmap_scan") == 0){
			/* s1 is bitmap name */
			/* s2 is start, s3 is cnt, s4 is boolean value */
			struct bitmap *b = find_bitmap(s1);
			size_t start = (size_t)atoi(s2);
			size_t cnt = (size_t)atoi(s3);
			bool value = true;
			if(strcmp(s4, "false") == 0) value = false;
			printf ("%zu\n", bitmap_scan(b, start, cnt, value));
		}

		else if(strcmp(cmd, "bitmap_scan_and_flip") == 0){
			/* s1 is bitmap name */
			/* s2 is start, s3 is cnt, s4 is boolean value */
			struct bitmap *b = find_bitmap(s1);
			size_t start = (size_t)atoi(s2);
			size_t cnt = (size_t)atoi(s3);
			bool value = true;
			if(strcmp(s4, "false") == 0) value = false;
			printf ("%zu\n", bitmap_scan_and_flip(b, start, cnt, value));
		}

		else if(strcmp(cmd, "bitmap_set") == 0){
			/* s1 is bitmap name */
			/* s2 is idx, s3 is boolean value */
			struct bitmap *b = find_bitmap(s1);
			size_t idx = (size_t)atoi(s2);
			bool value = true;
			if(strcmp(s3, "false") == 0) value = false;
			bitmap_set(b, idx, value);
		}

		else if(strcmp(cmd, "bitmap_set_all") == 0){
			/* s1 is bitmap name */
			/* s2 is boolean value */
			struct bitmap *b = find_bitmap(s1);
			bool value = true;
			if(strcmp(s2, "false") == 0) value = false;
			bitmap_set_all(b, value);
		}
		
		else if(strcmp(cmd, "bitmap_set_multiple") == 0){
			/* s1 is bitmap name */
			/* s2 is start, s3 is cnt, s4 is boolean value */
			struct bitmap *b = find_bitmap(s1);
			size_t size = (size_t)atoi(s2);
			size_t cnt = (size_t)atoi(s3);
			bool value = true;
			if(strcmp(s4, "false") == 0) value = false;
			bitmap_set_multiple(b, size, cnt, value);
		}

		else if(strcmp(cmd, "bitmap_size") == 0){
			/* s1 is bitmap name */
			struct bitmap *b = find_bitmap(s1);
			printf ("%zu\n", bitmap_size(b));
		}

		else if(strcmp(cmd, "bitmap_test") == 0){
			/* s1 is bitmap name */
			/* s2 is idx */
			struct bitmap *b = find_bitmap(s1);
			size_t idx = (size_t)atoi(s2);
			if(bitmap_test(b, idx) == 0) printf ("false\n");
			else printf ("true\n");
		}

		else{
			printf ("your command is wrong\n");
		}

		
	}// while_loop

	return 0;
}
