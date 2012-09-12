/**
 *@file actions.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#ifndef APM_ACTION_H
#define APM_ACTION_H


typedef struct state_t {

	/*  */
	bool paused;

	bool deselect;
	
} state_t;

/* action data */
typedef struct ad_t {

	strm_t *strm;

	action_ls_t *action_ls;

	state_t *state;

	joiner_t *curr_joiner;

	mmt_t *mmt;

} ad_t;


extern int action_pause(ad_t *ad);
extern int action_resume(ad_t *ad);
extern int action_set_speed(ad_t *ad);
extern int action_inc_speed(ad_t *ad);
extern int action_dec_speed(ad_t *ad);
extern int action_save(ad_t *ad);
extern int action_saving_done(ad_t *ad);
extern int action_ability_0_lt_107(ad_t *ad);
extern int action_ability_0_ge_107_lt_113(ad_t *ad);
extern int action_ability_0_ge_113(ad_t *ad);
extern int action_ability_1_lt_107(ad_t *ad);
extern int action_ability_1_ge_107_lt_113(ad_t *ad);
extern int action_ability_1_ge_113(ad_t *ad);
extern int action_ability_2_lt_107(ad_t *ad);
extern int action_ability_2_ge_107_lt_113(ad_t *ad);
extern int action_ability_2_ge_113(ad_t *ad);
extern int action_move_item_lt_107(ad_t *ad);
extern int action_move_item_ge_107_lt_113(ad_t *ad);
extern int action_move_item_ge_113(ad_t *ad);
extern int action_choose_ability_lt_107(ad_t *ad);
extern int action_choose_ability_ge_107_lt_113(ad_t *ad);
extern int action_choose_ability_ge_113(ad_t *ad);
extern int action_change_selection(ad_t *ad);
extern int action_assign_group_hotkey(ad_t *ad);
extern int action_select_group_hotkey(ad_t *ad);
extern int action_select_subgroup_lt_114b(ad_t *ad);
extern int action_select_subgroup_ge_114b(ad_t *ad);
extern int action_pre_subselection(ad_t *ad);
extern int action_0x1A_0x1B_unknown(ad_t *ad);
extern int action_select_ground_item(ad_t *ad);
extern int action_cancel_hero_revival(ad_t *ad);
extern int action_reduce_building_queue(ad_t *ad);
extern int action_cheat_TheDudeAbides(ad_t *ad);
extern int action_0x21_unknown(ad_t *ad);
extern int action_cheat_SomebodySetUpUsTheBomb(ad_t *ad);
extern int action_cheat_WarpTen(ad_t *ad);
extern int action_cheat_IocainePowder(ad_t *ad);
extern int action_cheat_PointBreak(ad_t *ad);
extern int action_cheat_WhosYourDaddy(ad_t *ad);
extern int action_cheat_KeyserSoze(ad_t *ad);
extern int action_cheat_LeafitToMe(ad_t *ad);
extern int action_cheat_ThereIsNoSpoon(ad_t *ad);
extern int action_cheat_StrengthAndHonor(ad_t *ad);
extern int action_cheat_itvexesme(ad_t *ad);
extern int action_cheat_WhoIsJohnGalt(ad_t *ad);
extern int action_cheat_GreedIsGood(ad_t *ad);
extern int action_cheat_DayLightSavings(ad_t *ad);
extern int action_cheat_ISeeDeadPeople(ad_t *ad);
extern int action_cheat_Synergy(ad_t *ad);
extern int action_cheat_SharpAndShiny(ad_t *ad);
extern int action_cheat_AllYourBaseAreBelongToUs(ad_t *ad);
extern int action_change_ally_opts(ad_t *ad);
extern int action_transfer_resources(ad_t *ad);
extern int action_map_trigger_chat_command(ad_t *ad);
extern int action_esc_keystroke(ad_t *ad);
extern int action_scenario_trigger(ad_t *ad);
extern int action_hero_skill_submenu(ad_t *ad);
extern int action_worker_building_submenu(ad_t *ad);
extern int action_minimap_signal(ad_t *ad);
extern int action_continue_game_0(ad_t *ad);
extern int action_continue_game_1(ad_t *ad);
extern int action_0x75_unknown(ad_t *ad);

#endif /* APM_ACTIONS_H */
