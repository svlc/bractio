/**
 *@file init_deinit.h
 *@brief
 *@athor Slavomir Vlcek
 *@copyright GPLv2
 */

#ifndef APM_INIT_DEINIT_H
#define APM_INIT_DEINIT_H


extern int _init_stream(apm_t *apm);

extern int _init_block_table(apm_t *apm);

extern int _init_block(blk_t *blk);

extern int _prolong_player_table(player_tab_t *player_tab);

/* extern void _init_player_record(player_rec_t *player_rec); */

extern int _init_player_table(apm_t *apm);

extern int _init_slot_table(apm_t *apm, const int cnt);

extern int _init_game_start_rec(apm_t *apm);

extern int _init_info(apm_t *apm);

extern apm_t *apm_wc3_init(const char *path);

extern void apm_wc3_deinit(apm_t *apm);


#endif /* APM_INIT_DEINIT_H */
