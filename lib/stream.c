/**
 *@file stream.c
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "apm.h"
#include "debug.h"
#include "str.h"
#include "init_deinit.h"
#include "decode.h"



/**
 *@brief 
 */
static int _compute_item_size(char *start, const char *limit, aux_t *item)
{

	int ret;

	/* if size of item is empty, it has not been computed yet */
	if(0 == item->size) {

		ret = _safe_str_len(start, limit);
		GUARD(-1 == ret, 1);

		MALLOC(*((char **)item->dest), --,
		       ret + 1, return 1);

		item->dest = *((char **)item->dest);

		/* now we know the size of string */
		item->size = ret + 1;
	} else {
		GUARD(_position_is_out_of_range(start, limit, 1), 1);
	}
	/* all OK */
	return 0;
}

/**
 *@brief 
 */
static int _add_new_record(player_tab_t *player_tab)
{
	int ret;

	if(player_tab->cnt >= player_tab->len) {
		ret = _prolong_player_table(player_tab);
		GUARD(0 != ret, ret);
	}

	CALLOC(player_tab->arr[player_tab->cnt], --,
	       sizeof(player_rec_t), return 1);

	/* one more record allocated */
	++player_tab->cnt;

	return 0;
}

/**
 *@brief 
 */
static int _safe_read(char **curr_pos, const char *limit,
		 aux_t *aux_arr, const int cnt)
{

	int ret;

        for(int idx = 0;  idx < cnt;  ++idx) {

		ret = _compute_item_size(*curr_pos, limit, &aux_arr[idx]);
		GUARD(0 != ret, 1);

                /* copy data from buffer */
                memcpy(aux_arr[idx].dest, *curr_pos, aux_arr[idx].size);

                /* shift buffer offset */
                *curr_pos += aux_arr[idx].size;
        }

	return 0;
}


/**
 *@brief 
 */
static int _read_player_record(player_tab_t *player_tab,
			       strm_t *strm, const uint8_t rec_id)
{
#define GAME_TYPE_IDX 4

	int ret;

	ret = _add_new_record(player_tab);
	GUARD(0 != ret, ret);

	player_rec_t *player_rec = player_tab->arr[player_tab->cnt - 1];


	/* empty size means 'to be computed' */
        aux_t aux_arr[] = {

		{ &player_rec->player_id, sizeof(player_rec->player_id) },
		/* cast "(char **)" to "(void *)" */
		{ (void *)&player_rec->player_name, 0},
		{ &player_rec->game_type, sizeof(player_rec->game_type) },
		{ &player_rec->uptime, sizeof(player_rec->uptime) },
		{ &player_rec->player_race, sizeof(player_rec->player_race) },
		{ &player_rec->unknown, sizeof(player_rec->unknown) }
        };


	ret = _safe_read(&strm->pos, strm->limit, aux_arr, 3);
	GUARD(0 != ret, 0);

	/* have to make choice according to game_type value */
	if(0x01 == player_rec->game_type) {
		/* '\0' expected, no further player_rec data */
		++strm->pos;
	} else if(0x08 == player_rec->game_type) {

		ret = _safe_read(&strm->pos, strm->limit, aux_arr + 3, 2);
		GUARD(0 != ret, 0);

	} /* if game_type is invalid */
	else {
		return 1;
	}

	/* if this player record is not the game host */
	if(0x16 == rec_id) {

		ret = _safe_read(&strm->pos, strm->limit, aux_arr + 5, 1);
		GUARD(0 != ret, 0);
	}

	/* all OK */
	return 0;
#undef GAME_TYPE_IDX
}


/**
 *@brief
 */
static int _read_game_opts(apm_t *apm,
			   char **dcd_str_pos)
{

	/* {0,1}-th bit = 2^0 + 2^1 */
	apm->info->game_opts.speed = (*dcd_str_pos)[0] & 3;

	/* apm->info->game_opts.visibility = *dcd_str_pos[0] & ; */

	/* {4,5}-th bit = 2^4 + 2^5 */
	apm->info->game_opts.observer = (*dcd_str_pos)[1] & 48;

	/* 6-th bit = 2^6 */
	apm->info->game_opts.teams_together = (bool)((*dcd_str_pos)[1] & 32);

	/* {0,1}-th bit = 2^0 + 2^1 */
	apm->info->game_opts.fixed_teams = (*dcd_str_pos)[2] & 3;

	/* 0-th bit = 2^0 */
	apm->info->game_opts.full_shared_unit_ctrl =
		(bool)((*dcd_str_pos)[3] & 1);

	/* 1-th bit = 2^1 */
	apm->info->game_opts.random_hero = (bool)((*dcd_str_pos)[3] & 2);

	/* 2-th bit = 2^2 */
	apm->info->game_opts.random_races = (bool)((*dcd_str_pos)[3] & 4);

	/* pointer jump to map path */
	*dcd_str_pos += 13;

	/* all OK */
	return 0;
}


/**
 *@brief
 */
static int _read_remaining_players(apm_t *apm)
{
	int ret;
	
	while(0x16 == *apm->strm.pos) {

		apm->strm.pos += 1;

		ret = _read_player_record(&apm->info->player_tab,
					  &apm->strm, 0x16);
		GUARD(0 != ret, ret);
	}

	return 0;
}


static int _read_slot(strm_t *strm, slot_rec_t *rec,
		      const int valid_member_cnt)
{
	int ret;

	/* empty size means 'to be computed' */
        aux_t aux_arr[ ] = {

		{ &rec->player_id, sizeof(rec->player_id) },
		{ &rec->map_dl, sizeof(rec->map_dl) },
		{ &rec->slot_status, sizeof(rec->slot_status) },
		{ &rec->ai_player, sizeof(rec->ai_player) },
		{ &rec->team_no, sizeof(rec->team_no) },
		{ &rec->color, sizeof(rec->color) },
		{ &rec->race, sizeof(rec->race) },
		{ &rec->ai_lvl, sizeof(rec->ai_lvl) },
		{ &rec->hp_handicap, sizeof(rec->hp_handicap) },
        };

	ret = _safe_read(&strm->pos, strm->limit,
				 aux_arr, valid_member_cnt);
	GUARD(0 != ret, ret);

	return 0;
}


/**
 *@brief
 */
static int _read_slot_sequence(apm_t *apm, int cnt)
{

	slot_tab_t *slot_tab = &apm->info->game_start_rec->slot_tab;
	int ret, valid_member_cnt;


	if(apm->sub_hder.patch_ver < 0x03) {
		valid_member_cnt = 7;
	} else if(apm->sub_hder.patch_ver < 0x07) {
		valid_member_cnt = 8;
	} /* patch_ver >= 0x07 */
	else {
		valid_member_cnt = 9;
	}
	

	for(int idx = 0;  idx < cnt;  ++idx) {

		ret = _read_slot(&apm->strm, &slot_tab->arr[idx],
				 valid_member_cnt);
		GUARD(0 != ret, ret);

		++slot_tab->cnt;
	}

	return 0;
}


/**
 *@brief
 */
static int _read_game_start_record(apm_t *apm)
{

	game_start_rec_t *rec = apm->info->game_start_rec;

	int ret;

	/* size of the rest of record in bytes */
	uint16_t rest_size;

	/* empty size means 'to be computed' */
        aux_t aux_arr[] = {

		{ &rest_size, sizeof(rest_size) },
		{ &rec->slot_cnt, sizeof(rec->slot_cnt) },
		{ &rec->rand_seed, sizeof(rec->rand_seed) },
		{ &rec->select_mode, sizeof(rec->select_mode) },
		{ &rec->start_spot_cnt, sizeof(rec->start_spot_cnt) }
        };
	

	ret = _safe_read(&apm->strm.pos, apm->strm.limit, aux_arr, 2);
	GUARD(0 != ret, 1);

	ret =  _init_slot_table(apm, rec->slot_cnt);
	GUARD(0 != ret, ret);

	ret = _read_slot_sequence(apm, rec->slot_cnt);
	GUARD(0 != ret, 1);

	ret = _safe_read(&apm->strm.pos, apm->strm.limit, aux_arr + 2, 3);
	GUARD(0 != ret, 1);

	return 0;
}


/**
 *@brief
 */
static int _read_particular_record(apm_t *apm, uint8_t rec_id)
{

	int ret;

	/* TODO: THIS IS NOT SAFE OPERATION */
	if(rec_id != *apm->strm.pos) {
		return 1;
	}


	/* shift position */
	++apm->strm.pos;

	if(0x00 == rec_id) {
		/* initialize data structures */
		ret = _init_player_table(apm);
		GUARD(0 != ret, ret);

		ret = _read_player_record(&apm->info->player_tab,
					  &apm->strm, rec_id);
		GUARD(0 != ret, ret);
	}
	else if(0x19 == rec_id) {

		ret = _init_game_start_rec(apm);
		GUARD(0 != ret, ret);
		/* call relevant function */
		ret = _read_game_start_record(apm);
	}

	return 0;
}


/**
 *@brief
 */
int _process_info(apm_t *apm)
{

	/********************************************/
	/********************************************/
	/********************************************/
	/* IN PROGRESS, POTENCIAL MEMORY LEAKS, ETC */
	/********************************************/
	/********************************************/
	/********************************************/

	/* auxiliary pointer */
	info_t *info = apm->info;
	int ret;
	char *ecd_str = NULL;
	char *dcd_str = NULL;


	aux_t aux_arr[] = {
		{ &info->unknown, sizeof(info->unknown) },
		{ &info->game_name, 0 },
		{ &ecd_str, 0 },
		{ (void *)&info->map_path, 0 },
		{ (void *)&info->game_creator, 0 },
		{ &info->player_cnt, sizeof(info->player_cnt) }
	};


	ret = _safe_read(&apm->strm.pos, apm->strm.limit, aux_arr, 1);
	GUARD(0 != ret, ret);


	ret = _read_particular_record(apm, 0x00);
	GUARD(0 != ret, ret);


	ret = _safe_read(&apm->strm.pos, apm->strm.limit, aux_arr + 1, 1);
	GUARD(0 != ret, ret);

	/* nullbyte after game_name string */
	++apm->strm.pos;

	ret = _safe_read(&apm->strm.pos, apm->strm.limit, aux_arr + 2, 1);
	GUARD(0 != ret, ret);


	MALLOC(dcd_str, --, aux_arr[2].size, ret = 1; goto error);

	_decode_opts_map_creator_str(ecd_str, dcd_str);

	/* current position */
	char *dcd_str_pos = dcd_str;
	char *dcd_limit = dcd_str + aux_arr[2].size;

	ret = _read_game_opts(apm, &dcd_str_pos);
	GUARD(0 != ret, 1);


	ret = _safe_read(&dcd_str_pos, dcd_limit, aux_arr + 3, 2);
	GUARD(0 != ret, ret);

	ret = _safe_read(&apm->strm.pos, apm->strm.limit, aux_arr + 5, 1);
	GUARD(0 != ret, ret);

	/* skip gametype (4 bytes) */
	apm->strm.pos += 4;

	/* skip languageID (4 bytes) */
	apm->strm.pos += 4;

	ret = _read_remaining_players(apm);
	GUARD(0 != ret, ret);

	/* request game_start_record */
	ret = _read_particular_record(apm, 0x19);
	GUARD(0 != ret, ret);

error:
	free(ecd_str);
	free(dcd_str);

	return ret;
}

