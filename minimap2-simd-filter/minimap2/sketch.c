#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#define __STDC_LIMIT_MACROS
#include "kvec.h"
#include "mmpriv.h"
#include <openssl/md5.h>
#include <gmp.h>
//#include <xxhash.h>

unsigned char seq_nt4_table[256] = {
	0, 1, 2, 3,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 0, 4, 1,  4, 4, 4, 2,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  3, 3, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 0, 4, 1,  4, 4, 4, 2,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  3, 3, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4
};

//static inline uint64_t hash64(uint64_t key, uint64_t mask)
static uint64_t __attribute__ ((noinline)) hash64(uint64_t key, uint64_t mask)
{
//    printf("10: in the wrong hash function :(\n");
	key = (~key + (key << 21)) & mask; // key = (key << 21) - key - 1;
	key = key ^ key >> 24;
	key = ((key + (key << 3)) + (key << 8)) & mask; // key * 265
	key = key ^ key >> 14;
	key = ((key + (key << 2)) + (key << 4)) & mask; // key * 21
	key = key ^ key >> 28;
	key = (key + (key << 31)) & mask;
	return key;
}


static uint64_t __attribute__ ((noinline))  hash64_2(const mpz_t *key, const mpz_t *mask, double *hash_time)
{
//    printf("10: in the wrong hash function :(\n");
    clock_t start = clock();
	mpz_t temp0;
    mpz_t temp;
    mpz_t temp2;
    mpz_init2(temp0, 256);
    mpz_init2(temp, 256);
    mpz_init2(temp2, 256);
    //mpz_set_ui(temp, 1);
    //mpz_set_ui(temp2, 1);

    //key = (~key + (key << 21)) & mask;
    mpz_com(temp, *key);
    mpz_mul_2exp(temp0, *key, 21);
    mpz_add(temp0, temp0, temp);
    mpz_and(temp0, temp0, *mask);

	//key = key ^ key >> 24;
    mpz_tdiv_q_2exp(temp, temp0, 24);
    mpz_xor(temp0, temp0, temp);

	//key = ((key + (key << 3)) + (key << 8)) & mask; 
    mpz_mul_2exp(temp, temp0, 3);
    mpz_add(temp, temp, temp0);
    mpz_mul_ui(temp2, temp0, 8);
    mpz_add(temp, temp, temp2);
    mpz_and(temp0, temp, *mask);


	//key = key ^ key >> 14;
    mpz_tdiv_q_2exp(temp, temp0, 14);
    mpz_xor(temp0, temp0, temp);

	//key = ((key + (key << 2)) + (key << 4)) & mask; 
    mpz_mul_2exp(temp, temp0, 2);
    mpz_add(temp, temp, temp0);
    mpz_mul_2exp(temp2, temp0, 4);
    mpz_add(temp0, temp, temp2);
    mpz_and(temp0, temp0, *mask);

	//key = key ^ key >> 28;
    mpz_tdiv_q_2exp(temp, temp0, 28);
    mpz_xor(temp0, temp0, temp);

	//key = (key + (key << 31)) & mask;
    mpz_mul_2exp(temp, temp0, 31);
    mpz_add(temp0, temp0, temp);
    mpz_and(temp0, temp0, *mask);
    
    size_t nlimbs = mpz_size(temp0);
    uint64_t key_return;
    key_return = mpz_getlimbn(temp0, nlimbs-1);
    
    //printf("%" PRIu64 "\n", key_return);
    clock_t end = clock();
    *hash_time = *hash_time + (double)(end - start);

    //printf("hash_time inside hash64_2: %lf \n", *hash_time);
	return key_return;
}


//static inline uint64_t hash_md5(uint64_t key, uint64_t mask)
static inline uint64_t hash_md5(const mpz_t *key, const mpz_t *mask, double *hash_time)
{
    clock_t start = clock();
//    printf("9: finally generating the hash\n");
    unsigned char *buf;
    size_t count;
    //snprintf(buf, sizeof buf, "%"PRIu64, key);
    buf = (unsigned char*)mpz_export(NULL, &count, 1, 1, 1, 0, *key); 

    //int i;
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5(buf, count, result);

    free(buf);

    uint64_t key_return;
    

//  sscanf(result, "%" SCNu64, &key_return);

  //  key_return = strtoull(result, &buf, 16);
    //key_return = (*((__uint128_t*) result)) >> 64;
    key_return = (*((__uint128_t*) result)) >> 64;
    
//    printf("key_return: %" PRIu64 "\n", key_return);
//    printf("result: %02x \n", result);
//      for(int i = 0; i < MD5_DIGEST_LENGTH; i++)
//          printf("%02x", result[i]);
//      printf("\n");
//    printf("%" PRIu64 "\n", key_return);
    clock_t end = clock();
    *hash_time = *hash_time + (double)(end - start);
	return key_return;
}

static inline uint64_t hash_md5_short(const mpz_t *key, const mpz_t *mask)
{
//    printf("9: finally generating the hash\n");
    unsigned char *buf;
    size_t count;
    //snprintf(buf, sizeof buf, "%"PRIu64, key);
    buf = (unsigned char*)mpz_export(NULL, &count, 1, 1, 1, 0, *key); 

    //int i;
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5(buf, count, result);

    free(buf);

    uint64_t key_return;
    

//  sscanf(result, "%" SCNu64, &key_return);

  //  key_return = strtoull(result, &buf, 16);
    key_return = (*((__uint128_t*) result)) >> 96; //TEMP: replace with 64
//    printf("key_return: %" PRIu64 "\n", key_return);
//    printf("result: %02x \n", result);
//      for(int i = 0; i < MD5_DIGEST_LENGTH; i++)
//          printf("%02x", result[i]);
//      printf("\n");
//    printf("%" PRIu64 "\n", key_return);
//     key_return |= (key_return<<32);
   
	return key_return;
}

/*
static inline uint64_t hash_xx(const mpz_t *key, const mpz_t *mask, double *hash_time)
{
    clock_t start = clock();
//    printf("9: finally generating the hash\n");
    unsigned char *buf;
    size_t count;
    //snprintf(buf, sizeof buf, "%"PRIu64, key);
    buf = (unsigned char*)mpz_export(NULL, &count, 1, 1, 1, 0, *key); 

    //int i;
    //unsigned char result[MD5_DIGEST_LENGTH];
    uint64_t key_return = XXH64(buf, count, 42);

    free(buf);

  //  uint64_t key_return;
    

   // key_return = (*((__uint128_t*) result)) >> 64;
    
    clock_t end = clock();
    *hash_time = *hash_time + (double)(end - start);
	return key_return;
}
*/



typedef struct { // a simplified version of kdq
	int front, count;
	int a[32];
} tiny_queue_t;

static inline void tq_push(tiny_queue_t *q, int x)
{
	q->a[((q->count++) + q->front) & 0x1f] = x;
}

static inline int tq_shift(tiny_queue_t *q)
{
	int x;
	if (q->count == 0) return -1;
	x = q->a[q->front++];
	q->front &= 0x1f;
	--q->count;
	return x;
}

/**
 * Find symmetric (w,k)-minimizers on a DNA sequence
 *
 * @param km     thread-local memory pool; using NULL falls back to malloc()
 * @param str    DNA sequence
 * @param len    length of $str
 * @param w      find a minimizer for every $w consecutive k-mers
 * @param k      k-mer size
 * @param rid    reference ID; will be copied to the output $p array
 * @param is_hpc homopolymer-compressed or not
 * @param p      minimizers
 *               p->a[i].x = kMer<<8 | kmerSpan
 *               p->a[i].y = rid<<32 | lastPos<<1 | strand
 *               where lastPos is the position of the last base of the i-th minimizer,
 *               and strand indicates whether the minimizer comes from the top or the bottom strand.
 *               Callers may want to set "p->n = 0"; otherwise results are appended to p
 */
void mm_sketch(void *km, const char *str, int len, int w, int k, uint32_t rid, int is_hpc, mm128_v *p, bool exact_flag, double *hash_time)
{
    //printf("8: starting sketch\n");
    

    //TODO: check what else you need to change in this function.
    //check the minimizer and window size.
    //check the read size as a kmer.
	uint64_t shift1 = 2 * (k - 1);

    mpz_t mask;
    mpz_t kmer[2];
    mpz_t c_big;
    mpz_t temp;
    mpz_t temp2;
    mpz_t imm_num;
    mpz_init2(mask, 256);
    mpz_init2(kmer[0], 256);
    mpz_init2(kmer[1], 256);
    mpz_init2(c_big, 256);
    mpz_init2(temp, 256);
    mpz_init2(temp2, 256);
    mpz_init2(imm_num, 256);
    mpz_set_ui(imm_num, 3);
    mpz_set_ui(temp, 0);
    mpz_set_ui(temp2, 0);

    uint64_t mask_fake = (1ULL<<2*k) - 1;
    
    mpz_set_ui(mask, 1);
    mpz_mul_2exp(mask, mask, 2*k);
    mpz_sub_ui(mask, mask, 1);
 //   printf("mask_fake: %" PRIu64 "\n", mask_fake);
 //   printf("mask_limb: %" PRIu64 "\n", mpz_getlimbn(mask, 0));

    assert((k > 28) || (mask_fake == mpz_getlimbn(mask, 0)));


    /*
    size_t nlimbs = mpz_size(mask);
    mp_limb_t *mask_ptr = mpz_limbs_modify(mask, nlimbs);
    mpn_lshift(mask_ptr, mask_ptr, nlimbs, 2*k);
    */
    
    uint64_t kmer_fake[2] = {0,0};

	int i, j, l, buf_pos, min_pos, kmer_span = 0;
	mm128_t buf[256], min = { UINT64_MAX, UINT64_MAX };
	tiny_queue_t tq;

    //TODO: double check this
	//assert(len > 0 && (w > 0 && w < 256) && (k > 0 && k <= 28)); // 56 bits for k-mer; could use long k-mers, but 28 enough in practice
	assert(len > 0 && (w > 0 && w < 256) && (k > 0)); // 56 bits for k-mer; could use long k-mers, but 28 enough in practice
	memset(buf, 0xff, w * 16);
	memset(&tq, 0, sizeof(tiny_queue_t));
	kv_resize(mm128_t, km, *p, p->n + len/w);

	for (i = l = buf_pos = min_pos = 0; i < len; ++i) {
		int c = seq_nt4_table[(uint8_t)str[i]];
		mm128_t info = { UINT64_MAX, UINT64_MAX };
		if (c < 4) { // not an ambiguous base
			int z;
			if (is_hpc) {
				int skip_len = 1;
				if (i + 1 < len && seq_nt4_table[(uint8_t)str[i + 1]] == c) {
					for (skip_len = 2; i + skip_len < len; ++skip_len)
						if (seq_nt4_table[(uint8_t)str[i + skip_len]] != c)
							break;
					i += skip_len - 1; // put $i at the end of the current homopolymer run
				}
				tq_push(&tq, skip_len);
				kmer_span += skip_len;
				if (tq.count > k) kmer_span -= tq_shift(&tq);
			} else kmer_span = l + 1 < k? l + 1 : k;

            //nika

			kmer_fake[0] = (kmer_fake[0] << 2 | c) & mask_fake;           // forward k-mer

            mpz_set_ui(c_big, c);  
            mpz_mul_2exp(kmer[0], kmer[0], 2); //TODO: replace mul with the more efficient operation
            mpz_ior(kmer[0], kmer[0], c_big);
            mpz_and(kmer[0], kmer[0], mask);
   //         printf("kmer_fake[0]: %" PRIu64 "\n", kmer_fake[0]);
  //          printf("kmer_limb[0]: %" PRIu64 "\n", mpz_getlimbn(kmer[0], 0));

            assert((k > 28) || (kmer_fake[0] == mpz_getlimbn(kmer[0], 0)));


			//kmer[1] = (kmer[1] >> 2) | (3ULL^c) << shift1; // reverse k-mer
            mpz_tdiv_q_2exp(kmer[1], kmer[1], 2);
            /*
            size_t nlimbs = mpz_size(kmer[1]);
            printf("first line div\n");
            mp_limb_t *kmer1_ptr = mpz_limbs_modify(kmer[1], nlimbs);
            printf("second line div\n");
            mpn_rshift(kmer1_ptr, kmer1_ptr, nlimbs, 2); //end of div
            printf("third line div\n");
            */
            mpz_xor(c_big, c_big, imm_num);
            mpz_mul_2exp(c_big, c_big, shift1); //TODO: replace mul with the more efficient operation
            mpz_ior(kmer[1], kmer[1], c_big);

            


			//if (kmer[0] == kmer[1]) continue; // skip "symmetric k-mers" as we don't know it strand
            int kmer_cmp = mpz_cmp(kmer[0], kmer[1]);
            if(!kmer_cmp) continue; // skip "symmetric k-mers" as we don't know it strand
            double hash_time_s = 0;

			//z = kmer[0] < kmer[1]? 0 : 1; // strand
			z = (kmer_cmp < 0) ? 0 : 1; // strand
			++l;
			if (l >= k && kmer_span < 256) {
                if(!exact_flag)
				    info.x = hash64(mpz_getlimbn(kmer[z], 0), mpz_getlimbn(mask, 0)) << 8 | kmer_span;
                else{
                    //info.x = hash_md5_short(&kmer[z], &mask) << 8 | kmer_span;
			        //info.x = hash64_2(&kmer[z], &mask, &hash_time_s) << 8 | kmer_span;
				    info.x = hash_md5(&kmer[z], &mask, hash_time) << 8 | kmer_span;
				    //info.x = hash_xx(&kmer[z], &mask, hash_time) << 8 | kmer_span;
                    *hash_time =  hash_time_s + *hash_time;
                    //printf("hash_time inside sketch: %lf \n", *hash_time);
                }

				info.y = (uint64_t)rid<<32 | (uint32_t)i<<1 | z;
			}
		} else l = 0, tq.count = tq.front = 0, kmer_span = 0;
		buf[buf_pos] = info; // need to do this here as appropriate buf_pos and buf[buf_pos] are needed below
		if (l == w + k - 1 && min.x != UINT64_MAX) { // special case for the first window - because identical k-mers are not stored yet
			for (j = buf_pos + 1; j < w; ++j)
				if (min.x == buf[j].x && buf[j].y != min.y) { /*if(i%2)*/ kv_push(mm128_t, km, *p, buf[j]);}
			for (j = 0; j < buf_pos; ++j)
				if (min.x == buf[j].x && buf[j].y != min.y) { /*if(i%2)*/ kv_push(mm128_t, km, *p, buf[j]);}
		}
		if (info.x <= min.x) { // a new minimum; then write the old min
			if (l >= w + k && min.x != UINT64_MAX) { /*if(i%2)*/ kv_push(mm128_t, km, *p, min);}
			min = info, min_pos = buf_pos;
		} else if (buf_pos == min_pos) { // old min has moved outside the window
			if (l >= w + k - 1 && min.x != UINT64_MAX) { /*if(i%2)*/ kv_push(mm128_t, km, *p, min);}
			for (j = buf_pos + 1, min.x = UINT64_MAX; j < w; ++j) // the two loops are necessary when there are identical k-mers
				if (min.x >= buf[j].x) min = buf[j], min_pos = j; // >= is important s.t. min is always the closest k-mer
			for (j = 0; j <= buf_pos; ++j)
				if (min.x >= buf[j].x) min = buf[j], min_pos = j;
			if (l >= w + k - 1 && min.x != UINT64_MAX) { // write identical k-mers
				for (j = buf_pos + 1; j < w; ++j) // these two loops make sure the output is sorted
					if (min.x == buf[j].x && min.y != buf[j].y) { /*if(i%2)*/ kv_push(mm128_t, km, *p, buf[j]);}
				for (j = 0; j <= buf_pos; ++j)
					if (min.x == buf[j].x && min.y != buf[j].y) { /*if(i%2)*/ kv_push(mm128_t, km, *p, buf[j]);}
			}
		}
		if (++buf_pos == w) buf_pos = 0;
	}

    mpz_clears(mask, kmer[0], kmer[1], c_big, imm_num, NULL);
	if (min.x != UINT64_MAX)
    { /*if(i%2)*/ kv_push(mm128_t, km, *p, min);}
}
