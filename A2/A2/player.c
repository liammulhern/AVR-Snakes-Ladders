/*
 * player.c
 *
 * Created: 22/10/2022 11:13:03 AM
 *  Author: LiamM
 */ 

void init_player(struct player * game_player) {
	*game_player->x = 0;
	*game_player->y = 0;
	*game_player->visibility = 1;		
}

// Move the player by the given number of spaces forward.
void move_player_n(uint8_t num_spaces, struct player * game_player) {
	// Create temporary x, y coord that is set depending on player number.
	int8_t player_x, player_y;
	player_x = game_player->x;
	player_y = game_player->y;

	uint8_t object_at_cursor = get_object_at_cursor(player_x, player_y, player_num);
	update_square_colour(player_x, player_y, object_at_cursor);
	
	for(int moves = 0; moves < num_spaces; moves++) {
		// Player is on odd row move left.
		if (player_y % 2) {
			if (player_x - 1 >= 0) player_x--;
			else player_y++;
		}
		// Player is on even row move right.
		else {
			if (player_x + 1 < WIDTH) player_x++;
			else player_y++;
		}
	}
	
	// Player is outside bounds, move to correct top corner.
	if (player_y >= HEIGHT) {
		player_y = HEIGHT - 1; //Move player to top row
		player_x = (WIDTH - 1) * !((HEIGHT - 1) % 2); //Move player to top corner depending on odd/even row.
	}
	else {
		player_turns_inc(1);
	}
	
	set_player_n_position(player_num, player_x, player_y);
	update_square_colour(player_x, player_y, player_num);
	handle_player_collision(player_x, player_y, player_num);
}