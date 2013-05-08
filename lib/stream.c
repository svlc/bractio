/**
 * @file stream.c
 * @brief
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "rapm.h"
#include "debug.h"
#include "str.h"
#include "init_deinit.h"
#include "decode.h"
#include "list.h"
#include "actions.h"
#include "mmt.h"
#include "arr.h"

#include "stream.h"



/**
 * @brief Copy some slot data into joiner.
 *
 * @param slot
 * @param joiner
 */
void export_slot_to_joiner(slot_t *slot, joiner_t *joiner)
{
	joiner->id = slot->id;
	joiner->team_no = slot->team_no;
	joiner->color_id = slot->color_id;
	joiner->race_flags = slot->race_flags;
	joiner->ai_lvl = slot->ai_lvl;
	joiner->hp_handicap = slot->hp_handicap;
}


/**
 * @brief Copy some person data into joiner.
 *
 * @param prsn
 * @param joiner
 */
void export_prsn_to_joiner(prsn_t *prsn, joiner_t *joiner)
{
	joiner->host = prsn->host;

	joiner->name = prsn->name;
	/* to avoid failure when deallocating resources */
	prsn->name = NULL;
}


/**
 * @brief Complete joiner's name.
 *
 * @param j joiner item 
 * @return 0 on success
 */
int complete_joiner(joiner_t *j)
{
	int len;
	int ret;

	/* "+ 1" because of '\0' */
	len = (int)strlen("unknown_??") + 1;

	j->name = (char *)malloc(len);
	if (!j->name) {
		ret = APM_E_NO_MEM;
		goto out;
	}

	ret = snprintf(j->name, len, "unknown_%d", j->id);
	/* if "id" has more then 2 digits */
	if (ret > (len - 1)) {
		return 1;
	}
	ret = 0;
out:
	return ret;
}


/**
 * @brief Find person with particular id.
 *
 * @param tbl
 * @param id
 *
 * @return NULL on failure
 */
prsn_t *seek_prsn(prsn_tbl_t *tbl, const unsigned id)
{
	prsn_t **prsn = (prsn_t **)tbl->arr;
	size_t cnt = tbl->cnt;

	while (cnt--) {
		if (id == (*prsn)->id) {
			return *prsn;
		}
		++prsn;
	}
	return NULL;
}


/**
 * @brief
 *
 * @TODO Do some small data structures update to seek joiner out in O(1).
 */
joiner_t *seek_joiner(joiner_tbl_t *tbl, unsigned id)
{
	joiner_t **joiner = (joiner_t **)tbl->arr;
	size_t cnt = tbl->cnt;

	while (cnt--) {
		if (id == (*joiner)->id) {
			return *joiner;
		}
		++joiner;
	}
	return NULL;
}


/**
 * @brief Form joiner table based on person table and slot table.
 * @note One of joiner/slot table may be empty.
 * @TODO Decompose this function.
 */
int form_joiner_tbl(prsn_tbl_t *prsn_tbl, prsn_t *host,
		    slot_tbl_t *slot_tbl, joiner_tbl_t *joiner_tbl)
{
	assert(0 == joiner_tbl->cnt);

	int ret;

	size_t slot_cnt = slot_tbl->cnt;
	size_t joiner_cnt;

	slot_t **slot = (slot_t **)slot_tbl->arr;
	joiner_t **joiner;
	joiner_t *j_item;

	prsn_t *prsn;


	/* if slot table is not empty */
	if (0 != slot_tbl->cnt) {

		while (slot_cnt--) {

			/* if slot is used */
			if (0x02 == (*slot)->slot_status) {
				j_item = (joiner_t *)malloc(sizeof(*j_item));
				if (!j_item) {
					return 1;
				}
				joiner_zero(j_item);

				export_slot_to_joiner(*slot, j_item);
				ret = tbl_add_item(joiner_tbl, j_item);
				GUARD(0 != ret, free(j_item); return ret);
			}
			++slot;
		}

		joiner_cnt = joiner_tbl->cnt;
		joiner = (joiner_t **)joiner_tbl->arr;

		while (joiner_cnt--) {

			prsn = seek_prsn(prsn_tbl, (*joiner)->id);
			if (NULL == prsn) {
				if ((*joiner)->id == host->id) {
					export_prsn_to_joiner(host, *joiner);
				} else {
					/* supply missing joiner_t data */
					ret = complete_joiner(*joiner);
					GUARD(0 != ret, return ret);
				}
			} else {
				export_prsn_to_joiner(prsn, *joiner);
			}
			++joiner;
		}
	} else {
		/*
		 * TODO: inspect some tournament replays
		 * (could not find any)
		 */
		return APM_E_OFC_REP;
	}
	return 0;
}


/**
 * @brief
 *
 * @param prsn
 * @param strm
 * @return 0 on success
 */
int save_prsn_rec(prsn_t *prsn, strm_t *strm)
{
	int ret;
	unsigned rest_size;

	/* empty size means 'to be computed' */
	aux_t aux_arr[] = {

		{ &prsn->id, APM_UINT, 1 },
		/* cast "(char **)" to "(void *)" */
		{ (void *)&prsn->name, APM_UCHAR, 0 },
		{ &rest_size, APM_UINT, 1 },
		{ &prsn->uptime, APM_ULONG, 4 },
		{ &prsn->player_race, APM_ULONG, 4 },
	};


	ret = safe_mem_read(&strm->pos, strm->lim, aux_arr, 3);
	GUARD(0 != ret, return ret);

	if (0x01 == rest_size) {
		/* '\0' expected, no further person data */
		ret = safe_pos_fw(&strm->pos, strm->lim, 1);
		GUARD(0 != ret, return ret);
	} else if (0x08 == rest_size) {

		ret = safe_mem_read(&strm->pos, strm->lim, aux_arr + 3, 2);
		GUARD(0 != ret, return ret);

	} /* must be invalid */
	else {
		return 1;
	}

	return 0;
}


/**
 * @brief
 */
static int save_game_opts(game_opts_t *opts, char **dcd_str_pos)
{

	/* NOT SAFE, PASS STRING LIM ALSO */

	/* {0,1}-th bit = 2^0 + 2^1 */
	opts->speed = (*dcd_str_pos)[0] & 3;

	/* opts->visibility = *dcd_str_pos[0] & ; */

	/* {4,5}-th bit = 2^4 + 2^5 */
	opts->observer = (*dcd_str_pos)[1] & 48;

	/* 6-th bit = 2^6 */
	opts->teams_together = (bool)((*dcd_str_pos)[1] & 32);

	/* {0,1}-th bit = 2^0 + 2^1 */
	opts->fixed_teams = (*dcd_str_pos)[2] & 3;

	/* 0-th bit = 2^0 */
	opts->full_shared_unit_ctrl = (bool)((*dcd_str_pos)[3] & 1);

	/* 1-th bit = 2^1 */
	opts->random_hero = (bool)((*dcd_str_pos)[3] & 2);

	/* 2-th bit = 2^2 */
	opts->random_races = (bool)((*dcd_str_pos)[3] & 4);

	/* pointer jump to map path */
	*dcd_str_pos += 13;

	return 0;
}


/**
 * @brief
 * @note "valid_memb_cnt" is not computed in this function to avoid wasting
 * of resources (is dependent on replay build version -- @see save_slot_seq).
 */
static int save_slot(strm_t *strm, slot_t *slot,
		     const unsigned valid_memb_cnt)
{
	int ret;

	/* empty size means 'to be computed' */
	aux_t aux_arr[] = {

		{ &slot->id,		APM_UINT, 1 },
		{ &slot->map_dl,	APM_UINT, 1 },
		{ &slot->slot_status,	APM_UINT, 1 },
		{ &slot->ai_player,	APM_UINT, 1 },
		{ &slot->team_no,	APM_UINT, 1 },
		{ &slot->color_id,	APM_UINT, 1 },
		{ &slot->race_flags,	APM_UINT, 1 },
		{ &slot->ai_lvl,	APM_UINT, 1 },
		{ &slot->hp_handicap,	APM_UINT, 1 },
	};

	ret = safe_mem_read(&strm->pos, strm->lim, aux_arr, valid_memb_cnt);
	GUARD(0 != ret, return ret);

	return 0;
}


/**
 * @brief
 */
int save_slot_seq(strm_t *strm, slot_tbl_t *slot_tbl,
		  unsigned cnt, const unsigned build)
{
	int ret;
	slot_t *slot;
	unsigned valid_memb_cnt;

	if (build < 4572) {
		valid_memb_cnt = 7;
	} else if (build < 6031) {
		valid_memb_cnt = 8;
	} /* build >= 6031 (patch_ver >= 0x07) */
	else {
		valid_memb_cnt = 9;
	}
	
	while (cnt--) {
		slot = (slot_t *)malloc(sizeof(*slot));
		if (!slot) {
			ret = APM_E_NO_MEM;
			goto error;
		}
		ret = save_slot(strm, slot, valid_memb_cnt);
		GUARD(0 != ret, goto error);

		ret = tbl_add_item(slot_tbl, slot);
		GUARD(0 != ret, goto error);
	}

	return 0;

error:
	free(slot);
	return ret;
}


/**
 * @brief
 */
int save_guest_blk(strm_t *strm, prsn_t *prsn)
{
	int ret;

	ret =  save_prsn_rec(prsn, strm);
	GUARD(0 != ret, return ret);
	
	return 0;
}


/**
 * @brief
 */
int save_0x1A_blk(strm_t *strm)
{
	int ret;

	ret = safe_pos_fw(&strm->pos, strm->lim, 4);
	GUARD(0 != ret, return ret);

	return 0;
}


/**
 * @brief
 */
int save_0x1B_blk(strm_t *strm)
{
	int ret;

	ret = safe_pos_fw(&strm->pos, strm->lim, 4);
	GUARD(0 != ret, return ret);

	return 0;
}


/**
 * @brief 
 */
int save_0x1C_blk(strm_t *strm)
{
	int ret;

	ret = safe_pos_fw(&strm->pos, strm->lim, 4);
	GUARD(0 != ret, return ret);

	return 0;
}


/**
 * @brief Unknown1 block
 */
int save_0x22_blk(strm_t *strm)
{
	int ret;

	ret = safe_pos_fw(&strm->pos, strm->lim, 5);
	GUARD(0 != ret, return ret);

	return 0;
}


/**
 * @brief Unknown2 block
 *
 * @note very rare
 */
int save_0x23_blk(strm_t *strm)
{
	int ret;

	ret = safe_pos_fw(&strm->pos, strm->lim, 10);
	GUARD(0 != ret, return ret);

	return 0;
}


/**
 * @brief
 */
int save_countdown_blk(strm_t *strm)
{
	int ret;

	ret = safe_pos_fw(&strm->pos, strm->lim, 8);
	GUARD(0 != ret, return ret);

	return 0;
}


/**
 * @brief ChatMessage block
 */
int save_chatmsg_blk(strm_t *strm, chat_ls_t *chat_ls, mmt_t *curr_mmt)
{
	int ret;
	msgbox_t *msgbox;

	msgbox = (msgbox_t *)malloc(sizeof(*msgbox));
	if (!msgbox) {
		ret = 1;
		goto out;
	}

	unsigned rest_size;

	aux_t aux_arr[] = {

		{ &msgbox->joiner_id, APM_UINT, 1 },
		{ &rest_size, APM_UINT, 2 },
		{ &msgbox->flags, APM_UCHAR, 1 },
		{ &msgbox->chat_mode, APM_ULONG, 4 },
		{ &msgbox->msg, APM_UCHAR, 0 },
	};

	ret = safe_mem_read(&strm->pos, strm->lim, aux_arr, 5);
	GUARD(0 != ret, return 0);

	msgbox->mmt = *curr_mmt;

	msgbox->no = chat_ls->cnt;

	ret = list_add_item(chat_ls, msgbox);
	GUARD(0 != ret, free(msgbox); return 1);
	ret = 0;
out:
	return ret;
}


/**
 * @brief
 */
void prep_action_ptrs(arr_t *fn_arr, const unsigned build)
{
	assert(NULL != fn_arr);
	assert(0x76 == fn_arr->cnt);

	fnbox_t fnbox;

	fnbox.fn = action_pause;
	arr_add_item(fn_arr, 0x01, &fnbox);

	fnbox.fn = action_resume;
	arr_add_item(fn_arr, 0x02, &fnbox);

	fnbox.fn = action_set_speed;
	arr_add_item(fn_arr, 0x03, &fnbox);

	fnbox.fn = action_inc_speed;
	arr_add_item(fn_arr, 0x04, &fnbox);

	fnbox.fn = action_dec_speed;
	arr_add_item(fn_arr, 0x05, &fnbox);

	fnbox.fn = action_save;
	arr_add_item(fn_arr, 0x06, &fnbox);

	fnbox.fn = action_saving_done;
	arr_add_item(fn_arr, 0x07, &fnbox);

	fnbox.fn = action_change_selection;
	arr_add_item(fn_arr, 0x16, &fnbox);

	fnbox.fn = action_assign_group_hotkey;
	arr_add_item(fn_arr, 0x17, &fnbox);

	fnbox.fn = action_select_group_hotkey;
	arr_add_item(fn_arr, 0x18, &fnbox);

	fnbox.fn = action_cheat_TheDudeAbides;
	arr_add_item(fn_arr, 0x20, &fnbox);

	fnbox.fn = action_cheat_SomebodySetUpUsTheBomb;
	arr_add_item(fn_arr, 0x22, &fnbox);

	fnbox.fn = action_cheat_WarpTen;
	arr_add_item(fn_arr, 0x23, &fnbox);

	fnbox.fn = action_cheat_IocainePowder;
	arr_add_item(fn_arr, 0x24, &fnbox);

	fnbox.fn = action_cheat_PointBreak;
	arr_add_item(fn_arr, 0x25, &fnbox);

	fnbox.fn = action_cheat_WhosYourDaddy;
	arr_add_item(fn_arr, 0x26, &fnbox);

	fnbox.fn = action_cheat_KeyserSoze;
	arr_add_item(fn_arr, 0x27, &fnbox);

	fnbox.fn = action_cheat_LeafitToMe;
	arr_add_item(fn_arr, 0x28, &fnbox);

	fnbox.fn = action_cheat_ThereIsNoSpoon;
	arr_add_item(fn_arr, 0x29, &fnbox);

	fnbox.fn = action_cheat_StrengthAndHonor;
	arr_add_item(fn_arr, 0x2A, &fnbox);

	fnbox.fn = action_cheat_itvexesme;
	arr_add_item(fn_arr, 0x2B, &fnbox);

	fnbox.fn = action_cheat_WhoIsJohnGalt;
	arr_add_item(fn_arr, 0x2C, &fnbox);

	fnbox.fn = action_cheat_GreedIsGood;
	arr_add_item(fn_arr, 0x2D, &fnbox);

	fnbox.fn = action_cheat_DayLightSavings;
	arr_add_item(fn_arr, 0x2E, &fnbox);

	fnbox.fn = action_cheat_ISeeDeadPeople;
	arr_add_item(fn_arr, 0x2F, &fnbox);

	fnbox.fn = action_cheat_Synergy;
	arr_add_item(fn_arr, 0x30, &fnbox);

	fnbox.fn = action_cheat_SharpAndShiny;
	arr_add_item(fn_arr, 0x31, &fnbox);

	fnbox.fn = action_cheat_AllYourBaseAreBelongToUs;
	arr_add_item(fn_arr, 0x32, &fnbox);

	fnbox.fn = action_change_ally_opts;
	arr_add_item(fn_arr, 0x50, &fnbox);

	fnbox.fn = action_transfer_resources;
	arr_add_item(fn_arr, 0x51, &fnbox);

	fnbox.fn = action_map_trigger_chat_command;
	arr_add_item(fn_arr, 0x60, &fnbox);

	fnbox.fn = action_esc_keystroke;
	arr_add_item(fn_arr, 0x61, &fnbox);

	fnbox.fn = action_scenario_trigger;
	arr_add_item(fn_arr, 0x62, &fnbox);

	fnbox.fn =  action_0x75_unknown;
	arr_add_item(fn_arr, 0x75, &fnbox);

	/* if  */
	if (build < 6031) {

		fnbox.fn = action_ability_0_lt_107;
		arr_add_item(fn_arr, 0x10, &fnbox);

		fnbox.fn = action_ability_1_lt_107;
		arr_add_item(fn_arr, 0x11, &fnbox);

		fnbox.fn = action_ability_2_lt_107;
		arr_add_item(fn_arr, 0x12, &fnbox);

		fnbox.fn = action_move_item_lt_107;
		arr_add_item(fn_arr, 0x13, &fnbox);

		fnbox.fn = action_choose_ability_lt_107;
		arr_add_item(fn_arr, 0x14, &fnbox);

		fnbox.fn = action_hero_skill_submenu;
		arr_add_item(fn_arr, 0x65, &fnbox);

		fnbox.fn = action_worker_building_submenu;
		arr_add_item(fn_arr, 0x66, &fnbox);

		fnbox.fn = action_minimap_signal;
		arr_add_item(fn_arr, 0x67, &fnbox);

		fnbox.fn = action_continue_game_1;
		arr_add_item(fn_arr, 0x68, &fnbox);

		fnbox.fn = action_continue_game_0;
		arr_add_item(fn_arr, 0x69, &fnbox);

	} else if (build < 6037) {

		fnbox.fn = action_ability_0_ge_107_lt_113;
		arr_add_item(fn_arr, 0x10, &fnbox);

		fnbox.fn = action_ability_1_ge_107_lt_113;
		arr_add_item(fn_arr, 0x11, &fnbox);

		fnbox.fn = action_ability_2_ge_107_lt_113;
		arr_add_item(fn_arr, 0x12, &fnbox);

		fnbox.fn = action_move_item_ge_107_lt_113;
		arr_add_item(fn_arr, 0x13, &fnbox);

		fnbox.fn = action_choose_ability_ge_107_lt_113;
		arr_add_item(fn_arr, 0x14, &fnbox);

	} /* build >= 6037 */
	else {

		fnbox.fn = action_ability_0_ge_113;
		arr_add_item(fn_arr, 0x10, &fnbox);

		fnbox.fn = action_ability_1_ge_113;
		arr_add_item(fn_arr, 0x11, &fnbox);

		fnbox.fn = action_ability_2_ge_113;
		arr_add_item(fn_arr, 0x12, &fnbox);

		fnbox.fn = action_move_item_ge_113;
		arr_add_item(fn_arr, 0x13, &fnbox);

		fnbox.fn = action_choose_ability_ge_113;
		arr_add_item(fn_arr, 0x14, &fnbox);
	}


	/* if ver >= 1.07 */
	if (build >= 6031) {

		fnbox.fn = action_hero_skill_submenu;
		arr_add_item(fn_arr, 0x66, &fnbox);

		fnbox.fn = action_worker_building_submenu;
		arr_add_item(fn_arr, 0x67, &fnbox);

		fnbox.fn = action_minimap_signal;
		arr_add_item(fn_arr, 0x68, &fnbox);

		fnbox.fn = action_continue_game_1;
		arr_add_item(fn_arr, 0x69, &fnbox);

		fnbox.fn = action_continue_game_0;
		arr_add_item(fn_arr, 0x6A, &fnbox);
	}


	/* if ver < 1.14b */
	if (build < 6040) {

		fnbox.fn = action_select_subgroup_lt_114b;
		arr_add_item(fn_arr, 0x19, &fnbox);
	} /* build >= 6040 */
	else {
		fnbox.fn = action_select_subgroup_ge_114b;
		arr_add_item(fn_arr, 0x19, &fnbox);
	}

	/* if ver < 1.15 */
	if (build < 6043) {

		fnbox.fn = action_0x1A_0x1B_unknown;
		arr_add_item(fn_arr, 0x1A, &fnbox);

		fnbox.fn = action_select_ground_item;
		arr_add_item(fn_arr, 0x1B, &fnbox);

		fnbox.fn = action_cancel_hero_revival;
		arr_add_item(fn_arr, 0x1C, &fnbox);

		fnbox.fn = action_reduce_building_queue;
		arr_add_item(fn_arr, 0x1D, &fnbox);
	} /* build >= 6043 */
	else {

		fnbox.fn = action_pre_subselection;
		arr_add_item(fn_arr, 0x1A, &fnbox);

		fnbox.fn = action_0x1A_0x1B_unknown;
		arr_add_item(fn_arr, 0x1B, &fnbox);

		fnbox.fn = action_select_ground_item;
		arr_add_item(fn_arr, 0x1C, &fnbox);

		fnbox.fn = action_cancel_hero_revival;
		arr_add_item(fn_arr, 0x1D, &fnbox);

		fnbox.fn = action_reduce_building_queue;
		arr_add_item(fn_arr, 0x1E, &fnbox);
	}
}


/**
 * @brief 
 */
int process_action_field(strm_t *strm, action_ls_t *ls, arr_t *fn_arr,
			 joiner_t *joiner, state_t *state, mmt_t *mmt,
			 unsigned len)
{
	int ret;
	int cnt;

	unsigned action_id;

	aux_t aux = { &action_id, APM_UINT, 1 };
	/* action_t *action = NULL; */

	/* action data */
	ad_t ad = {

		.strm = strm,
		.state = state,
		.curr_joiner = joiner,
		.mmt = mmt,
		.action_ls = ls,
	};

	while (len > 0) {

		ret = safe_mem_read(&strm->pos, strm->lim, &aux, 1);
		GUARD(0 != ret, return ret);

		if (action_id < 1  ||  action_id >= fn_arr->cnt) {
			return -1;
		}

		fnbox_t *fnbox = arr_get_item(fn_arr, action_id);
		if (NULL == fnbox->fn) {
			return -1;
		} else if (action_change_selection != fnbox->fn) {
			state->deselect = false;
		}

		cnt = fnbox->fn(&ad);
		if (-1 == cnt) {
			return -1;
		}

		--len;
		len -= cnt;
	}

	assert(0 == len);

	return 0;
}


/**
 * @brief
 */
int save_time_blk(strm_t *strm, action_ls_t *ls, arr_t *fn_arr,
		  joiner_tbl_t *joiner_tbl, state_t *state, mmt_t *mmt)
{
	int ret;

	unsigned rest_size;
	unsigned time_inc;

	aux_t aux_arr[2] = {

		{ &rest_size, APM_UINT, 2 },
		{ &time_inc, APM_UINT, 2 },
	};

	ret = safe_mem_read(&strm->pos, strm->lim, aux_arr, 2);
	GUARD(0 != ret, return ret);

	if (!state->paused) {
		moment_inc(mmt, time_inc);
	}

	rest_size -= 2;

	int len, id;

	aux_t aux_arr2[2] = {

		{ &id, APM_UINT, 1 },
		{ &len, APM_UINT, 2 },
	};

	/* process "move field" */
	while (rest_size > 0) {

		ret = safe_mem_read(&strm->pos, strm->lim, aux_arr2, 2);
		GUARD(0 != ret, return 1);

		joiner_t *joiner = seek_joiner(joiner_tbl, id);
		GUARD(NULL == joiner, return -1);

		ret = process_action_field(strm, ls, fn_arr, joiner,
					   state, mmt, len);
		GUARD(0 != ret, return 1);
		
		rest_size -= len + 3;
	}

	assert(0 == rest_size);

	return 0;
}


/**
 * @brief
 */
int save_join_scrn_blk(strm_t *strm, join_scrn_blk_t *blk,
		       const unsigned build)
{
	int ret;

	/* size of the rest of record in bytes */
	unsigned rest_size;

	/* empty size means 'to be computed' */
	aux_t aux_arr[] = {

		{ &rest_size,		APM_UINT,	2 },
		{ &blk->slot_cnt,	APM_UINT,	1 },
		{ &blk->rand_seed,	APM_ULONG,	4 },
		{ &blk->select_mode,	APM_UCHAR,	1 },
		{ &blk->start_spot_cnt, APM_UINT,	1 }
	};
	

	ret = safe_mem_read(&strm->pos, strm->lim, aux_arr, 2);
	GUARD(0 != ret, return ret);

	ret = save_slot_seq(strm, &blk->slot_tbl, blk->slot_cnt, build);
	GUARD(0 != ret, return ret);

	ret = safe_mem_read(&strm->pos, strm->lim, aux_arr + 2, 3);
	GUARD(0 != ret, return ret);

	return 0;
}


/**
 * @brief
 */
int save_host_blk(strm_t *strm, host_blk_t *host_blk)
{

	int ret;
	/* NULL assignment is required */
	char *ecd_str = NULL, *dcd_str = NULL;

	aux_t aux_arr[] = {

		{ &host_blk->game_name,			APM_UCHAR, 0 },
		{ &ecd_str,				APM_UCHAR, 0 },
		{ (void *)&host_blk->map_path,		APM_UCHAR, 0 },
		{ (void *)&host_blk->game_creator,	APM_UCHAR, 0 },
		{ &host_blk->player_cnt,		APM_UINT,  4 }
	};

	ret = save_prsn_rec(&host_blk->prsn, strm);
	GUARD(0 != ret, return ret);


	ret = safe_mem_read(&strm->pos, strm->lim, aux_arr, 1);
	GUARD(0 != ret, return ret);

	/* extra nullbyte after game_name string */
	ret = safe_pos_fw(&strm->pos, strm->lim, 1);
	GUARD(0 != ret, return ret);

	ret = safe_mem_read(&strm->pos, strm->lim, aux_arr + 1, 1);
	GUARD(0 != ret, return ret);

	dcd_str = (char *)malloc(aux_arr[1].size);
	if (!dcd_str) {
		ret = APM_E_NO_MEM;
		goto cleanup;
	}
	decode_opts_map_creator_str(ecd_str, dcd_str);

	/* current position */
	char *dcd_str_pos = dcd_str;
	char *dcd_str_lim = dcd_str + aux_arr[1].size;

	ret = save_game_opts(&host_blk->game_opts, &dcd_str_pos);
	GUARD(0 != ret, goto cleanup);


	ret = safe_mem_read(&dcd_str_pos, dcd_str_lim, aux_arr + 2, 2);
	GUARD(0 != ret, goto cleanup);

	ret = safe_mem_read(&strm->pos, strm->lim, aux_arr + 4, 1);
	GUARD(0 != ret, goto cleanup);

	/* skip gametype (4 bytes) and languageID (4 bytes) */
	ret = safe_pos_fw(&strm->pos, strm->lim, 8);
	GUARD(0 != ret, goto cleanup);

cleanup:
	free(ecd_str);
	free(dcd_str);

	return ret;
}


/**
 * @brief
 */
int save_leave_blk(strm_t *strm, joiner_tbl_t *joiner_tbl,
		   mmt_t *curr_mmt, joiner_t **last)
{
	unsigned id;
	joiner_t *joiner;
	int ret;

	aux_t aux[] = {

		{ &id, APM_UINT, 1 }
	};

	ret = safe_pos_fw(&strm->pos, strm->lim, 4);
	GUARD(0 != ret, return ret);

	ret = safe_mem_read(&strm->pos, strm->lim, aux, 1);
	GUARD(0 != ret, return ret);

	ret = safe_pos_fw(&strm->pos, strm->lim, 8);
	GUARD(0 != ret, return ret);

	joiner = seek_joiner(joiner_tbl, id);
	if (NULL == joiner) {
		return 1;
	}

	joiner->leave_mmt = *curr_mmt;

	*last = joiner;

	return 0;
}


/**
 * @brief
 */
int get_blk_id(strm_t *strm, unsigned *id)
{
	int ret;

	aux_t aux = { id, APM_UINT, 1 };

	ret = safe_mem_read(&strm->pos, strm->lim, &aux, 1);
	GUARD(0 != ret, return 1);

	return 0;
}


/**
 * @brief
 */
int save_dyn_blk_seq(strm_t *strm, action_ls_t *action_ls, chat_ls_t *chat_ls,
		     joiner_tbl_t *joiner_tbl, const unsigned build)
{
	int ret;
	unsigned blk_id;

	/* last leaver */
	joiner_t *last = NULL;

	arr_t *fn_arr = NULL;
	ret = arr_alloc(&fn_arr);
	GUARD(0 != ret, return ret);

	ret = arr_prep(fn_arr, 0x76, sizeof(fnbox_t));
	GUARD(0 != ret, goto cleanup);

	prep_action_ptrs(fn_arr, build);

	mmt_t mmt;
	moment_zero(&mmt);

	/* init state */
	state_t state = { false, false };


	while (1) {

		ret = get_blk_id(strm, &blk_id);
		GUARD(0 != ret, break);

		if (0x17 == blk_id) {
			ret = save_leave_blk(strm, joiner_tbl, &mmt, &last);
			GUARD(0 != ret, break);
			/*
			 * if pos stopped exactly behind last valid
			 * stream character and last block was "leave_blk"
			 * (which is) then we can succesfully return
			 * (this is the only successful way)
			 */
			if (pos_eq_lim(strm->pos, strm->lim)) {
				last->saver = true;
				break;
			}

		} else if (0x20 == blk_id) {
			if (build < 6031) {
				/* this block was introduced in 1.07 */
				ret = 1;
				break;
			}
			ret = save_chatmsg_blk(strm, chat_ls, &mmt);
			GUARD(0 != ret, break);

		} else if (0x22 == blk_id) {
			ret = save_0x22_blk(strm);
			GUARD(0 != ret, break);

		} else if (0x23 == blk_id) {
			ret = save_0x23_blk(strm);
			GUARD(0 != ret, break);

		} else if (0x1A == blk_id) {
			ret = save_0x1A_blk(strm);
			GUARD(0 != ret, break);

		} else if (0x1B == blk_id) {
			ret = save_0x1B_blk(strm);
			GUARD(0 != ret, break);

		} else if (0x1C == blk_id) {
			ret = save_0x1C_blk(strm);
			GUARD(0 != ret, break);

		} else if (0x1E == blk_id) {
			ret = save_time_blk(strm, action_ls, fn_arr,
					    joiner_tbl, &state, &mmt);
			GUARD(0 != ret, break);

		} else if (0x1F == blk_id) {
			if (build < 4531) {
				/* this block cannot occur in this ver */
				ret = 1;
				break;
			}
			ret = save_time_blk(strm, action_ls, fn_arr,
					    joiner_tbl, &state, &mmt);
			GUARD(0 != ret, break);

		} else if (0x2F == blk_id) {
			if (build < 6031) {
				/* this block was introduced in 1.07 version */
				ret = 1;
				break;
			}

			ret = save_countdown_blk(strm);
			GUARD(0 != ret, break);
		} else {
			ret = 1;
			break;
		}
	}

cleanup:
	arr_dealloc(&fn_arr);

	return ret;
}


/**
 * @brief Read concrete static block.
 */
int save_static_blk(strm_t *strm, rfnd_t *rfnd,
		    const unsigned build, const unsigned blk_id)
{

	int ret;
	prsn_t *prsn = NULL;

	switch (blk_id) {

	case 0x00:
		ret = save_host_blk(strm, rfnd->host_blk);
		break;

	case 0x16:
		prsn = (prsn_t *)malloc(sizeof(*prsn));
		if (!prsn) {
			ret = APM_E_NO_MEM;
			break;
		}
		prsn_zero(prsn);

		ret = save_guest_blk(strm, prsn);
		GUARD(0 != ret, free(prsn); break);

		ret = tbl_add_item(rfnd->prsn_tbl, prsn);
		GUARD(0 != ret, free(prsn); break);

		ret = safe_pos_fw(&strm->pos, strm->lim, 4);
		break;

	case 0x19:
		ret = save_join_scrn_blk(strm, rfnd->join_scrn_blk, build);
		break;

	default:
		ret = 1;
		break;
	}
	return ret;
}


/**
 * @brief Read sequence of static blocks.
 *
 * @param strm
 * @param rfnd
 * @param build
 * @param blk_nfo description of blocks
 * @param cnt number of blk_nfo_t items
 * @return 0 on success
 */
int save_static_blk_seq(strm_t *strm, rfnd_t *rfnd, const unsigned build,
			blk_nfo_t *blk_nfo, size_t cnt)
{
	int ret;
	unsigned blk_id;
	/* pointer to first item behind array */
	blk_nfo_t *lim = blk_nfo + cnt;


	while (blk_nfo < lim) {

		/* read block id */
		ret = get_blk_id(strm, &blk_id);
		GUARD(0 != ret, return ret);

		if (blk_id == blk_nfo->id) {

			ret = save_static_blk(strm, rfnd, build, blk_id);
			GUARD(0 != ret, return ret);

			if (!blk_nfo->repeat) {
				++blk_nfo;
			}

		} else {
			if (blk_nfo->mandatory) {
				/* id was other than expected */
				return 1;
			} else {
				unsafe_pos_bw(&strm->pos, 1);
				++blk_nfo;
			}
		}
	}
	return 0;
}


/**
 * @brief Evaluate APM of all players.
 *
 * @param tbl joiner table
 */
void eval_apm(joiner_tbl_t *tbl)
{
	size_t cnt = tbl->cnt;
	joiner_t **joiner = (joiner_t **)tbl->arr;

	while (cnt--)	 {
		/* if joiner is player */
		if (12 != (*joiner)->team_no) {
			(*joiner)->apm /= moment_min(&(*joiner)->leave_mmt);
		}
		++joiner;
	}
}


/**
 * @brief Process the stream -- sequence of various blocks.
 *
 * @param strm stream
 * @param rfnd refined data
 * @param build
 * @return 
 */
int process_stream(strm_t *strm, rfnd_t *rfnd, const unsigned build)
{
	int ret;
	unsigned unknown;

	/* auxiliary pointer */
	extra_t *extra = &rfnd->extra;

	aux_t aux = { &unknown, APM_ULONG, 4 };

	ret = safe_mem_read(&strm->pos, strm->lim, &aux, 1);
	GUARD(0 != ret, return ret);

	blk_nfo_t nfo_seq[] = {
		{ .id =	 0x00, .repeat = false, .mandatory = true  },
		{ .id =	 0x16, .repeat = true,	.mandatory = false },
		{ .id =	 0x19, .repeat = false, .mandatory = true  },
	};


	/* allocate prsn_tbl_t structures */
	ret = tbl_alloc(&rfnd->prsn_tbl);
	GUARD(0 != ret, return ret);

	ret = tbl_prep(rfnd->prsn_tbl, 10, prsn_free_fn);
	GUARD(0 != ret, return ret);

	ret = join_scrn_blk_alloc(&rfnd->join_scrn_blk);
	GUARD(0 != ret, return ret);


	/* read all blocks that typically occurs only at the stream beginning */
	ret = save_static_blk_seq(strm, rfnd, build, nfo_seq, 3);
	GUARD(0 != ret, return ret);

	/* prepare joiner table */
	ret = tbl_prep(&extra->joiner_tbl, rfnd->host_blk->player_cnt,
		       joiner_free_fn);
	GUARD(0 != ret, return ret);


	ret = form_joiner_tbl(rfnd->prsn_tbl, &rfnd->host_blk->prsn,
			      &rfnd->join_scrn_blk->slot_tbl,
			      &extra->joiner_tbl);
	GUARD(0 != ret, return ret);

	/* sort joiner table */
	tbl_sort(&extra->joiner_tbl, joiner_cmp_fn);

	/* allocate chat list structures */
	ret = list_alloc(&extra->chat_ls);
	GUARD(0 != ret, return ret);

	/* prepare chat list */
	list_prep(extra->chat_ls, msgbox_free_fn);

	/* allocate action list structures */
	list_alloc(&extra->action_ls);
	list_prep(extra->action_ls, free);


	/* read sequence of various blocks */
	ret = save_dyn_blk_seq(strm, extra->action_ls, extra->chat_ls,
			       &extra->joiner_tbl, build);
	GUARD(0 != ret, return ret);

	return 0;
}
