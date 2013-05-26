/**
 *@file rapm.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#ifndef APM_APM_H
#define APM_APM_H

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "list.h"
#include "table.h"
#include "buffer.h"
#include "attr.h"
#include "mmt.h"


enum err_code {
	APM_E_FILE_READING = 1,
	APM_E_FILE_OPENING,
	APM_E_REP_VER,
	APM_E_NO_MEM,
	APM_E_RANGE,
	APM_E_OFC_REP,
	APM_E_TASK
};


/**
 * @brief main header
 */
typedef struct main_hdr_t {

	/* 0x40 for version <= 1.06, 0x44 otherwise  */
	unsigned strm_offset;

	/* sometimes is invalid (e.g. for official ladder reps) */
	unsigned total_file_size;

	/* 0x00 (ver < 1.07), 0x01 (ver >= 1.07) */
	unsigned sub_hdr_ver;

	/* size of decompressed data stream */
	unsigned dcd_data_size;

	/* number of encoded stream segments */
	unsigned ecd_sgmt_cnt;

} main_hdr_t;


/**
 * @brief Subheader: two versions (ver <= 1.06, ver >= 1.07).
 */
typedef struct sub_hdr_t {

	/* {0,1} for {WAR3,W3XP} */
	unsigned rls;

	/* of format "1.%u" */
	unsigned patch_ver;

	/* build number */
	unsigned build;

	/* must be 0x0000 for "singleplayer" or 0x8000 for "multiplayer" */
	unsigned player_mode;

	/* replay length in milliseconds */
	unsigned long rep_len_ms;

	/* complete header CRC32, initially set to 0 */
	unsigned long hdr_CRC32;

} sub_hdr_t;


/**
 * @brief Segment.
 */
typedef struct sgmt_t {

	/* size of encoded data stream */
	unsigned ecd_size;

	char *ecd_data;

	/* size of decoded data stream, should always be 8k */
	unsigned dcd_size;

	unsigned long unknown;

} sgmt_t;


typedef tbl_t sgmt_tbl_t;


/**
 * @brief Miscellaneous core data.
 */
typedef struct core_t {

	/* file pointer */
	FILE *fp;

	buff_t buff;

	task_t task;

} core_t;


/**
 * @brief Decoded data stream.
 */
typedef struct strm_t {

	char *arr;

	/* allocated */
	size_t len;

	/* fulfilled */
	size_t cnt;

	char *pos;

	/* points to char that is right behind array */
	char *lim;

} strm_t;


/**
 *@brief Person record.
 *@note Uptime and player_race are present only if (game_type == 0x08).
 */
typedef struct prsn_t {

	bool host;

	/* id generated by WC3 */
	unsigned id;
	char *name;

	/* 0x01 custom, 0x08 ladder */
	unsigned game_type;

	/* uptime of WC3 saver */
	unsigned long uptime;
	unsigned long player_race;

	/* valid only if (host == false) */
	unsigned long unknown;

} prsn_t;


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
typedef struct slot_t {

	unsigned id;
	/* map download state in percentages */
	unsigned map_dl;
	unsigned slot_status;

	unsigned ai_player;
	/* { 0 - 11, 12 for non-players } */
	unsigned team_no;

	unsigned color_id;

	unsigned race_flags;

	unsigned ai_lvl;

	/* health points handicap in percentages */
	unsigned hp_handicap;

} slot_t;


typedef tbl_t slot_tbl_t;


/**
 *@brief 
 */
typedef struct join_scrn_blk_t {

	unsigned slot_cnt;

	slot_tbl_t slot_tbl;

	unsigned long rand_seed;

	unsigned char select_mode;

	unsigned start_spot_cnt;

} join_scrn_blk_t;


/**
 *@brief 
 */
typedef struct joiner_t {

	/* id, generated by WC3 */
	unsigned id;

	bool host;

	bool saver;

	char *name;

	/* in percentages */
	int map_dl;

	int team_no;

	int color_id;

	/*
	 * 0x00 unknown
	 * 0x01 HU
	 * 0x02 ORC
	 * 0x03 NE
	 * 0x04 UD
	 * 0x05 RND
	 */
	int race_flags;

	int ai_lvl;

	/* [ 50, 100 ] */
	int hp_handicap;

	/* moment when joiner left the game */
	mmt_t leave_mmt;

	unsigned long apm;

} joiner_t;

typedef tbl_t joiner_tbl_t;



/**
 *@brief 
 */
typedef struct msgbox_t {

	/* message number */
	unsigned no;

	unsigned joiner_id;

	mmt_t mmt;

	char flags;

	unsigned long chat_mode;

	char *msg;

} msgbox_t;

typedef list_t chat_ls_t;


typedef struct action_t {

	joiner_t *joiner;

	mmt_t mmt;

} action_t;

typedef list_t action_ls_t;


/**
 *@brief 
 */
typedef struct extra_t {

	joiner_tbl_t joiner_tbl;

	chat_ls_t *chat_ls;

	action_ls_t *action_ls;

} extra_t;


/**
 *@brief Structure to save replay body.
 */
typedef struct body_t {

	/* table of segments -- encoded data chunks */
	sgmt_tbl_t sgmt_tbl;

	/* contiguous stream of zlib-decoded segments */
	strm_t strm;

} body_t;


/**
 *@brief 
 */
typedef struct host_blk_t {

	prsn_t prsn;

	char *game_name;

	game_opts_t game_opts;
	/* file system path of map */
	char *map_path;

	char *game_creator;

	unsigned player_cnt;
	/* game_type_t game_type; */

} host_blk_t;

/* make prsn_tbl_t alias to tbl_t */
typedef tbl_t prsn_tbl_t;


/**
 *@brief 
 */
typedef struct rfnd_t {

	/* host info */
	host_blk_t *host_blk;

	/* join screen info */
	join_scrn_blk_t *join_scrn_blk;

	prsn_tbl_t *prsn_tbl;

	/* processed data to have it saved in more convinient way */
	extra_t extra;

} rfnd_t;


/**
 * @brief The most superior data structure.
 */
typedef struct apm_t {

	core_t core;
	
	main_hdr_t main_hdr;
	
	sub_hdr_t sub_hdr;

	body_t *body;

	/* refined data, derived from stream */
	rfnd_t *rfnd;

} apm_t;


/* API */

extern void apm_wc3_attrinit(apm_wc3_attr_t *attr);
extern void apm_wc3_attr_deinit(apm_wc3_attr_t *attr);

extern void *apm_wc3_init(void);

extern int apm_wc3_operate(apm_t *p, apm_wc3_attr_t *attr);
 
extern void apm_wc3_deinit(apm_t *p);

extern unsigned apm_wc3_getrlsver(apm_t *apm);

extern unsigned apm_wc3_getpatchver(apm_t *apm);

extern unsigned apm_wc3_getbuild(apm_t *apm);

extern unsigned apm_wc3_getreplen(apm_t *apm);

extern char *apm_wc3_getmappath(apm_t *apm);
extern unsigned apm_wc3_getmapposcnt(apm_t *apm);

unsigned apm_wc3_getjoinercnt(apm_t *apm);
joiner_t *apm_wc3_getjoiner(apm_t *apm, const unsigned no);

extern unsigned apm_wc3_getmsgcnt(apm_t *apm);
extern msgbox_t *apm_wc3_getmsg(apm_t *apm, const unsigned no);

#endif /* APM_APM_H */