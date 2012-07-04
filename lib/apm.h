/**
 *@file apm.h
 *@brief
 *@athor Slavomir Vlcek
 */

#ifndef APM_APM_H
#define APM_APM_H

#include <inttypes.h>           /* fixed width integer types */


/* request bitmap: what data should be collected out of replay */
typedef enum request_t {

        /* header data */
        APM_REQ_HEADER = (1 << 0),
        /* actions per minute */
        APM_REQ_APM = (1 << 1)

} request_t;


/* main header */
typedef struct main_hder_t {

        /* not actually a string (missing final '\0') */
        char wc3_str[28];

        /* 0x40 for version <= 1.06 */
        uint32_t fir_blk;

        /* sometimes is invalid (e.g. for official ladder reps) */
        uint32_t total_file_size;

        /* 0x00 (ver <= 1.06), 0x01 (ver > 1.06) */
        uint32_t hder_ver;

        /* size of decompressed data stream */
        uint32_t decod_data_size;

        /* number of compressed data blocks in file */
        uint32_t encod_blk_cnt;

} main_hder_t;


/* subheader: two versions (ver <= 1.06, ver >= 1.07) */
typedef struct {

        /* must be either { 'W','A','R','3' }  or  { 'W','3','X','P' } */
        /* 5th item is reserved for '\0' */
        char rls[5];

        /* of format "1.??" */
        uint32_t patch_ver;

        /* built number */
        uint16_t built_no;

        /* must be 0x0000 for "singleplayer" or 0x8000 for "multiplayer" } */
        uint16_t player_mode;

        /* replay length in milliseconds */
        uint32_t rep_len_ms;

        /* UNIMPORTANT
         * complete header CRC32, initially this DWORD is set to 0 */
        uint32_t hder_CRC32;

} sub_hder_t;


/* individual block records */
typedef struct blk_t {

        /* size of encoded data stream */
        uint16_t encod_size;

        /* size of decoded data stream, should always be 8k */
        uint16_t decod_size;

        uint32_t unknown;

        unsigned char *encod_strm;

} blk_t;


/* block table */
typedef struct blk_tab_t {
        
        /* pointer to array of compressed data blocks */
        blk_t *arr;

        /* number allocated "blk_t" items */
        unsigned len;

        /* counter of fulfilled blocks (index of first empty block) */
        unsigned cnt;

} blk_tab_t;


/* miscellaneous core data */
typedef struct core_t {

        /* file pointer */
        FILE *fp;

        /* enum */
        request_t req;

        /* "buff" pointer must be modified
         * at init and deinit only */
        char *buff;

        int buff_len;

} core_t;


/* decoded data stream */
typedef struct strm_t {

        unsigned char *arr;

        /* allocated */
        int len;

        /* fulfilled */
        int cnt;

} strm_t;


/* the most superior data structure */
typedef struct sup_t {

        core_t core;
        
        main_hder_t main_hder;
        
        sub_hder_t sub_hder;

        blk_tab_t blk_tab;

        strm_t strm;

} sup_t;


/* API */
extern sup_t *apm_wc3_init(const char *path);
 
extern int apm_wc3_process(sup_t *sup, request_t req);
 
extern void apm_wc3_deinit(sup_t *sup);

extern char *apm_wc3_get_release(sup_t *sup);

extern unsigned apm_wc3_get_length(sup_t *sup);

#endif /* APM_APM_H */
