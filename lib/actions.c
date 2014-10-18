/**
 * @file actions.c
 * @brief
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#include <stdlib.h>
#include "bract.h"
#include "str.h"
#include "check.h"

#include "actions.h"

/**
 * @brief
 */
int add_action(struct ulist *ls, joiner_t *joiner, mmt_t *mmt)
{
	int ret;
	action_t *action;

	action = (action_t *)malloc(sizeof(*action));
	if (!action) {
		ret = -1;
		goto out;
	}
	action->joiner = joiner;
	action->mmt = *mmt;

	ret = ulist_append(ls, action);
	if (0 != ret) {
		ret = -1;
	}
out:
	return ret;
}

/**
 * @brief
 */
int action_pause(ad_t *ad)
{
	ad->state->paused = true;
	return 0;
}

/**
 * @brief
 */
int action_resume(ad_t *ad)
{
	ad->state->paused = false;
	return 0;
}

/**
 * @brief
 */
int action_set_speed(ad_t *ad)
{
	int ret;
	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 1);
	if (0 != ret) {
		return -1;
	}
	return 1;
}

/**
 * @brief
 * 
 * @note this can be done in single player game only
 */
int action_inc_speed(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 *
 * @note this can be done in single player game only
 */
int action_dec_speed(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_save(ad_t *ad)
{
	int ret;
	char *str = NULL;

	aux_t aux = { &str, BRACT_UCHAR, 0 };

	ret = safe_mem_read(&ad->strm->pos, ad->strm->lim, &aux, 1);
	if(0 != ret) {
		return -1;
	}
	return aux.size;
}

/**
 * @brief
 */
int action_saving_done(ad_t *ad)
{
	int ret;
	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 4);
	if(0 != ret) {
		return -1;
	}
	return 4;
}

/**
 * @brief
 */
int action_ability_0_lt_107(ad_t *ad)
{
	int ret;
	int len = 5;

	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);
	++ad->curr_joiner->apm;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_ability_0_ge_107_lt_113(ad_t *ad)
{
	int ret;
	int len = 13;

	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_ability_0_ge_113(ad_t *ad)
{
	int ret;
	int len = 14;
	action_t *action;

	++ad->curr_joiner->apm;
	action = (action_t *)malloc(sizeof(*action));
	if (!action) {
		return -1;
	}
	action->joiner = ad->curr_joiner;
	action->mmt = *ad->mmt;

	ret = ulist_append(ad->action_ls, action);
	if(0 != ret) {
		return -1;
	}
	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_ability_1_lt_107(ad_t *ad)
{
	int ret;
	int len = 13;

	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_ability_1_ge_107_lt_113(ad_t *ad)
{
	int ret;
	int len = 21;

	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);
	++ad->curr_joiner->apm;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_ability_1_ge_113(ad_t *ad)
{
	int ret;
	int len = 22;
	action_t *action;

	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);
	++ad->curr_joiner->apm;
	action = (action_t *)malloc(sizeof(*action));
	if (!action) {
		return -1;
	}
	action->joiner = ad->curr_joiner;
	action->mmt = *ad->mmt;

	ret = ulist_append(ad->action_ls, action);
	if(0 != ret) {
		return -1;
	}
	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_ability_2_lt_107(ad_t *ad)
{
	int ret;
	int len = 21;

	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);
	++ad->curr_joiner->apm;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_ability_2_ge_107_lt_113(ad_t *ad)
{
	int ret;
	int len = 29;

	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_ability_2_ge_113(ad_t *ad)
{
	int ret;
	int len = 30;

	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_move_item_lt_107(ad_t *ad)
{

	int ret;
	int len = 29;

	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_move_item_ge_107_lt_113(ad_t *ad)
{

	int ret;
	int len = 37;

	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_move_item_ge_113(ad_t *ad)
{

	int ret;
	int len = 38;

	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_choose_ability_lt_107(ad_t *ad)
{

	int ret;
	int len = 34;

	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_choose_ability_ge_107_lt_113(ad_t *ad)
{

	int ret;
	int len = 42;

	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_choose_ability_ge_113(ad_t *ad)
{
	int ret;
	int len = 43;

	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, len);
	if(0 != ret) {
		return -1;
	}
	return len;
}

/**
 * @brief
 */
int action_change_selection(ad_t *ad)
{
	int ret;
	unsigned mode;
	unsigned cnt;
	aux_t aux_arr[2] = {

		{ &mode, BRACT_UINT, 1 },
		{ &cnt, BRACT_UINT, 2 }
	};

	ret = safe_mem_read(&ad->strm->pos, ad->strm->lim, aux_arr, 2);
	if(0 != ret) {
		return -1;
	}
	ret = check_action_change_selection(mode);
	if(0 != ret) {
		return ret;
	}

	/* if "select" */
	if (0x01 == mode) {
		/* if previous action was not deselect */
		if (false == ad->state->deselect) {
			/* count this apm action */
			++ad->curr_joiner->apm;
			ret = add_action(ad->action_ls, ad->curr_joiner,
			ad->mmt);
		}
		/* in some cases causes no change, but better then condition */
		ad->state->deselect = false;
	}
	/* this must be "deselect" -- @see check_action_change_selection() */
	else {
		ad->state->deselect = true;
		++ad->curr_joiner->apm;
		ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);
	}

	unsigned shift = cnt * 8;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, shift);
	if(0 != ret) {
		return -1;
	}
	return 3 + shift;
}

/**
 * @brief
 */
int action_assign_group_hotkey(ad_t *ad)
{

	int ret;
	unsigned grp_no;
	unsigned cnt;
	aux_t aux_arr[2] = {
		{ &grp_no, BRACT_UINT, 1 },
		{ &cnt, BRACT_UINT, 2 }
	};

	ret = safe_mem_read(&ad->strm->pos, ad->strm->lim, aux_arr, 2);
	if(0 != ret) {
		return -1;
	}

	unsigned shift = cnt * 8;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, shift);
	if(0 != ret) {
		return -1;
	}
	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);
	return 3 + shift;
}

/**
 * @brief
 */
int action_select_group_hotkey(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 2);
	if(0 != ret) {
		return -1;
	}
	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);
	return 2;
}

/**
 * @brief
 */
int action_select_subgroup_lt_114b(ad_t *ad)
{

	int ret;
	unsigned subgrp_no;

	aux_t aux = { &subgrp_no, BRACT_UINT, 1 };

	ret = safe_mem_read(&ad->strm->pos, ad->strm->lim, &aux, 1);
	if(0 != ret) {
		return -1;
	}
	if (0x00 != subgrp_no  &&  0xFF != subgrp_no) {
		++ad->curr_joiner->apm;
		ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);

	}
	return 1;
}

/**
 * @brief
 */
int action_select_subgroup_ge_114b(ad_t *ad)
{

	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 12);
	if(0 != ret) {
		return -1;
	}
	return 12;
}

/**
 * @brief
 */
int action_pre_subselection(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_0x1A_0x1B_unknown(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 9);
	if(0 != ret) {
		return -1;
	}
	return 9;
}

/**
 * @brief
 */
int action_select_ground_item(ad_t *ad)
{

	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 9);
	if(0 != ret) {
		return -1;
	}
	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);
	return 9;
}

/**
 * @brief
 */
int action_cancel_hero_revival(ad_t *ad)
{

	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 8);
	if(0 != ret) {
		return -1;
	}
	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);
	return 8;
}

/**
 * @brief
 */
int action_reduce_building_queue(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 5);
	if(0 != ret) {
		return -1;
	}
	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);
	return 5;
}

/**
 * @brief
 */
int action_cheat_TheDudeAbides(ad_t *ad)
{
	ad = ad;	
	return 0;
}

/**
 * @brief
 */
int action_0x21_unknown(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 9);
	if(0 != ret) {
		return -1;
	}
	return 9;
}

/**
 * @brief
 */
int action_cheat_SomebodySetUpUsTheBomb(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_cheat_WarpTen(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_cheat_IocainePowder(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_cheat_PointBreak(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_cheat_WhosYourDaddy(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_cheat_KeyserSoze(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 5);
	if(0 != ret) {
		return -1;
	}
	return 5;
}

/**
 * @brief
 */
int action_cheat_LeafitToMe(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 5);
	if(0 != ret) {
		return -1;
	}
	return 5;
}

/**
 * @brief
 */
int action_cheat_ThereIsNoSpoon(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_cheat_StrengthAndHonor(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_cheat_itvexesme(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_cheat_WhoIsJohnGalt(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_cheat_GreedIsGood(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 5);
	if(0 != ret) {
		return -1;
	}
	return 5;
}

/**
 * @brief
 */
int action_cheat_DayLightSavings(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 4);
	if(0 != ret) {
		return -1;
	}
	return 4;
}

/**
 * @brief
 */
int action_cheat_ISeeDeadPeople(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_cheat_Synergy(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_cheat_SharpAndShiny(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_cheat_AllYourBaseAreBelongToUs(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_change_ally_opts(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 5);
	if(0 != ret) {
		return -1;
	}
	return 5;
}

/**
 * @brief
 */
int action_transfer_resources(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 9);
	if(0 != ret) {
		return -1;
	}
	return 9;
}

/**
 * @brief
 */
int action_map_trigger_chat_command(ad_t *ad)
{
	int ret;
	char *str;
	aux_t aux = { (void *)&str, BRACT_UCHAR, 0 };

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 8);
	if(0 != ret) {
		return -1;
	}
	ret = safe_mem_read(&ad->strm->pos, ad->strm->lim, &aux, 1);
	if(0 != ret) {
		return -1;
	}
	return 8 + aux.size;
}

/**
 * @brief
 */
int action_esc_keystroke(ad_t *ad)
{
	int ret;

	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);
	if(0 != ret) {
		return -1;
	}
	return 0;
}

/**
 * @brief
 */
int action_scenario_trigger(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 12);
	if(0 != ret) {
		return -1;
	}
	return 12;
}

/**
 * @brief
 */
int action_hero_skill_submenu(ad_t *ad)
{
	int ret;

	++ad->curr_joiner->apm;
	ret = add_action(ad->action_ls, ad->curr_joiner, ad->mmt);
	if(0 != ret) {
		return -1;
	}
	return 0;
}

/**
 * @brief
 */
int action_worker_building_submenu(ad_t *ad)
{
	ad = ad;
	return 0;
}

/**
 * @brief
 */
int action_minimap_signal(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 12);
	if(0 != ret) {
		return -1;
	}
	return 12;
}

/**
 * @brief
 */
int action_continue_game_0(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 16);
	if(0 != ret) {
		return -1;
	}
	return 16;
}

/**
 * @brief
 */
int action_continue_game_1(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 16);
	if(0 != ret) {
		return -1;
	}
	return 16;
}

/**
 * @brief
 */
int action_0x75_unknown(ad_t *ad)
{
	int ret;

	ret = safe_pos_fw(&ad->strm->pos, ad->strm->lim, 1);
	if(0 != ret) {
		return -1;
	}
	return 1;
}
