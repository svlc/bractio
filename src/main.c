
/******************************************************************************/
/* very brief example of library use                                          */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../lib/apm.h"

void usage(char **argv)
{
        printf("Program inspects WarCraft III replay files.\n");
        printf("usage: %s FILE\n", argv[0]);
}

int main(int argc, char **argv)
{

        if(2 != argc) {
                usage(argv);
                return EXIT_FAILURE;
        } else if(0 == strcmp("-h", argv[1])) {
                usage(argv);
                return EXIT_SUCCESS;
        }

        int ret;

        sup_t *sup = apm_wc3_init(argv[1]);
        if(NULL == sup) {
                return EXIT_FAILURE;
        }

        /* parse replay values into memory */
        ret = apm_wc3_process(sup, APM_REQ_HEADER | APM_REQ_APM);
        if(0 != ret) {
                apm_wc3_deinit(sup);
                return EXIT_FAILURE;
        }

        /* WAR3 (WarCraft III -- The Reign Of Chaos) */
        /* W3XP (WarCraft III expansion set -- The Frozen Throne) */
        printf("Release version: %s\n", apm_wc3_get_release(sup));

        printf("Length in ms: %u\n", apm_wc3_get_length(sup));

        /* to not cause memory leaks */
        apm_wc3_deinit(sup);

        return EXIT_SUCCESS;
}
