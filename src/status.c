/* 
 * (c)2003-2004 by Warren Dukes (shank@mercury.chem.pitt.edu)
 * This project's homepage is: http://www.musicpd.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include "libmpdclient.h"
#include "charConv.h"
#include "util.h"
#include "mpc.h"

#include <stdio.h>
#include <string.h>

void print_status (mpd_Connection *conn)
{
	mpd_Status * status;
	mpd_InfoEntity * entity;
	status = mpd_getStatus(conn);
	printErrorAndExit(conn);

	if(status->state == MPD_STATUS_STATE_PLAY || 
			status->state == MPD_STATUS_STATE_PAUSE) {
		mpd_sendPlaylistInfoCommand(conn,status->song);
		printErrorAndExit(conn);

		while((entity = mpd_getNextInfoEntity(conn))) {
			mpd_Song * song = entity->info.song;
			
			if(entity->type!=MPD_INFO_ENTITY_TYPE_SONG) {
				mpd_freeInfoEntity(entity);
				continue;
			}

			if(song->artist && song->title &&
				strlen(song->artist) &&
				strlen(song->title)) {
				printf("%s - ",fromUtf8(song->artist));
				printf("%s\n",fromUtf8(song->title));
			}
			else printf("%s\n",fromUtf8(song->file));

			mpd_freeInfoEntity(entity);

			break;
		}

		printErrorAndExit(conn);

		mpd_finishCommand(conn);
		printErrorAndExit(conn);

		if(status->state==MPD_STATUS_STATE_PLAY) {
			printf("[playing]");
		}
		else printf("[paused] ");

		printf(" #%i/%i %3i:%02i (%.0f%c)\n",
				status->song+1,
				status->playlistLength,
				status->elapsedTime/60,
				status->elapsedTime%60,
				100.0*status->elapsedTime/
				status->totalTime,'%');
	}

	if(status->volume!=MPD_STATUS_NO_VOLUME) {
		printf("volume:%3i%c   ",status->volume,'%');
	}
	else {
		printf("volume: n/a   ");
	}

	printf("repeat: ");
	if(status->repeat) printf("on    ");
	else printf("off   ");

	printf("random: ");
	if(status->random) printf("on \n");
	else printf("off\n");

	mpd_freeStatus(status);
}

