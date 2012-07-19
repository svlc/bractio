
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

        apm_t *apm = apm_wc3_init(argv[1]);
        if(NULL == apm) {
                return EXIT_FAILURE;
        }

        /* parse replay values into memory */
        ret = apm_wc3_process(apm, APM_REQ_HEADER | APM_REQ_INFO | APM_REQ_APM);
        if(0 != ret) {
                apm_wc3_deinit(apm);
                return EXIT_FAILURE;
        }

        /* WAR3 (WarCraft III -- The Reign Of Chaos) */
        /* W3XP (WarCraft III expansion set -- The Frozen Throne) */
        printf("Release version: %s\n", apm_wc3_get_release(apm));

        printf("Patch version: %u\n", apm_wc3_get_patch_version(apm));

        printf("Length in ms: %u\n", apm_wc3_get_length(apm));

	printf("Map path: %s\n", apm_wc3_get_map_path(apm));

	printf("Number of positions on map: %u\n",
	       apm_wc3_get_map_position_cnt(apm));

	printf("Hostname: %s\n", apm_wc3_get_hostname(apm));

	printf("Game speed level: %u\n", apm_wc3_get_game_speed(apm));

        /* to not cause memory leaks */
        apm_wc3_deinit(apm);

        return EXIT_SUCCESS;
}
