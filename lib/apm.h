/**
 *@file apm.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#ifndef APM_APM_H
#define APM_APM_H

#include <stdio.h>		/* FILE */
#include <inttypes.h>           /* fixed width integer types */
#include <stdbool.h>


/**
 * @brief request bitmap: what data should be collected out of replay
 */
typedef enum request_t {

        /* header data */
        APM_REQ_HEADER = (1 << 0),

	APM_REQ_INFO = (1 << 1),
        /* actions per minute */
        APM_REQ_APM = (1 << 2)

} request_t;

/**
 *@brief
 *
 *@note none of members can be 'const'
 */
typedef struct aux_t {

        void *dest;
        int size;
} aux_t;

/**
 * @brief main header
 */
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
        uint32_t dcd_data_size;

        /* number of compressed data blocks in file */
        uint32_t ecd_blk_cnt;

} main_hder_t;


/**
 * @brief subheader: two versions (ver <= 1.06, ver >= 1.07)
 */
typedef struct {

        /* must be either { 'W','A','R','3' }  or  { 'W','3','X','P' } */
        /* 5th item is reserved for '\0' */
        char rls[5];

        /* of format "1.??" */
        uint32_t patch_ver;

        /* build number */
        uint16_t build_no;

        /* must be 0x0000 for "singleplayer" or 0x8000 for "multiplayer" } */
        uint16_t player_mode;

        /* replay length in milliseconds */
        uint32_t rep_len_ms;

        /* UNIMPORTANT
         * complete header CRC32, initially this DWORD is set to 0 */
        uint32_t hder_CRC32;

} sub_hder_t;


/**
 * @brief individual block records
 */
typedef struct blk_t {

        /* size of encoded data stream */
        uint16_t ecd_size;

        /* size of decoded data stream, should always be 8k */
        uint16_t dcd_size;

        uint32_t unknown;

        char *ecd_strm;

} blk_t;


/**
 * @brief block table
 */
typedef struct blk_tab_t {
        
        /* pointer to array of compressed data blocks */
        blk_t *arr;

        /* number allocated "blk_t" items */
        unsigned len;

        /* counter of fulfilled blocks (index of first empty block) */
        unsigned cnt;

} blk_tab_t;


/**
 * @brief miscellaneous core data
 */
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


/**
 * @brief decoded data stream
 */
typedef struct strm_t {

        char *arr;

        /* allocated */
        int len;

        /* fulfilled */
        int cnt;

	char *pos;

	/* to not get out of array range */
	char *limit;

} strm_t;

/**
 *@brief
 *@note uptime and player_race are present for (game_type == 0x08) only
 */
typedef struct player_rec_t {

	uint8_t player_id;
	char *player_name;

	/* 0x01 custom, 0x08 ladder */
	uint8_t game_type;

	/* uptime of WC3 saver */
	uint32_t uptime;
	uint32_t player_race;

	uint32_t unknown;	

} player_rec_t;



/**
 *@brief Player table
 */
typedef struct player_tab_t {

	/* dynamic array of pointers */
	player_rec_t **arr;

	/* number of allocated records */
	int len;

	/* last unused record index */
	int cnt;

} player_tab_t;


/**
 *@brief 
 */
typedef struct game_opts_t {

	int speed;
	int visibility;
	int observer;
	bool teams_together;
	int fixed_teams;
	bool full_shared_unit_ctrl;
	bool random_hero;
	bool random_races;

} game_opts_t;


/**
 *@brief 
 */
typedef struct slot_rec_t {

	uint8_t player_id;
	/* map download state in percentages */
	uint8_t map_dl;
	uint8_t slot_status;

	uint8_t ai_player;
	/* { 0 - 11, 12 for non-players } */
	uint8_t team_no;

	uint8_t color;

	uint8_t race;

	uint8_t ai_lvl;

	/* health points handicap in percentages */
	uint8_t hp_handicap;

} slot_rec_t;


/**
 *@brief 
 */
typedef struct slot_tab_t {

	slot_rec_t *arr;

	int len;

	int cnt;

} slot_tab_t;


/**
 *@brief 
 */
typedef struct game_start_rec_t {

	uint8_t slot_cnt;

	slot_tab_t slot_tab;

	uint32_t rand_seed;

	uint8_t select_mode;

	uint8_t start_spot_cnt;

} game_start_rec_t;


/**
 *@brief info data of decompressed stream
 */
typedef struct info_t {

	uint32_t unknown;
	player_tab_t player_tab;
	char *game_name;

	game_opts_t game_opts;
	/* file system path of map */
	char *map_path;

	char *game_creator;

	int player_cnt;
	/* game_type_t game_type; */

	game_start_rec_t *game_start_rec;

} info_t;


/**
 * @brief the most superior data structure
 */
typedef struct apm_t {

        core_t core;
        
        main_hder_t main_hder;
        
        sub_hder_t sub_hder;

        blk_tab_t blk_tab;

        strm_t strm;

	info_t *info;

} apm_t;


/* API */
extern apm_t *apm_wc3_init(const char *path);
 
extern int apm_wc3_process(apm_t *apm, request_t req);
 
extern void apm_wc3_deinit(apm_t *apm);

extern char *apm_wc3_get_release(apm_t *apm);

extern unsigned apm_wc3_get_patch_version(apm_t *apm);

extern unsigned apm_wc3_get_length(apm_t *apm);

extern char *apm_wc3_get_map_path(apm_t *apm);

extern unsigned apm_wc3_get_map_position_cnt(apm_t *apm);

extern char *apm_wc3_get_hostname(apm_t *apm);

extern unsigned apm_wc3_get_game_speed(apm_t *apm);

#endif /* APM_APM_H */
