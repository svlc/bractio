/**
 * @file check.h
 * @brief 
 * @athor Slavomir Vlcek
 * @copyright GPLv2
 */

#ifndef APM_CHECK_H
#define APM_CHECK_H

extern int check_task(task_t task);
extern int check_rep_main_hdr(char *magic_id, main_hdr_t *m_hdr);
extern int check_rep_sub_hdr(sub_hdr_t *s_hdr, char *rls_seq);
extern int check_action_change_selection(unsigned mode);

#endif /* APM_CHECK_H */
