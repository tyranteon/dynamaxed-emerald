#include "constants/global.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/map_scripts.h"
#include "constants/mevent.h"
#include "generated/move_ids.h"
#include "constants/region_map_sections.h"
#include "constants/songs.h"
#include "generated/species.h"
#include "constants/vars.h"
	.include "asm/macros.inc"
	.include "asm/macros/event.inc"
	.include "constants/constants.inc"

	.section .rodata

	.align 2
	.include "data/scripts/mevent_stamp_card.inc"
	.include "data/scripts/mevent_pichu.inc"
	.include "data/scripts/mevent_trainer.inc"
	.include "data/scripts/mevent_battle_card.inc"
	.include "data/scripts/mevent_aurora_ticket.inc"
	.include "data/scripts/mevent_mystic_ticket.inc"
	.include "data/scripts/mevent_altering_cave.inc"
	.include "data/scripts/mevent_old_sea_map.inc"
