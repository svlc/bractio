/**
 *@file str.c
 *@brief
 *@athor Slavomir Vlcek
 */


/**
 *@brief Reverses given sequence of characters (is not '\0' controlled)
 *
 *@param seq
 *@param len
 */
void _rev_char_seq(char *const seq, const int len)
{
        int head = 0;
        int tail = len - 1;

        /* auxiliary var */
        char aux;

        while(head < tail)
        {
                aux = seq[head];

                seq[head] = seq[tail];

                seq[tail] = aux;

                ++head;
                --tail;
        }
}
